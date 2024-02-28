import itertools
import os
import pprint
import re
import secrets
import shutil
import string
import subprocess
from abc import ABC, abstractmethod
from enum import Enum
from typing import TypedDict

import tabulate

pp = pprint.PrettyPrinter(indent=4)


def parse_algo(algo: str) -> tuple[str, str] | None:
    exp = re.compile("(.+)-(ecb|cfb[18]?|cbc|ofb|ctr)")
    res = exp.match(algo)
    if res is None:
        return None
    alg, cipher_mode = res.groups()
    return alg, cipher_mode


class Colors:
    RED = '\x1b[31m'
    GREEN = '\x1b[32m'
    CLEAR = '\x1b[0m'


class TestCategory:
    def __init__(self, use_pbkdf2: bool, encrypt_mode: bool, base64: bool):
        self.use_pbkdf2 = use_pbkdf2
        self.encrypt_mode = encrypt_mode
        self.base64 = base64

        cat_description = [
            "Encrypt" if self.encrypt_mode else "Decrypt",
            "using", "PBKDF2 generated secrets" if self.use_pbkdf2 else "given key/IV couple",
            "and dump", "base64 encoded output" if self.base64 else "raw binary output"
        ]
        self.cat_description = " ".join(cat_description)

        repr_cat_name = ["enc" if self.encrypt_mode else "dec", "pbkdf2" if self.use_pbkdf2 else "key"]
        if self.base64:
            repr_cat_name.append("base64")
        self.repr_cat_name = "_".join(repr_cat_name)

        pretty_cat_name = [
            "Encrypt" if self.encrypt_mode else "Decrypt",
            "PBKDF2" if self.use_pbkdf2 else "raw key/IV",
            "base64" if self.base64 else "binary"
        ]
        self.pretty_cat_name = " - ".join(pretty_cat_name)

    def __eq__(self, other):
        if isinstance(other, TestCategory):
            return (
                    self.use_pbkdf2 == other.use_pbkdf2 and
                    self.encrypt_mode == other.encrypt_mode and
                    self.base64 == other.base64
            )
        return NotImplemented

    def __hash__(self):
        return hash((self.use_pbkdf2, self.encrypt_mode, self.base64))

    def __str__(self) -> str:
        return self.pretty_cat_name

    def __repr__(self) -> str:
        return (f"TestCategory({self.repr_cat_name}) -> {{ "
                f"use_pbkdf2: {self.use_pbkdf2}, "
                f"encrypt_mode: {self.encrypt_mode}, "
                f"base64: {self.base64} "
                f"}}")


class TestStatsSummary(Enum):
    ENCRYPT = 0
    DECRYPT = 1
    KEY = 2
    PBKDF2 = 3
    ALGO = 4
    CIPHER_MODE = 5
    BASE64_OUTPUT = 6
    BINARY_OUTPUT = 7

    @staticmethod
    def get_by_category(cat: TestCategory) -> list['TestStatsSummary']:
        res: list[TestStatsSummary] = [
            TestStatsSummary.PBKDF2 if cat.use_pbkdf2 else TestStatsSummary.KEY,
            TestStatsSummary.ENCRYPT if cat.encrypt_mode else TestStatsSummary.DECRYPT,
            TestStatsSummary.BASE64_OUTPUT if cat.base64 else TestStatsSummary.BINARY_OUTPUT,
        ]

        return res

    def __str__(self) -> str:
        if self == TestStatsSummary.PBKDF2:
            return "Tests using pbkdf2 function"
        elif self == TestStatsSummary.ENCRYPT:
            return "Encrypt tests"
        elif self == TestStatsSummary.DECRYPT:
            return "Decrypt tests"
        elif self == TestStatsSummary.KEY:
            return "Tests using raw key/iv couple"
        elif self == TestStatsSummary.ALGO or self == TestStatsSummary.CIPHER_MODE:
            return "Tests for"
        elif self == TestStatsSummary.BINARY_OUTPUT:
            return "Tests with binary output"
        elif self == TestStatsSummary.BASE64_OUTPUT:
            return "Tests with base64 output"


