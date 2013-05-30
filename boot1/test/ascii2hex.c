#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

void
print_hex(uint8_t *buf, int bufsz)
{
	unsigned int byte;

	while (bufsz-- > 0) {
		byte = (int)*buf++;
		printf("%.2x", byte);
	}
	printf("\n");
}

int
main(int argc, char *argv[])
{
	if (argc == 2) {
		print_hex(argv[1], strlen(argv[1]));
	} else {
		exit(1);
	}

	return 0;
}
