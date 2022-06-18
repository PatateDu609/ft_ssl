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

static uint16_t endian = __ORDER_LITTLE_ENDIAN__;
static size_t block_size = MD5_BLOCK_SIZE;
static size_t last = MD5_LAST_BLOCK_SIZE;

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
		printf("MD5(%s)= %s\n", input ? input : "error", hash);
	else
		printf("MD5(%s)= %s\n", name, hash);
}

static void ft_process_file(struct s_env *e, char *filename)
{
	int fd = ft_getfd(filename);
	if (fd == -1)
		return;
	ft_stream *stream = ft_sopen_fd(fd);
	if (stream == NULL)
	{
		fprintf(stderr, "ft_ssl: %s: %s\n", filename, strerror(errno));
		return;
	}

	struct s_msg *msg = NULL;
	struct s_msg *final = NULL;

	if (fd == 0)
		final = ft_calloc(1, sizeof *final);

	struct s_md5_ctx ctx;
	md5_init(&ctx);
	uint8_t cont;

	do
	{
		msg = ft_bufferize(stream, block_size);
		struct s_blocks *blks = ft_file_padding(msg, block_size, last, endian);

		md5_process(blks, &ctx);
		if (final)
		{
			if (!final->data)
				final->data = ft_memdup(msg->data, msg->len);
			else
				final->data = ft_memjoin(final->data, final->len, msg->data, msg->block_size);
			final->len += msg->block_size;
		}

		cont = msg->block_size == block_size;
		free_msg(&msg);
		free_blocks(&blks);
	} while (cont);

	ft_print_hash(&ctx, e, (char *)(final ? final->data : NULL), STRING_ARG_NAME);
	ft_sclose(stream);
}

static void ft_process_string(struct s_env *e, char *str)
{
	struct s_blocks *blks;
	struct s_msg msg;
	msg.len = ft_strlen(str);
	msg.data = (uint8_t *)str;
	msg.bits = msg.len * CHAR_BIT;

	blks = ft_get_blocks(&msg, block_size, last, endian);
	if (blks == NULL)
		throwe("ft_ssl: error: cannot get blocks");
	struct s_md5_ctx ctx;
	md5_init(&ctx);

	md5_process(blks, &ctx);
	ft_print_hash(&ctx, e, str, STRING_ARG_NAME);

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