class TestStats:
    def __init__(self, categories: list[TestCategory], algos: set[str]):
        self.__stats: dict[TestCategory, dict[str, tuple[int, int]]] = {}
        self.__summary: dict[TestStatsSummary, tuple[int, int] | dict[str, tuple[int, int]]] = {
            TestStatsSummary.ENCRYPT: (0, 0),
            TestStatsSummary.DECRYPT: (0, 0),
            TestStatsSummary.KEY: (0, 0),
            TestStatsSummary.PBKDF2: (0, 0),
            TestStatsSummary.ALGO: {},
            TestStatsSummary.CIPHER_MODE: {},
            TestStatsSummary.BASE64_OUTPUT: (0, 0),
            TestStatsSummary.BINARY_OUTPUT: (0, 0),
        }

        for cat in categories:
            self.__stats[cat] = {}

            for algo in algos:
                self.__stats[cat][algo] = (0, 0)

    def increment(self, cat: TestCategory, algo: str, success: bool):
        if cat in self.__stats:
            stat_cat = self.__stats[cat]
            if algo in stat_cat:
                cnt, total = stat_cat[algo]
                if success:
                    cnt += 1
                stat_cat[algo] = (cnt, total + 1)

                summary_cats = TestStatsSummary.get_by_category(cat)
                for summary_cat in summary_cats:
                    cnt, total = self.__summary[summary_cat]
                    if success:
                        cnt += 1
                    self.__summary[summary_cat] = (cnt, total + 1)

                alg, cipher_mode = parse_algo(algo)
                summary_algos = self.__summary[TestStatsSummary.ALGO]
                summary_cipher_modes = self.__summary[TestStatsSummary.CIPHER_MODE]

                cnt, total = summary_algos.get(alg, (0, 0))
                if success:
                    cnt += 1
                summary_algos[alg] = (cnt, total + 1)

                cnt, total = summary_cipher_modes.get(cipher_mode, (0, 0))
                if success:
                    cnt += 1
                summary_cipher_modes[cipher_mode] = (cnt, total + 1)

    @staticmethod
    def __get_percentage(count: int, total: int) -> str:
        if total == 0:
            return "NaN"
        return f"{100 * count / total:.2f}"

    def print(
            self,
            cat: TestCategory | None,
            algo: str | None = None,
            pbkdf2: bool = True,
            key: bool = True,
            base64: bool = True,
            encrypt: bool = True,
            decrypt: bool = True,
            only_cat_summary: bool = False):
        algo_cnt, algo_total = 0, 0

        def print_cat(local_cat: TestCategory, cat_stats: dict[str, tuple[int, int]]):
            nonlocal algo_cnt
            nonlocal algo_total
            nonlocal algo

            end = '\n' if algo is None else ' '
            if not only_cat_summary:
                print(f"{local_cat.pretty_cat_name}:", end=end)
            cat_cnt, cat_total = 0, 0

            for local_algo in cat_stats:
                cnt, total = cat_stats.get(local_algo)

                if algo and local_algo == algo:
                    if total != 0:
                        print(f"{algo}: {cnt}/{total} ({self.__get_percentage(cnt, total)}%)")
                    else:
                        print("\r", end="")
                    algo_cnt += cnt
                    algo_total += total
                    return
                elif algo is None:
                    if not only_cat_summary:
                        if total != 0:
                            print(f"\t{local_algo}: {cnt}/{total} ({self.__get_percentage(cnt, total)}%)")
                        else:
                            print("\r", end="")
                    cat_cnt += cnt
                    cat_total += total

            if cat_total != 0:
                print(f"{'\t' if only_cat_summary else local_cat.pretty_cat_name + ':'}"
                      f" Total: {cat_cnt}/{cat_total}"
                      f" ({self.__get_percentage(cat_cnt, cat_total)}%)")
            else:
                print("\r", end="")

        if cat:
            print_cat(cat, self.__stats[cat])
            return
        for item_cat in self.__stats.keys():
            if not pbkdf2 and item_cat.use_pbkdf2:
                continue
            if not key and not item_cat.use_pbkdf2:
                continue
            if not base64 and item_cat.base64:
                continue
            if not encrypt and item_cat.encrypt_mode:
                continue
            if not decrypt and not item_cat.encrypt_mode:
                continue
            print_cat(item_cat, self.__stats[item_cat])

        if algo and algo_total != 0:
            print(f"{algo}: Total: {algo_cnt}/{algo_total} ({self.__get_percentage(algo_cnt, algo_total)}%)")

    def print_summary(self, summary_categories: list[TestStatsSummary] | None = None):
        if summary_categories is None:
            summary_categories = list(TestStatsSummary)

        for cat in summary_categories:
            stats = self.__summary.get(cat, (0, 0))

            if isinstance(stats, tuple):
                cnt, total = stats
                if total == 0:
                    continue
                print(f"{cat}: {cnt}/{total} ({self.__get_percentage(cnt, total)}%)")
            else:
                for key in stats:
                    cnt, total = stats[key]
                    if total == 0:
                        continue
                    print(f"{cat} {key}: {cnt}/{total} ({self.__get_percentage(cnt, total)}%)")


