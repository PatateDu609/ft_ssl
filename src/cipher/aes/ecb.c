#include "ft_ssl.h"
#include "utils.h"

int ft_aes128_ecb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES128_ECB);
}

int ft_aes192_ecb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES192_ECB);
}

int ft_aes256_ecb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES256_ECB);
}

