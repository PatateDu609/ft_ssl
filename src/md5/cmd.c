#include "md5.h"
#include "ft_ssl.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int ft_getfd(char *filename)
{
	int fd;

	if (filename == NULL)
		fd = STDIN_FILENO;
	else
		fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		char msg[512];
		snprintf(msg, 512, "ft_ssl: %s", filename);
		perror(msg);
		return -1;
	}
	return (fd);
}

int ft_md5(struct s_env *e)
{
	int fd = ft_getfd(e->params[0]);

	if (fd == -1)
		return (1);
	if (e->opts & MD5_FLAG_HELP)
		return (ft_usage(0, e->cmd->name, e->cmd));

	char *input = read_all(fd);
	if (input == NULL)
		return (1);
	close(fd);

	struct s_blocks *blks = ft_get_blocks(input, MD5_BLOCK_SIZE, MD5_LAST_BLOCK_SIZE);

	struct s_md5_ctx ctx;
	md5_init(&ctx);
	md5_process(blks, &ctx);

	printf("%08x%08x%08x%08x\n", ctx.a, ctx.b, ctx.c, ctx.d);

	free(blks->data);
	free(blks);
	free(input);
	return 0;
}
