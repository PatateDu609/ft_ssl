#ifndef RELEASE

#include "defines.h"
#include "error.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool check_existence(char **names, size_t len, const char *name) {
	for (size_t i = 0; i < len; i++) {
		if (names[i] == NULL)
			break;

		size_t name_len = strlen(names[i]);

		if (name_len != strlen(name))
			continue;

		if (strncmp(names[i], name, name_len) == 0)
			return true;
	}
	return false;
}

static bool are_options_valid(const char *name, const struct s_option *opts, size_t len) {
	bool   res     = true;
	char **names   = calloc(len, sizeof *opts);
	size_t cur_idx = 0;

	for (size_t i = 0; i < len; i++) {
		const struct s_option *current = opts + i;

		if (!opts->name) {
			fprintf(stderr, "error: %s: option %ld don't have a name\n", name, i);
			res = false;
		}

		if (check_existence(names, cur_idx, current->name)) {
			fprintf(stderr, "error: %s: found option name %s twice\n", name, current->name);
			res = false;
		}
	}

	free(names);

	return res;
}

__constructor(1) static void sanitize_commands() {
	char **names   = calloc(2 * NB_COMMANDS, sizeof *commands);
	size_t cur_idx = 0;

	if (!names)
		throwe("couldn't allocate memory", true);

	bool should_exit = false;

	for (size_t i = 0; i < NB_COMMANDS; i++) {
		const struct s_command *current = commands + i;

		if (!current->name) {
			fprintf(stderr, "error: command %ld should have a name", i);
			should_exit = true;
			continue;
		}

		if (check_existence(names, cur_idx, current->name)) {
			fprintf(stderr, "error: found command name %s twice\n", current->name);
			should_exit = true;
		}

		if (current->alias && check_existence(names, cur_idx, current->alias)) {
			fprintf(stderr, "error: found command alias %s twice\n", current->alias);
			should_exit = true;
		}

		names[cur_idx++] = current->name;
		if (current->alias)
			names[cur_idx++] = current->alias;

		if (!are_options_valid(current->name, current->options, current->opts_count)) {
			should_exit = false;
		}
	}

	free(names);

	if (should_exit)
		exit(1);
}

#endif