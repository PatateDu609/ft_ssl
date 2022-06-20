#include "utils.h"
#include "libft.h"

void free_msg(struct s_msg **msg)
{
	if (!*msg)
		return;

	free((*msg)->data);
	free(*msg);
	*msg = NULL;
}

void free_blocks(struct s_blocks **blks)
{
	if (!*blks)
		return;

	free((*blks)->data);
	free(*blks);
	*blks = NULL;
}
