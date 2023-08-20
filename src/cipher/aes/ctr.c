#include "ft_ssl.h"
#include "utils.h"

int ft_aes128_ctr(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES128_CTR);
}

int ft_aes192_ctr(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES192_CTR);
}

int ft_aes256_ctr(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES256_CTR);
}

