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


#define memcpy inl_memcpy

void *inl_memcpy(void *dst, const void *src, int n);
void *_memcpy(void *dst, const void *src, int n);
#pragma aux inl_memcpy =	\
	"or cx,cx"		\
	"jz finish"		\
	"start: lodsb"		\
	"stosb"			\
	"loop start"		\
	"finish:"		\
	modify [ax]		\
	parm [di] [si] [cx]



#define memset inl_memset

void *inl_memset(void *s, int c, int n);
void *_memset(void *s, int c, int n);
#pragma aux inl_memset =	\
	"rep stosb"		\
	parm [di] [ax] [cx]



#define memxor inl_memxor

void *inl_memxor(void *s, int c, int n);
void *_memxor(void *s, int c, int n);
#pragma aux inl_memxor =	\
	"or cx,cx"		\
	"jz finish"		\
	"mov di, si"		\
	"start: lodsb"		\
	"xor ax, bx"		\
	"stosb"			\
	"loop start"		\
	"finish:"		\
	modify [ax di]		\
	parm [si] [bx] [cx]



#define memmove _memmove

void *inl_memmove(void *dst, const void *src, int n);
void *_memmove(void *dst, const void *src, int n);
#pragma aux inl_memmove =	\
	"or cx,cx"		\
	"jz finish"		\
	"mov ax, di"		\
	"sub ax, si"		\
	"cmp ax, cx"		\
	"jnb do_copy"		\
				\
	"setup_backwards:"	\
	"dec cx"		\
	"add di, cx"		\
	"add si, cx"		\
	"inc cx"		\
	"std"			\
				\
	"do_copy:"		\
	"lodsb"			\
	"stosb"			\
	"loop do_copy"		\
				\
	"cld"			\
	"finish:"		\
	modify [ax]		\
	parm [di] [si] [cx]



#define strlen inl_strlen

int inl_strlen(char *str);
#pragma aux inl_strlen =	\
	"xor cx,cx"		\
	"start:"		\
	"lodsb"			\
	"or al,al"		\
	"jz finish"		\
	"inc cx"		\
	"jmp start"		\
	"finish:"		\
	modify [ax]		\
	parm [si]		\
	value [cx]



#define bswap _bswap

void inl_bswap(void *dst, const void *src, int n);
void _bswap(void *dst, const void *src, int n);
#pragma aux inl_bswap =		\
	"add di,cx"		\
	"start:"		\
	"dec di"		\
	"lodsb"			\
	"mov [di],al"		\
	"loop start"		\
	modify [ax]		\
	parm [di] [si] [cx]



#define bswap_inplace _bswap_inplace

void inl_bswap_inplace(void *mem, int n);
void _bswap_inplace(void *mem, int n);
#pragma aux inl_bswap_inplace =	\
	"mov di,si"		\
	"add di,cx"		\
	"shr cx,1"		\
	"start:"		\
	"dec di"		\
	"mov bx,si"		\
	"lodsb"			\
	"mov dl,[di]"		\
	"mov [bx],dl"		\
	"mov [di],al"		\
	"dec cx"		\
	"jnz start"		\
	modify [ax bx dx]	\
	parm [si] [cx]



#define rotr64 _rotr64

uint64_t inl_rotr64(uint64_t a, int b);
uint64_t _rotr64(uint64_t a, int b);
uint64_t _rotr64_c(uint64_t a, int b);
#pragma aux inl_rotr64 =	\
	"rot_bit:"		\
	"mov di,dx"		\
	"shr di,1"		\
	"rcr ax,1"		\
	"rcr bx,1"		\
	"rcr cx,1"		\
	"rcr dx,1"		\
	"dec si"		\
	"jnz rot_bit"		\
	modify [di]		\
	parm [ax bx cx dx] [si]	\
	value [ax bx cx dx]

