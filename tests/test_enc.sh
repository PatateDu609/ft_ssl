#!/usr/bin/env bash

declare -A ENC_ALGORITHM
declare -A IN_OUT_MAP

# Generates a random password with a random length using optional
# min (-m option)/max (-M option) values.
function random_password() {
  local MIN=5
  local MAX=20
  local POS_CNT=0
  local RANDSTR
  local CNUM

  local OPTIND OPTARG
  while getopts ":m:M:" opt; do
    case $opt in
    'm') MIN=$OPTARG ;;
    'M') MAX=$OPTARG ;;
    ':')
      echo "option -${OPTARG} needs an argument. Please refer to -h for help."
      return 1
      ;;
    '?')
      echo "Unknown option -${OPTARG}"
      return 1
      ;;
    esac
  done

  if ((MIN > MAX)); then
    echo "error: random_password: min=$MIN, max=$MAX, min should be less than or equal to max"
    return 1
  fi

  local CHARS=(a b c d e f g h i j k l m n o p q r s t u v w x y z
    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
    0 1 2 3 4 5 6 7 8 9 ";" ":" "." "~" "!" "@" "#" "$" "%" "^" "&" "*" "-" "+" "=" "?")
  CNUM="${#CHARS[@]}"

  ((MIN == MAX)) && MAX_LEN=$MIN || MAX_LEN=$((MIN + (RANDOM % (MAX - MIN))))
  if ((MAX_LEN <= 0)); then
    MAX_LEN=1
  fi

  RANDSTR=""
  while true; do
    if [ $POS_CNT -ge $MAX_LEN ]; then
      break
    fi
    POS_CNT=$((POS_CNT + 1))
    RANDSTR="${RANDSTR}${CHARS[${RANDOM} % ${CNUM}]}"
  done

  echo "$RANDSTR"
  return 0
}

