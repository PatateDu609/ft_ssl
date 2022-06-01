#ifndef UTILS_H
#define UTILS_H

#include "defines.h"
#include "structs.h"
#include "error.h"

__unused_result char *read_all(int fd);
__unused_result struct s_blocks *ft_get_blocks(char *msg, size_t block_len, size_t last);

#endif