class TestCase(ABC):
    def __init__(self, typ: str, file: str):
        self.typ = typ
        self.file = file
        self.name = os.path.basename(self.file)

    @abstractmethod
    def create(self):
        pass

    def __str__(self) -> str:
        return f"typ = {self.typ}, name = {self.name}, file = {self.file}"

    @abstractmethod
    def __repr__(self) -> str:
        pass


class DDTestCase(TestCase):
    def __init__(self, file: str, src: str, blk_size: str, cnt: int):
        super().__init__('dd', file)
        self.src = src
        self.blk_size = blk_size
        self.cnt = cnt

    def create(self):
        cmd = [
            'dd',
            f'if={self.src}',
            f'bs={self.blk_size}',
            f'count={self.cnt}',
            f'of={self.file}'
        ]

        proc = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        proc.wait()

    def __str__(self) -> str:
        res = super().__str__()

        return f"{res}, src = {self.src}, bs = {self.blk_size}, count = {self.cnt}"

    def __repr__(self) -> str:
        return f"DDTestCase({self.__str__()})"


class EchoTestCase(TestCase):
    def __init__(self, file: str, content: str):
        super().__init__('echo', file)
        self.content = content

    def create(self):
        with open(self.file, mode="w") as f:
            f.write(self.content)

    def __str__(self) -> str:
        res = super().__str__()

        return f"{res}, src = {self.content}"

    def __repr__(self) -> str:
        return f"EchoTestCase({self.__str__()})"


class CopyTestCase(TestCase):
    def __init__(self, file: str, src: str):
        super().__init__('cp', file)
        self.src = src

    def create(self):
        shutil.copy(self.src, self.file)

    def __str__(self) -> str:
        res = super().__str__()

        return f"{res}, src = {self.src}"

    def __repr__(self) -> str:
        return f"CopyTestCase({self.__str__()})"


class TestCaseStruct(TypedDict):
    name: str
    file: str


