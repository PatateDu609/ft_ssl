#include "ft_ssl.h"
#include "utils.h"

int ft_3des_ede_cbc(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_3DES_EDE2_CBC);
}