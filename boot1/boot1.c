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
#include "pbkdf2.h"

int biosdev;

int
main()
{
	char *passphrase;
	char salt[] = "NaCL";
	uint8_t *dk;
	int r;
	int iterations = 1000;
	int dklen = 30;
	int saltlen = strlen(salt);

	dk = malloc(64);
	passphrase = malloc(65);

	bios_print("tcplay boot1\r\n");
	bios_print("Enter Passphrase: ");
	bios_clear_kbd_buf();
	r = bios_read_line(passphrase, 64, BIOS_RL_ECHO_STAR | BIOS_RL_CAN_ESC);
	if (r < 0) {
		bios_print("\r\nPressed ESC!\r\n");
	} else {
		passphrase[r] = '\0';
		bios_print("\r\nEntered: ");
		bios_print(passphrase);
		bios_print("\r\n");

		bios_print("PBKDF2-HMAC-SHA512:\r\n");
		pbkdf2(dk, dklen, passphrase, strlen(passphrase), salt,
		    saltlen, iterations, sha512_hmac, SHA512_DIGEST_SZ);
		bios_print_hex(dk, dklen);
		bios_print("\r\n");

		bios_print("PBKDF2-HMAC-RIPEMD160:\r\n");
		pbkdf2(dk, dklen, passphrase, strlen(passphrase), salt,
		    saltlen, iterations, rmd160_hmac, RMD160_DIGEST_SZ);
		bios_print_hex(dk, dklen);
		bios_print("\r\n");

	}

	return 0;
}
