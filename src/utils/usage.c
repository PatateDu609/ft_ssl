#include "usage.h"
#include "commands.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#if __has_include(<stdio_ext.h>)
#include <stdio_ext.h>
#define HAS_STDIO_EXT
#endif

static size_t get_longest(const struct s_command *cmd) {
	static size_t longest = 0;// Avoid useless re-computation

	if (cmd && longest)
		longest = 0;
	else if (!longest) {
		for (size_t i = 0; i < cmd->opts_count; i++) {
			size_t len = strlen(cmd->options[i].name) + OPT_PREFIX_LEN;
			if (cmd->options[i].need_value)
				len += strlen(cmd->options[i].val_desc) + 2;// 2 spaces
			if (longest < len)
				longest = len;
		}
		for (size_t i = 0; i < cmd->param_cnt; i++) {
			size_t len = strlen(cmd->params[i].name);
			if (longest < len)
				longest = len;
		}
	}
	return (longest);
}

static void ft_print(const struct s_option *options, size_t opts_len, enum e_opt_type type) {
	size_t longest = get_longest(NULL);
	int    printed = 0;

	for (size_t i = 0; i < opts_len; i++) {
		if (options[i].type != type)
			continue;
		if (!printed)
			fprintf(stderr, "\n");

		if (options[i].need_value) {
			size_t len = longest - strlen(options[i].name) - 1;
			fprintf(stderr,
			        "%s-%s%s  %s%-*s%s",
			        isatty(2) ? COLOR_RED COLOR_BOLD: "",
			        options[i].name,
			        isatty(2) ? COLOR_RESET : "",
			        isatty(2) ? COLOR_BLUE : "",
			        (int) len,
			        options[i].val_desc,
			        isatty(2) ? COLOR_RESET : "");
		} else
			fprintf(stderr, "%s-%-*s%s", COLOR_RED COLOR_BOLD, (int) longest + 1, options[i].name, COLOR_RESET);

		fprintf(stderr, " %s\n", options[i].desc);
		printed = 1;
	}
	if (printed) {
		fprintf(stderr, "\n");
		fflush(stderr);
	} else {
#ifdef HAS_STDIO_EXT
		__fpurge(stderr);// Print nothing if no options found
#else
		fpurge(stderr);// Print nothing if no options found
#endif
	}
}

static void print_parameters(const struct s_command *cmd) {
	size_t longest = get_longest(NULL);
	int    printed = 0;

	fprintf(stderr, "\nParameters:");
	for (size_t i = 0; i < cmd->param_cnt; i++) {
		if (!printed)
			fprintf(stderr, "\n");
		char opt[512];
		if (cmd->params[i].is_required)
			opt[0] = 0;
		else if (cmd->params[i].def)
			snprintf(opt, 512, " (optional; default is %s)", cmd->params[i].def);
		else
			snprintf(opt, 512, " (optional)");
		fprintf(stderr, "%-*s %s%s", (int) longest + 2, cmd->params[i].name, cmd->params[i].description, opt);
		printed = 1;
	}
	if (printed) {
		fprintf(stderr, "\n");
		fflush(stderr);
	} else {
#ifdef HAS_STDIO_EXT
		__fpurge(stderr);// Print nothing if no options found
#else
		fpurge(stderr);// Print nothing if no options found
#endif
	}
}

void ft_usage_line(const struct s_command *cmd, char *name) {
	fprintf(stderr, "Usage: %s", name);
	if (cmd->options && cmd->opts_count)
		fprintf(stderr, " [options]");
	if (cmd->params) {
		for (size_t i = 0; i < cmd->param_cnt; i++) {
			char *mul = cmd->params[i].can_be_multiple ? "..." : "";
			if (cmd->params[i].is_required)
				fprintf(stderr, " %s", cmd->params[i].name);
			else
				fprintf(stderr, " [%s%s]", cmd->params[i].name, mul);
		}
	}
}

int ft_usage(int err, char *name, const struct s_command *cmd) {
	if (!cmd)
		cmd = get_cmd(name);
	if (!cmd) {
		fprintf(stderr, "ft_ssl: '%s' is an invalid command.\n", name);
		return 1;
	}
	ft_usage_line(cmd, name);
	get_longest(cmd);

	fprintf(stderr, "\n\n");

	char buf[4096];
	setvbuf(stderr, buf, _IOFBF, 4096);// Line buffering

	fprintf(stderr, "General options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_GENERAL);

	fprintf(stderr, "\nInput options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_IN);

	fprintf(stderr, "\nOutput options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_OUT);

	fprintf(stderr, "\nEncryption options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_ENC);

	fprintf(stderr, "\nSigning options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_SIGNING);

	fprintf(stderr, "\nRandom state options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_RAND);

	fprintf(stderr, "\nPVK options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_PVK);

	fprintf(stderr, "\nProvider options:");
	ft_print(cmd->options, cmd->opts_count, OPT_TYPE_PROVIDER);

	print_parameters(cmd);

	setvbuf(stderr, NULL, _IONBF, 0);// No buffering

	get_longest(cmd);// Reset
	return err;
}
