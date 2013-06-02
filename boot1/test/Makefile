CC?=gcc
RM?=rm -f
SRC_PATH?=..

CFLAGS= -O4 -flto

all: sha512test rmd160test pbkdf2test aes256test
	./sha512test
	./rmd160test
	./aes256test

sha512ref: sha384-512.c
	$(CC) sha384-512.c -o sha512ref

sha512.o: $(SRC_PATH)/sha512.c
	$(CC) $(CFLAGS) -D_STANDALONE -I$(SRC_PATH) $(SRC_PATH)/sha512.c -c

rmd160.o: $(SRC_PATH)/rmd160.c
	$(CC) $(CFLAGS) -D_STANDALONE -I$(SRC_PATH) $(SRC_PATH)/rmd160.c -c

pbkdf2.o: $(SRC_PATH)/pbkdf2.c
	$(CC) $(CFLAGS) -D_STANDALONE -I$(SRC_PATH) $(SRC_PATH)/pbkdf2.c -c

generic_xts.o: $(SRC_PATH)/generic_xts.c
	$(CC) $(CFLAGS) -D_STANDALONE -I$(SRC_PATH) $(SRC_PATH)/generic_xts.c -c

aes256.o: $(SRC_PATH)/aes256.c
	$(CC) $(CFLAGS) -D_STANDALONE -I$(SRC_PATH) $(SRC_PATH)/aes256.c -c

sha512test: sha512test.c sha512.o
	$(CC) $(CFLAGS) -o sha512test -I$(SRC_PATH) sha512test.c sha512.o

rmd160test: rmd160test.c rmd160.o
	$(CC) $(CFLAGS) -o rmd160test -I$(SRC_PATH) rmd160test.c rmd160.o

pbkdf2test: pbkdf2test.c pbkdf2.o rmd160.o sha512.o
	$(CC) $(CFLAGS) -o pbkdf2test -I$(SRC_PATH) pbkdf2test.c pbkdf2.o rmd160.o sha512.o -lcrypto

aes256test: aes256test.c aes256.o generic_xts.o
	$(CC) $(CFLAGS) -o aes256test -I$(SRC_PATH) aes256test.c aes256.o generic_xts.o

ascii2hex: ascii2hex.c
	$(CC) -o ascii2hex ascii2hex.c

clean:
	$(RM) *.o
	$(RM) sha512ref
	$(RM) sha512test
	$(RM) rmd160test
	$(RM) pbkdf2test
	$(RM) aes256test
	$(RM) ascii2hex