class Tester:
    OUTPUT_FOLDER = ""
    DIFF_FOLDER = ""
    __ALGO_KEY_LEN = {
        "aes-128": 16,
        "aes-192": 24,
        "aes-256": 32,

        "des": 8,
        "des-ede": 16,
        "des-ede3": 24,
    }

    def __init__(self, mine: str, sys: str, test: TestCaseStruct, use_pbkdf2: bool, encrypt_mode: bool, base64: bool):
        self.mine = mine
        self.sys = sys

        b64_part = ".base64" if base64 else ""
        mode_part = "enc" if encrypt_mode else "dec"

        self.test = test
        self.out_mine = os.path.join(Tester.OUTPUT_FOLDER, f"{self.test["name"]}.{mode_part}.{mine}{b64_part}.ft_ssl")
        self.out_sys = os.path.join(Tester.OUTPUT_FOLDER, f"{self.test["name"]}.{mode_part}.{mine}{b64_part}.openssl")
        self.diff = os.path.join(Tester.DIFF_FOLDER, f"{self.test["name"]}.{mode_part}.{mine}{b64_part}")

        self.use_pbkdf2 = use_pbkdf2
        self.encrypt_mode = encrypt_mode
        self.base64 = base64

        self.password = None
        self.salt = None
        self.key = None
        self.iv = None

        lens = self.__get_algo_lens()
        if lens is None:
            raise ValueError(f"invalid algorithm {mine}")

        key_len, blk_len, need_iv = lens

        if self.use_pbkdf2:
            self.password = Tester.__random_password()
            self.salt = Tester.__random_hex(8)
        else:
            self.key = Tester.__random_hex(key_len)
            if need_iv:
                self.iv = Tester.__random_hex(blk_len)

    @staticmethod
    def __random_password(min_len: int = 5, max_len: int = 20):
        charset = string.digits + string.ascii_letters + r"!@#$%^&*()_-=+~`'\\||?/.><,;:[]{}"

        if min_len > max_len:
            random_len = min_len
        else:
            random_len = secrets.randbelow(max_len - min_len) + min_len

        return "".join(secrets.choice(charset) for _ in range(random_len))

    @staticmethod
    def __random_hex(length: int):
        return secrets.token_hex(length)

    def __get_algo_lens(self) -> tuple[int, int, bool] | None:
        parsed = parse_algo(self.mine)
        if not parsed:
            return None
        alg, cipher_mode = parsed

        if alg not in Tester.__ALGO_KEY_LEN:
            return None

        need_iv = cipher_mode != "ecb"

        if "des" in alg:
            blk_len = 8
        elif "aes" in alg:
            blk_len = 16
        else:
            return None

        return Tester.__ALGO_KEY_LEN[alg], blk_len, need_iv

    def __construct_ft_ssl_command(self, infile: str | None = None) -> list[str]:
        cmd = ["./ft_ssl", self.mine]

        if self.encrypt_mode:
            cmd.append("-e")
        else:
            cmd.append("-d")

        if self.base64:
            cmd.append("-a")

        cmd.append("-i")
        cmd.append(infile if infile is not None else self.test["file"])
        cmd.append("-o")
        cmd.append(self.out_mine)

        if self.password:
            cmd.append("-p")
            cmd.append(self.password)
        if self.salt:
            cmd.append("-s")
            cmd.append(self.salt)
        if self.key:
            cmd.append("-k")
            cmd.append(self.key)
        if self.iv:
            cmd.append("-v")
            cmd.append(self.iv)

        return cmd

    def __construct_openssl_command(self, infile: str | None = None, outfile: str | None = None,
                                    force_enc: bool = False) -> list[str]:
        cmd = ["openssl", "enc", self.sys]

        if self.encrypt_mode or force_enc:
            cmd.append("-e")
        else:
            cmd.append("-d")

        if self.base64:
            cmd.append("-a")

        cmd.append("-in")
        cmd.append(infile if infile is not None else self.test["file"])
        cmd.append("-out")
        cmd.append(outfile if outfile is not None else self.out_sys)

        if self.password:
            cmd.append("-k")
            cmd.append(self.password)
            cmd.append("-pbkdf2")
            cmd.append("-iter")
            cmd.append("10000")
        if self.salt:
            cmd.append("-S")
            cmd.append(self.salt)
        if self.key:
            cmd.append("-K")
            cmd.append(self.key)
        if self.iv:
            cmd.append("-iv")
            cmd.append(self.iv)

        exp = re.compile(r"^-?des-(?!ede).*$")
        if exp.match(self.sys):
            cmd.append("-provider")
            cmd.append("default")
            cmd.append("-provider")
            cmd.append("legacy")

        return cmd

    def run(self) -> bool:
        if self.encrypt_mode:
            mine = self.__construct_ft_ssl_command()
            sys = self.__construct_openssl_command()
        else:
            b64_part = ".base64" if self.base64 else ""
            dec_input = os.path.join(Tester.OUTPUT_FOLDER, f"{self.test["name"]}.{self.mine}.dec_input{b64_part}")
            setup_input_cmd = self.__construct_openssl_command(outfile=dec_input, force_enc=True)

            completed_process_setup_input = subprocess.run(
                setup_input_cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                universal_newlines=True)
            if completed_process_setup_input.returncode != 0:
                from sys import stderr
                print(f"stdout = {completed_process_setup_input.stdout!r}", file=stderr)
                print(f"stderr = {completed_process_setup_input.stderr!r}", file=stderr)
                return False

            mine = self.__construct_ft_ssl_command(infile=dec_input)
            sys = self.__construct_openssl_command(infile=dec_input)

        ret = True

        completed_process_mine = subprocess.run(
            mine,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True)
        completed_process_sys = subprocess.run(
            sys,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True)

        if completed_process_mine.returncode != 0:
            print(f"Completed ft_ssl process with bad return code {completed_process_mine.returncode}"
                  f" for file {self.test["file"]}")

            from sys import stderr
            if len(completed_process_mine.stdout) != 0:
                print(f"stdout = {completed_process_mine.stdout!r}", file=stderr)
            if len(completed_process_mine.stderr) != 0:
                print(f"stderr = {completed_process_mine.stderr!r}", file=stderr)
            # ret = False

        if completed_process_sys.returncode != 0:
            print(f"Completed system process with bad return code {completed_process_sys.returncode}"
                  f" for file {self.test["file"]}")

            from sys import stderr
            if len(completed_process_sys.stdout) != 0:
                print(f"stdout = {completed_process_sys.stdout!r}", file=stderr)
            if len(completed_process_sys.stderr) != 0:
                print(f"stderr = {completed_process_sys.stderr!r}", file=stderr)
            # ret = False

        # if not ret:
        #     return False

        gen_diff = [
            "cmp",
            "-l",
            self.out_sys,
            self.out_mine,
        ]

        gawk = [
            "gawk",
            '{printf "%08X %02X %02X\\n", $1, strtonum(0$2), strtonum(0$3)}',
        ]

        with open(self.diff, mode="w+") as f:
            cmp_proc = subprocess.Popen(gen_diff, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            gawk_proc = subprocess.Popen(gawk, stdout=f, stdin=cmp_proc.stdout)
            cmp_proc.stdout.close()

            gawk_ret = gawk_proc.wait()
            cmp_proc.wait()
            _, cmp_stderr = cmp_proc.communicate()

            if gawk_ret != 0:
                print(f"bad return from gawk({gawk_ret}) commands")
                exit(1)

        file_stat = os.stat(self.diff)
        with open(self.diff, mode="w+") as f:
            if file_stat.st_size == 0:
                ret = True
            else:
                ret = False
                f.seek(0, 0)

                content = f.read()

                f.seek(0, 0)
                print(f"# diff cmd: {" ".join(gen_diff)}", file=f, end="\n")
                print(f"# OpenSSL command: {" ".join(sys)}", file=f, end="\n")
                print(f"# ft_ssl command: {" ".join(mine)}", file=f, end="\n")

                f.write(content)

                if len(cmp_stderr) > 0:
                    f.seek(0, 2)
                    f.write(cmp_stderr.decode(encoding="utf-8"))

        if ret:
            os.unlink(self.diff)
            pass

        return ret


def get_my_algorithms() -> list[str]:
    ft_ssl_prog = subprocess.Popen(
        ["./ft_ssl", "help"],
        stdout=None,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )

    _, stderr = ft_ssl_prog.communicate()
    exp = re.compile(r'Cipher commands')
    res = exp.search(stderr)
    if res is None:
        return []

    algorithms = stderr[res.end() + 1:]
    algorithms = algorithms.replace("base64", "")
    arr = algorithms.split()
    return arr


def get_sys_algorithms() -> list[str]:
    ft_ssl_prog = subprocess.Popen(
        ["openssl", "enc", "-list"],
        stdout=subprocess.PIPE,
        universal_newlines=True,
    )

    stdout, _ = ft_ssl_prog.communicate()
    stdout = " ".join(stdout.splitlines()[1:])

    return stdout.split()


def match_algorithms(mine: list[str], sys: list[str]) -> dict[str, str]:
    res = {}
    table = []

    for alg in mine:
        exp = re.compile(r"^-?" + alg + "$")
        sub = list(filter(exp.match, sys))
        if len(sub) == 1:
            found = True
            res[alg] = sub[0]
        elif len(sub) == 0:
            found = False
        else:
            print(f"CONFLICT for {alg} -> {sub!r}")
            found = False

        table.append([alg, f"{Colors.GREEN}found{Colors.CLEAR}" if found else f"{Colors.RED}not found{Colors.CLEAR}"])

    print(tabulate.tabulate(table, tablefmt='plain'))
    print()
    print(f"found {len(res)}/{len(mine)}, not associated from sys {len(sys) - len(mine)}/{len(sys)}")
    return res


def create_test_structure() -> list[TestCaseStruct]:
    tmp_folder = os.path.abspath(".tmp")
    inputs_folder = os.path.join(tmp_folder, "inputs")
    outputs_folder = os.path.join(tmp_folder, "outputs")
    diffs_folder = os.path.join(tmp_folder, "diffs")

    Tester.OUTPUT_FOLDER = outputs_folder
    Tester.DIFF_FOLDER = diffs_folder

    def mkdir(path: str):
        try:
            os.mkdir(path, 0o755)
        except FileExistsError:
            from sys import stderr
            print(f"folder {path} already exists, skipping", file=stderr)

    mkdir(tmp_folder)
    mkdir(inputs_folder)
    mkdir(outputs_folder)
    mkdir(diffs_folder)

    test_commands = [
        EchoTestCase(os.path.join(inputs_folder, "empty"), ""),
        EchoTestCase(os.path.join(inputs_folder, "small1"), "a"),
        EchoTestCase(os.path.join(inputs_folder, "small2"), "abcdef"),
        EchoTestCase(os.path.join(inputs_folder, "small3"), "0123456789"),

        DDTestCase(os.path.join(inputs_folder, "null1"), "/dev/zero", "2k", 1),
        DDTestCase(os.path.join(inputs_folder, "null2"), "/dev/zero", "2k", 5),

        DDTestCase(os.path.join(inputs_folder, "small_binary1"), "/dev/random", "1w", 1),
        DDTestCase(os.path.join(inputs_folder, "small_binary2"), "/dev/random", "1w", 2),
        DDTestCase(os.path.join(inputs_folder, "small_binary3"), "/dev/random", "1w", 3),

        DDTestCase(os.path.join(inputs_folder, "medium_binary1"), "/dev/random", "1k", 1),
        DDTestCase(os.path.join(inputs_folder, "medium_binary2"), "/dev/random", "1k", 2),
        DDTestCase(os.path.join(inputs_folder, "medium_binary3"), "/dev/random", "1k", 5),

        CopyTestCase(os.path.join(inputs_folder, "Makefile"), os.path.abspath("Makefile")),
        CopyTestCase(os.path.join(inputs_folder, ".clang-format"), os.path.abspath(".clang-format")),
        CopyTestCase(os.path.join(inputs_folder, "commands.h"), os.path.abspath(os.path.join("include", "commands.h"))),
    ]

    print()

    tests: list[TestCaseStruct] = []
    for cmd in test_commands:
        cmd.create()
        test: TestCaseStruct = {
            "file": cmd.file,
            "name": cmd.name,
        }

        tests.append(test)

    return tests


def main():
    tests = create_test_structure()

    mine = get_my_algorithms()
    sys = get_sys_algorithms()
    matched_alg = match_algorithms(mine, sys)

    categories: list[TestCategory] = []

    for (use_pbkdf2, encrypt_mode, base64) in list(itertools.product([True, False], repeat=3)):
        categories.append(TestCategory(use_pbkdf2, encrypt_mode, base64))

    stats = TestStats(categories=categories, algos=set(mine))
    print("-" * 64)

    try:
        for cat in categories:
            for alg in matched_alg.items():
                mine, sys = alg

                for test in tests:
                    tester = Tester(mine, sys, test, cat.use_pbkdf2, cat.encrypt_mode, cat.base64)
                    stats.increment(cat, mine, tester.run())

                stats.print(cat, mine)

            stats.print(cat, only_cat_summary=True)
            print("-" * 64)
    except KeyboardInterrupt:
        print("-" * 64)
        print("Interrupted...")

    stats.print_summary()


if __name__ == "__main__":
    main()
