#include "commands.h"
#include "common.h"
#include "pbkdf.h"
#include "utils.h"

#include <ctype.h>
#include <string.h>

static void print_hex(FILE *out, uint8_t *data, size_t s) {
	if (data) {
		for (size_t i = 0; i < s; i++)
			fprintf(out, "%02X", data[i]);
	} else {
		fprintf(out, "<NULL>");
	}
}

void debug_print(FILE *out, struct salted_cipher_ctx *ctx) {
	fprintf(out, "key=");
	print_hex(out, ctx->ctx->key, ctx->ctx->key_len);
	fprintf(out, "\niv=");
	print_hex(out, ctx->ctx->iv, ctx->ctx->iv_len);
	fprintf(out, "\nsalt=");
	print_hex(out, ctx->salt, ctx->salt_len);
	fprintf(out, "\n");
}

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

static uint8_t *ft_init_salt(struct s_env *e, struct salted_cipher_ctx *salted_ctx, const char *ukey) {
	if (ukey)
	{
		salted_ctx->salt_len = 0;
		return NULL;
	}

	if (e->opts & CIPHER_FLAG_salt) {
		char *usalt = get_opt_value(e, CIPHER_FLAG_salt);// User salt is ignored if decrypt mode

		if (usalt)
			return ft_str_to_hex(usalt, salted_ctx->salt_len);
	}

	if (e->opts & CIPHER_FLAG_d) {
		char *infile = e->in_file;
		FILE *in     = infile ? fopen(infile, "rb") : stdin;
		if (in == NULL)
			throwe(infile, true);

		uint8_t buf[SALT_MAGIC_LEN + salted_ctx->salt_len];// Openssl uses 16 here (MAGIC_LEN == 8 and SALT_LEN == 8)

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

		if (strncmp(SALT_MAGIC, (char *) buf, SALT_MAGIC_LEN) != 0)
			throwe("bad magic number", false);
		uint8_t *salt = malloc(salted_ctx->salt_len);
		memcpy(salt, buf + SALT_MAGIC_LEN, salted_ctx->salt_len);
		return salt;
	} else {
		salted_ctx->write_salt = true;
		return gensalt(salted_ctx->salt_len);
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

static uint8_t *ft_init_initial_vector(struct s_env      *e,
                                       struct cipher_ctx *ctx,
                                       const char        *ukey,
                                       const uint8_t     *keystream,
                                       size_t             len) {
	char *uiv = get_opt_value(e, CIPHER_FLAG_v);

	if (!uiv && ukey)
		throwe("iv false", false);
	if (uiv)
		return ft_str_to_hex(uiv, len);
	else {
		uint8_t *iv = malloc(len * sizeof *iv);
		if (!iv)
			throwe("malloc", true);

		memcpy(iv, keystream + ctx->key_len, len);

		return iv;
	}
}

struct salted_cipher_ctx *ft_init_cipher(struct s_env *e, struct cipher_ctx *ctx) {
	struct salted_cipher_ctx *salted = calloc(1, sizeof *salted);
	if (!salted)
		return NULL;

	salted->ctx        = ctx;
	salted->salt_len   = SALT_LEN;
	salted->write_salt = false;

	char *ukey         = get_opt_value(e, CIPHER_FLAG_k);
	char *pass         = ft_init_pass(e, ukey);

	salted->salt       = ft_init_salt(e, salted, ukey);

	uint8_t *keystream = NULL;
	if (!ukey) {
		struct pbkdf2_hmac_req req;

		req.algo         = HMAC_SHA2_256;
		req.dklen        = ctx->key_len + ctx->iv_len + ctx->nonce_len;
		req.salt         = salted->salt;
		req.salt_len     = salted->salt_len;
		req.iterations   = PBKDF_ITER_COUNT;
		req.password     = (uint8_t *) pass;
		req.password_len = strlen(pass);

		keystream        = pbkdf2(req);
		ctx->key         = malloc(ctx->key_len * sizeof *ctx->key);
		memcpy(ctx->key, keystream, ctx->key_len);
	} else {
		ctx->key           = ft_str_to_hex(ukey, ctx->key_len);
		salted->write_salt = false;
	}

	if (ctx->iv_len)
		ctx->iv = ft_init_initial_vector(e, ctx, ukey, keystream, ctx->iv_len);
	if (ctx->nonce_len)
		ctx->nonce = ft_init_initial_vector(e, ctx, ukey, keystream, ctx->nonce_len);

	free(keystream);
	free(pass);

	return salted;
}

struct cipher_ctx *ft_init_cipher_ctx(bool is_enc, enum block_cipher cipher_type) {
	struct cipher_ctx *ctx = new_cipher_context(cipher_type);

	ctx->plaintext_len = ctx->ciphertext_len = ctx->algo.blk_size;

	if (is_enc) {
		if ((ctx->plaintext = calloc(ctx->plaintext_len, sizeof *ctx->plaintext)) == NULL)
			throwe("couldn't allocate memory", true);
	} else {
		if ((ctx->ciphertext = calloc(ctx->ciphertext_len, sizeof *ctx->ciphertext)) == NULL)
			throwe("couldn't allocate memory", true);
	}

	return ctx;
}
