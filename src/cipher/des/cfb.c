#include "ft_ssl.h"
#include "utils.h"

int ft_des_cfb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_DES_CFB);
}

int ft_des_cfb1(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_DES_CFB1);
}

int ft_des_cfb8(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_DES_CFB8);
}
