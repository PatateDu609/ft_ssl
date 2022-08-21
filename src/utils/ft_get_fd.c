#include "utils.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int ft_getfd(char *filename)
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
