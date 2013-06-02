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
#include "malloc.h"

#define HEAP_SZ		2048
#define BLOCK_SZ	64
#define BLOCK_SZ_MASK	(BLOCK_SZ-1)
#ifdef _STANDALONE
#define HEAP_BASE	1024*1024
#else
/* Populated by startup code */
int heap_base;
#define HEAP_BASE	heap_base
#endif
#define BITMAP_BYTES	(HEAP_SZ/BLOCK_SZ/8)

static uint8_t heap_bitmap[BITMAP_BYTES];

void *
malloc(int sz)
{
	int i, j;
	int blocks;
	int blocks_found;
	int block_no;
	uint8_t *p;

	/* Round size up to nearest block sz */
	if ((sz & BLOCK_SZ_MASK) != 0)
		sz = (sz & ~BLOCK_SZ_MASK)+BLOCK_SZ;

	blocks = sz/BLOCK_SZ;

	blocks_found = 0;
	block_no = -1;

	for (i = 0; i < BITMAP_BYTES; i++) {
		for (j = 8*sizeof(uint8_t)-1; j >= 0; j--) {
			if ((heap_bitmap[i] & (1 << j)) == 0) {
				++blocks_found;
				if (block_no == -1)
					block_no = i*8+(7-j);
				if (blocks_found == blocks)
					goto allocd;
			} else {
				blocks_found = 0;
				block_no = -1;
			}
		}
	}

allocd:
	if (block_no == -1 || blocks_found < blocks)
		return (void *)0;

#ifdef _STANDALONE
	printf("malloc(sz=%d, block_no=%d, blocks_found=%d, blocks=%d)\n", sz, block_no, blocks_found, blocks);
#endif

	i = (block_no / 8);
	j = 7-(block_no % 8);
	while (blocks_found-- > 0) {
		heap_bitmap[i] |= (1 << j);
		if (j-- == 0) {
			++i;
			j = 7;
		}
	}

	p = (uint8_t *)(block_no*BLOCK_SZ);
	p += HEAP_BASE;

	return (void *)p;
}

void
free(void *ptr, int sz)
{
	uint8_t *p = ptr;
	int block_no;
	int blocks;
	int i, j;
	p -= HEAP_BASE;

	block_no = (int)p;
	block_no /= BLOCK_SZ;

	/* Round size up to nearest block sz */
	if ((sz & BLOCK_SZ_MASK) != 0)
		sz = (sz & ~BLOCK_SZ_MASK)+BLOCK_SZ;

	blocks = sz/BLOCK_SZ;

#ifdef _STANDALONE
	printf("free(block_no=%d, sz=%d, blocks=%d)\n", block_no, sz, blocks);
#endif

	i = (block_no / 8);
	j = 7-(block_no % 8);
	while (blocks-- > 0) {
		heap_bitmap[i] &= ~(1 << j);
		if (j-- == 0) {
			++i;
			j = 7;
		}
	}
}
