#include "error.h"
#include "commands.h"
#include "libft.h"

static void init_options(void) __constructor(2);

size_t get_opt_cnt(struct s_option **options)
{
	size_t cnt;

	cnt = 0;
	while (options[cnt])
		cnt++;
	return (cnt);
}

static void add_option(char *cmd_name, struct s_option *opt)
{
	struct s_option ***options = get_options(cmd_name);

	if (!options)
		throwe("Command not found");
	int opt_cnt = get_opt_cnt(*options);
	*options = realloc(*options, sizeof(struct s_option *) * (opt_cnt + 2));
	if (!*options)
		throwe("Failed to allocate memory for options");
	(*options)[opt_cnt] = opt;
	(*options)[opt_cnt + 1] = NULL;
}

static struct s_option *register_option(char *cmd_name, char *name, enum e_opt_type type, uint64_t flag)
{
	struct s_option *option = ft_calloc(1, sizeof(struct s_option));
	if (!option)
		throwe("Failed to allocate memory for option");

	option->name = name;
	option->type = type;
	option->flag = flag;

	add_option(cmd_name, option);
	return option;
}

static void set_desc(struct s_option *option, char *desc)
{
	option->desc = desc;
}

__unused static void set_value(struct s_option *option, fn_chk_opt_arg chk, char *val_desc)
{
	option->need_value = 1;
	option->chk = chk;
	option->val_desc = val_desc;
}

void init_options(void)
{
	struct s_option *opt = register_option("help", "help", OPT_TYPE_GENERAL, HELP_FLAG_HELP);
	set_desc(opt, "Print this summary");
}
