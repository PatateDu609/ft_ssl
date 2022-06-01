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

	if (!option->val_desc)
	{
		char msg[128];
		snprintf(msg, sizeof(msg), "Value description needed for option %s", option->name);
		fprintf(stderr, "ft_ssl: %s\n", msg);
		exit(1);
	}
}

void init_options(void)
{
	struct s_option *opt = register_option("help", "help", OPT_TYPE_GENERAL, HELP_FLAG_HELP);
	set_desc(opt, "Print this summary");

	opt = register_option("md5", "help", OPT_TYPE_GENERAL, MD5_FLAG_HELP);
	set_desc(opt, "Print this summary");
	opt = register_option("md5", "p", OPT_TYPE_GENERAL, MD5_FLAG_p);
	set_desc(opt, "Echo STDIN to STDOUT and append the checksum to STDOUT");
	opt = register_option("md5", "q", OPT_TYPE_GENERAL, MD5_FLAG_q);
	set_desc(opt, "Quiet mode, only print the checksum");
	opt = register_option("md5", "r", OPT_TYPE_GENERAL, MD5_FLAG_r);
	set_desc(opt, "Reverse the format of the output");
	opt = register_option("md5", "s", OPT_TYPE_GENERAL, MD5_FLAG_s);
	set_desc(opt, "Print the sum of the given string");
	set_value(opt, NULL, "string");

	opt = register_option("sha256", "help", OPT_TYPE_GENERAL, SHA256_FLAG_HELP);
	set_desc(opt, "Print this summary");
	opt = register_option("sha256", "p", OPT_TYPE_GENERAL, SHA256_FLAG_p);
	set_desc(opt, "Echo STDIN to STDOUT and append the checksum to STDOUT");
	opt = register_option("sha256", "q", OPT_TYPE_GENERAL, SHA256_FLAG_q);
	set_desc(opt, "Quiet mode, only print the checksum");
	opt = register_option("sha256", "r", OPT_TYPE_GENERAL, SHA256_FLAG_r);
	set_desc(opt, "Reverse the format of the output");
	opt = register_option("sha256", "s", OPT_TYPE_GENERAL, SHA256_FLAG_s);
	set_desc(opt, "Print the sum of the given string");
	set_value(opt, NULL, "string");
}
