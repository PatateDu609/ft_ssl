#include "sha2.h"

#define a0 ctx->u32.st[0]
#define b0 ctx->u32.st[1]
#define c0 ctx->u32.st[2]
#define d0 ctx->u32.st[3]
#define e0 ctx->u32.st[4]
#define f0 ctx->u32.st[5]
#define g0 ctx->u32.st[6]
#define h0 ctx->u32.st[7]
#define SSIG0(x) SSIG0_32(x)
#define SSIG1(x) SSIG1_32(x)
#define BSIG0(x) BSIG0_32(x)
#define BSIG1(x) BSIG1_32(x)

static void sha2_32_process_block(struct s_sha2_ctx *ctx, uint32_t *block)
{
	uint32_t a = a0, b = b0, c = c0, d = d0,
			 e = e0, f = f0, g = g0, h = h0; // local copies

	uint32_t w[64]; // message schedule
	uint32_t t1, t2;

	// copy the block to the message schedule and expand it
	for (size_t i = 0; i < 16; i++)
		w[i] = block[i];
	for (size_t i = 16; i < ctx->nb_rounds; i++)
		w[i] = SSIG1(w[i - 2]) + w[i - 7] + SSIG0(w[i - 15]) + w[i - 16];

	for (size_t i = 0; i < ctx->nb_rounds; i++)
	{
		t1 = h + BSIG1(e) + Ch(e, f, g) + ctx->u32.k[i] + w[i];
		t2 = BSIG0(a) + Ma(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	a0 += a; // update state
	b0 += b;
	c0 += c;
	d0 += d;
	e0 += e;
	f0 += f;
	g0 += g;
	h0 += h;
}

#undef a0
#undef b0
#undef c0
#undef d0
#undef e0
#undef f0
#undef g0
#undef h0
#undef SSIG0
#undef SSIG1
#undef BSIG0
#undef BSIG1

#define a0 ctx->u64.st[0]
#define b0 ctx->u64.st[1]
#define c0 ctx->u64.st[2]
#define d0 ctx->u64.st[3]
#define e0 ctx->u64.st[4]
#define f0 ctx->u64.st[5]
#define g0 ctx->u64.st[6]
#define h0 ctx->u64.st[7]
#define SSIG0(x) SSIG0_64(x)
#define SSIG1(x) SSIG1_64(x)
#define BSIG0(x) BSIG0_64(x)
#define BSIG1(x) BSIG1_64(x)

static void sha2_64_process_block(struct s_sha2_ctx *ctx, uint64_t *block)
{
	uint64_t a = a0, b = b0, c = c0, d = d0,
			 e = e0, f = f0, g = g0, h = h0; // local copies

	uint64_t w[80]; // message schedule
	uint64_t t1, t2;

	// copy the block to the message schedule and expand it
	for (size_t i = 0; i < 16; i++)
		w[i] = block[i];
	for (size_t i = 16; i < ctx->nb_rounds; i++)
		w[i] = SSIG1(w[i - 2]) + w[i - 7] + SSIG0(w[i - 15]) + w[i - 16];

	for (size_t i = 0; i < ctx->nb_rounds; i++)
	{
		t1 = h + BSIG1(e) + Ch(e, f, g) + ctx->u64.k[i] + w[i];
		t2 = BSIG0(a) + Ma(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	a0 += a; // update state
	b0 += b;
	c0 += c;
	d0 += d;
	e0 += e;
	f0 += f;
	g0 += g;
	h0 += h;
}

#undef a0
#undef b0
#undef c0
#undef d0
#undef e0
#undef f0
#undef g0
#undef h0

void sha2_update(struct s_sha2_ctx *ctx, struct s_blocks *blks)
{
	for (size_t i = 0; i < blks->nb; i++)
	{
		if (ctx->type == SHA2_TYPE_SHA256 || ctx->type == SHA2_TYPE_SHA224)
		{
			uint32_t *block = (uint32_t *)blks->data + i * ctx->block_size;
			sha2_32_process_block(ctx, block);
		}
		else
		{
			uint64_t *block = (uint64_t *)blks->data + i * ctx->block_size;
			sha2_64_process_block(ctx, block);
		}
	}
}
