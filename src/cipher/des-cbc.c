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
		if (ret != ctx->plaintext_len)
			padded = true;

		CBC_encrypt(ctx);

		if (e->opts & CIPHER_FLAG_a)
			stream_base64_enc(out, ctx->ciphertext, ctx->ciphertext_len);
		else
			fwrite(ctx->ciphertext, *ctx->ciphertext, ctx->ciphertext_len, out);

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

static void ft_des_cbc_dec(__unused struct s_env *e, __unused struct cipher_ctx *ctx) {}

static void ft_print_init_ctx(struct s_cipher_init_ctx *init_ctx) {
	if (init_ctx->salt) {
		fprintf(stderr, "salt=");
		for (size_t i = 0; i < init_ctx->salt_len; i++)
			fprintf(stderr, "%02x", init_ctx->salt[i]);
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "key=");
	for (size_t i = 0; i < init_ctx->key_len; i++)
		fprintf(stderr, "%02x", init_ctx->key[i]);
	fprintf(stderr, "\n");

	fprintf(stderr, "iv =");
	for (size_t i = 0; i < init_ctx->iv_len; i++)
		fprintf(stderr, "%02x", init_ctx->iv[i]);
	fprintf(stderr, "\n");
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
	ft_print_init_ctx(&init_ctx);

	struct cipher_ctx ctx = ft_init_cipher_ctx(!(e->opts & CIPHER_FLAG_d), BLOCK_CIPHER_DES, init_ctx);

	if (e->opts & CIPHER_FLAG_d)
		ft_des_cbc_dec(e, &ctx);
	else
		ft_des_cbc_enc(e, &init_ctx, &ctx);

	return 0;
}
