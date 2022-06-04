#ifndef UTILS_H
#define UTILS_H

#include "defines.h"
#include "structs.h"
#include "error.h"

__unused_result struct s_msg *read_all(int fd);
__unused_result struct s_blocks *ft_get_blocks(struct s_msg *msg, size_t block_len, size_t last);
__unused_result union u_input *ft_get_input(struct s_env *env, uint8_t use_str);

#endif
