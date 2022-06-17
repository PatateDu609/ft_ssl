#include "utils.h"
#include "libft.h"

#include <limits.h>
#include <unistd.h>

#define BUFF_SIZE 4096

// TODO: stream file to buffer

static uint8_t *merge(uint8_t *a, uint8_t *b, size_t old, size_t len)
{
	uint8_t *res = realloc(a, len * sizeof(uint8_t));
	if (res == NULL)
		throwe("Allocation error");
	ft_memcpy(res + old, b, len - old);
	return (res);
}

struct s_msg *read_all(int fd)
{
	uint8_t buf[BUFF_SIZE + 1];
	__unused uint8_t *tmp;
	struct s_msg *msg;
	int r;

	msg = ft_calloc(1, sizeof *msg);
	if (msg == NULL)
		throwe("Allocation error");

	while ((r = read(fd, buf, BUFF_SIZE)) > 0)
	{
		msg->len += r;	   // will overflow if r > UINT64_MAX but it's not a problem
		msg->len_128 += r; // will overflow if r > UINT128_MAX (for sha512)
		msg->data = merge(msg->data, buf, msg->len - r, msg->len);
	}
	if (r == -1)
	{
		free(msg->data);
		free(msg);
		throwe("Read error");
	}
	msg->bits = msg->len * CHAR_BIT;
	return (msg);
}
