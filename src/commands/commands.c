#include "commands.h"
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

static void init_cmd(void) __constructor(1);
static void free_cmd(void) __destructor(1);

static uint8_t check_cmd_name(char *name)
{
	for (size_t i = 0; i < data.cnt; i++)
		if (!ft_strcmp(name, data.cmds[i].name))
			return 1;
	return 0;
}

static uint8_t register_cmd(char *name, enum e_command_type type, fn_command cmd)
{
	if (check_cmd_name(name))
	{
		fprintf(stderr, "Command %s already registered\n", name);
		return (0);
	}

	data.cmds = realloc(data.cmds, sizeof(struct s_command) * (data.cnt + 1));
	if (!data.cmds)
		throwe("Failed to allocate memory for commands");

	data.cmds[data.cnt].name = name;
	data.longest_name = MAX(data.longest_name, ft_strlen(name));
	data.cmds[data.cnt].type = type;
	data.cmds[data.cnt].cmd = cmd;
	data.cmds[data.cnt].options = ft_calloc(1, sizeof(struct s_option *));
	data.cmds[data.cnt].params = NULL;
	data.cmds[data.cnt].param_cnt = 0;
	if (!data.cmds[data.cnt].options)
		throwe("Failed to allocate memory for options");
	data.cnt++;

	return (1);
}

static struct s_command *get_cmd_no_const(const char *name)
{
	for (size_t i = 0; i < data.cnt; i++)
		if (!ft_strcmp(name, data.cmds[i].name))
			return &data.cmds[i];
	return NULL;
}

static void register_param(char *cmd, char *name,
						   enum e_param_type type,
						   uint8_t mul, uint8_t is_required,
						   char *def, char *desc)
{
	struct s_command *cmd_data = get_cmd_no_const(cmd);
	if (!cmd_data)
		throwe("Command not found");

	struct s_param param;

	param.name = name;
	param.type = type;
	param.can_be_multiple = mul;
	param.is_required = is_required;
	param.def = def;
	param.description = desc;

	size_t size = sizeof(struct s_param) * (cmd_data->param_cnt + 1);
	cmd_data->params = realloc(cmd_data->params, size);
	if (!cmd_data->params)
		throwe("Failed to allocate memory for params");

	cmd_data->params[cmd_data->param_cnt] = param;
	cmd_data->param_cnt++;
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
	// skip program name
	ac--;
	av++;

	size_t i;
	struct s_env env;
	setup_env(ac, av, &env);

	if (ac < 1)
		return ft_help(&env);

	for (i = 0; i < data.cnt; i++)
	{
		if (!ft_strcmp(av[0], data.cmds[i].name))
		{
			env.cmd = data.cmds + i;
			return data.cmds[i].cmd(&env);
		}
	}
	fprintf(stderr, "Invalid command '%s'; type \"help\" for a list.\n", av[1]);
	return 1;
}

size_t get_longest_name(void)
{
	return data.longest_name;
}

struct s_option ***get_options(const char *name)
{
	size_t i;

	for (i = 0; i < data.cnt; i++)
	{
		if (!ft_strcmp(data.cmds[i].name, name))
			return &data.cmds[i].options;
	}
	return (OPT_NOT_FOUND);
}

void init_cmd(void)
{
	uint8_t ret = 0;

	ret |= register_cmd("help", STANDARD, ft_help);
	register_param("help", "command", PARAM_TYPE_COMMAND, 0, 0, NULL, "Display help for command");

	ret |= register_cmd("md5", DIGEST, NULL);
	register_param("md5", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret |= register_cmd("sha256", DIGEST, NULL);
	register_param("sha256", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	if (!ret)
		throwe("Failed to register commands");
}

const struct s_command *get_cmd(const char *name)
{
	return (get_cmd_no_const(name));
}

void free_cmd(void)
{
	for (size_t i = 0; i < data.cnt; i++)
	{
		for (size_t j = 0; data.cmds[i].options[j]; j++)
			free(data.cmds[i].options[j]);
		free(data.cmds[i].options);
	}

	free(data.cmds->params);
	free(data.cmds);
	data.cmds = NULL;
	data.cnt = 0;
	data.longest_name = 0;
}
