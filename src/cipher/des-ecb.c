#include "ft_ssl.h"
#include "common.h"
#include "cipher.h"
#include "defines.h"
#include "usage.h"
#include "commands.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static void ft_des_ecb_enc(struct s_env *e, struct s_cipher_init_ctx *init_ctx)
{
	FILE *in = e->in_file ? fopen(e->in_file, "r") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out)
	{
		fclose(in);
		throwe(e->out_file, true);
	}

	fwrite(SALT_MAGIC, 1, SALT_MAGIC_LEN, out);
	fwrite(init_ctx->salt, 1, init_ctx->salt_len, out);

	uint64_t blk = 0;
	uint64_t key;
	memcpy(&key, init_ctx->key, init_ctx->key_len);
	key = bswap_64(key);
	size_t ret;
	bool padded = false;
	while ((ret = fread(&blk, 1, sizeof blk, in)))
	{
		uint8_t *ptr = (uint8_t *)(&blk);
		if (ret != sizeof blk)
		{
			for (size_t i = ret; i < sizeof blk; i++)
				ptr[i] = sizeof blk - ret;
			padded = true;
		}

		blk = bswap_64(blk);
		uint64_t enc = des_encrypt(blk, key);
		enc = bswap_64(enc);
		fwrite(&enc, sizeof enc, 1, out);

		blk = 0;
	}
	if (!padded)
	{
		uint64_t padding;
		memset(&padding, 0x08, sizeof padding);

		uint64_t enc = des_encrypt(padding, key);
		enc = bswap_64(enc);
		fwrite(&enc, sizeof enc, 1, out);
	}

	fclose(in);
	fclose(out);
}

static void ft_des_ecb_dec(struct s_env *e, struct s_cipher_init_ctx *ctx)
{
	FILE *in = e->in_file ? fopen(e->in_file, "r") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out)
	{
		fclose(in);
		throwe(e->out_file, true);
	}

	fseek(in, SALT_MAGIC_LEN + ctx->salt_len, SEEK_SET);

	uint64_t key;
	memcpy(&key, ctx->key, ctx->key_len);
	key = bswap_64(key);
	uint64_t enc;
	uint64_t blk;
	size_t ret;
	bool loop = false;

	while (true)
	{
		ret = fread(&enc, 1, sizeof enc, in);
		if (ret != 0 && ret != 8)
			throwe("bad decrypt", false);
		if (!loop)
			loop = true;
		else
		{
			uint len = sizeof blk;
			if (ret == 0)
			{
				uint8_t *ptr = (uint8_t *)&blk;
				len -= ptr[7];
			}
			fwrite(&blk, 1, len, out);

			if (ret == 0)
				break;
		}
		enc = bswap_64(enc);
		blk = des_decrypt(enc, key);
		blk = bswap_64(blk);
	}

	fclose(in);
	fclose(out);
}

int ft_des_ecb(struct s_env *e)
{
	if (e->opts & DES_FLAG_HELP)
		return ft_usage(0, e->cmd->name, e->cmd);

	struct s_cipher_init_ctx init_ctx;
	init_ctx.need_iv = false;
	init_ctx.salt_len = 8;
	init_ctx.key_len = 8;

	ft_init_cipher(e, &init_ctx);

	if (e->opts & DES_FLAG_d)
		ft_des_ecb_dec(e, &init_ctx);
	else
		ft_des_ecb_enc(e, &init_ctx);
	return 0;
}
