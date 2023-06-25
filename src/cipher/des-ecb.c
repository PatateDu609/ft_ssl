#include "cipher.h"
#include "common.h"
#include "defines.h"
#include "ft_ssl.h"
#include "usage.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void ft_des_ecb_enc(struct s_env *e, struct s_cipher_init_ctx *ctx) {
	FILE *in = e->in_file ? fopen(e->in_file, "r+") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	if (ctx->write_salt) {
		if (e->opts & CIPHER_FLAG_a) {
			stream_base64_enc(out, (uint8_t *) SALT_MAGIC, SALT_MAGIC_LEN);
			stream_base64_enc(out, ctx->salt, ctx->salt_len);
		} else {
			fwrite(SALT_MAGIC, 1, SALT_MAGIC_LEN, out);
			fwrite(ctx->salt, 1, ctx->salt_len, out);
		}
	}

	uint64_t blk = 0;
	uint64_t key;
	memcpy(&key, ctx->key, ctx->key_len);
	key = bswap_64(key);
	size_t ret;
	bool   padded = false;
	while ((ret = fread(&blk, 1, sizeof blk, in))) {
		uint8_t *ptr = (uint8_t *) (&blk);
		if (ret != sizeof blk) {
			for (size_t i = ret; i < sizeof blk; i++) ptr[i] = sizeof blk - ret;
			padded = true;
		}

		blk          = bswap_64(blk);
		uint64_t enc = des_encrypt(blk, key);
		enc          = bswap_64(enc);

		if (e->opts & CIPHER_FLAG_a)
			stream_base64_enc(out, (uint8_t *) &enc, sizeof enc);
		else
			fwrite(&enc, 1, sizeof enc, out);

		blk = 0;
	}
	if (!padded) {
		uint64_t padding;
		memset(&padding, 0x08, sizeof padding);

		uint64_t enc = des_encrypt(padding, key);
		enc          = bswap_64(enc);
		if (e->opts & CIPHER_FLAG_a)
			stream_base64_enc(out, (uint8_t *) &enc, sizeof enc);
		else
			fwrite(&enc, 1, sizeof enc, out);
	}

	if (e->opts & CIPHER_FLAG_a) {
		stream_base64_enc_flush(out);
		fwrite("\n", 1, 1, out);
	}

	if (e->in_file)
		fclose(in);
	if (e->out_file)
		fclose(out);
}

static void ft_des_ecb_dec(struct s_env *e, struct s_cipher_init_ctx *ctx) {
	FILE *in = e->in_file ? fopen(e->in_file, "r+") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	if (!(e->opts & CIPHER_FLAG_salt)) {
		fseek(in, SALT_MAGIC_LEN + ctx->salt_len, SEEK_SET);
	}

	uint64_t key;
	memcpy(&key, ctx->key, ctx->key_len);
	key = bswap_64(key);
	uint64_t enc;
	uint64_t blk;
	size_t   ret;

	while (true) {
		if (e->opts & CIPHER_FLAG_a) {
			size_t res = stream_base64_dec(in, (uint8_t *) &enc, sizeof enc);
			if (res != 0 && res != 8)
				throwe("bad decrypt", false);
			if (res == 0)
				break;
		} else {
			ret = fread(&enc, 1, sizeof enc, in);
			if (ret != 0 && ret != 8)
				throwe("bad decrypt", false);
			else {
				size_t len = sizeof blk;
				if (ret == 0) {
					uint8_t *ptr = (uint8_t *) &blk;
					len -= ptr[7];
				}
				fwrite(&blk, 1, len, out);

				if (ret == 0)
					break;
			}
		}

		enc             = bswap_64(enc);
		blk             = des_decrypt(enc, key);
		blk             = bswap_64(blk);

		size_t true_len = 8;
		if (feof(in)) {
			uint8_t last = (blk >> 56) & 0xff;
			if (last == 0x08)
				break;

			if (0 < last && last < 9)
				true_len -= last;
		}

		fwrite(&blk, 1, true_len, out);
	}

	if (e->in_file)
		fclose(in);
	if (e->out_file)
		fclose(out);
}

int ft_des_ecb(struct s_env *e) {
	if (e->opts & FLAG_HELP)
		return ft_usage(0, e->cmd->name, e->cmd);

	struct s_cipher_init_ctx init_ctx;
	init_ctx.need_iv  = false;
	init_ctx.salt_len = 8;
	init_ctx.key_len  = 8;

	ft_init_cipher(e, &init_ctx);

	if (!init_ctx.salt) {
		init_ctx.salt_len = 0;
	}

	if (e->opts & CIPHER_FLAG_d)
		ft_des_ecb_dec(e, &init_ctx);
	else
		ft_des_ecb_enc(e, &init_ctx);
	return 0;
}
