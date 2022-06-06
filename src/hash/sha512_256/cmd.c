#include "commands.h"
#include "ft_ssl.h"
#include "libft.h"
#include "sha2.h"
#include "utils.h"

int ft_sha512_256(struct s_env *e)
{
	if (e->opts & SHA512_256_FLAG_HELP)
		return ft_help(e);
	struct s_sha2_ctx ctx;

	ft_memset(&ctx, 0, sizeof(struct s_sha2_ctx));
	ctx.type = SHA2_TYPE_SHA512_256;
	ctx.nb_rounds = SHA512_256_NB_ROUNDS;
	ctx.block_size = SHA512_256_BLOCK_SIZE;
	ctx.last_block = SHA512_256_LAST_BLOCK_SIZE;
	ctx.hash_size = SHA512_256_HASH_SIZE;
	ctx.use_str = 1;

	ft_sha2(&ctx, e);

	return (0);
}
