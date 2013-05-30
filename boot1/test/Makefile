CC?=gcc
RM?=rm -f
SRC_PATH?=..

all: sha512test rmd160test
	./sha512test
	./rmd160test

sha512ref: sha384-512.c
	$(CC) sha384-512.c -o sha512ref

sha512.o: $(SRC_PATH)/sha512.c
	$(CC) -D_STANDALONE -I$(SRC_PATH) $(SRC_PATH)/sha512.c -c

rmd160.o: $(SRC_PATH)/rmd160.c
	$(CC) -D_STANDALONE -I$(SRC_PATH) $(SRC_PATH)/rmd160.c -c

sha512test: sha512test.c sha512.o
	$(CC) -o sha512test -I$(SRC_PATH) sha512test.c sha512.o

rmd160test: rmd160test.c rmd160.o
	$(CC) -o rmd160test -I$(SRC_PATH) rmd160test.c rmd160.o

ascii2hex: ascii2hex.c
	$(CC) -o ascii2hex ascii2hex.c

clean:
	$(RM) *.o
	$(RM) sha512ref
	$(RM) sha512test
	$(RM) rmd160test
	$(RM) ascii2hex
