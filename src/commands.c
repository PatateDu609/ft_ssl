#include "ft_ssl.h"
#include "libft.h"
#include "error.h"

#include <stdlib.h>

// global static is initialized to 0 (bss section)
static struct s_command_data
{
	struct s_command *cmds;
	size_t cnt;
	size_t longest_name;
} data;

static void init_cmd(void) __attribute__((constructor));
static void free_cmd(void) __attribute__((destructor));

static void register_cmd(char *name, enum e_command_type type, fn_command cmd)
{
	data.cmds = realloc(data.cmds, sizeof(struct s_command) * (data.cnt + 1));
	if (!data.cmds)
		throwe("Failed to allocate memory for commands");

	data.cmds[data.cnt].name = name;
	data.longest_name = MAX(data.longest_name, ft_strlen(name));
	data.cmds[data.cnt].type = type;
	data.cmds[data.cnt].cmd = cmd;
	data.cnt++;
}

char **get_cmd_names(enum e_command_type type)
{
	size_t i;
	size_t j;
	char **names;

	names = malloc(sizeof(char *) * (data.cnt + 1));
	if (!names)
		throwe("Failed to allocate memory for command names");

	for (i = 0, j = 0; i < data.cnt; i++)
	{
		if (data.cmds[i].type == type)
		{
			names[j] = data.cmds[i].name;
			j++;
		}
	}
	names[j] = NULL;
	return names;
}

int ft_exec(int ac, char **av)
{
	size_t i;

	if (ac < 2)
		return ft_help(ac, av);

	for (i = 0; i < data.cnt; i++)
	{
		if (!ft_strcmp(av[1], data.cmds[i].name))
			return data.cmds[i].cmd(ac + 2, av + 2);
	}
	fprintf(stderr, "Invalid command '%s'; type \"help\" for a list.\n", av[1]);
	return 1;
}

size_t get_longest_name(void)
{
	return data.longest_name;
}

void init_cmd(void)
{
	register_cmd("help", STANDARD, ft_help);
}

void free_cmd(void)
{
	free(data.cmds);
	data.cmds = NULL;
	data.cnt = 0;
	data.longest_name = 0;
}
