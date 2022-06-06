#ifndef DEFINES_H
#define DEFINES_H

#include <stddef.h>
#include <stdint.h>

#define __unused __attribute__((unused))
#define __constructor(pri) __attribute__((constructor(pri + 100)))
#define __destructor(pri) __attribute__((destructor(pri + 100)))
#define __unused_result __attribute__((warn_unused_result))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define OPT_NOT_FOUND NULL
#define OPT_PREFIX "-"

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

// Block size
#define MD5_BLOCK_SIZE 64	 // 512 bits = 64 bytes
#define SHA256_BLOCK_SIZE 64 // 512 bits = 64 bytes

// Last block size is always 64 bytes (len of message)
#define MD5_LAST_BLOCK_SIZE (MD5_BLOCK_SIZE - 8)
#define SHA256_LAST_BLOCK_SIZE (SHA256_BLOCK_SIZE - 8)

#define SHA256_NB_ROUNDS 64
#define SHA256_HASH_SIZE 32 * 2 + 1 // 32 bytes * 2 = 64 characters + 1 for the null byte

// #define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
// #define ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

// #define ROTL64(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
// #define ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#define ROTL(x, n) (((x) << (n)) | ((x) >> (sizeof(x) * 8 - (n))))
#define ROTR(x, n) (((x) >> (n)) | ((x) << (sizeof(x) * 8 - (n))))

#define SHL(x, n) ((x) << (n))
#define SHR(x, n) ((x) >> (n))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LE_64(x) (x)
#else
#define LE_64(x) ((x) << 56 | ((x)&0xff00) << 40 | ((x)&0xff0000) << 24 | ((x)&0xff000000) << 8 | (x) >> 8)
#endif

#define SWAP_BYTES(x) (((x) >> 56) | ((x)&0xff00) << 56 | ((x)&0xff0000) << 24 | ((x)&0xff000000) << 8 | ((x) >> 8) | ((x)&0xff) << 24)

#endif
