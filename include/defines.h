#ifndef DEFINES_H
#define DEFINES_H

#include <stddef.h>
#include <stdint.h>

#define __unused           __attribute__((unused))
#define __constructor(pri) __attribute__((constructor(pri + 100)))
#define __destructor(pri)  __attribute__((destructor(pri + 100)))
#define __unused_result    __attribute__((warn_unused_result))
#define __deprecated(msg)  __attribute__((deprecated(msg)))

#define MAX(a, b)          ((a) > (b) ? (a) : (b))

#define OPT_NOT_FOUND      NULL
#define OPT_PREFIX         "-"
#define OPT_PREFIX_LEN     1

#define STRING_ARG_NAME    "arg"

#define FLAG_HELP          0x1

#ifndef COLOR
#define COLOR

#define COLOR_RED       "\033[38;5;160m"
#define COLOR_GREEN     "\033[38;5;46m"
#define COLOR_YELLOW    "\033[38;5;226m"
#define COLOR_BLUE      "\033[38;5;63m"
#define COLOR_MAGENTA   "\033[38;5;126m"
#define COLOR_CYAN      "\033[38;5;51m"
#define COLOR_RESET     "\033[0m"

#define COLOR_BOLD      "\033[1m"
#define COLOR_ITALIC    "\033[3m"
#define COLOR_UNDERLINE "\033[4m"
#define COLOR_REVERSE   "\033[7m"

#endif

#define SALT_MAGIC       "Salted__"
#define SALT_MAGIC_LEN   8

// Command flag opt for ciphering
#define PBKDF_ITER_COUNT 10000

#define CIPHER_FLAG_d    0x2  // decrypt
#define CIPHER_FLAG_e    0x4  // encrypt
#define CIPHER_FLAG_i    0x8  // input file
#define CIPHER_FLAG_o    0x10 // output file
#define CIPHER_FLAG_k    0x20 // key
#define CIPHER_FLAG_pass 0x40 // passphrase
#define CIPHER_FLAG_salt 0x80 // salt
#define CIPHER_FLAG_v    0x100// IV
#define CIPHER_FLAG_a    0x200// base64

#define BASE64_FLAG_d    CIPHER_FLAG_d
#define BASE64_FLAG_e    CIPHER_FLAG_e
#define BASE64_FLAG_i    CIPHER_FLAG_i
#define BASE64_FLAG_o    CIPHER_FLAG_o

// command_flag_opt for hashes
#define DIGEST_FLAG_p    0x2
#define DIGEST_FLAG_q    0x4
#define DIGEST_FLAG_r    0x8
#define DIGEST_FLAG_s    0x10

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LE_64(x) (x)
#else
#define LE_64(x) ((x) << 56 | ((x) &0xff00) << 40 | ((x) &0xff0000) << 24 | ((x) &0xff000000) << 8 | (x) >> 8)
#endif

#define SWAP_BYTES(x)                                                                                                  \
	(((x) &0xff00000000000000) >> 56 | ((x) &0x00ff000000000000) >> 40 | ((x) &0x0000ff0000000000) >> 24 |             \
	 ((x) &0x000000ff00000000) >> 8 | ((x) &0x00000000ff000000) << 8 | ((x) &0x0000000000ff0000) << 24 |               \
	 ((x) &0x000000000000ff00) << 40 | ((x) &0x00000000000000ff) << 56)

#define SWAP_BYTES32(x)  (((x) >> 24) | ((x) &0x00ff0000) >> 8 | ((x) &0x0000ff00) << 8 | ((x) &0x000000ff) << 24)

// No optimisation here, we need to do it in two steps (no mask for 128 bits)
#define SWAP_BYTES128(x) ((SWAP_BYTES(x) << 64) | SWAP_BYTES(x >> 64))

#endif
