#include "ft_ssl.h"
#include "common.h"
#include "defines.h"
#include "usage.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static int ft_base64_encode(struct s_env *e)
{
	char *infile = e->in_file;

	char *res = base64_encode_file(infile);
	if (res == NULL)
	{
		if (errno)
			perror(infile);
		return (1);
	}

	char *outfile = e->out_file;

	FILE *f = outfile ? fopen(outfile, "w") : stdout;
	if (f == NULL)
	{
		perror(outfile);
		return (1);
	}
	for (size_t i = 0, len = strlen(res); i < len; i += 64)
		fprintf(f, "%.64s\n", res + i);

	if (outfile)
		fclose(f);
	free(res);
	return (0);
}

static int ft_base64_decode(struct s_env *e)
{
	char *infile = e->in_file;
	size_t len;

	uint8_t *res = base64_decode_file(infile, &len);
	if (res == NULL)
	{
		if (errno)
			perror(infile);
		return (1);
	}
	char *outfile = e->out_file;

	FILE *f = outfile ? fopen(outfile, "w") : stdout;
	if (f == NULL)
	{
		perror(outfile);
		return (1);
	}
	fwrite(res, 1, len, f);
	fflush(f);
	if (outfile)
		fclose(f);
	free(res);
	return (0);
}

int ft_base64(struct s_env *e)
{
	if (e->opts & FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));
	if (e->opts & BASE64_FLAG_d)
		return (ft_base64_decode(e));
	else
		return (ft_base64_encode(e));
}
