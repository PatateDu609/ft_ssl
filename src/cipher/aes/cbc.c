#include "ft_ssl.h"
#include "utils.h"

int ft_aes128_cbc(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES128_CBC);
}

int ft_aes192_cbc(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES192_CBC);
}

int ft_aes256_cbc(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES256_CBC);
}

