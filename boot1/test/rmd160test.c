#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "rmd160.h"



struct hmac_test {
	const char *key;
	const char *data;
	const char *res;
};

struct hmac_test hmac_tests[] = {
	{
		.key  = "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"
			"0b0b0b0b",
		.data = "4869205468657265",
		.res  =	"24cb4bd67d20fc1a5d2ed7732dcc3937"
			"7f0a5668"
	},
	{
		.key  = "4a656665",
		.data = "7768617420646f2079612077616e7420"
			"666f72206e6f7468696e673f",
		.res  = "dda6c0213a485a9e24f4742064a7f033"
			"b43c4069"
	},
	{
		.key  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaa",
		.data = "dddddddddddddddddddddddddddddddd"
			"dddddddddddddddddddddddddddddddd"
			"dddddddddddddddddddddddddddddddd"
			"dddd",
		.res  = "b0b105360de759960ab4f35298e116e2"
			"95d8e7c1"
	},
	{
		.key  = "0102030405060708090a0b0c0d0e0f10"
			"111213141516171819",
		.data = "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
			"cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
			"cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
			"cdcd",
		.res  = "d5ca862f4d21d5e610e18b4cf1beb97a"
			"4365ecf4"
	},
	{
		.key  = "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
			"0c0c0c0c",
		.data = "546573742057697468205472756e6361"
			"74696f6e",
		.res  = "7619693978f91d90539ae786"
	},
	{
		.key  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		.data = "54657374205573696e67204c61726765"
			"72205468616e20426c6f636b2d53697a"
			"65204b6579202d2048617368204b6579"
			"204669727374",
		.res  = "6466ca07ac5eac29e1bd523e5ada7605"
			"b791fd8b"
	},
	{
		.key  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		.data = "54657374205573696e67204c61726765"
			"72205468616e20426c6f636b2d53697a"
			"65204b657920616e64204c6172676572"
			"205468616e204f6e6520426c6f636b2d"
			"53697a652044617461",
		.res  = "69ea60798d71616cce5fd0871e23754c"
			"d75d5a0a"
	},
	{
		.key  = "00112233445566778899aabbccddeeff"
			"01234567",
		.data = "6d65737361676520646967657374",
		.res  = "f83662cc8d339c227e600fcd636c57d2"
			"571b1c34"
	}
};

int
hex2buf(uint8_t *dst, const char *hex)
{
	char buf[3];
	int dstlen = 0;
	int len = strlen(hex);
	int i;

	buf[2] = '\0';

	for (i = 0; i < len; i += 2) {
		memcpy(buf, &hex[i], 2);
		dst[dstlen++] = (uint8_t)strtoul(buf, NULL, 16);
	}

	return dstlen;
}

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


void
run_hmac_tests(void)
{
	uint8_t key[2048];
	uint8_t data[2048];
	uint8_t md[1024];
	uint8_t res[1024];
	struct hmac_test *test;
	int ntests = sizeof(hmac_tests)/sizeof(struct hmac_test);
	int klen;
	int reslen;
	int datalen;
	int i;

	printf("-- HMAC-RIPEMD160 tests (RFC 2286) --\n");
	test = &hmac_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		datalen = hex2buf(data, test->data);
		reslen = hex2buf(res, test->res);

		rmd160_hmac(md, key, klen, data, datalen);

		printf("Test %d: ", ++i);
		if (memcmp(md, res, reslen) != 0) {
			printf("Mismatch!\n");
			printf("Expected: ");
			print_hex(res, reslen);
			printf("Got:      ");
			print_hex(md,  reslen);
		} else {
			printf("Ok!\n");
		}
		++test;
	}
}

void
single_hash(char *msg)
{
        uint8_t hashcode[64];
        int j;

        rmd160_hash(hashcode, (uint8_t *)msg, strlen(msg));
        for (j=0; j < 20; j++)
                printf("%02x", hashcode[j]);
        printf("\n");
}


int
main(int argc, char *argv[])
{
	if (argc < 2) {
		run_hmac_tests();
	} else {
		single_hash(argv[1]);
	}

	return 0;
}
