#include "commands.h"
#include "ft_ssl.h"
#include "internal.h"

#include <stdio.h>
#include <string.h>

static bool ft_check_params(struct s_env *env) {
	if (env->params[0] && !env->cmd->param_cnt) {
		fprintf(stderr, "Extra arguments given.\n");
		fprintf(stderr, "%s: Use -help for summary.\n", env->cmd->name);
		return false;
	}

	for (size_t i = 0; i < env->cmd->param_cnt; i++) {
		if (env->cmd->params[i].is_required && !env->params[i]) {
			fprintf(stderr, "Missing required argument: %s\n", env->cmd->params[i].name);
			return false;
		}
	}
	return true;
}

int ft_exec(int ac, char **av) {
	// skip program name
	ac--;
	av++;

	size_t       i;
	struct s_env env;

	if (ac < 1)
		return ft_help(NULL);

	setup_env(ac, av, &env);
	if (env.opts == (uint64_t) -1)
		return ft_help(NULL);

	for (i = 0; i < NB_COMMANDS; i++) {
		if (!strcmp(av[0], commands[i].name) || (commands[i].alias && !strcmp(av[0], commands[i].alias))) {
			env.cmd = commands + i;
			if (!ft_check_params(&env))
				return 1;
			return commands[i].cmd(&env);
		}
	}
	fprintf(stderr, "ft_ssl: Error: '%s' is an invalid command.\n", av[0]);
	return ft_help(&env);
}