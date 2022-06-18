#include "utils.h"

struct s_msg *ft_bufferize(ft_stream *stream, size_t block_size)
{
	struct s_msg *msg;

	if (!stream)
		return (NULL);
	if (!(msg = malloc(sizeof *msg)))
		return (NULL);
	if (!(msg->data = malloc(block_size * sizeof *msg->data)))
	{
		free(msg);
		return (NULL);
	}
	ssize_t len = ft_sread(stream, msg->data, block_size);
	if (len < 0)
	{
		free(msg->data);
		free(msg);
		return (NULL);
	}
	msg->block_size = len;
	msg->len = stream->filesize; // make it overflow if filesize is too big
	msg->bits = msg->len * 8;
	msg->len_128 = stream->filesize;
	return (msg);
}
