#ifndef UTILS_H
#define UTILS_H

#include "cipher.h"
#include "defines.h"
#include "error.h"
#include "ft_stream.h"
#include "structs.h"

#include <stdio.h>

__unused_result struct s_msg    *ft_bufferize(ft_stream *stream, char *filename, size_t block_size);
__unused_result char            *ft_print_sp(unsigned char *str, size_t len);

__unused_result struct s_blocks *ft_file_padding(struct s_msg *msg, size_t block_len, size_t last, uint16_t endian);
__unused_result struct s_blocks *ft_get_blocks(struct s_msg *msg, size_t block_len, size_t last, uint16_t endian);

__unused_result union u_input   *ft_get_input(struct s_env *env, uint8_t use_str);
__unused_result int              ft_getfd(char *filename);

void                             free_msg(struct s_msg **msg);
void                             free_blocks(struct s_blocks **blks);


void                             ft_process_hash(struct s_env *e,
                                                 char         *alg_pretty_name,
                                                 char *(*hash_str)(const char *),
                                                 char *(hash_filename) (const char *),
                                                 char *(hash_descriptor) (int) );

struct salted_cipher_ctx {
	struct cipher_ctx *ctx;

	uint8_t           *salt;
	uint8_t            salt_len;
	bool               write_salt;
};

struct salted_cipher_ctx *ft_init_cipher(struct s_env *e, struct cipher_ctx *ctx);
struct cipher_ctx        *ft_init_cipher_ctx(bool is_enc, enum block_cipher cipher_type);

void debug_print(FILE *out, struct salted_cipher_ctx *ctx);

int                       ft_cipher(struct s_env *e, enum block_cipher algo);

#endif
