#include "utils.h"
#include "libft.h"

void free_msg(struct s_msg **msg)
{
	ft_memset(*msg, 0, sizeof **msg);
	free((*msg)->data);
	free(*msg);
	*msg = NULL;
}

void free_blocks(struct s_blocks **blks)
{
	ft_memset(*blks, 0, sizeof **blks);
	free((*blks)->data);
	free(*blks);
	*blks = NULL;
}
