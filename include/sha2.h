#ifndef SHA2_H
#define SHA2_H

#include "defines.h"
#include "structs.h"

void setup_consts(struct s_sha2_ctx *ctx);
void setup_iv(struct s_sha2_ctx *ctx);

void sha2_update(struct s_sha2_ctx *ctx, struct s_blocks *blks);
void sha2_final(struct s_sha2_ctx *ctx, const struct s_msg *msg, uint64_t opts, const char *name);

void ft_sha2(struct s_sha2_ctx *ctx, struct s_env *env);

#undef Ch
#undef Ma
#undef sum0
#undef sum1

// Operations defined in RFC 6234
#define Ch(x, y, z) ((x & y) ^ (~x & z))
#define Ma(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define BSIG0_32(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BSIG1_32(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SSIG0_32(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define SSIG1_32(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

#define BSIG0_64(x) (ROTR(x, 28) ^ ROTR(x, 34) ^ ROTR(x, 39))
#define BSIG1_64(x) (ROTR(x, 14) ^ ROTR(x, 18) ^ ROTR(x, 41))
#define SSIG0_64(x) (ROTR(x, 1) ^ ROTR(x, 8) ^ SHR(x, 7))
#define SSIG1_64(x) (ROTR(x, 19) ^ ROTR(x, 61) ^ SHR(x, 6))

#endif
