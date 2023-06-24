#include "commands.h"
#include "error.h"
#include "ft_ssl.h"
#include "libft.h"

#include <stdlib.h>

// global static is initialized to 0 (bss section)
static struct s_command_data {
	struct s_command *cmds;
	size_t            cnt;
	size_t            longest_name;
} data;

static void    init_cmd(void) __constructor(1);
static void    free_cmd(void) __destructor(1);

static uint8_t check_cmd_name(char *name) {
	for (size_t i = 0; i < data.cnt; i++) {
		if (!ft_strcmp(name, data.cmds[i].name))
			return 1;
		if (data.cmds[i].alias && !ft_strcmp(name, data.cmds[i].alias))
			return 1;
	}
	return 0;
}

static uint8_t register_cmd_with_alias(char *name, enum e_command_type type, fn_command cmd, char *alias_name) {
	if (check_cmd_name(name)) {
		fprintf(stderr, "Command name %s already registered\n", name);
		return (0);
	}
	if (alias_name && check_cmd_name(alias_name)) {
		fprintf(stderr, "Alias %s already registered\n", name);
		return (0);
	}

	struct s_command *tmp = realloc(data.cmds, sizeof *data.cmds * (data.cnt + 1));
	if (!tmp)
		throwe("Failed to allocate memory for commands", true);

	data.cmds                     = tmp;

	size_t alias_len              = alias_name ? ft_strlen(alias_name) : 0;
	data.longest_name             = MAX(data.longest_name, MAX(ft_strlen(name), alias_len));
	data.cmds[data.cnt].name      = name;
	data.cmds[data.cnt].alias     = alias_name;
	data.cmds[data.cnt].type      = type;
	data.cmds[data.cnt].cmd       = cmd;
	data.cmds[data.cnt].options   = ft_calloc(1, sizeof *data.cmds[data.cnt].options);
	data.cmds[data.cnt].params    = NULL;
	data.cmds[data.cnt].param_cnt = 0;
	if (!data.cmds[data.cnt].options)
		throwe("Failed to allocate memory for options", true);
	data.cnt++;

	return (1);
}

static uint8_t register_cmd(char *name, enum e_command_type type, fn_command cmd) {
	return register_cmd_with_alias(name, type, cmd, NULL);
}

static struct s_command *get_cmd_no_const(const char *name) {
	for (size_t i = 0; i < data.cnt; i++)
		if (!ft_strcmp(name, data.cmds[i].name))
			return &data.cmds[i];
	return NULL;
}

static void register_param(char *cmd, char *name, enum e_param_type type, uint8_t mul, uint8_t is_required, char *def,
                           char *desc) {
	struct s_command *cmd_data = get_cmd_no_const(cmd);
	if (!cmd_data)
		throwe("Command not found", false);

	struct s_param param;

	param.name            = name;
	param.type            = type;
	param.can_be_multiple = mul;
	param.is_required     = is_required;
	param.def             = def;
	param.description     = desc;

	size_t size           = sizeof(struct s_param) * (cmd_data->param_cnt + 1);
	cmd_data->params      = realloc(cmd_data->params, size);
	if (!cmd_data->params)
		throwe("Failed to allocate memory for params", true);

	cmd_data->params[cmd_data->param_cnt] = param;
	cmd_data->param_cnt++;
}

char **get_cmd_names(enum e_command_type type) {
	size_t i;
	size_t j;
	char **names;

	names = malloc(sizeof(char *) * (data.cnt + 1));
	if (!names)
		throwe("Failed to allocate memory for command names", true);

	for (i = 0, j = 0; i < data.cnt; i++) {
		if (data.cmds[i].type == type) {
			names[j] = data.cmds[i].name;
			j++;
		}
	}
	names[j] = NULL;
	return names;
}

bool ft_check_params(struct s_env *env) {
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

	for (i = 0; i < data.cnt; i++) {
		if (!ft_strcmp(av[0], data.cmds[i].name) || (data.cmds[i].alias && !ft_strcmp(av[0], data.cmds[i].alias))) {
			env.cmd = data.cmds + i;
			if (!ft_check_params(&env))
				return 1;
			return data.cmds[i].cmd(&env);
		}
	}
	fprintf(stderr, "Invalid command '%s'; type \"help\" for a list.\n", av[1]);
	return 1;
}

size_t get_longest_name(void) {
	return data.longest_name;
}

struct s_option ***get_options(const char *name) {
	size_t i;

	for (i = 0; i < data.cnt; i++) {
		if (!ft_strcmp(data.cmds[i].name, name) || (data.cmds[i].alias && !ft_strcmp(data.cmds[i].alias, name)))
			return &data.cmds[i].options;
	}
	return (OPT_NOT_FOUND);
}

void init_cmd(void) {
	uint8_t ret = 1;

	ret &= register_cmd("help", STANDARD, ft_help);
	register_param("help", "command", PARAM_TYPE_COMMAND, 0, 0, NULL, "Display help for command");

	ret &= register_cmd("md5", DIGEST, ft_md5);
	register_param("md5", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret &= register_cmd("sha256", DIGEST, ft_sha256);
	register_param("sha256", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret &= register_cmd("sha224", DIGEST, ft_sha224);
	register_param("sha224", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret &= register_cmd("sha512", DIGEST, ft_sha512);
	register_param("sha512", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret &= register_cmd("sha384", DIGEST, ft_sha384);
	register_param("sha384", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret &= register_cmd("sha512-224", DIGEST, ft_sha512_224);
	register_param("sha512-224", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret &= register_cmd("sha512-256", DIGEST, ft_sha512_256);
	register_param("sha512-256", "file", PARAM_TYPE_FILE, 1, 0, "stdin", "Files to digest");

	ret &= register_cmd("base64", CIPHER, ft_base64);
	ret &= register_cmd("des-ecb", CIPHER, ft_des_ecb);
	ret &= register_cmd_with_alias("des-cbc", CIPHER, ft_des_cbc, "des");

	if (!ret)
		throwe("Failed to register commands", false);
}

const struct s_command *get_cmd(const char *name) {
	return (get_cmd_no_const(name));
}

void free_cmd(void) {
	for (size_t i = 0; i < data.cnt; i++) {
		for (size_t j = 0; data.cmds[i].options[j]; j++) free(data.cmds[i].options[j]);
		free(data.cmds[i].options);
		free(data.cmds[i].params);
	}
	free(data.cmds);
	data.cmds         = NULL;
	data.cnt          = 0;
	data.longest_name = 0;
}