# Runs the binaries and generates 2 output files with a common prefix which
# must be provided using the -o option. This function will add an extension to
# differentiate the output files: <prefix>.ft_ssl for the ft_ssl output and
# <prefix>.openssl for the openssl output.
#
# Usage:
# 		-h prints this help message
# 		-g prints debug information about tester
# 		-a will ask for base64 support
# 		-e will enable encryption mode
# 		-d will enable decryption mode
# 		-i <infile> will forward an input file
# 		-o <outfile> will forward an output file
# 		-p <passphrase> will forward a passphrase
# 		-s <salt> will forward a salt (in hex form)
# 		-k <key> will forward a key (in hex form)
# 		-v <iv> will forward an initial vector (in hex form)
function run_enc() {
  function usage() {
    printf "Usage:\n" 1>&2
    printf "\t-h prints this help message\n" 1>&2
    printf "\t-g prints debug information about tester\n" 1>&2
    printf "\t-a will ask for base64 support\n" 1>&2
    printf "\t-e will enable encryption mode\n" 1>&2
    printf "\t-d will enable decryption mode\n" 1>&2
    printf "\t-i <infile> will forward an input file\n" 1>&2
    printf "\t-o <outfile> will forward an output file\n" 1>&2
    printf "\t-p <passphrase> will forward a passphrase\n" 1>&2
    printf "\t-s <salt> will forward a salt (in hex form)\n" 1>&2
    printf "\t-k <key> will forward a key (in hex form)\n" 1>&2
    printf "\t-v <iv> will forward an initial vector (in hex form)\n" 1>&2
  }

  function debug() {
    printf "%s\n" "------------------------------------------------------------" 1>&2
    (
      echo "encrypt::\"${encrypt}\""
      echo "decrypt::\"${decrypt}\""
      echo "input::\"${input}\""
      echo "output::\"${output}\""
      echo "base64::\"${base64}\""
      echo "passphrase::\"${passphrase}\""

      echo "key \"${key}\""
      echo "salt::\"${salt}\""
      echo "iv::\"${iv}\""
      echo "ft_ssl command::$(construct_ft_ssl_cmd)"
      echo "openssl command::$(construct_openssl_cmd)"
    ) | column -t -s'::' 1>&2
    printf "%s\n" "------------------------------------------------------------" 1>&2
  }

  function min_checks() {
    if $encrypt && $decrypt; then
      echo "Both encrypt and decrypt are specified"
      return 1
    elif ! $encrypt && ! $decrypt; then
      echo "Neither of encrypt and decrypt are specified"
      return 1
    fi

    if [ "$input" = "" ]; then
      echo "An input file should be provided"
      return 1
    fi

    if [ "$output" = "" ]; then
      echo "An output file should be provided"
      return 1
    fi

    return 0
  }

  function construct_openssl_cmd() {
    local cmd="openssl enc"

    cmd="$cmd $openssl_algo"
    if $encrypt; then cmd="$cmd -e"; fi
    if $decrypt; then cmd="$cmd -d"; fi
    if $base64; then cmd="$cmd -a"; fi

    cmd="$cmd -in \"${input}\""
    cmd="$cmd -out \"${output}.openssl\""

    if [ "$passphrase" != "" ]; then cmd="$cmd -k \"$passphrase\""; fi
    if [ "$salt" != "" ]; then cmd="$cmd -S \"$salt\""; fi
    if [ "$key" != "" ]; then cmd="$cmd -K \"$key\""; fi
    if [ "$iv" != "" ]; then cmd="$cmd -iv \"$iv\""; fi

    cmd="$cmd -pbkdf2 -iter 10000"

    if echo "$openssl_algo" | grep -E "^[-]?des" | grep -vqe "ede"; then
      cmd="$cmd -provider default -provider legacy"
    fi

    echo "$cmd"
  }

  function construct_ft_ssl_cmd() {
    local cmd="$MY_COMMAND"

    cmd="$cmd $my_algo"
    if $encrypt; then cmd="$cmd -e"; fi
    if $decrypt; then cmd="$cmd -d"; fi
    if $base64; then cmd="$cmd -a"; fi

    cmd="$cmd -i \"${input}\""
    cmd="$cmd -o \"${output}.ft_ssl\""

    if [ "$passphrase" != "" ]; then cmd="$cmd -p \"$passphrase\""; fi
    if [ "$salt" != "" ]; then cmd="$cmd -s \"$salt\""; fi
    if [ "$key" != "" ]; then cmd="$cmd -k \"$key\""; fi
    if [ "$iv" != "" ]; then cmd="$cmd -v \"$iv\""; fi

    echo "$cmd"
  }

  local my_algo
  local openssl_algo

  local print_debug=false
  local encrypt=false
  local decrypt=false
  local base64=false
  local input=""
  local output=""

  local passphrase=""
  local key=""
  local salt=""
  local iv=""

  my_algo=$1
  openssl_algo=${ENC_ALGORITHM[$my_algo]}
  shift

  local OPTIND OPTARG
  while getopts ":hadi:o:p:s:k:v:eg" opt; do
    case $opt in
    'a') base64=true ;;
    'e') encrypt=true ;;
    'd') decrypt=true ;;
    'i') input="${OPTARG}" ;;
    'o') output="${OPTARG}" ;;
    'p') passphrase="${OPTARG}" ;;
    's') salt="${OPTARG}" ;;
    'k') key="${OPTARG}" ;;
    'v') iv="${OPTARG}" ;;
    'g') print_debug=true ;;
    ':')
      echo "option -${OPTARG} needs an argument. Please refer to -h for help."
      return 1
      ;;
    'h')
      usage
      return 0
      ;;
    '?')
      usage
      return 1
      ;;
    esac
  done

  if $print_debug; then
    debug
  fi

  if ! min_checks; then
    return 2
  fi

  echo -ne "[\033[32;1m$my_algo\033[0m]\t"
  echo "Testing '$input'"
  eval " $(construct_openssl_cmd)"

  if ! eval " $(construct_ft_ssl_cmd)"; then
    exit 1
  fi

  return 0
}

