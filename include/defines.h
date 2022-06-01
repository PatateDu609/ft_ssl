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

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define ROTATE_RIGHT(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

#endif
