#include "md5.h"
#include "libft.h"

#define INIT_BUFFER(buffer, a, b, c, d) \
	{                                   \
		(buffer)[0] = (a);              \
		(buffer)[1] = (b);              \
		(buffer)[2] = (c);              \
		(buffer)[3] = (d);              \
	}

void md5_init(struct s_md5_ctx *md5)
{
	ft_memset(md5, 0, sizeof(struct s_md5_ctx));

	// Initialize IV
	md5->a = 0x67452301;
	md5->b = 0xefcdab89;
	md5->c = 0x98badcfe;
	md5->d = 0x10325476;

	// Initialize state buffer
	INIT_BUFFER(md5->buf + 0, 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee);
	INIT_BUFFER(md5->buf + 4, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501);
	INIT_BUFFER(md5->buf + 8, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be);
	INIT_BUFFER(md5->buf + 12, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821);
	INIT_BUFFER(md5->buf + 16, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa);
	INIT_BUFFER(md5->buf + 20, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8);
	INIT_BUFFER(md5->buf + 24, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed);
	INIT_BUFFER(md5->buf + 28, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a);
	INIT_BUFFER(md5->buf + 32, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c);
	INIT_BUFFER(md5->buf + 36, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70);
	INIT_BUFFER(md5->buf + 40, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05);
	INIT_BUFFER(md5->buf + 44, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665);
	INIT_BUFFER(md5->buf + 48, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039);
	INIT_BUFFER(md5->buf + 52, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1);
	INIT_BUFFER(md5->buf + 56, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1);
	INIT_BUFFER(md5->buf + 60, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391);

	// Initialize shift amounts
	INIT_BUFFER(md5->s + 0, 7, 12, 17, 22);
	INIT_BUFFER(md5->s + 4, 7, 12, 17, 22);
	INIT_BUFFER(md5->s + 8, 7, 12, 17, 22);
	INIT_BUFFER(md5->s + 12, 7, 12, 17, 22);

	INIT_BUFFER(md5->s + 16, 5, 9, 14, 20);
	INIT_BUFFER(md5->s + 20, 5, 9, 14, 20);
	INIT_BUFFER(md5->s + 24, 5, 9, 14, 20);
	INIT_BUFFER(md5->s + 28, 5, 9, 14, 20);

	INIT_BUFFER(md5->s + 32, 4, 11, 16, 23);
	INIT_BUFFER(md5->s + 36, 4, 11, 16, 23);
	INIT_BUFFER(md5->s + 40, 4, 11, 16, 23);
	INIT_BUFFER(md5->s + 44, 4, 11, 16, 23);

	INIT_BUFFER(md5->s + 48, 6, 10, 15, 21);
	INIT_BUFFER(md5->s + 52, 6, 10, 15, 21);
	INIT_BUFFER(md5->s + 56, 6, 10, 15, 21);
	INIT_BUFFER(md5->s + 60, 6, 10, 15, 21);
}
