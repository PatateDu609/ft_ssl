#include "sha2.h"
#include "utils.h"
#include "libft.h"
#include <stdlib.h>
#include <unistd.h>

#define USE_STR 1

static void process_msgs(struct s_sha2_ctx *ctx, struct s_env *e, union u_input *input)
{
	if ((e->opts & SHA256_FLAG_s) && USE_STR)
	{
		struct s_msg msg = {.data = (uint8_t *)input->str, .len = ft_strlen(input->str)};
		msg.bits = msg.len * 8;
		struct s_blocks *blks = ft_get_blocks(&msg, ctx->block_size, ctx->last_block, __ORDER_BIG_ENDIAN__);
		sha2_update(ctx, blks);
		sha2_final(ctx, &msg);
	}
	else
	{
		int i = 0;
		do
		{
			int fd = ft_getfd(input->files[i]);
			struct s_msg *msg = read_all(fd);
			struct s_blocks *blks = ft_get_blocks(msg, ctx->block_size, ctx->last_block, __ORDER_BIG_ENDIAN__);
			sha2_update(ctx, blks);
			sha2_final(ctx, msg);
			close(fd);
			free(msg->data);
			free(msg);
			free(blks->data);
			free(blks);

			setup_consts(ctx);
			setup_iv(ctx);
		} while (input->files[i++]);
	}
}

void ft_sha2(struct s_sha2_ctx *ctx, struct s_env *env)
{
	union u_input *input = ft_get_input(env, USE_STR);

	setup_consts(ctx);
	setup_iv(ctx);

	process_msgs(ctx, env, input);

	free(input);
}
