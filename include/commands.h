#ifndef COMMANDS_H
#define COMMANDS_H

#include "defines.h"
#include "structs.h"

char **get_cmd_names(enum e_command_type type);
struct s_option *get_options(const char *name, size_t *opts_len);
size_t get_longest_name(void);
void setup_env(int ac, char **av, struct s_env *env);

char *get_opt_value(struct s_env *env, uint64_t flag);
const struct s_command *get_cmd(const char *name);

#endif
