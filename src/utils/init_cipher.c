#include "commands.h"
#include "common.h"
#include "pbkdf.h"
#include "utils.h"

#include <ctype.h>
#include <malloc.h>
#include <string.h>

uint8_t *ft_str_to_hex(char *str, size_t target_len) {
	uint8_t *hex = calloc(target_len, sizeof *hex);
	if (!hex)
		throwe("Error while allocation", true);

	char   *set = "0123456789abcdef";
	uint8_t tmp = 0;

	size_t  i = 0, j = 0;
	for (; j < target_len && str[i]; i++) {
		char *found = strchr(set, tolower(str[i]));
		if (!found)
			return NULL;

		ssize_t index = found - set;
		if (i % 2 == 0)
			tmp = index;
		else
			hex[j++] = tmp * 16 + index;
	}
	if (j < target_len)
		fprintf(stderr, "hex string is too short, padding with zero bytes to length");
	return hex;
}

static uint8_t *ft_init_salt(struct s_env *e, struct s_cipher_init_ctx *ctx, const char *ukey) {
	if (e->opts & CIPHER_FLAG_salt) {
		char *usalt = get_opt_value(e, CIPHER_FLAG_salt);// User salt is ignored if decrypt mode

		if (usalt)
			return ft_str_to_hex(usalt, ctx->salt_len);
		if (ukey)
			return NULL;
	}

	if (e->opts & CIPHER_FLAG_d) {
		char *infile = e->in_file;
		FILE *in     = infile ? fopen(infile, "r") : stdin;
		if (in == NULL)
			throwe(infile, true);

		uint8_t buf[SALT_MAGIC_LEN + ctx->salt_len];// Openssl uses 16 here (MAGIC_LEN == 8 and SALT_LEN == 8)

		if (e->opts & CIPHER_FLAG_a)
			stream_base64_dec(in, buf, sizeof buf);
		else {
			size_t ret = fread(buf, sizeof *buf, sizeof buf / sizeof *buf, in);
			if (ret != sizeof buf) {
				if (ferror(in))
					throwe("error reading input file", true);
				else if (feof(in))
					throwe("error reading input file: got EOF", false);
				else
					throwe("error reading input file: unknown error", false);
			}
		}
		// File not needed anymore (since we have what we wanted)
        if (infile)
		    fclose(in);

		if (strncmp(SALT_MAGIC, (char *)buf, SALT_MAGIC_LEN) != 0)
			throwe("bad magic number", false);
		uint8_t *salt = malloc(ctx->salt_len);
		memcpy(salt, buf + SALT_MAGIC_LEN, ctx->salt_len);
		return salt;
	} else {
		ctx->write_salt = true;
		return gensalt(ctx->salt_len);
	}
}

static char *ft_init_pass(struct s_env *e, const char *ukey) {
	if (ukey)
		return NULL;

	char *upass = get_opt_value(e, CIPHER_FLAG_pass);
	if (upass)
		return strdup(upass);
	char  msg[512];
	char *mode  = e->opts & CIPHER_FLAG_d ? "decryption" : "encryption";
	char *verif = "Verifying - ";
	snprintf(msg, sizeof msg, "%senter %s %s password: ", verif, e->cmd->name, mode);

	char *first = askpass(msg + strlen(verif));
	if (!first)
		throwe("couldn't ask for new password", true);

	if (!(e->opts & CIPHER_FLAG_d)) {
		char *second = askpass(msg);
		if (!second)
			throwe("couldn't ask for new password", true);

		size_t len = strlen(first);
		if (len != strlen(second) || strncmp(first, second, len) != 0) {
			free(first);
			free(second);
			throwe("Verify failure, password mismatch", false);
		}
		free(second);
	}
	return first;
}

void ft_init_cipher(struct s_env *e, struct s_cipher_init_ctx *ctx) {
	char *ukey      = get_opt_value(e, CIPHER_FLAG_k);

	ctx->write_salt = false;

	char *pass      = ft_init_pass(e, ukey);
	ctx->salt       = ft_init_salt(e, ctx, ukey);

	ctx->iv         = NULL;
	if (!ctx->need_iv)
		ctx->iv_len = 0;

	uint8_t *keystream = NULL;
	if (!ukey) {
		struct pbkdf2_hmac_req req;

		req.algo         = HMAC_SHA2_256;
		req.dklen        = ctx->key_len + ctx->iv_len;
		req.salt         = ctx->salt;
		req.salt_len     = ctx->salt_len;
		req.iterations   = PBKDF_ITER_COUNT;
		req.password     = (uint8_t *) pass;
		req.password_len = strlen(pass);

		keystream        = pbkdf2(req);
		ctx->key         = malloc(ctx->key_len * sizeof *ctx->key);
		memcpy(ctx->key, keystream, ctx->key_len);
	} else {
		ctx->key        = ft_str_to_hex(ukey, ctx->key_len);
		ctx->write_salt = false;
	}

	if (ctx->need_iv) {
		char *uiv = get_opt_value(e, CIPHER_FLAG_v);

		if (!uiv && ukey)
			throwe("iv false", false);
		if (uiv)
			ctx->iv = ft_str_to_hex(uiv, ctx->iv_len);
		else {
			ctx->iv = malloc(ctx->iv_len * sizeof *ctx->iv);
			memcpy(ctx->iv, keystream + ctx->key_len, ctx->iv_len);
		}
	}

	free(keystream);
	free(pass);
}

struct cipher_ctx ft_init_cipher_ctx(bool is_enc, enum block_cipher cipher_type, struct s_cipher_init_ctx init_ctx) {
	struct cipher_ctx ctx;
	memset(&ctx, 0, sizeof ctx);

	ctx.algo          = setup_algo(cipher_type);

	ctx.key_len       = init_ctx.key_len;
	ctx.key           = init_ctx.key;
	ctx.iv_len        = init_ctx.iv_len;
	ctx.iv            = init_ctx.iv;

	ctx.plaintext_len = ctx.ciphertext_len = ctx.algo.blk_size;

	if (is_enc) {
		if ((ctx.plaintext = calloc(ctx.plaintext_len, sizeof *ctx.plaintext)) == NULL)
			throwe("couldn't allocate memory", true);
	} else {
		if ((ctx.ciphertext = calloc(ctx.ciphertext_len, sizeof *ctx.ciphertext)) == NULL)
			throwe("couldn't allocate memory", true);
	}

	return ctx;
}