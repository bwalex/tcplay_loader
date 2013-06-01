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

void inl_bios_print(char *str);
#pragma aux inl_bios_print =	\
	"start: lodsb"		\
	"or al, al"		\
	"jz finish"		\
	"mov ah, 0x0E"		\
	"mov bx, 0x0007"	\
	"int 0x10"		\
	"jmp start"		\
	"finish:"		\
	modify [ax bx]		\
	parm [si]

void inl_bios_putc(char c);
#pragma aux inl_bios_putc =	\
	"mov ah, 0x0E"		\
	"mov bx, 0x0007"	\
	"int 0x10"		\
	modify [bx]		\
	parm [al]

int inl_bios_read_sectors(int dev, void *dst, int start, int count);
#pragma aux inl_bios_read_sectors = \
	"mov ah, 0x02"		\
	"xor ch, ch"		\
	"xor dh, dh"		\
	"int 0x13"		\
	"jnc finish"		\
	"xor al, al"		\
	"finish: xor ah, ah"	\
	parm [dx] [bx] [cx] [ax]	\
	value [ax]

void inl_bios_clear_kbd_buf(void);
#pragma aux inl_bios_clear_kbd_buf =	\
	"start: mov ah, 0x01"		\
	"int 0x16"			\
	"jz finish"			\
	"mov ah, 0x00"			\
	"int 0x16"			\
	"jmp start"			\
	"finish:"			\
	modify [ax]

char inl_bios_getc(void);
#pragma aux inl_bios_getc =		\
	"mov ah, 0x00"			\
	"int 0x16"			\
	value [al]

void
bios_print(char *str)
{
	inl_bios_print(str);
}

void
bios_clear_kbd_buf(void)
{
	inl_bios_clear_kbd_buf();
}

int
bios_read_line(char *buf, int maxlen, int flags)
{
	char c;
	int read = 0;

	while (read != maxlen) {
		c = inl_bios_getc();
		switch (c) {
		case 0x0d: /* RETURN */
			goto done;
			/* NOT REACHED */

		case 0x1b: /* ESC */
			if (flags & BIOS_RL_CAN_ESC)
				return -1;
			break;
			/* NOT REACHED */

		default:
			*buf++ = c;
			++read;
			if (flags & BIOS_RL_ECHO) {
				if ((flags & BIOS_RL_ECHO_STAR) == BIOS_RL_ECHO_STAR)
					c = '*';
				inl_bios_putc(c);
			}
		}
	}

done:
	return read;
}

int
bios_read_sectors(int dev, void *dst, int start, int count)
{
	return inl_bios_read_sectors(dev, dst, start, count);
}
