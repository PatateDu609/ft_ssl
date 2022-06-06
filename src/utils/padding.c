#include "utils.h"
#include "libft.h"
#include <limits.h>

/*
 * Should perform the padding of the message according to RFC 1321 and RFC 6234
 * https://tools.ietf.org/html/rfc1321 https://tools.ietf.org/html/rfc6234
 *
 * @param msg The message to pad
 * @param block_len The length of the block
 * @param last The length of the last block
 * @param endian The endian of the length
 * @return The padded message
 */
struct s_blocks *ft_get_blocks(struct s_msg *msg, size_t block_len, size_t last, uint16_t endian)
{
	struct s_blocks *blocks;
	blocks = malloc(sizeof(struct s_blocks));
	if (blocks == NULL)
		throwe("Allocation error");

	blocks->block_size = block_len;
	blocks->nb = msg->len / block_len;

	if (msg->len % block_len || msg->len == 0)
		blocks->nb++;
	if (msg->len % block_len >= last)
		blocks->nb++;
	blocks->data = ft_calloc(blocks->nb * block_len, sizeof(uint8_t));
	if (blocks->data == NULL)
		throwe("Allocation error");

	ft_memcpy(blocks->data, msg->data, msg->len);
	blocks->data[msg->len] = 0x80;

	size_t s = blocks->nb * block_len - sizeof(size_t);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	size_t bits = (endian == __ORDER_LITTLE_ENDIAN__) ? msg->bits : SWAP_BYTES(msg->bits);
#else
	size_t bits = (endian == __ORDER_LITTLE_ENDIAN__) ? SWAP_BYTES(msg->bits) : msg->bits;
#endif
	ft_memcpy(blocks->data + s, &bits, sizeof(bits));

	return (blocks);
}
