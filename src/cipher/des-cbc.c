#include "cipher.h"
#include "defines.h"
#include "ft_ssl.h"
#include "usage.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

static void ft_des_cbc_enc(struct s_env *e, struct s_cipher_init_ctx *init_ctx, struct cipher_ctx *ctx) {
	FILE *in = e->in_file ? fopen(e->in_file, "r+") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	if (init_ctx->write_salt) {
		if (e->opts & CIPHER_FLAG_a) {
			stream_base64_enc(out, (uint8_t *) SALT_MAGIC, SALT_MAGIC_LEN);
			stream_base64_enc(out, init_ctx->salt, init_ctx->salt_len);
		} else {
			fwrite(SALT_MAGIC, 1, SALT_MAGIC_LEN, out);
			fwrite(init_ctx->salt, 1, init_ctx->salt_len, out);
		}
	}

	size_t ret;
	bool   padded = false;
	while ((ret = fread(ctx->plaintext, sizeof *ctx->plaintext, ctx->plaintext_len, in))) {
		if (ret != ctx->plaintext_len) {
			ctx->plaintext_len = ret;
			padded = true;
		}

		CBC_encrypt(ctx);

		if (e->opts & CIPHER_FLAG_a)
			stream_base64_enc(out, ctx->ciphertext, ctx->ciphertext_len);
		else
			fwrite(ctx->ciphertext, sizeof *ctx->ciphertext, ctx->ciphertext_len, out);

		memset(ctx->plaintext, 0, ctx->plaintext_len);
	}
	if (!padded) {
		ctx->plaintext_len = 0;
		free(ctx->plaintext);

		CBC_encrypt(ctx);
		if (e->opts & CIPHER_FLAG_a)
			stream_base64_enc(out, ctx->ciphertext, ctx->ciphertext_len);
		else
			fwrite(ctx->ciphertext, sizeof *ctx->ciphertext, ctx->ciphertext_len, out);
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

static void ft_des_cbc_dec(struct s_env *e, struct s_cipher_init_ctx *init_ctx, struct cipher_ctx *ctx) {
	FILE *in = e->in_file ? fopen(e->in_file, "r+") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	stream_base64_reset_all();

	if (!(e->opts & CIPHER_FLAG_salt)) {
		off_t off = SALT_MAGIC_LEN + init_ctx->salt_len;
		if (e->opts & CIPHER_FLAG_a)
			stream_base64_seek(in, off);
		else
			fseek(in, off, SEEK_SET);
	}

	size_t   ret;

	while (true) {
		if (e->opts & CIPHER_FLAG_a) {
			size_t res = stream_base64_dec(in, ctx->ciphertext, ctx->ciphertext_len);
			if (res != 0 && res != 8)
				throwe("bad decrypt", false);
			if (res == 0)
				break;
		} else {
			ret = fread(ctx->ciphertext, sizeof *ctx->ciphertext, ctx->ciphertext_len, in);
			if (ret != 0 && ret != 8)
				throwe("bad decrypt", false);
		}

		CBC_decrypt(ctx);

		fwrite(ctx->plaintext, sizeof *ctx->plaintext, ctx->plaintext_len, out);
	}

	if (e->in_file)
		fclose(in);
	if (e->out_file)
		fclose(out);
}

int ft_des_cbc(struct s_env *e) {
	if (e->opts & FLAG_HELP)
		return ft_usage(0, e->av[0], e->cmd);

	struct s_cipher_init_ctx init_ctx;
	init_ctx.need_iv  = true;
	init_ctx.iv_len   = 8;
	init_ctx.key_len  = 8;
	init_ctx.salt_len = 8;

	ft_init_cipher(e, &init_ctx);

	struct cipher_ctx ctx = ft_init_cipher_ctx(!(e->opts & CIPHER_FLAG_d), BLOCK_CIPHER_DES, init_ctx);

	if (e->opts & CIPHER_FLAG_d)
		ft_des_cbc_dec(e, &init_ctx, &ctx);
	else
		ft_des_cbc_enc(e, &init_ctx, &ctx);

	return 0;
}
