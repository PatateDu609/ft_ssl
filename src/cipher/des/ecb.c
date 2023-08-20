#include "ft_ssl.h"
#include "utils.h"

int ft_des_ecb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_DES_ECB);
}
