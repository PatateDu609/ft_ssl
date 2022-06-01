#include "md5.h"
#include "libft.h"

#define ROUND1(a, b, c, d, f, g, i) \
	{                               \
		f = F(b, c, d);             \
		g = i;                      \
	}

#define ROUND2(a, b, c, d, f, g, i) \
	{                               \
		f = G(b, c, d);             \
		g = (5 * i + 1) % 16;       \
	}

#define ROUND3(a, b, c, d, f, g, i) \
	{                               \
		f = H(b, c, d);             \
		g = (3 * i + 5) % 16;       \
	}

#define ROUND4(a, b, c, d, f, g, i) \
	{                               \
		f = I(b, c, d);             \
		g = (7 * i) % 16;           \
	}

static void md5_update(struct s_md5_ctx *md5, const uint8_t *input, size_t len)
{
	uint32_t *data = (uint32_t *)input; // split input into 32-bit words
	size_t nb = len / sizeof(uint32_t); // number of 32-bit words
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;

	for (size_t j = 0; j < nb; j++)
	{
		a = md5->a;
		b = md5->b;
		c = md5->c;
		d = md5->d;

		for (int32_t i = 0; i < 64; i++)
		{
			uint32_t f, g;

			if (0 <= i && i < 16)
				ROUND1(a, b, c, d, f, g, i)
			else if (16 <= i && i < 32)
				ROUND2(a, b, c, d, f, g, i)
			else if (32 <= i && i < 48)
				ROUND3(a, b, c, d, f, g, i)
			else if (48 <= i && i < 64)
				ROUND4(a, b, c, d, f, g, i)

			f += a + md5->buf[i] + data[g];
			a = d;
			d = c;
			c = b;
			b += ROTATE_LEFT(f, md5->s[i]);
		}
	}
	md5->a += a;
	md5->b += b;
	md5->c += c;
	md5->d += d;
}

void md5_process(struct s_blocks *blks, struct s_md5_ctx *md5)
{
	for (size_t i = 0; i < blks->nb; i++)
	{
		size_t index = i * blks->block_size;
		md5_update(md5, blks->data + index, blks->block_size);
	}
}
