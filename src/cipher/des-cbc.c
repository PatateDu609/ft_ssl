#include "ft_ssl.h"
#include "common.h"
#include "defines.h"
#include "usage.h"


int ft_des_cbc(struct s_env *e) {
	if (e->opts & DES_FLAG_HELP)
		return ft_usage(0, e->cmd->name, e->cmd);

	return 0;
}
