#include "sha2.h"
#include "utils.h"
#include "libft.h"
#include "ft_stream.h"

#include <stdlib.h>
#include <unistd.h>

static uint16_t endian = __ORDER_BIG_ENDIAN__;

static uint8_t *ft_memdup(uint8_t *src, size_t len)
{
	uint8_t *dst;

	if (!(dst = malloc(len)))
		return (NULL);
	ft_memcpy(dst, src, len);
	return (dst);
}

static uint8_t *ft_memjoin(uint8_t *src1, size_t len1, uint8_t *src2, size_t len2)
{
	uint8_t *dst;

	if (!(dst = malloc(len1 + len2)))
		return (NULL);
	ft_memcpy(dst, src1, len1);
	ft_memcpy(dst + len1, src2, len2);
	free(src1);
	return (dst);
}

static void stream(int fd, struct s_sha2_ctx *ctx, struct s_env *e, char *name)
{
	ft_stream *stream = ft_sopen_fd(fd);
	struct s_msg *final = NULL;
	if (fd == 0)
		final = ft_calloc(1, sizeof *final);

	struct s_msg *msg;
	uint8_t cont;
	uint64_t i = 0;
	uint64_t max = 164773888;

	do
	{
		printf("processing block %.4f [%lu / %lu]\n", ((double)i / max) * 100, i, max);
		i++;
		msg = ft_bufferize(stream, ctx->block_size);
		struct s_blocks *blks = ft_file_padding(msg, ctx->block_size, ctx->last_block, endian);

		sha2_update(ctx, blks);
		if (final)
		{
			if (!final->data)
				final->data = ft_memdup(msg->data, msg->len);
			else
				final->data = ft_memjoin(final->data, final->len, msg->data, msg->block_size);
			final->len += msg->block_size;
		}

		cont = msg->block_size == ctx->block_size;
		free(msg->data);
		free(msg);
		msg = NULL;

		free(blks->data);
		free(blks);
		blks = NULL;
	} while (cont);

	sha2_final(ctx, final, e->opts, name ? name : "stdin");
	ft_sclose(stream);
	stream = NULL;
	if (final)
		free(final->data);
	free(final);
	final = NULL;
}

static void process_msgs(struct s_sha2_ctx *ctx, struct s_env *e, union u_input *input)
{
	if ((e->opts & SHA256_FLAG_s) && ctx->use_str)
	{
		struct s_msg msg = {.data = (uint8_t *)input->str, .len = ft_strlen(input->str)};
		msg.bits = msg.len * 8;
		struct s_blocks *blks = ft_get_blocks(&msg, ctx->block_size, ctx->last_block, endian);
		sha2_update(ctx, blks);
		sha2_final(ctx, &msg, e->opts, STRING_ARG_NAME);
		free(blks->data);
		free(blks);
	}
	else
	{
		int i = 0;
		do
		{
			int fd = ft_getfd(input->files[i]);
			stream(fd, ctx, e, input->files[i]);
			setup_consts(ctx);
			setup_iv(ctx);
			if (!input->files[i])
				break;
		} while (input->files[++i]);
	}
}

void ft_sha2(struct s_sha2_ctx *ctx, struct s_env *env)
{
	union u_input *input = ft_get_input(env, ctx->use_str);

	setup_consts(ctx);
	setup_iv(ctx);

	process_msgs(ctx, env, input);

	free(input);
}
