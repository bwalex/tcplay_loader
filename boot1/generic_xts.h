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
#include "aes256.h"


#define XTS_MAX_BLOCKSIZE	16
#define XTS_BLOCKSIZE_MASK	0x0FUL
#define XTS_IVSIZE		8
#define XTS_ALPHA		0x87	/* GF(2^128) generator polynomial */

typedef void (*encrypt_decrypt_fn)(void *, uint8_t *);
typedef void (*set_key_fn)(void *, uint8_t *);
typedef void (*zero_key_fn)(void *);

union cipher_ctx {
	aes256_context aes256;
};

struct xts_ctx {
	encrypt_decrypt_fn	encrypt_fn;
	encrypt_decrypt_fn	decrypt_fn;
	set_key_fn		set_key_fn;

	union cipher_ctx ctx1;
	union cipher_ctx ctx2;
	uint8_t		tweak[XTS_MAX_BLOCKSIZE];
	unsigned int	blk_sz;
};

int xts_init(struct xts_ctx *ctxp, set_key_fn set_key_fn,
    encrypt_decrypt_fn encrypt_fn,
    encrypt_decrypt_fn decrypt_fn, unsigned int blk_sz, uint8_t *key, int len);
int xts_encrypt(struct xts_ctx *ctx, uint8_t *data, size_t len, void *iv, int ivlen);
int xts_decrypt(struct xts_ctx *ctx, uint8_t *data, size_t len, void *iv, int ivlen);
int xts_uninit(struct xts_ctx *ctxp);