function test_enc() {
  local mode="key"
  local alg security_args

  local OPTARG OPTIND
  while getopts ":pka:" opt; do
    case $opt in
    'p') mode="pass" ;;
    'k') mode="key" ;;
    'a') alg="$OPTARG" ;;
    ':')
      echo "option -${OPTARG} requires an argument"
      return 1
      ;;
    '?')
      echo "unknown option -${OPTARG}"
      return 1
      ;;
    esac
  done

  if [[ "$mode" = "key" ]]; then
    security_args="-k $(head -c 16 /dev/random | hexdump -e '/1 "%02X"')"

    if ! echo "$alg" | grep -q 'ecb'; then
      security_args="$security_args -v $(head -c 8 /dev/random | hexdump -e '/1 "%02X"')"
    fi
  else
    security_args="-p $(random_password -m 16 -M 32)"
    security_args="$security_args -s $(head -c 16 /dev/random | hexdump -e '/1 "%02X"')"
  fi

  for input in "${!IN_OUT_MAP[@]}"; do
    # shellcheck disable=SC2086
    if ! run_enc "$alg" -g -e $security_args -i "${input}" -o "${IN_OUT_MAP[$input]}.enc"; then
      echo "error: $alg: $input: couldn't run alg on input file" 1>&2
      exit 1
    fi
    #		# shellcheck disable=SC2086
    #		if ! run_enc "$alg" -d $security_args -i "${input}" -o "${IN_OUT_MAP[$input]}.dec"; then
    #			echo "error: $alg: $input: couldn't run alg on input file" 1>&2
    #			exit 1
    #		fi
    #		# shellcheck disable=SC2086
    #		if ! run_enc "$alg" -e -a $security_args -i "${input}" -o "${IN_OUT_MAP[$input]}.enc.base64"; then
    #			echo "error: $alg: $input: couldn't run alg on input file" 1>&2
    #			exit 1
    #		fi
    #		# shellcheck disable=SC2086
    #		if ! run_enc "$alg" -d -a $security_args -i "${input}" -o "${IN_OUT_MAP[$input]}.dec.base64"; then
    #			echo "error: $alg: $input: couldn't run alg on input file" 1>&2
    #			exit 1
    #		fi

    cmp -l "${IN_OUT_MAP[$input]}.enc.openssl" "${IN_OUT_MAP[$input]}.enc.ft_ssl" | gawk '{printf "%08X %02X %02X\n", $1, strtonum(0$2), strtonum(0$3)}' >"${input//inputs/diff}.enc.diff"
    #		diff "${IN_OUT_MAP[$input]}.dec."{openssl,ft_ssl}
    #		diff "${IN_OUT_MAP[$input]}.enc.base64."{openssl,ft_ssl}
    #		diff "${IN_OUT_MAP[$input]}.dec.base64."{openssl,ft_ssl}
  done
}

CIPHER_COMMANDS="$(./ft_ssl help 2>&1 |
  sed -n '/Cipher commands/,$p' |
  grep -v -e "Cipher commands" |
  sed -E "s/base64//g" |
  tr '\n' ' ' |
  sed -E 's/ +/ /g' |
  sed -E 's/ $//g')"
OPENSSL_COMMANDS="$(openssl enc -list |
  tail -n +2 |
  tr '\n' ' ' |
  sed -E 's/ +/ /g' |
  sed -E 's/ $//g')"

output=""
found=0
cmds=0
for cmd in $CIPHER_COMMANDS; do
  output="$output$cmd"

  cmds="$(echo "$cmds + 1" | bc)"
  arg="$(echo "$OPENSSL_COMMANDS" | grep -ow -E "[-]?$cmd")"
  if [[ $arg != "" ]]; then
    output="$output \033[32mfound\033[0m\n"
    found="$(echo "$found + 1" | bc)"
    ENC_ALGORITHM[$cmd]="$arg"
  else
    output="$output \033[31mnot found\033[0m\n"
  fi
done

printf "%b" "$output" | column -t
echo "Found $found/$cmds"
printf "\n\n"

rm -rf "$PWD"/.tmp
mkdir -p "$PWD"/.tmp/{inputs,outputs,diff}

# Generate inputs

echo -n "" >"$PWD"/.tmp/inputs/empty

echo -n "a" >"$PWD"/.tmp/inputs/small1
echo -n "abcdef" >."$PWD"/.tmp/inputs/small2
echo -n "0123456789" >"$PWD"/.tmp/inputs/small3

(
  dd if=/dev/zero bs=2k count=1 of="$PWD"/.tmp/inputs/null1
  dd if=/dev/zero bs=2k count=5 of="$PWD"/.tmp/inputs/null2

  dd if=/dev/random bs=1w count=1 of="$PWD"/.tmp/inputs/small_binary1
  dd if=/dev/random bs=1w count=2 of="$PWD"/.tmp/inputs/small_binary2
  dd if=/dev/random bs=1w count=3 of="$PWD"/.tmp/inputs/small_binary3

  dd if=/dev/random bs=1k count=1 of="$PWD"/.tmp/inputs/medium_binary1
  dd if=/dev/random bs=1k count=2 of="$PWD"/.tmp/inputs/medium_binary2
  dd if=/dev/random bs=1k count=5 of="$PWD"/.tmp/inputs/medium_binary3
) >/dev/null 2>/dev/null

cp Makefile "$PWD"/.tmp/inputs/Makefile
cp .clang-format "$PWD"/.tmp/inputs/.clang-format
cp include/commands.h "$PWD"/.tmp/inputs/commands.h

# Generate output dict

for file in "$PWD"/.tmp/inputs/*; do
  IN_OUT_MAP["$file"]="${file//inputs/outputs}"
done

test_enc -a "des-ecb"

#for alg in "${!ENC_ALGORITHM[@]}"; do
#	test_enc -a "$alg" -p
#	test_enc -a "$alg" -k
#done

#rm -rf .tmp
