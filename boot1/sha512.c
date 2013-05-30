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
#include "fun.h"
#include "sha512.h"


#if 0
#define _SEPARATE_TEMP_VARS
#endif

#define rotr rotr64

#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define BSIG0(x) (rotr(x,28) ^ rotr(x,34) ^ rotr(x,39))
#define BSIG1(x) (rotr(x,14) ^ rotr(x,18) ^ rotr(x,41))
#define SSIG0(x) (rotr(x, 1) ^ rotr(x, 8) ^ (x >> 7))
#define SSIG1(x) (rotr(x,19) ^ rotr(x,61) ^ (x >> 6))

static uint64_t K[] = {
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
	0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
	0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
	0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
	0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
	0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
	0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
	0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
	0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
	0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
	0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
	0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
	0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
	0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};


void
sha512_init(struct sha512_ctx *ctx)
{
        ctx->h[0] = 0x6a09e667f3bcc908ULL;
        ctx->h[1] = 0xbb67ae8584caa73bULL;
        ctx->h[2] = 0x3c6ef372fe94f82bULL;
        ctx->h[3] = 0xa54ff53a5f1d36f1ULL;
        ctx->h[4] = 0x510e527fade682d1ULL;
        ctx->h[5] = 0x9b05688c2b3e6c1fULL;
        ctx->h[6] = 0x1f83d9abfb41bd6bULL;
        ctx->h[7] = 0x5be0cd19137e2179ULL;
	ctx->bytes_in_x = 0;
	ctx->bytes = 0;
}

void
sha512_compress(struct sha512_ctx *ctx, uint64_t *X)
{
	uint64_t *H = ctx->h;
#ifdef _SEPARATE_TEMP_VARS
	uint64_t a, b, c, d, e, f, g, h;
#else
	uint64_t abc[8];
#endif
	uint64_t T1, T2;
	int j;

#ifdef _SEPARATE_TEMP_VARS
	a = H[0]; b = H[1]; c = H[2]; d = H[3];
	e = H[4]; f = H[5]; g = H[6]; h = H[7];
#else
	memcpy(abc, H, sizeof(abc));
#endif

	/*
	 * This pretty much follows the algorithm description, just using a
	 * bit of cleverness around Wt. ctx->X is reused as W, but only the
	 * 16 most recent elements (plus the current one) are kept, acting
	 * effectively as a sliding window W[t-16 .. t].
	 */
	for (j = 0; j < 80; j++) {
		if (j > 15) {
			ctx->X[16] = SSIG1(ctx->X[14]) + ctx->X[9] + SSIG0(ctx->X[1]) + ctx->X[0];
		} else {
			/*
			 * ctx->X slides along, so X[0] will always contain
			 * X[j] (for j < 16)
			 */
			bswap(&ctx->X[16], &X[0], sizeof(uint64_t));
		}

#ifdef _SEPARATE_TEMP_VARS
		T1 = h + BSIG1(e) + CH(e,f,g) + K[j] + ctx->X[16];
		T2 = BSIG0(a) + MAJ(a,b,c);
		h = g; g = f; f = e; e = d + T1;
		d = c; c = b; b = a; a = T1 + T2;
#else
		T1 = abc[7] + BSIG1(abc[4]) + CH(abc[4],abc[5],abc[6]) + K[j] + ctx->X[16];
		T2 = BSIG0(abc[0]) + MAJ(abc[0],abc[1],abc[2]);
		memmove(&abc[1], &abc[0], sizeof(uint64_t)*7);
		abc[0] = T1 + T2;
		abc[4] += T1;
#endif

		memmove(&ctx->X[0], &ctx->X[1], sizeof(uint64_t)*16);
	}

#ifdef _SEPARATE_TEMP_VARS
	H[0] += a;
	H[1] += b;
	H[2] += c;
	H[3] += d;
	H[4] += e;
	H[5] += f;
	H[6] += g;
	H[7] += h;
#else
	H[0] += abc[0];
	H[1] += abc[1];
	H[2] += abc[2];
	H[3] += abc[3];
	H[4] += abc[4];
	H[5] += abc[5];
	H[6] += abc[6];
	H[7] += abc[7];
#endif
}

