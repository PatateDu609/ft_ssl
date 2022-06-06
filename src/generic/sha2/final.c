#include "sha2.h"
#include "utils.h"
#include "libft.h"

#include <stdio.h>
#include <stdlib.h>

#define SHA2_DIGEST_MAX_SIZE 129 // SHA-512: 128 + 1 for null byte

static char *get_fn_disp(enum e_sha2_type type)
{
	switch (type)
	{
	case SHA2_TYPE_SHA256:
		return "SHA256";
	case SHA2_TYPE_SHA512:
		return "SHA512";
	case SHA2_TYPE_SHA224:
		return "SHA224";
	case SHA2_TYPE_SHA384:
		return "SHA384";
	default:
		return "UNKNOWN";
	}
}

static void prepare_hash(struct s_sha2_ctx *ctx, char *hash)
{
	if (ctx->type == SHA2_TYPE_SHA256 || ctx->type == SHA2_TYPE_SHA224)
	{
		for (size_t i = 0; i < 8; i++)
			ctx->u32.st[i] = SWAP_BYTES32(ctx->u32.st[i]);
		for (size_t i = 0; i < ctx->hash_size / 2; i++)
			snprintf(hash + i * 2, SHA2_DIGEST_MAX_SIZE, "%02x", ctx->u32.hash[i]);
	}
	else
	{
		for (size_t i = 0; i < ctx->hash_size / 2; i++)
			snprintf(hash + i * 2, SHA2_DIGEST_MAX_SIZE, "%02x", ctx->u64.hash[i]);
	}
}

void sha2_final(struct s_sha2_ctx *ctx, const struct s_msg *msg, uint64_t opts, const char *name)
{
	char *fn = get_fn_disp(ctx->type);
	char hash[SHA2_DIGEST_MAX_SIZE];
	prepare_hash(ctx, hash);
	char *msg_disp = malloc(msg->len + 1);
	ft_memcpy(msg_disp, msg->data, msg->len);
	msg_disp[msg->len] = '\0';

	if (opts & SHA256_FLAG_p)
		printf("%s(\"%s\")= %s\n", fn, msg_disp, hash);
	else if (opts & SHA256_FLAG_r)
		printf("%s *%s\n", hash, name);
	else if (opts & SHA256_FLAG_q)
		printf("%s\n", hash);
	else
		printf("%s(%s)= %s\n", fn, name, hash);
	free(msg_disp);
}
