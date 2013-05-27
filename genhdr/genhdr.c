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
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

uint32_t crc32(const void *buf, size_t size);

#define OFF_LOADSZ	0x00
#define OFF_START	0x04
#define OFF_CRC		0x08
#define OFF_CRCSZ	0x0c
#define OFF_OFF		0x10
#define OFF_SKIP	0x14

int
main(int argc, char *argv[])
{
	uint32_t crc;
	uint32_t off, skip;
	uint32_t sz, crcsz;
	struct stat sb;
	unsigned char *buf;
	int fd;

	if (argc < 2)
		exit(1);

	if ((fd = open(argv[1], O_RDWR)) < 0) {
		perror("open");
		exit(1);
	}

	if ((fstat(fd, &sb)) < 0) {
		perror("stat");
		exit(1);
	}

	sz = (size_t)sb.st_size;

	if ((buf = malloc(sz)) == NULL) {
		perror("malloc");
		exit(1);
	}

	if ((read(fd, buf, sz)) < 0) {
		perror("read");
		exit(1);
	}

	skip = *((uint32_t *)(buf + OFF_SKIP));
	crcsz = sz - skip;

	crc = crc32(buf+skip, crcsz);

	printf("------------------------------------------------\n");
	printf("-> Updating header for %s\n", argv[1]);
	printf("    + LOADSZ= %u\n", sz);
	printf("    + CRCSZ=  %u\n", crcsz);
	printf("    + CRC=    %x\n", crc);
	printf("    + SKIP=   %u\n", skip);
	printf("------------------------------------------------\n");

	lseek(fd, OFF_LOADSZ, SEEK_SET);
	if ((write(fd, &sz, sizeof(sz))) < sizeof(crc)) {
		perror("write");
		exit(1);
	}

	lseek(fd, OFF_CRCSZ, SEEK_SET);
	if ((write(fd, &crcsz, sizeof(crcsz))) < sizeof(crc)) {
		perror("write");
		exit(1);
	}

	lseek(fd, OFF_CRC, SEEK_SET);
	if ((write(fd, &crc, sizeof(crc))) < sizeof(crc)) {
		perror("write");
		exit(1);
	}

	close(fd);
	return 0;
}
