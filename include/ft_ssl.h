#ifndef FT_SSL_H
#define FT_SSL_H

#include "defines.h"
#include "structs.h"

int ft_exec(int ac, char **av);
char **get_cmd_names(enum e_command_type type);
size_t get_longest_name(void);

int ft_help(int ac, char **av);

#endif
