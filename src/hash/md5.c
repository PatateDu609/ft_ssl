#include "crypto.h"
#include "usage.h"
#include "utils.h"

int ft_md5(struct s_env *e)
{
	if (e->opts & FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));

	ft_process_hash(e, "MD5", md5, md5_file, md5_descriptor);
	return 0;
}
