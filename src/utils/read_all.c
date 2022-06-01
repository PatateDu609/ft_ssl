#include "utils.h"
#include "libft.h"

#include <unistd.h>

#define BUFF_SIZE 4096

char *read_all(int fd)
{
	char buf[BUFF_SIZE + 1];
	char *tmp;
	char *str;
	int r;

	str = NULL;
	while ((r = read(fd, buf, BUFF_SIZE)) > 0)
	{
		buf[r] = '\0';
		tmp = str;
		str = ft_strjoin(str, buf);
		free(tmp);
	}
	if (r == -1)
	{
		perror("ft_ssl: read");
		free(str);
		return (NULL);
	}
	if (str == NULL)
		str = ft_strdup("");
	return (str);
}
