#include "ft_ssl.h"
#include "utils.h"

int ft_3des_ede3_cfb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_3DES_EDE3_CFB);
}

int ft_3des_ede3_cfb1(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_3DES_EDE3_CFB1);
}

int ft_3des_ede3_cfb8(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_3DES_EDE3_CFB8);
}