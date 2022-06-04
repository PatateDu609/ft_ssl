#ifndef MD5_H
#define MD5_H

#include "defines.h"
#include "structs.h"
#include "usage.h"

#undef F
#undef G
#undef H
#undef I

#undef FF
#undef GG
#undef HH
#undef II

// Auxiliary functions for MD5 defined in RFC 1321
#define F(B, C, D) (D ^ (B & (C ^ D)))
#define G(B, C, D) F(D, B, C)
#define H(B, C, D) (B ^ C ^ D)
#define I(B, C, D) (C ^ (B | ~D))

#define MD5_HASH_SIZE 16 * 2 + 1

void md5_init(struct s_md5_ctx *md5);
void md5_process(struct s_blocks *blks, struct s_md5_ctx *md5);

#endif
