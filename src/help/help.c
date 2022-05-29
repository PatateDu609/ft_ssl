#include "ft_ssl.h"
#include <stdio.h>
#include <stdlib.h>

#define CMD_PER_LINE 4

static void ft_print(enum e_command_type type)
{
	char **names;
	int longest = get_longest_name();

	names = get_cmd_names(type);
	if (!*names)
		printf("No commands found\n\n");
	for (int i = 0; names[i]; i++)
	{
		for (int j = 0; j < CMD_PER_LINE && names[i]; j++, i++)
			printf("%-*s\n", longest + 1, names[i]);
		printf("\n");
	}
	free(names);
}

int ft_help(__unused int ac, __unused char **av)
{
	printf("help:\n\n");

	printf("\033[1;38;5;69mStandard commands\033[0m\n");
	ft_print(STANDARD);

	printf("\033[1;38;5;69mDigest commands\033[0m\n");
	ft_print(DIGEST);

	printf("\033[1;38;5;69mCipher commands\033[0m\n");
	ft_print(CIPHER);

	return 0;
}
