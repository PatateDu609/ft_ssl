#include "crypto.h"

#include "ft_ssl.h"
#include "libft.h"
#include "utils.h"
#include "usage.h"

int ft_sha512_224(struct s_env *e)
{
	if (e->opts & SHA512_224_FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));

	ft_process_hash(e, "SHA512_224", sha2_512_224, sha2_512_224_file, sha2_512_224_descriptor);
	return (0);
}
