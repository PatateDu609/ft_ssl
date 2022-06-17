#ifndef UTILS_H
#define UTILS_H

#include "defines.h"
#include "structs.h"
#include "error.h"
#include "ft_stream.h"

struct s_msg *ft_bufferize(ft_stream *stream, size_t block_size);
struct s_blocks *ft_file_padding(struct s_msg *msg, size_t block_len, size_t last, uint16_t endian);

__unused_result __deprecated("Use ft_bufferize instead") struct s_msg *read_all(int fd);
__unused_result struct s_blocks *ft_get_blocks(struct s_msg *msg, size_t block_len, size_t last, uint16_t endian);
__unused_result union u_input *ft_get_input(struct s_env *env, uint8_t use_str);
__unused_result int ft_getfd(char *filename);

#endif
