#include "utils.h"
#include "libft.h"

/*
 * Should perform the padding of the message according to RFC 1321 and RFC 6234
 * https://tools.ietf.org/html/rfc1321 https://tools.ietf.org/html/rfc6234
 *
 * @param msg The message to pad
 * @param block_len The length of the block
 * @param last The length of the last block
 * @return The padded message
 */
struct s_blocks *ft_get_blocks(char *msg, size_t block_len, size_t last)
{
	struct s_blocks *blocks;
	blocks = malloc(sizeof(struct s_blocks));
	if (blocks == NULL)
		throwe("Allocation error");

	size_t len = ft_strlen(msg);
	blocks->block_size = block_len;
	blocks->nb = len / block_len;

	uint32_t remaining = len % block_len;
	if (remaining != 0 || remaining >= last)
		blocks->nb++;
	blocks->data = ft_calloc(blocks->nb * block_len, sizeof(uint8_t *));

	ft_memcpy(blocks->data, msg, len);
	blocks->data[len] = 0b10000000;

	len *= 8; // number of bits in the message
	size_t last_index = (blocks->nb - 1) * block_len + last;
	ft_memcpy(blocks->data + last_index, &len, sizeof(size_t));

	return (blocks);
}
