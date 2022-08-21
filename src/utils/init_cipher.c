#include "utils.h"
#include "common.h"
#include "commands.h"
#include "pbkdf.h"

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>

uint8_t *ft_str_to_hex(char *str, size_t target_len)
{
	uint8_t *hex = calloc(target_len, sizeof *hex);
	if (!hex)
		throwe("Error while allocation", true);

	char *set = "0123456789abcdef";
	uint8_t tmp = 0;

	size_t i = 0, j = 0;
	for (; j < target_len && str[i]; i++)
	{
		char *found = strchr(set, tolower(str[i]));
		if (!found)
			return NULL;

		int index = found - set;
		if (i % 2 == 0)
			tmp = index;
		else
			hex[j++] = tmp * 16 + index;
	}
	if (j < target_len)
		fprintf(stderr, "hex string is too short, padding with zero bytes to length");
	return hex;
}

static uint8_t *ft_init_salt(struct s_env *e, struct s_cipher_init_ctx *ctx, char *ukey)
{

	if (e->opts & FLAG_d)
	{
		char *infile = e->in_file;
		int fd = infile ? open(infile, O_RDONLY) : STDIN_FILENO;
		if (fd == -1)
		{
			char msg[512];
			snprintf(msg, sizeof msg, "Can't open %s", infile);
			throwe(msg, true);
		}
		char buf[SALT_MAGIC_LEN + ctx->salt_len]; // Openssl uses 16 here
		ssize_t ret = read(fd, buf, sizeof buf);
		close(fd); // File not needed anymore (since we have what we wanted)
		if (ret != (ssize_t)sizeof buf)
			throwe("error reading input file", ret == -1);
		if (strncmp(SALT_MAGIC, buf, SALT_MAGIC_LEN))
			throwe("bad magic number", false);
		uint8_t *salt = malloc(ctx->salt_len);
		memcpy(salt, buf + SALT_MAGIC_LEN, ctx->salt_len);
		return salt;
	}
	else
	{
		char *usalt = get_value(e, DES_FLAG_s); // User salt is ignored if decrypt mode

		if (usalt)
			return ft_str_to_hex(usalt, ctx->salt_len);
		if (ukey)
			return ft_str_to_hex("3F", ctx->salt_len);
		return get_random_bytes(ctx->salt_len);
	}
}

static char *ft_init_pass(struct s_env *e, char *ukey)
{
	if (ukey)
		return NULL;

	char *upass = get_value(e, DES_FLAG_p);
	if (upass)
		return strdup(upass);
	char msg[512];
	char *mode = e->opts & FLAG_d ? "decryption" : "encryption";
	char *verif = "Verifying - ";
	snprintf(msg, sizeof msg, "%senter %s %s password: ", verif, e->cmd->name, mode);

	char *first = askpass(msg + strlen(verif));

	if (!(e->opts & FLAG_d))
	{
		char *second = askpass(msg);

		if (strcmp(first, second))
		{
			free(first);
			free(second);
			throwe("Verify failure, password mismatch", false);
		}
		free(second);
	}
	return first;
}

void ft_init_cipher(struct s_env *e, struct s_cipher_init_ctx *ctx)
{
	char *ukey = get_value(e, DES_FLAG_k);

	ctx->salt = ft_init_salt(e, ctx, ukey);
	char *pass = ft_init_pass(e, ukey);

	ctx->iv = NULL;
	if (!ctx->need_iv)
		ctx->iv_len = 0;

	uint8_t *keystream = NULL;
	if (!ukey)
	{
		struct pbkdf2_hmac_req req;
		req.algo = HMAC_SHA2_256;
		req.dklen = ctx->key_len + ctx->iv_len;
		req.salt = ctx->salt;
		req.salt_len = ctx->salt_len;
		req.iterations = 10000;
		req.password = (uint8_t *)pass;
		req.password_len = strlen(pass);

		keystream = pbkdf2(req);
		ctx->key = malloc(ctx->key_len * sizeof *ctx->key);
		memcpy(ctx->key, keystream, ctx->key_len);
	}
	else
		ctx->key = ft_str_to_hex(ukey, ctx->key_len);

	if (ctx->need_iv)
	{
		char *uiv = get_value(e, DES_FLAG_v);

		if (!uiv && ukey)
			throwe("iv false", false);
		if (uiv)
			ctx->iv = ft_str_to_hex(uiv, ctx->iv_len);
		else
		{
			ctx->iv = malloc(ctx->iv_len * sizeof *ctx->iv);
			memcpy(ctx->iv, keystream + ctx->key_len, ctx->iv_len);
		}
	}

	free(pass);
	free(keystream);
}
