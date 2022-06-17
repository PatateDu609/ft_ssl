#include "utils.h"
#include "libft.h"
#include <limits.h>

#define SET_LENGTH(x, x128)                                   \
	{                                                         \
		if (size_len == 8)                                    \
		{                                                     \
			uint64_t bits = x;                                \
			ft_memcpy(blocks->data + s, &bits, sizeof(bits)); \
		}                                                     \
		else                                                  \
		{                                                     \
			uint128_t bits = x128;                            \
			ft_memcpy(blocks->data + s, &bits, sizeof(bits)); \
		}                                                     \
	}

static void append_length(struct s_blocks *blocks, struct s_msg *msg, size_t last, uint16_t endian)
{
	size_t size_len = blocks->block_size - last;
	size_t s = blocks->nb * blocks->block_size - size_len;

	// Ensure that bits is always passed in the right endian (specified by endian)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

	if (endian == __ORDER_LITTLE_ENDIAN__)
		SET_LENGTH(msg->bits, msg->len_128 * CHAR_BIT)
	else // endian == __ORDER_BIG_ENDIAN__
		SET_LENGTH(SWAP_BYTES(msg->bits), SWAP_BYTES128(msg->len_128 * CHAR_BIT))

#else // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

	if (endian == __ORDER_LITTLE_ENDIAN__)
		SET_LENGTH(SWAP_BYTES(msg->bits), SWAP_BYTES128(msg->len_128 * CHAR_BIT))
	else // endian == __ORDER_BIG_ENDIAN__
		SET_LENGTH(msg->bits, msg->len_128 * CHAR_BIT)

#endif
}

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

	append_length(blocks, msg, last, endian);

	return (blocks);
}

struct s_blocks *ft_file_padding(struct s_msg *msg, size_t block_len, size_t last, uint16_t endian)
{
	struct s_blocks *blocks;
	blocks = malloc(sizeof(struct s_blocks));
	if (blocks == NULL)
		throwe("Allocation error");

	blocks->block_size = block_len;
	blocks->nb = 1;

	uint8_t is_last = msg->block_size >= last && msg->block_size < block_len;

	if (is_last)
		blocks->nb++;
	blocks->data = ft_calloc(blocks->nb * block_len, sizeof *blocks->data);
	if (blocks->data == NULL)
		throwe("Allocation error");

	ft_memcpy(blocks->data, msg->data, msg->block_size);

	if (is_last || msg->block_size < last)
	{
		blocks->data[msg->block_size] = 0x80;
		append_length(blocks, msg, last, endian);
	}
	return (blocks);
}
