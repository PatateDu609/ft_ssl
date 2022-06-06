#include "commands.h"
#include "ft_ssl.h"
#include "libft.h"
#include "sha2.h"
#include "utils.h"

int ft_sha256(struct s_env *e)
{
	if (e->opts & SHA256_FLAG_HELP)
		return ft_help(e);
	struct s_sha2_ctx ctx;

	ft_memset(&ctx, 0, sizeof(struct s_sha2_ctx));
	ctx.type = SHA2_TYPE_SHA256;
	ctx.nb_rounds = SHA256_NB_ROUNDS;
	ctx.block_size = SHA256_BLOCK_SIZE;
	ctx.last_block = SHA256_LAST_BLOCK_SIZE;
	ctx.hash_size = SHA256_HASH_SIZE;

	ft_sha2(&ctx, e);

	return (0);
}
