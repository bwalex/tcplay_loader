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
#include "malloc.h"
#else
#include <string.h>
#endif
#include "pbkdf2.h"


#define min(a, b) ((a > b) ? b : a)

#ifdef _STANDALONE
static
void
bswap_inplace(uint32_t *p, size_t sz)
{
	*p = (((*p & 0x000000FFUL) << 24) |
	      ((*p & 0x0000FF00UL) << 8) |
	      ((*p & 0x00FF0000UL) >> 8) |
	      ((*p & 0xFF000000UL) >> 24));
}

static
void
bswap(void *dstv, uint32_t *src, size_t sz)
{
	uint32_t *dst = dstv;
	/* sz is assumed to be 8 */
	*dst = *src;
	bswap_inplace(dst, sz);
}
#endif

void
pbkdf2(uint8_t *dk, int dklen, uint8_t *pass, int passlen, uint8_t *salt,
    int saltlen, int iterations, hmac_fn_t hmac_fn, int hmac_digest_len)
{
	int rem;
	uint32_t i = 0;
	uint8_t ut[PBKDF2_MAX_DIGEST_LENGTH];
	uint8_t ux[PBKDF2_MAX_DIGEST_LENGTH];
	uint8_t *saltp;
	int c, j;

	saltp = malloc(saltlen + 4);
	memcpy(saltp, salt, saltlen);

	while (dklen > 0) {
		++i;

		rem = min(dklen, hmac_digest_len);

		bswap(saltp+saltlen, &i, sizeof(uint32_t));
		hmac_fn(ut, pass, passlen, saltp, saltlen+sizeof(uint32_t));
		memcpy(dk, ut, rem);

		for (c = 1; c < iterations; c++) {
			hmac_fn(ux, pass, passlen, ut, hmac_digest_len);
			for (j = 0; j < rem; j++)
				dk[j] ^= ux[j];
			memcpy(ut, ux, sizeof(ut));
		}

		dk += rem;
		dklen -= rem;
	}

#ifdef _STANDALONE
	free(saltp);
#else
	free(saltp, saltlen + 4);
#endif
}

