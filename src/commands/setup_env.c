#include "commands.h"
#include "libft.h"
#include "error.h"

static uint64_t get_active_options(int ac, char **av, int *params)
{
	char *cmd_name = av[0];
	struct s_option ***opt = get_options(cmd_name);
	if (!opt)
		throwe("Command not found");

	struct s_option **options = *opt;
	size_t opt_cnt = get_opt_cnt(options);
	uint64_t active_options = 0;

	for (int i = 1; i < ac; i++)
	{
		for (size_t j = 0; j < opt_cnt; j++)
		{
			if (ft_strncmp(OPT_PREFIX, av[i], ft_strlen(OPT_PREFIX)))
				continue;

			if (!ft_strcmp(av[i] + 1, options[j]->name))
			{
				if (options[j]->need_value)
				{
					if (i + 1 >= ac || ft_strncmp(OPT_PREFIX, av[i + 1], ft_strlen(OPT_PREFIX)))
					{
						char msg[512];
						sprintf(msg, "Option %s requires a value", options[j]->name);
						throwe(msg);
					}
					options[j]->value = av[i + 1];
					i++;
				}
				active_options |= options[j]->flag;
				break;
			}
		}
		// If we didn't find the option, then it's a param
		if (i + 1 < ac && ft_strncmp(OPT_PREFIX, av[i + 1], ft_strlen(OPT_PREFIX)))
			*params = i + 1;
		else if (i + 1 >= ac && ft_strncmp(OPT_PREFIX, av[i], ft_strlen(OPT_PREFIX)))
			*params = i;
	}
	return (active_options);
}

void setup_env(int ac, char **av, struct s_env *env)
{
	ft_memset(env, 0, sizeof(struct s_env));
	int params = 0;

	env->ac = ac;
	env->av = av;

	env->opts = get_active_options(ac, av, &params);
	env->params = av + params;
}
