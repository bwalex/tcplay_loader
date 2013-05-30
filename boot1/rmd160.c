/*
 * Copyright (c) 2013 Alex Hornung <alex@alexhornung.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <stdint.h>
#ifndef _STANDALONE
#include "fun.h"
#else
#include <string.h>
#endif
#include "rmd160.h"


#ifdef _STANDALONE
static
void
memxor(void *mem, int c, size_t cnt)
{
	uint8_t *m = mem;
	c = c & 0xff;

	while(cnt-- > 0) {
		*m++ ^= (uint8_t)c;
	}
}
#endif


#define f0(x, y, z)	(x ^ y ^ z)
#define f16(x, y, z)	((x & y) | (~x & z))
#define f32(x, y, z)	((x | ~y) ^ z)
#define f48(x, y, z)	((x & z) | (y & ~z))
#define f64(x, y, z)	(x ^ (y | ~z))

#define idx(j) (j >> 4)

#define GEN_SMALL

#ifdef GEN_SMALL
static
uint32_t
f(int j, uint32_t x, uint32_t y, uint32_t z) {
	if	(j <= 15) return f0(x, y, z);
	else if	(j <= 31) return f16(x, y, z);
	else if	(j <= 47) return f32(x, y, z);
	else if	(j <= 63) return f48(x, y, z);
	else		  return f64(x, y, z);
}
#else
#define f(j, x, y, z) (\
	(j <= 15) ? f0(x, y, z)  : \
	(j <= 31) ? f16(x, y, z) : \
	(j <= 47) ? f32(x, y, z) : \
	(j <= 63) ? f48(x, y, z) : \
		    f64(x, y, z))
#endif

static uint32_t K[] = {
	0x00000000UL,
	0x5A827999UL,
	0x6ED9EBA1UL,
	0x8F1BBCDCUL,
	0xA953FD4EUL
};

static uint32_t K_[] = {
	0x50A28BE6UL,
	0x5C4DD124UL,
	0x6D703EF3UL,
	0x7A6D76E9UL,
	0x00000000UL
};

static uint8_t r[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
	3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
	1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
	4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13
};

static uint8_t r_[] = {
	5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
	6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
	15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
	8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
	12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11
};

static uint8_t s[] = {
	11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
	7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
	11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
	11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
	9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6
};

static uint8_t s_[] = {
	8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
	9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
	9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
	15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
	8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11
};

#if 0
#define rol(a, b) \
	((a << b) | (a >> ((8*sizeof(a))-b)))
#endif

static
uint32_t
rol(uint32_t a, int b)
{
	return ((a << b) | (a >> ((8*sizeof(a))-b)));
}

#define rol_s(j, expr) \
	(rol((expr), s[j]))

#define rol_s_(j, expr) \
	(rol((expr), s_[j]))

#define rol_10(expr) \
	(rol((expr), 10))


void
rmd160_init(struct rmd160_ctx *ctx)
{
	ctx->h[0] = 0x67452301UL;
	ctx->h[1] = 0xEFCDAB89UL;
	ctx->h[2] = 0x98BADCFEUL;
	ctx->h[3] = 0x10325476UL;
	ctx->h[4] = 0xC3D2E1F0UL;
	ctx->bytes_in_x = 0;
	ctx->bytes = 0;

	return;
}

void
rmd160_compress(struct rmd160_ctx *ctx, uint32_t *X)
{
	uint32_t *h = ctx->h;
	uint32_t A, B, C, D, E;
	uint32_t A_, B_, C_, D_, E_;
	uint32_t T;
	int j = 0;

	A  = h[0]; B  = h[1]; C  = h[2]; D  = h[3]; E  = h[4];
	A_ = h[0]; B_ = h[1]; C_ = h[2]; D_ = h[3]; E_ = h[4];

	for (j = 0; j < 80; j++) {
		T = rol_s(j, A + f(j, B, C, D) + X[r[j]] + K[idx(j)]) + E;
		A = E; E = D; D = rol_10(C); C = B; B = T;

		T = rol_s_(j, A_ + f(79-j, B_, C_, D_) + X[r_[j]] + K_[idx(j)]) + E_;
		A_ = E_; E_ = D_; D_ = rol_10(C_); C_ = B_; B_ = T;
	}

	T = h[1] + C + D_; h[1] = h[2] + D + E_; h[2] = h[3] + E + A_;
	h[3] = h[4] + A + B_; h[4] = h[0] + B + C_; h[0] = T;

	return;
}

void
rmd160_update(struct rmd160_ctx *ctx, uint8_t *msg, uint32_t msglen)
{
	int count = ctx->bytes_in_x + msglen;
	int missing = 64-ctx->bytes_in_x;

	ctx->bytes += msglen;

	while (count >= 64) {
		memcpy(((uint8_t *)ctx->X)+ctx->bytes_in_x, msg, missing);
		rmd160_compress(ctx, ctx->X);
		msg    += missing;
		msglen -= missing;
		count  -= 64;
		missing = 64;
		ctx->bytes_in_x = 0;
	}
		
	if (msglen > 0) {
		memcpy(((uint8_t *)ctx->X)+ctx->bytes_in_x, msg, msglen);
		ctx->bytes_in_x += msglen;
	}
}

void
rmd160_finalize(struct rmd160_ctx *ctx, uint8_t *dst)
{
	memset(((uint8_t *)ctx->X)+ctx->bytes_in_x, 0, sizeof(ctx->X)-ctx->bytes_in_x);

	/* append the bit m_n == 1 */
	ctx->X[(ctx->bytes_in_x >> 2)] ^= (uint32_t)1UL << (((ctx->bytes_in_x & 0x3) << 3) + 7);

	if ((ctx->bytes_in_x) >= 56) {
		/* length goes to next block */
		rmd160_compress(ctx, ctx->X);
		memset(ctx->X, 0, sizeof(ctx->X));
	}

	/* append length in bits */
	ctx->X[14] = ctx->bytes << 3;
	ctx->X[15] = ctx->bytes >> 29;
	rmd160_compress(ctx, ctx->X);

	memcpy(dst, ctx->h, sizeof(ctx->h));

	return;
}

