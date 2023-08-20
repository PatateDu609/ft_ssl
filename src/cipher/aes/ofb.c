#include "ft_ssl.h"
#include "utils.h"

int ft_aes128_ofb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES128_OFB);
}

int ft_aes192_ofb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES192_OFB);
}

int ft_aes256_ofb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES256_OFB);
}

