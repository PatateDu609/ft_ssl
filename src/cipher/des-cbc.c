#include "ft_ssl.h"
#include "defines.h"
#include "usage.h"
#include "utils.h"


int ft_des_cbc(struct s_env *e) {
	if (e->opts & DES_FLAG_HELP)
		return ft_usage(0, e->av[0], e->cmd);

	struct s_cipher_init_ctx init_ctx;
	init_ctx.need_iv = true;
	init_ctx.iv_len = 8;
	init_ctx.key_len = 8;
	init_ctx.salt_len = 8;

	ft_init_cipher(e, &init_ctx);

	return 0;
}
