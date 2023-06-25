#include "error.h"
#include <stdio.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

__noreturn void throwe(const char *msg, bool errno_msg)
{
	fprintf(stderr, "ft_ssl: %s.", msg);
	if (errno_msg)
		fprintf(stderr, " %s", strerror(errno));
	fprintf(stderr, "\n");

	exit(1);
}
