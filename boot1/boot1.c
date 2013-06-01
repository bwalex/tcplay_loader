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
#include "rmd160.h"
#include "sha512.h"
#include "pbkdf2.h"

int biosdev;

int
main()
{
	char *test = "Foobar";
	char passphrase[65];
	uint8_t moo[] = { 0xDE, 0xAD, 0xC0, 0xDE };
	char salt[] = "NaCL\0\0\0\0\0\0";
	uint8_t dk[128];
	uint64_t deadc = 0xDEADC0DE12345678ULL;
	uint8_t deadc_el[8];
	int r;
	int iterations = 1000;
	int dklen = 30;
	int saltlen = strlen(salt);

	bios_print("tcplay boot1\r\n");
	bios_print("Enter Passphrase: ");
	bios_clear_kbd_buf();
	r = bios_read_line(passphrase, 64, BIOS_RL_ECHO_STAR | BIOS_RL_CAN_ESC);
	if (r < 0) {
		bios_print("\r\nPressed ESC!\r\n");
		bios_print_hex((uint8_t *)&deadc, sizeof(deadc));
		bios_print("\r\n");

		bswap(deadc_el, &deadc, sizeof(deadc));
		bios_print_hex(deadc_el, sizeof(deadc));
		bios_print("\r\n");

		bswap_inplace(&deadc, sizeof(deadc));
		bios_print_hex((uint8_t *)&deadc, sizeof(deadc));
		bios_print("\r\n");

		deadc = 0xDEADC0DE12345678ULL;
		bios_print_hex((uint8_t *)&deadc, sizeof(deadc));
		bios_print("\r\n");
		deadc = rotr64(deadc, 32);
		bios_print_hex((uint8_t *)&deadc, sizeof(deadc));
		bios_print("\r\n");
	} else {
		passphrase[r] = '\0';
		bios_print("\r\nEntered: ");
		bios_print(passphrase);
		bios_print("\r\n");


		pbkdf2(dk, dklen, passphrase, strlen(passphrase), salt,
		    saltlen, iterations, sha512_hmac, SHA512_DIGEST_SZ);
		bios_print_hex(dk, dklen);
		bios_print("\r\n");

		pbkdf2(dk, dklen, passphrase, strlen(passphrase), salt,
		    saltlen, iterations, rmd160_hmac, RMD160_DIGEST_SZ);
		bios_print_hex(dk, dklen);
		bios_print("\r\n");

	}

	bios_print_hex(moo, sizeof(moo));

	bios_print("\r\n");
	bios_print_number(37, 10);
	bios_print("\r\n");
	bios_print_number(37, 16);


	return crc32(test, 6);
}
