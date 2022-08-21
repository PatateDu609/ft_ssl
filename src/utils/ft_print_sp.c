#include "utils.h"
#include "libft.h"

#define MAX_DISP 64

static unsigned char *ft_prepare(unsigned char *str, size_t *len)
{
	size_t printable = *len < MAX_DISP ? *len : MAX_DISP;
	unsigned char *res = malloc(printable == *len ? *len : printable * 2);

	if (!res)
		return (NULL);
	if (printable == *len)
		return ft_memcpy(res, str, printable);

	size_t first = printable / 2 - 4;
	size_t last = printable / 2 + 4;
	// Copy the first MAX_DISP / 2 characters then add "..." and the last MAX_DISP / 2 characters.
	ft_memcpy(res, str, first);

	res[first++] = ' ';
	res[first++] = '.';
	res[first++] = '.';
	res[first++] = '.';
	res[first++] = ' ';

	ft_memcpy(res + first, str + *len - last, last);
	*len = printable + 5;

	return res;
}

char *ft_print_sp(unsigned char *str, size_t len)
{
	if (!str)
		return (NULL);

	char *res = malloc(len + 1);

	if (!res)
		return (NULL);
	unsigned char *tmp = ft_prepare(str, &len);
	if (tmp)
		str = tmp;

	size_t cur = len + 1;
	size_t j = 0;

	for (size_t i = 0; i < len; i++, j++)
	{
		if (j >= cur)
		{
			res = realloc(res, cur *= 2);
			if (!res)
				return (NULL);
		}

		if (ft_isprint(str[i]))
			res[j] = str[i];
		else if (7 <= str[i] && str[i] <= 13)
			j += snprintf(res + j, cur - j, "\\%c", "abtnvfr"[str[i] - 7]) - 1;
		else
			j += snprintf(&res[j], cur - j, "\\x%02x", str[i]) - 1;
	}
	res[j] = '\0';
	free(tmp);
	return (res);
}
