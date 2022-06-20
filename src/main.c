#include <stdio.h>
#include <unistd.h>

#include "ft_ssl.h"
#include "libft.h"

static int ft_count(char **av)
{
	int i;

	i = 0;
	while (av[i])
		i++;
	return (i);
}

static void free_strs(char **strs)
{
	if (!strs)
		return;
	int i;

	i = 0;
	while (strs[i])
	{
		free(strs[i]);
		i++;
	}
	free(strs);
}

int main(int ac, char **av)
{
	if (ac >= 2) // Default behavior with args
		return ft_exec(ac, av);

	while (1) // Interactive mode
	{
		write(1, "ft_ssl> ", 8);
		char *line = NULL;
		int ret = get_next_line(0, &line);
		char **args = NULL;

		if (ret == -1)
		{
			free(line);
			return (1);
		}
		if (ret == 0)
		{
			free_strs(args);
			free(line);
			break;
		}

		args = ft_split(line, ' ');
		if (!args)
		{
			free(line);
			fprintf(stderr, "Error: Allocation error\n");
			return (1);
		}

		if (args[0] == NULL)
			continue;
		if (!ft_strcmp(args[0], "exit"))
			break;
		int nb = ft_count(args);
		ft_exec(nb + 1, args - 1);

		free_strs(args);
		free(line);
	}
	return 0;
}
