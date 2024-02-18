#include "commands.h"
#include "error.h"
#include "ft_ssl.h"
#include "internal.h"
#include "string.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

char **get_cmd_names(enum e_command_type type) {
	size_t i;
	size_t j;
	char **names;

	names = calloc(NB_COMMANDS + 1, sizeof(char *));
	if (!names)
		throwe("Failed to allocate memory for command names", true);

	for (i = 0, j = 0; i < NB_COMMANDS; i++) {
		if (commands[i].type == type) {
			names[j++] = commands[i].name;
		}
	}
	return names;
}

size_t get_longest_name(void) {
	static size_t longest_name = 0;

	if (longest_name != 0)
		return longest_name;

	for (size_t i = 0; i < NB_COMMANDS; i++)
		longest_name = MAX(longest_name, strlen(commands[i].name));

	return longest_name;
}

struct s_option *get_options(const char *name, size_t *len) {
	for (size_t i = 0; i < NB_COMMANDS; i++) {
		if (!strcmp(commands[i].name, name) || (commands[i].alias && !strcmp(commands[i].alias, name))) {
			if (len)
				*len = commands[i].opts_count;
			return commands[i].options;
		}
	}
	return (OPT_NOT_FOUND);
}

const struct s_command *get_cmd(const char *name) {
	for (size_t i = 0; i < NB_COMMANDS; i++) {
		if (!strcmp(commands[i].name, name) || (commands[i].alias && !strcmp(commands[i].alias, name)))
			return commands + i;
	}
	return NULL;
}

char *get_opt_value(struct s_env *env, uint64_t flag) {
	size_t                 opts_count;
	const struct s_option *opts = get_options(env->cmd->name, &opts_count);

	if (!opts)
		throwe("Error while getting options", false);

	for (size_t i = 0; i < opts_count; i++) {
		if (opts[i].flag == flag)
			return opts[i].value;
	}
	return NULL;
}

bool check_path_readable(char *val) {
	struct stat res;

	if (stat(val, &res) == -1) {
		fprintf(stderr, "error: %s: %s", val, strerror(errno));
		return false;
	}

	if (access(val, R_OK))
		throwe("couldn't open file in read access", false);

	return true;
}

bool check_path_writable(char *val) {
	int fd = open(val, O_CREAT | O_TRUNC | O_WRONLY, 0600);
	if (fd == -1)
		throwe("couldn't open file in write access", true);

	close(fd);
	return true;
}
