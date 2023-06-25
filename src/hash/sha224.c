#include "crypto.h"

#include "ft_ssl.h"
#include "utils.h"
#include "usage.h"

int ft_sha224(struct s_env *e)
{
	if (e->opts & FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));

	ft_process_hash(e, "SHA224", sha2_224, sha2_224_file, sha2_224_descriptor);
	return (0);
}
