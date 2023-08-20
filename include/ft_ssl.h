#ifndef FT_SSL_H
#define FT_SSL_H

#include "structs.h"

int ft_exec(int ac, char **av);

int ft_help(struct s_env *env);

int ft_md5(struct s_env *env);
int ft_sha256(struct s_env *env);
int ft_sha224(struct s_env *env);
int ft_sha512(struct s_env *env);
int ft_sha384(struct s_env *env);
int ft_sha512_224(struct s_env *env);
int ft_sha512_256(struct s_env *env);

int ft_base64(struct s_env *env);

int ft_3des_ede3_cbc(struct s_env *e);
int ft_3des_ede3_cfb(struct s_env *e);
int ft_3des_ede3_cfb1(struct s_env *e);
int ft_3des_ede3_cfb8(struct s_env *e);
int ft_3des_ede3_ofb(struct s_env *e);

int ft_3des_ede_cbc(struct s_env *e);
int ft_3des_ede_cfb(struct s_env *e);
int ft_3des_ede_ofb(struct s_env *e);

int ft_aes128_cbc(struct s_env *e);
int ft_aes128_cfb(struct s_env *e);
int ft_aes128_cfb1(struct s_env *e);
int ft_aes128_cfb8(struct s_env *e);
int ft_aes128_ctr(struct s_env *e);
int ft_aes128_ecb(struct s_env *e);
int ft_aes128_ofb(struct s_env *e);

int ft_aes192_cbc(struct s_env *e);
int ft_aes192_cfb(struct s_env *e);
int ft_aes192_cfb1(struct s_env *e);
int ft_aes192_cfb8(struct s_env *e);
int ft_aes192_ctr(struct s_env *e);
int ft_aes192_ecb(struct s_env *e);
int ft_aes192_ofb(struct s_env *e);

int ft_aes256_cbc(struct s_env *e);
int ft_aes256_cfb(struct s_env *e);
int ft_aes256_cfb1(struct s_env *e);
int ft_aes256_cfb8(struct s_env *e);
int ft_aes256_ctr(struct s_env *e);
int ft_aes256_ecb(struct s_env *e);
int ft_aes256_ofb(struct s_env *e);

int ft_des_cbc(struct s_env *e);
int ft_des_cfb(struct s_env *e);
int ft_des_cfb1(struct s_env *e);
int ft_des_cfb8(struct s_env *e);
int ft_des_ecb(struct s_env *e);
int ft_des_ofb(struct s_env *e);


#endif
