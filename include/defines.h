#ifndef DEFINES_H
#define DEFINES_H

#include <stddef.h>
#include <stdint.h>

#define __unused __attribute__((unused))
#define __constructor(pri) __attribute__((constructor(pri + 100)))
#define __destructor(pri) __attribute__((destructor(pri + 100)))
#define __unused_result __attribute__((warn_unused_result))
#define __deprecated __attribute__((deprecated))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define OPT_NOT_FOUND NULL
#define OPT_PREFIX "-"

#define STRING_ARG_NAME "arg"

// command_flag_opt
#define HELP_FLAG_HELP 0x1

#define MD5_FLAG_HELP 0x1
#define MD5_FLAG_p 0x2
#define MD5_FLAG_q 0x4
#define MD5_FLAG_r 0x8
#define MD5_FLAG_s 0x10

#define SHA256_FLAG_HELP 0x1
#define SHA256_FLAG_p 0x2
#define SHA256_FLAG_q 0x4
#define SHA256_FLAG_r 0x8
#define SHA256_FLAG_s 0x10

#define SHA512_FLAG_HELP 0x1
#define SHA512_FLAG_p 0x2
#define SHA512_FLAG_q 0x4
#define SHA512_FLAG_r 0x8
#define SHA512_FLAG_s 0x10

#define SHA224_FLAG_HELP 0x1
#define SHA224_FLAG_p 0x2
#define SHA224_FLAG_q 0x4
#define SHA224_FLAG_r 0x8
#define SHA224_FLAG_s 0x10

#define SHA384_FLAG_HELP 0x1
#define SHA384_FLAG_p 0x2
#define SHA384_FLAG_q 0x4
#define SHA384_FLAG_r 0x8
#define SHA384_FLAG_s 0x10

#define SHA512_224_FLAG_HELP 0x1
#define SHA512_224_FLAG_p 0x2
#define SHA512_224_FLAG_q 0x4
#define SHA512_224_FLAG_r 0x8
#define SHA512_224_FLAG_s 0x10

#define SHA512_256_FLAG_HELP 0x1
#define SHA512_256_FLAG_p 0x2
#define SHA512_256_FLAG_q 0x4
#define SHA512_256_FLAG_r 0x8
#define SHA512_256_FLAG_s 0x10

// Block size
#define MD5_BLOCK_SIZE 64	  // 512 bits = 64 bytes
#define SHA256_BLOCK_SIZE 64  // 512 bits = 64 bytes
#define SHA512_BLOCK_SIZE 128 // 1024 bits = 128 bytes

#define SHA224_BLOCK_SIZE SHA256_BLOCK_SIZE
#define SHA384_BLOCK_SIZE SHA512_BLOCK_SIZE
#define SHA512_224_BLOCK_SIZE SHA512_BLOCK_SIZE
#define SHA512_256_BLOCK_SIZE SHA512_BLOCK_SIZE

// Last block size is always 64 bytes (len of message)
#define MD5_LAST_BLOCK_SIZE (MD5_BLOCK_SIZE - 8)
#define SHA256_LAST_BLOCK_SIZE (SHA256_BLOCK_SIZE - 8)
#define SHA224_LAST_BLOCK_SIZE (SHA224_BLOCK_SIZE - 8)
#define SHA512_LAST_BLOCK_SIZE (SHA512_BLOCK_SIZE - 16)
#define SHA384_LAST_BLOCK_SIZE (SHA384_BLOCK_SIZE - 16)
#define SHA512_224_LAST_BLOCK_SIZE (SHA512_224_BLOCK_SIZE - 16)
#define SHA512_256_LAST_BLOCK_SIZE (SHA512_256_BLOCK_SIZE - 16)

#define SHA256_NB_ROUNDS 64
#define SHA256_HASH_SIZE 32 * 2 + 1 // 32 bytes * 2 = 64 characters + 1 for the null byte

#define SHA224_NB_ROUNDS SHA256_NB_ROUNDS
#define SHA224_HASH_SIZE 28 * 2 + 1 // 28 bytes * 2 = 56 characters + 1 for the null byte

#define SHA512_NB_ROUNDS 80
#define SHA512_HASH_SIZE 64 * 2 + 1 // 64 bytes * 2 = 128 characters + 1 for the null byte

#define SHA384_NB_ROUNDS SHA512_NB_ROUNDS
#define SHA384_HASH_SIZE 48 * 2 + 1 // 48 bytes * 2 = 96 characters + 1 for the null byte

#define SHA512_224_NB_ROUNDS SHA512_NB_ROUNDS
#define SHA512_224_HASH_SIZE SHA224_HASH_SIZE

#define SHA512_256_NB_ROUNDS SHA512_NB_ROUNDS
#define SHA512_256_HASH_SIZE SHA256_HASH_SIZE

// Rotate operations compatible with every word size (16/32/64 bits)
#define ROTL(x, n) (((x) << (n)) | ((x) >> (sizeof(x) * 8 - (n))))
#define ROTR(x, n) (((x) >> (n)) | ((x) << (sizeof(x) * 8 - (n))))

#define SHL(x, n) ((x) << (n))
#define SHR(x, n) ((x) >> (n))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LE_64(x) (x)
#else
#define LE_64(x) ((x) << 56 | ((x)&0xff00) << 40 | ((x)&0xff0000) << 24 | ((x)&0xff000000) << 8 | (x) >> 8)
#endif

#define SWAP_BYTES(x) (((x)&0xff00000000000000) >> 56 | \
					   ((x)&0x00ff000000000000) >> 40 | \
					   ((x)&0x0000ff0000000000) >> 24 | \
					   ((x)&0x000000ff00000000) >> 8 |  \
					   ((x)&0x00000000ff000000) << 8 |  \
					   ((x)&0x0000000000ff0000) << 24 | \
					   ((x)&0x000000000000ff00) << 40 | \
					   ((x)&0x00000000000000ff) << 56)

#define SWAP_BYTES32(x) (((x) >> 24) |           \
						 ((x)&0x00ff0000) >> 8 | \
						 ((x)&0x0000ff00) << 8 | \
						 ((x)&0x000000ff) << 24)

// No optimisation here, we need to do it in two steps (no mask for 128 bits)
#define SWAP_BYTES128(x) ((SWAP_BYTES(x) << 64) | SWAP_BYTES(x >> 64))

#endif
