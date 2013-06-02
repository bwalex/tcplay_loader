/*
 * Copyright (C) 2008, Damien Miller
 * Copyright (C) 2011, Alex Hornung
 *
 * Permission to use, copy, and modify this software with or without fee
 * is hereby granted, provided that this entire notice is included in
 * all copies of any software which is or includes a copy or
 * modification of this software.
 * You may use this code under the GNU public license if you so wish. Please
 * contribute changes back to the authors under this freer than GPL license
 * so that we may further the use of strong encryption without limitations to
 * all.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTY. IN PARTICULAR, NONE OF THE AUTHORS MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE
 * MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#ifndef _STANDALONE
#include "fun.h"
#endif
#include "generic_xts.h"



static int
xts_reinit_full(struct xts_ctx *ctx, uint8_t *iv, int ivlen)
{
	bzero(ctx->tweak, 2*XTS_IVSIZE);
	memcpy(ctx->tweak, iv, ivlen);

	ctx->encrypt_fn(&ctx->ctx2, ctx->tweak);

	return 0;
}

static int
xts_crypt(struct xts_ctx *ctx, uint8_t *data, unsigned int do_encrypt)
{
	unsigned int i, carry_in, carry_out;

	for (i = 0; i < ctx->blk_sz; i++)
		data[i] ^= ctx->tweak[i];

	if (do_encrypt)
		ctx->encrypt_fn(&ctx->ctx1, data);
	else
		ctx->decrypt_fn(&ctx->ctx1, data);

	for (i = 0; i < ctx->blk_sz; i++)
		data[i] ^= ctx->tweak[i];

	/* Exponentiate tweak */
	carry_in = 0;
	for (i = 0; i < ctx->blk_sz; i++) {
		carry_out = ctx->tweak[i] & 0x80;
		ctx->tweak[i] = (ctx->tweak[i] << 1) | (carry_in ? 1 : 0);
		carry_in = carry_out;
	}
	if (carry_in)
		ctx->tweak[0] ^= XTS_ALPHA;

	return 0;
}

int
xts_init(struct xts_ctx *ctx, set_key_fn _set_key_fn,
    encrypt_decrypt_fn _encrypt_fn, encrypt_decrypt_fn _decrypt_fn,
    unsigned int blk_sz, uint8_t *key, int len)
{
	if (len != 32 && len != 64)
		return -1;

	ctx->blk_sz = blk_sz;
	ctx->encrypt_fn = _encrypt_fn;
	ctx->decrypt_fn = _decrypt_fn;
	ctx->set_key_fn = _set_key_fn;

	ctx->set_key_fn(&ctx->ctx1, key);
	ctx->set_key_fn(&ctx->ctx2, key + (len >> 1));

	return 0;
}

int
xts_encrypt(struct xts_ctx *ctx, uint8_t *data, size_t len, void *iv, int ivlen)
{
	int err;

	if ((len & XTS_BLOCKSIZE_MASK) != 0)
		return -1;

	err = xts_reinit_full(ctx, iv, ivlen);
	if (err)
		return err;

	while (len > 0) {
		err = xts_crypt(ctx, data, 1);
		if (err)
			return -1;

		data += ctx->blk_sz;
		len -= ctx->blk_sz;
	}

	return err;
}

int
xts_decrypt(struct xts_ctx *ctx, uint8_t *data, size_t len, void *iv, int ivlen)
{
	int err;

	if ((len & XTS_BLOCKSIZE_MASK) != 0)
		return -1;

	err = xts_reinit_full(ctx, iv, ivlen);
	if (err)
		return err;

	while (len > 0) {
		err = xts_crypt(ctx, data, 0);
		if (err)
			return -1;

		data += ctx->blk_sz;
		len -= ctx->blk_sz;
	}

	return err;
}

int
xts_uninit(struct xts_ctx *ctx)
{
	bzero(&ctx->ctx1, sizeof(ctx->ctx1));
	bzero(&ctx->ctx2, sizeof(ctx->ctx2));

	return 0;
}

