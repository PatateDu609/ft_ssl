#ifndef UTILS_H
#define UTILS_H

#include "defines.h"
#include "structs.h"
#include "error.h"

__unused_result __deprecated struct s_msg *read_all(int fd);
__unused_result struct s_blocks *ft_get_blocks(struct s_msg *msg, size_t block_len, size_t last, uint16_t endian);
__unused_result union u_input *ft_get_input(struct s_env *env, uint8_t use_str);
__unused_result int ft_getfd(char *filename);

#endif
