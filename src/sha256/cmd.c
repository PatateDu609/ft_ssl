#include "commands.h"
#include "ft_ssl.h"

int ft_sha256(struct s_env *e)
{
	if (e->opts & SHA256_FLAG_HELP)
		return ft_help(e);
	return (0);
}