void
sha512_update(struct sha512_ctx *ctx, uint8_t *msg, uint32_t msglen)
{
	int count = ctx->bytes_in_x + msglen;
	int missing = 128-ctx->bytes_in_x;

	ctx->bytes += msglen;

	while (count >= 128) {
		memcpy(((uint8_t *)ctx->X)+ctx->bytes_in_x, msg, missing);
		sha512_compress(ctx, ctx->X);
		msg    += missing;
		msglen -= missing;
		count  -= 128;
		missing = 128;
		ctx->bytes_in_x = 0;
	}
		
	if (msglen > 0) {
		memcpy(((uint8_t *)ctx->X)+ctx->bytes_in_x, msg, msglen);
		ctx->bytes_in_x += msglen;
	}
}

void
sha512_finalize(struct sha512_ctx *ctx, uint8_t *dst)
{
	uint64_t len64 = (uint64_t)ctx->bytes;
	int i;

	bswap_inplace(&len64, sizeof(uint64_t));

	memset(((uint8_t *)ctx->X)+ctx->bytes_in_x, 0, sizeof(ctx->X)-ctx->bytes_in_x);

	ctx->X[(ctx->bytes_in_x >> 3)] ^= (uint64_t)1ULL << (((ctx->bytes_in_x & 0x7) << 3 /* ?? */) + 7);
	
	if ((ctx->bytes_in_x) >= 120) {
		/* length goes to next block */
		sha512_compress(ctx, ctx->X);
		memset(ctx->X, 0, sizeof(ctx->X));
	}

	/* append length in bits */
	ctx->X[14] = len64 >> 61;
	ctx->X[15] = len64 << 3;
	sha512_compress(ctx, ctx->X);

	for (i = 0; i < 8; i++) {	
		//ctx->h[i] = htobe64(ctx->h[i]);
		bswap(dst, &ctx->h[i], sizeof(uint64_t));
		dst += sizeof(uint64_t);
	}

	//memcpy(dst, ctx->h, sizeof(ctx->h));

	return;
}


void
sha512_hash(uint8_t *dst, uint8_t *msg, int msglen)
{
	struct sha512_ctx ctx;

	/* initialize */
	sha512_init(&ctx);
	
	sha512_update(&ctx, msg, msglen);

	sha512_finalize(&ctx, dst);

	return;
}


void
sha512_hmac(uint8_t *dst, uint8_t *key, int keylen, uint8_t *msg, int msglen)
{
	uint8_t ipad[SHA512_BLOCK_SZ];
	uint8_t opad[SHA512_BLOCK_SZ];
	struct sha512_ctx ctx;

	memset(ipad, 0, sizeof(ipad));
	memset(opad, 0, sizeof(opad));

	if (keylen > SHA512_BLOCK_SZ)
		sha512_hash(ipad, key, keylen);
	else
		memcpy(ipad, key, keylen);

	memcpy(opad, ipad, sizeof(opad));

	memxor(ipad, 0x36, sizeof(ipad));
	memxor(opad, 0x5c, sizeof(opad));

	sha512_init(&ctx);
	sha512_update(&ctx, ipad, sizeof(ipad));
	sha512_update(&ctx, msg, msglen);
	sha512_finalize(&ctx, ipad);

	sha512_init(&ctx);
	sha512_update(&ctx, opad, sizeof(opad));
	sha512_update(&ctx, ipad, SHA512_DIGEST_SZ);
	sha512_finalize(&ctx, dst);

	memset(ipad, 0, sizeof(ipad));
	memset(opad, 0, sizeof(opad));

	return;
}

