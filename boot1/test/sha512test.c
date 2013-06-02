#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "sha512.h"



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
		.res  =	"87aa7cdea5ef619d4ff0b4241a1d6cb0"
			"2379f4e2ce4ec2787ad0b30545e17cde"
			"daa833b7d6b8a702038b274eaea3f4e4"
			"be9d914eeb61f1702e696c203a126854"
	},
	{
		.key  = "4a656665",
		.data = "7768617420646f2079612077616e7420"
			"666f72206e6f7468696e673f",
		.res  = "164b7a7bfcf819e2e395fbe73b56e0a3"
			"87bd64222e831fd610270cd7ea250554"
			"9758bf75c05a994a6d034f65f8f0e6fd"
			"caeab1a34d4a6b4b636e070a38bce737"
	},
	{
		.key  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaa",
		.data = "dddddddddddddddddddddddddddddddd"
			"dddddddddddddddddddddddddddddddd"
			"dddddddddddddddddddddddddddddddd"
			"dddd",
		.res  = "fa73b0089d56a284efb0f0756c890be9"
			"b1b5dbdd8ee81a3655f83e33b2279d39"
			"bf3e848279a722c806b485a47e67c807"
			"b946a337bee8942674278859e13292fb"
	},
	{
		.key  = "0102030405060708090a0b0c0d0e0f10"
			"111213141516171819",
		.data = "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
			"cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
			"cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
			"cdcd",
		.res  = "b0ba465637458c6990e5a8c5f61d4af7"
			"e576d97ff94b872de76f8050361ee3db"
			"a91ca5c11aa25eb4d679275cc5788063"
			"a5f19741120c4f2de2adebeb10a298dd"
	},
	{
		.key  = "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
			"0c0c0c0c",
		.data = "546573742057697468205472756e6361"
			"74696f6e",
		.res  = "415fad6271580a531d4179bc891d87a6"
	},
	{
		.key  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaa",
		.data = "54657374205573696e67204c61726765"
			"72205468616e20426c6f636b2d53697a"
			"65204b6579202d2048617368204b6579"
			"204669727374",
		.res  = "80b24263c7c1a3ebb71493c1dd7be8b4"
			"9b46d1f41b4aeec1121b013783f8f352"
			"6b56d037e05f2598bd0fd2215d6a1e52"
			"95e64f73f63f0aec8b915a985d786598"
	},
	{
		.key  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaa",
		.data = "54686973206973206120746573742075"
			"73696e672061206c6172676572207468"
			"616e20626c6f636b2d73697a65206b65"
			"7920616e642061206c61726765722074"
			"68616e20626c6f636b2d73697a652064"
			"6174612e20546865206b6579206e6565"
			"647320746f2062652068617368656420"
			"6265666f7265206265696e6720757365"
			"642062792074686520484d414320616c"
			"676f726974686d2e",
		.res  = "e37b6a775dc87dbaa4dfa9f96e5e3ffd"
			"debd71f8867289865df5a32d20cdc944"
			"b6022cac3c4982b10d5eeb55c3e4de15"
			"134676fb6de0446065c97440fa8c6a58"
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
	int i = 0;

	printf("-- HMAC-SHA512 tests (RFC 4231) --\n");
	test = &hmac_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		datalen = hex2buf(data, test->data);
		reslen = hex2buf(res, test->res);

		sha512_hmac(md, key, klen, data, datalen);

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

        sha512_hash(hashcode, (uint8_t *)msg, strlen(msg));
        for (j=0; j < 64; j++)
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
