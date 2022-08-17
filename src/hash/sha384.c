#include "crypto.h"

#include "ft_ssl.h"
#include "libft.h"
#include "utils.h"
#include "usage.h"

int ft_sha384(struct s_env *e)
{
	if (e->opts & SHA384_FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));

	ft_process_hash(e, "SHA384", sha2_384, sha2_384_file, sha2_384_descriptor);
	return (0);
}
