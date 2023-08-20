#include "ft_ssl.h"
#include "utils.h"

int ft_aes128_cfb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES128_CFB);
}

int ft_aes192_cfb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES192_CFB);
}

int ft_aes256_cfb(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES256_CFB);
}

int ft_aes128_cfb1(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES128_CFB1);
}

int ft_aes192_cfb1(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES192_CFB1);
}

int ft_aes256_cfb1(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES256_CFB1);
}

int ft_aes128_cfb8(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES128_CFB8);
}

int ft_aes192_cfb8(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES192_CFB8);
}

int ft_aes256_cfb8(struct s_env *e) {
	return ft_cipher(e, BLOCK_CIPHER_AES256_CFB8);
}

