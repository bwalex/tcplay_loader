#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "generic_xts.h"


struct aes_test {
	const char *key;
	const char *pt;
	const char *ct;
};

struct aes_test aes_tests[] = {
	{
		.key = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		.pt  = "6bc1bee22e409f96e93d7e117393172a",
		.ct  = "f3eed1bdb5d2a03c064b5a7e3db181f8"
	},
	{
		.key = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		.pt  = "AE2D8A571E03AC9C9EB76FAC45AF8E51",
		.ct  = "591CCB10D410ED26DC5BA74A31362870"
	},
	{
		.key = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		.pt  = "30C81C46A35CE411E5FBC1191A0A52EF",
		.ct  = "B6ED21B99CA6F4F9F153E7B1BEAFED1D"
	},
	{
		.key = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		.pt  = "F69F2445DF4F9B17AD2B417BE66C3710",
		.ct  = "23304B7A39F9F3FF067D8D8F9E24ECC7"
	}
};

struct xts_test {
	const char *key;
	const char *iv;
	const char *pt;
	const char *ct;
	uint32_t   unit_no;
};

struct xts_test xts_tests[] = {
	{
		.key = "1ea661c58d943a0e4801e42f4b0947149e7f9f8e3e68d0c7505210bd311a0e7cd6e13ffdf2418d8d1911c004cda58da3d619b7e2b9141e58318eea392cf41b08",
		.iv  = "adf8d92627464ad2f0428e84a9f87564",
		.pt  = "2eedea52cd8215e1acc647e810bbc3642e87287f8d2e57e36c0a24fbc12a202e",
		.ct  = "cbaad0e2f6cea3f50b37f934d46a9b130b9d54f07e34f36af793e86f73c6d7db"
	},
	{
		.key = "e149be00177d76b7c1d85bcbb6b5054ee10b9f51cd73f59e0840628b9e7d854e2e1c0ab0537186a2a7c314bbc5eb23b6876a26bcdbf9e6b758d1cae053c2f278",
		.iv  = "0ea18818fab95289b1caab4e61349501",
		.pt  = "f5f101d8e3a7681b1ddb21bd2826b24e32990bca49b39291b5369a9bca277d75",
		.ct  = "5bf2479393cc673306fbb15e72600598e33d4d8a470727ce098730fd80afa959"
	},
};


struct xts_test xts_unit_no_tests[] = {
	{
		.key = "ef010ca1a3663e32534349bc0bae62232a1573348568fb9ef41768a7674f507a727f98755397d0e0aa32f830338cc7a926c773f09e57b357cd156afbca46e1a0",
		.pt  = "ed98e01770a853b49db9e6aaf88f0a41b9b56e91a5a2b11d40529254f5523e75",
		.ct  = "ca20c55e8dc149687d2541de39c3df6300bb5a163c10ced3666b1357db8bd39d",
		.unit_no = 187,
	},
	{
		.key = "bf04a34622e657b6cb7fc5acde6404bd20e36e8e6cb41dae9555e7bb669262f39f87d4ebdf3068c987b9c18de017fa575d17b205bca4aab97e42c95c26b28235",
		.pt  = "dd570336bad9a4f9eb88489b9cc811020780f6be6a094832417cb2032e70ea3ff80bbca51c97f576b8eb92ba06fc7d17",
		.ct  = "b21ce97c9db4aafe04b16bc1eb27508481abb417585dc55a4901f029a5dcffc4510b302fc70e2dcb75ddc7aefd714e70",
		.unit_no = 5,
	},
	{
		.key = "8caf607277f8471ade3f4eb634138f2222fb6ae71af6d4e2bad4c7e440cadf9ff697dd07315d37b723617fffb36d83645bc09af708810c2423d3ddb469d10425",
		.pt  = "1540a5370d3b83ec938932612ac42faa52a38a2be39dd14547519757fed9b18eb1a05577ce68632f88741d29e8042f48",
		.ct  = "f6b2f1996c97f1d90af812ddac031c0b15d0569e0ba35b318e35e6661b7ca62db5591dfd86854105866191726d19d067",
		.unit_no = 26,
	},
	{
		.key = "10c9da9064e9c777e1a7bfb90e1c6e9f5472cb5add6e3bb9ff46277f76614bd5064c5d95ae185cee9fd0d6d4d48a2294deaf230f454fa08fc9292bdf30d3ecc5",
		.pt  = "a0c2e18f86d373ae57abb5906cf3aed27779d66997e8e98ffc7fe23fb87697b5",
		.ct  = "1ef6cd636f171adb4f991ff4d9af1865158ce42607d24cd7328e8fd7996b5ee9",
		.unit_no = 78,
	},
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
run_xts_tests(void)
{
	uint8_t key[2048];
	uint8_t iv[2048];
	uint8_t ct[2048];
	uint8_t pt[2048];
	uint8_t ct_res[2048];
	uint8_t pt_res[2048];
	struct xts_ctx ctx;
	struct xts_test *test;
	int ntests;
	int i = 0;
	int klen, ptlen, ivlen;

	ntests = sizeof(xts_tests)/sizeof(struct xts_test);
	printf("-- XTS-AES-256 tests (XTSVS) (encrypt) --\n");
	test = &xts_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		ivlen = hex2buf(iv,  test->iv);
		ptlen = hex2buf(pt,  test->pt);
		hex2buf(ct,  test->ct);

		printf("Test %d: ", ++i);
		xts_init(&ctx, aes256_init, aes256_encrypt_ecb, aes256_decrypt_ecb, 16, key, klen);
		memcpy(ct_res, pt, ptlen);
		xts_encrypt(&ctx, ct_res, ptlen, iv, ivlen);
		if (memcmp(ct_res, ct, ptlen) != 0) {
			printf("Mismatch!\n");
			printf("Expected: ");
			print_hex(ct, ptlen);
			printf("Got:      ");
			print_hex(ct_res, ptlen);
		} else {
			printf("Ok!\n");
		}
		++test;
	}

