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
#include "bios.h"
#include "crc32_tiny.h"
#include "fun.h"
#include "malloc.h"
#include "rmd160.h"
#include "sha512.h"
#include "generic_xts.h"
#include "pbkdf2.h"
#include "tcplay.h"

int biosdev;

union tchdr hdr;


struct pbkdf_prf prfs[] = {
	{
		.hmac_fn	= rmd160_hmac,
		.iterations	= 2000,
		.digest_sz	= RMD160_DIGEST_SZ
	},
	{
		.hmac_fn	= rmd160_hmac,
		.iterations	= 1000,
		.digest_sz	= RMD160_DIGEST_SZ
	},
	{
		.hmac_fn	= sha512_hmac,
		.iterations	= 1000,
		.digest_sz	= SHA512_DIGEST_SZ
	},
	{
		.hmac_fn	= NULL
	}
};

int
main()
{
	char *passphrase;
	uint8_t *dk;
	int r;
	int iterations = 1000;
	int dklen = 64;
	struct xts_ctx *xts_ctx;
	uint32_t sector = 0;
	uint32_t crc;

	dk = malloc(dklen);
	passphrase = malloc(65);
	xts_ctx = malloc(sizeof(struct xts_ctx));

	bios_print("tcplay boot1\r\n");
	bios_print("Enter Passphrase: ");
	bios_clear_kbd_buf();
	r = bios_read_line(passphrase, 64, BIOS_RL_ECHO_STAR | BIOS_RL_CAN_ESC);
	if (r < 0) {
		bios_print("\r\nPressed ESC!\r\n");
		free(passphrase, 65);
	} else {
		passphrase[r] = '\0';
		bios_print("\r\n");

		bios_read_sectors(biosdev, &hdr, 63, 1);

		pbkdf2(dk, dklen, passphrase, strlen(passphrase), hdr.enc.salt,
		    SALT_LEN, iterations, sha512_hmac, SHA512_DIGEST_SZ);

		free(passphrase, 65);

		bios_print_hex(dk, dklen);
		bios_print("\r\n");

		xts_init(xts_ctx, aes256_init, aes256_encrypt_ecb,
		    aes256_decrypt_ecb, 16, dk, dklen);
		xts_decrypt(xts_ctx, hdr.enc.enc, sizeof(hdr.enc.enc), &sector, sizeof(sector));
		xts_uninit(xts_ctx);

		if (memcmp(hdr.dec.tc_str, TC_SIG, sizeof(hdr.dec.tc_str)) != 0) {
			bios_print("Signature mismatch\r\n");
			return -1;
		}

		bswap_inplace(&hdr.dec.tc_ver, sizeof(hdr.dec.tc_ver));
		bswap_inplace(&hdr.dec.crc_keys, sizeof(hdr.dec.crc_keys));
		bswap_inplace(&hdr.dec.off_mk_scope, sizeof(hdr.dec.off_mk_scope));
		bswap_inplace(&hdr.dec.sz_mk_scope, sizeof(hdr.dec.sz_mk_scope));
		bswap_inplace(&hdr.dec.flags, sizeof(hdr.dec.flags));

		crc = crc32((void *)hdr.dec.keys, sizeof(hdr.dec.keys));
		if (crc != hdr.dec.crc_keys) {
			bios_print("Keys CRC mismatch\r\n");
			return -1;
		}

		bios_print("Header successfully decrypted\r\n");
	}

	return 0;
}
