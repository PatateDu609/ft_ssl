#include "md5.h"
#include "ft_ssl.h"
#include "libft.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int ft_getfd(char *filename)
{
	int fd;

	if (filename == NULL)
		fd = STDIN_FILENO;
	else
		fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		char msg[512];
		snprintf(msg, 512, "ft_ssl: %s", filename);
		perror(msg);
		return -1;
	}
	return (fd);
}

static void ft_print_hash(struct s_md5_ctx *ctx, struct s_env *e, char *input, char *name)
{
	char hash[MD5_HASH_SIZE + 1];
	name = name ? name : "stdin";
	uint8_t hash_raw[16];
	ft_memset(hash_raw, 0, sizeof(hash_raw));
	ft_memcpy(hash_raw, &ctx->a, sizeof(uint32_t));
	ft_memcpy(hash_raw + sizeof(uint32_t), &ctx->b, sizeof(uint32_t));
	ft_memcpy(hash_raw + sizeof(uint32_t) * 2, &ctx->c, sizeof(uint32_t));
	ft_memcpy(hash_raw + sizeof(uint32_t) * 3, &ctx->d, sizeof(uint32_t));

	for (size_t i = 0; i < sizeof(hash_raw); i++)
		snprintf(hash + i * 2, MD5_HASH_SIZE + 1, "%02x", hash_raw[i]);

	if (e->opts & MD5_FLAG_q)
		printf("%s\n", hash);
	else if (e->opts & MD5_FLAG_r)
		printf("%s *%s\n", hash, name);
	else if (e->opts & MD5_FLAG_p)
		printf("MD5(%s)= %s\n", input, hash);
	else
		printf("MD5(%s)= %s\n", name, hash);
}

static void ft_process_file(struct s_env *e, char *filename)
{
	int fd = ft_getfd(filename);
	struct s_msg *msg = NULL;

	if (fd == -1)
	{
		fprintf(stderr, "ft_ssl: %s: %s\n", filename, strerror(errno));
		return;
	}

	struct s_md5_ctx ctx;
	md5_init(&ctx);

	msg = read_all(fd);
	struct s_blocks *blks = ft_get_blocks(msg, MD5_BLOCK_SIZE, MD5_LAST_BLOCK_SIZE);
	md5_process(blks, &ctx);
	ft_print_hash(&ctx, e, (char *)msg->data, filename);

	free(blks->data);
	free(blks);
	free(msg->data);
	free(msg);
}

static void ft_process_string(struct s_env *e, char *str)
{
	struct s_blocks *blks;
	struct s_msg msg;
	msg.len = ft_strlen(str);
	msg.data = (uint8_t *)str;
	msg.bits = msg.len * CHAR_BIT;

	blks = ft_get_blocks(&msg, MD5_BLOCK_SIZE, MD5_LAST_BLOCK_SIZE);
	if (blks == NULL)
		throwe("ft_ssl: error: cannot get blocks");
	struct s_md5_ctx ctx;
	md5_init(&ctx);

	md5_process(blks, &ctx);
	ft_print_hash(&ctx, e, str, "arg");

	free(blks->data);
	free(blks);
}

static void ft_process_all_input(struct s_env *e)
{
	int use_str = 1;
	union u_input *input = ft_get_input(e, use_str);

	if (use_str && e->opts & MD5_FLAG_s)
		ft_process_string(e, input->str);
	else if (input->files[0])
	{
		for (int i = 0; input->files[i]; i++)
			ft_process_file(e, input->files[i]);
	}
	else
		ft_process_file(e, NULL);
	free(input);
}

int ft_md5(struct s_env *e)
{
	if (e->opts & MD5_FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));
	ft_process_all_input(e);
	return 0;
}
