#include "commands.h"
#include "error.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void fill_env(struct s_env *env) {
	for (int i = 1; i < env->ac; i++) {
		if (strncmp(OPT_PREFIX, env->av[i], OPT_PREFIX_LEN) != 0) {
			env->params = env->av + i;
			return;
		}

		bool found = false;
		for (size_t j = 0; j < env->option_cnt; j++) {
			if (strcmp(env->av[i] + OPT_PREFIX_LEN, env->options[j].name) != 0)
				continue;

			found = true;
			if (env->options[j].need_value) {
				if (i + 1 >= env->ac) {
					char msg[512];
					snprintf(msg, sizeof msg, "option '%s' requires an argument", env->options[j].name);
					throwe(msg, false);
				}
				env->options[j].value = env->av[++i];

				if (env->options[j].chk)
					if (!env->options[j].chk(env->options[j].value))
						exit(1);


				if (!strcmp(env->options[j].name, "o"))
					env->out_file = env->options[j].value;
				else if (!strcmp(env->options[j].name, "i"))
					env->in_file = env->options[j].value;
			}
			env->opts |= env->options[j].flag;
			break;
		}
		if (!found) {
			char msg[512];
			snprintf(msg, sizeof msg, "unrecognized option '%s'", env->av[i]);
			throwe(msg, false);
		}
	}
	env->params = env->av + env->ac;
}

void setup_env(int ac, char **av, struct s_env *env) {
	memset(env, 0, sizeof(struct s_env));

	env->ac = ac;
	env->av = av;

	size_t                 opts_count;
	struct s_option *opts = get_options(env->av[0], &opts_count);
	if (!opts)
		return;

	env->options    = opts;
	env->option_cnt = opts_count;

	fill_env(env);
}