void
rmd160_hash(uint8_t *dst, uint8_t *msg, int msglen)
{
	struct rmd160_ctx ctx;

	/* initialize */
	rmd160_init(&ctx);
	
	rmd160_update(&ctx, msg, msglen);

	rmd160_finalize(&ctx, dst);

	return;
}

void
rmd160_hmac(uint8_t *dst, uint8_t *key, int keylen, uint8_t *msg, int msglen)
{
	uint8_t ipad[RMD160_BLOCK_SZ];
	uint8_t opad[RMD160_BLOCK_SZ];
	struct rmd160_ctx ctx;

	memset(ipad, 0, sizeof(ipad));
	memset(opad, 0, sizeof(opad));

	if (keylen > RMD160_BLOCK_SZ)
		rmd160_hash(ipad, key, keylen);
	else
		memcpy(ipad, key, keylen);

	memcpy(opad, ipad, sizeof(opad));

	memxor(ipad, 0x36, sizeof(ipad));
	memxor(opad, 0x5c, sizeof(opad));

	rmd160_init(&ctx);
	rmd160_update(&ctx, ipad, sizeof(ipad));
	rmd160_update(&ctx, msg, msglen);
	rmd160_finalize(&ctx, ipad);

	rmd160_init(&ctx);
	rmd160_update(&ctx, opad, sizeof(opad));
	rmd160_update(&ctx, ipad, RMD160_DIGEST_SZ);
	rmd160_finalize(&ctx, dst);

	memset(ipad, 0, sizeof(ipad));
	memset(opad, 0, sizeof(opad));

	return;
}

