#include "commands.h"
#include "usage.h"

#include <stdio.h>
#include <stdlib.h>

#define CMD_PER_LINE 4

static void ft_print(enum e_command_type type)
{
	char **names;
	int longest = (int)get_longest_name();

	names = get_cmd_names(type);
	if (!*names)
		fprintf(stderr, "No commands found\n");
	for (int i = 0; names[i]; i++)
	{
		for (int j = 0; j < CMD_PER_LINE && names[i]; j++, i++)
			fprintf(stderr, "%-*s", longest + 1, names[i]);
		fprintf(stderr, "\n");
		if (!names[i])
			break;
	}
	free(names);
}

int ft_help(__unused struct s_env *env)
{
	if (env)
	{
		if (env->opts & FLAG_HELP)
			return ft_usage(0, "help", env->cmd);

		if (env->ac == 2)
			return ft_usage(0, env->params[0], NULL);

		if (env->ac > 2)
		{
			fprintf(stderr, "Usage: help\n");
			return 1;
		}
	}

	fprintf(stderr, "help:\n\n");
	char *color = "\033[1;38;5;69m";
	char *clear = "\033[0m";

	fprintf(stderr, "%sStandard commands%s\n", color, clear);
	ft_print(STANDARD);

	fprintf(stderr, "\n%sDigest commands%s\n", color, clear);
	ft_print(DIGEST);

	fprintf(stderr, "\n%sCipher commands%s\n", color, clear);
	ft_print(CIPHER);

	return 0;
}
