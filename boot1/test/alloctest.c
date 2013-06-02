#include "malloc.h"

int
main(void)
{
	void *ptrs[64];
	int i;

	ptrs[0] = malloc(127);
	ptrs[1] = malloc(512);
	ptrs[2] = malloc(60);
	free(ptrs[1], 512);
	ptrs[3] = malloc(64);
	ptrs[4] = malloc(128);

	for (i = 0; i < 5; i++) {
		printf("Pointer %.2d: %p\n", i, ptrs[i]);
	}

	return 0;
}
