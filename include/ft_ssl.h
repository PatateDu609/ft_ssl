#ifndef FT_SSL_H
#define FT_SSL_H

#include "structs.h"

int ft_exec(int ac, char **av);

int ft_help(struct s_env *env);
int ft_md5(struct s_env *env);
int ft_sha256(struct s_env *env);

#endif
