#include "error.h"
#include <stdio.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

void throwe(const char *msg)
{
	fprintf(stderr, "ft_ssl: %s. %s\n", msg, strerror(errno));
	exit(1);
}