	ntests = sizeof(xts_tests)/sizeof(struct xts_test);
	printf("-- XTS-AES-256 tests (XTSVS) (decrypt) --\n");
	test = &xts_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		ivlen = hex2buf(iv,  test->iv);
		ptlen = hex2buf(pt,  test->pt);
		hex2buf(ct,  test->ct);

		printf("Test %d: ", ++i);
		xts_init(&ctx, aes256_init, aes256_encrypt_ecb, aes256_decrypt_ecb, 16, key, klen);
		memcpy(pt_res, ct, ptlen);
		xts_decrypt(&ctx, pt_res, ptlen, iv, ivlen);
		if (memcmp(pt_res, pt, ptlen) != 0) {
			printf("Mismatch!\n");
			printf("Expected: ");
			print_hex(pt, ptlen);
			printf("Got:      ");
			print_hex(pt_res, ptlen);
		} else {
			printf("Ok!\n");
		}
		++test;
	}


	ntests = sizeof(xts_unit_no_tests)/sizeof(struct xts_test);
	printf("-- XTS-AES-256 tests (XTSVS) (encrypt, using unit number) --\n");
	test = &xts_unit_no_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		ptlen = hex2buf(pt,  test->pt);
		hex2buf(ct,  test->ct);

		printf("Test %d: ", ++i);
		xts_init(&ctx, aes256_init, aes256_encrypt_ecb, aes256_decrypt_ecb, 16, key, klen);
		memcpy(ct_res, pt, ptlen);
		xts_encrypt(&ctx, ct_res, ptlen, &test->unit_no, sizeof(test->unit_no));
		if (memcmp(ct_res, ct, ptlen) != 0) {
			printf("Mismatch!\n");
			printf("Expected: ");
			print_hex(ct, ptlen);
			printf("Got:      ");
			print_hex(ct_res, ptlen);
		} else {
			printf("Ok!\n");
		}
		++test;
	}

	i = 0;
	ntests = sizeof(xts_unit_no_tests)/sizeof(struct xts_test);
	printf("-- XTS-AES-256 tests (XTSVS) (decrypt, using unit number) --\n");
	test = &xts_unit_no_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		ptlen = hex2buf(pt,  test->pt);
		hex2buf(ct,  test->ct);

		printf("Test %d: ", ++i);
		xts_init(&ctx, aes256_init, aes256_encrypt_ecb, aes256_decrypt_ecb, 16, key, klen);
		memcpy(pt_res, ct, ptlen);
		xts_decrypt(&ctx, pt_res, ptlen, &test->unit_no, sizeof(test->unit_no));
		if (memcmp(pt_res, pt, ptlen) != 0) {
			printf("Mismatch!\n");
			printf("Expected: ");
			print_hex(pt, ptlen);
			printf("Got:      ");
			print_hex(pt_res, ptlen);
		} else {
			printf("Ok!\n");
		}
		++test;
	}
}

void
run_aes_tests(void)
{
	uint8_t key[2048];
	uint8_t iv[2048];
	uint8_t ct[2048];
	uint8_t pt[2048];
	uint8_t ct_res[2048];
	uint8_t pt_res[2048];
	aes256_context ctx;
	struct aes_test *test;
	int ntests;
	int i = 0;
	int klen, ptlen;

	ntests = sizeof(aes_tests)/sizeof(struct aes_test);
	printf("-- AES-256-ECB tests (encrypt) --\n");
	test = &aes_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		ptlen = hex2buf(pt,  test->pt);
		hex2buf(ct,  test->ct);

		printf("Test %d: ", ++i);
		aes256_init(&ctx, key);
		memcpy(ct_res, pt, ptlen);
		aes256_encrypt_ecb(&ctx, ct_res);
		if (memcmp(ct_res, ct, ptlen) != 0) {
			printf("Mismatch!\n");
			printf("Expected: ");
			print_hex(ct, ptlen);
			printf("Got:      ");
			print_hex(ct_res, ptlen);
		} else {
			printf("Ok!\n");
		}
		++test;
	}

	i = 0;
	ntests = sizeof(aes_tests)/sizeof(struct aes_test);
	printf("-- AES-256-ECB tests (decrypt) --\n");
	test = &aes_tests[0];
	while (ntests-- > 0) {
		klen = hex2buf(key, test->key);
		ptlen = hex2buf(pt,  test->pt);
		hex2buf(ct,  test->ct);

		printf("Test %d: ", ++i);
		aes256_init(&ctx, key);
		memcpy(pt_res, ct, ptlen);
		aes256_decrypt_ecb(&ctx, pt_res);
		if (memcmp(pt_res, pt, ptlen) != 0) {
			printf("Mismatch!\n");
			printf("Expected: ");
			print_hex(pt, ptlen);
			printf("Got:      ");
			print_hex(pt_res, ptlen);
		} else {
			printf("Ok!\n");
		}
		++test;
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		run_aes_tests();
		run_xts_tests();
	} else {
	}

	return 0;
}
