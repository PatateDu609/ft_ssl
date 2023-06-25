#ifndef FT_SSL_INTERNAL_H
#define FT_SSL_INTERNAL_H

#include "structs.h"

#define NB_COMMANDS 11

extern const struct s_command commands[NB_COMMANDS];

bool check_path_readable(char *val);
bool check_path_writable(char *val);

#endif //FT_SSL_INTERNAL_H
