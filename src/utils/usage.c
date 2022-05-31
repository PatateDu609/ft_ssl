#include "usage.h"
#include "commands.h"
#include "libft.h"

#include <stdio.h>
#include <stdio_ext.h>

static int get_longest(const struct s_command *cmd)
{
	static int longest = 0; // Avoid useless re-computation

	if (cmd && longest)
		longest = 0;
	else if (!longest)
	{
		for (int i = 0; cmd->options[i]; i++)
		{
			int len = ft_strlen(cmd->options[i]->name);
			if (longest < len)
				longest = len;
		}
		for (size_t i = 0; i < cmd->param_cnt; i++)
		{
			int len = ft_strlen(cmd->params[i].name);
			if (longest < len)
				longest = len;
		}
	}
	return (longest);
}

static void ft_print(struct s_option **options, enum e_opt_type type)
{
	int longest = get_longest(NULL);
	int printed = 0;

	for (int i = 0; options[i]; i++)
	{
		if (options[i]->type != type)
			continue;
		if (!printed)
			fprintf(stderr, "\n");
		fprintf(stderr, "-%-*s %s",
				longest, // takes care of - (longest is common for all opt/params)
				options[i]->name,
				options[i]->desc);
		printed = 1;
	}
	if (printed)
	{
		fprintf(stderr, "\n");
		fflush(stderr);
	}
	else
		__fpurge(stderr); // Print nothing if no options found
}

static void print_parameters(const struct s_command *cmd)
{
	int longest = get_longest(NULL);
	int printed = 0;

	fprintf(stderr, "\nParameters:");
	for (size_t i = 0; i < cmd->param_cnt; i++)
	{
		if (!printed)
			fprintf(stderr, "\n");
		char opt[512];
		if (cmd->params[i].is_required)
			opt[0] = 0;
		else if (cmd->params[i].def)
			snprintf(opt, 512, "(optional; default is %s)", cmd->params[i].def);
		else
			snprintf(opt, 512, "(optional)");
		fprintf(stderr, "%-*s %s%s",
				longest + 1,
				cmd->params[i].name,
				cmd->params[i].description,
				opt);
		printed = 1;
	}
	if (printed)
	{
		fprintf(stderr, "\n");
		fflush(stderr);
	}
	else
		__fpurge(stderr); // Print nothing if no options found
}

void ft_usage_line(const struct s_command *cmd, char *name)
{
	if (cmd->options && cmd->options[0])
		fprintf(stderr, "Usage: %s [options]", name);
	if (cmd->params)
	{
		for (size_t i = 0; i < cmd->param_cnt; i++)
		{
			char *mul = cmd->params[i].can_be_multiple ? "..." : "";
			if (cmd->params[i].is_required)
				fprintf(stderr, " %s", cmd->params[i].name);
			else
				fprintf(stderr, " [%s%s]", cmd->params[i].name, mul);
		}
	}
}

int ft_usage(char *name, const struct s_command *cmd)
{
	if (!cmd)
		cmd = get_cmd(name);
	if (!cmd)
	{
		fprintf(stderr, "ft_ssl: '%s' is an invalid command.\n", name);
		return 1;
	}
	ft_usage_line(cmd, name);
	get_longest(cmd);

	fprintf(stderr, "\n\n");

	char buf[4096];
	setvbuf(stderr, buf, _IOFBF, 4096); // Line buffering

	fprintf(stderr, "General options:");
	ft_print(cmd->options, OPT_TYPE_GENERAL);

	fprintf(stderr, "\nInput options:");
	ft_print(cmd->options, OPT_TYPE_IN);

	fprintf(stderr, "\nOutput options:");
	ft_print(cmd->options, OPT_TYPE_OUT);

	fprintf(stderr, "\nEncryption options:");
	ft_print(cmd->options, OPT_TYPE_ENC);

	fprintf(stderr, "\nSigning options:");
	ft_print(cmd->options, OPT_TYPE_SIGNING);

	fprintf(stderr, "\nRandom state options:");
	ft_print(cmd->options, OPT_TYPE_RAND);

	fprintf(stderr, "\nPVK options:");
	ft_print(cmd->options, OPT_TYPE_PVK);

	fprintf(stderr, "\nProvider options:");
	ft_print(cmd->options, OPT_TYPE_PROVIDER);

	print_parameters(cmd);

	setvbuf(stderr, NULL, _IONBF, 0); // No buffering

	get_longest(cmd); // Reset
	return 0;
}
