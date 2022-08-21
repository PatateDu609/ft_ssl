#include "commands.h"
#include "error.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static void fill_env(struct s_env *env)
{
	for (int i = 1; i < env->ac; i++)
	{
		if (strncmp(OPT_PREFIX, env->av[i], OPT_PREFIX_LEN))
		{
			env->params = env->av + i;
			return;
		}

		bool found = false;
		for (size_t j = 0; j < env->option_cnt; j++)
		{
			if (strcmp(env->av[i] + OPT_PREFIX_LEN, env->options[j]->name))
				continue;

			found = true;
			if (env->options[j]->need_value)
			{
				if (i + 1 >= env->ac)
				{
					char msg[512];
					snprintf(msg, sizeof msg,
						"option '%s' requires an argument", env->options[j]->name);
					throwe(msg, false);
				}
				env->options[j]->value = env->av[++i];

				if (!strcmp(env->options[j]->name, "o"))
					env->out_file = env->options[j]->value;
				else if (!strcmp(env->options[j]->name, "i"))
					env->in_file = env->options[j]->value;
			}
			env->opts |= env->options[j]->flag;
			break;
		}
		if (!found)
		{
			char msg[512];
			snprintf(msg, sizeof msg, "unrecognized option '%s'", env->av[i]);
			throwe(msg, false);
		}
	}
	env->params = env->av + env->ac;
}

void setup_env(int ac, char **av, struct s_env *env)
{
	memset(env, 0, sizeof(struct s_env));

	env->ac = ac;
	env->av = av;

	struct s_option ***opts = get_options(env->av[0]);
	if (!opts)
		return;
	env->options = *opts;
	env->option_cnt = get_opt_cnt(*opts);

	fill_env(env);
}
