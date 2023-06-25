#include "crypto.h"
#include "ft_ssl.h"
#include "utils.h"
#include "usage.h"

int ft_sha512_256(struct s_env *e)
{
	if (e->opts & FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));

	ft_process_hash(e, "SHA512_256", sha2_512_256, sha2_512_256_file, sha2_512_256_descriptor);
	return (0);
}
