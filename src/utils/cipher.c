#include "usage.h"
#include "utils.h"
#include <cipher.h>

void write_salt(struct s_env *e, FILE *out, struct salted_cipher_ctx *salted_ctx) {
	if (salted_ctx->write_salt) {
		if (e->opts & CIPHER_FLAG_a) {
			stream_base64_enc(out, (uint8_t *) SALT_MAGIC, SALT_MAGIC_LEN);
			stream_base64_enc(out, salted_ctx->salt, salted_ctx->salt_len);
		} else {
			fwrite(SALT_MAGIC, 1, SALT_MAGIC_LEN, out);
			fwrite(salted_ctx->salt, 1, salted_ctx->salt_len, out);
		}
	}
}

static void ft_cipher_enc(struct s_env *e, struct salted_cipher_ctx *salted_ctx, struct cipher_ctx *ctx) {
	FILE *in = e->in_file ? fopen(e->in_file, "r") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	stream_base64_reset_all();
	write_salt(e, out, salted_ctx);

	size_t ret;
	ctx->final = false;
	while ((ret = fread(ctx->plaintext, sizeof *ctx->plaintext, ctx->plaintext_len, in))) {
		if (ret != ctx->plaintext_len) {
			ctx->plaintext_len = ret;
			ctx->final         = true;
		}

		block_cipher(ctx);

		if (e->opts & CIPHER_FLAG_a)
			stream_base64_enc(out, ctx->ciphertext, ctx->ciphertext_len);
		else
			fwrite(ctx->ciphertext, sizeof *ctx->ciphertext, ctx->ciphertext_len, out);

		memset(ctx->plaintext, 0, ctx->plaintext_len);
	}
	if (ferror(in)) {
		if (e->opts & CIPHER_FLAG_a)
			stream_base64_enc_flush(out);
		throwe("couldn't read from stream", true);
	}

	if (!ctx->final) {
		//		ctx->plaintext_len = 0;
		//		free(ctx->plaintext);
		//		ctx->plaintext = NULL;

		ctx->final = true;
		block_cipher(ctx);
		if (ctx->ciphertext) {
			if (e->opts & CIPHER_FLAG_a)
				stream_base64_enc(out, ctx->ciphertext, ctx->ciphertext_len);
			else
				fwrite(ctx->ciphertext, sizeof *ctx->ciphertext, ctx->ciphertext_len, out);
		}
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

static void ft_cipher_dec(struct s_env *e, struct salted_cipher_ctx *salted_ctx, struct cipher_ctx *ctx) {
	FILE *in = e->in_file ? fopen(e->in_file, "r") : stdin;
	if (!in)
		throwe(e->in_file, true);

	FILE *out = e->out_file ? fopen(e->out_file, "w") : stdout;
	if (!out) {
		fclose(in);
		throwe(e->out_file, true);
	}

	stream_base64_reset_all();

	if (!(e->opts & CIPHER_FLAG_salt)) {
		off_t off = SALT_MAGIC_LEN + salted_ctx->salt_len;
		if (e->opts & CIPHER_FLAG_a)
			stream_base64_seek(in, off);
		else
			fseek(in, off, SEEK_SET);
	}

	size_t ret;

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

		block_decipher(ctx);

		fwrite(ctx->plaintext, sizeof *ctx->plaintext, ctx->plaintext_len, out);
	}

	if (e->in_file)
		fclose(in);
	if (e->out_file)
		fclose(out);
}

int ft_cipher(struct s_env *e, enum block_cipher algo) {
	if (e->opts & FLAG_HELP)
		return ft_usage(0, e->av[0], e->cmd);

	struct cipher_ctx        *ctx        = ft_init_cipher_ctx(e->opts & CIPHER_FLAG_e, algo);
	struct salted_cipher_ctx *salted_ctx = ft_init_cipher(e, ctx);

//	debug_print(stderr, salted_ctx);

	if (e->opts & CIPHER_FLAG_e)
		ft_cipher_enc(e, salted_ctx, ctx);
	else
		ft_cipher_dec(e, salted_ctx, ctx);

	free(ctx->plaintext);
	free(ctx->ciphertext);
	free(ctx->iv);
	free(ctx->nonce);
	free(ctx->key);
	free(ctx);

	free(salted_ctx->salt);
	free(salted_ctx);

	return 0;
}
