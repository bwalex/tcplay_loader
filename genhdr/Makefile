CFLAGS?=	-O0 -g

CC?=	gcc
AS?=	as
LD?=	ld
RM?=	rm -f

genhdr: genhdr.o crc32.o
	$(CC) $(CFLAGS_NATIVE) $^ -o $@

clean:
	$(RM) genhdr
	$(RM) *.o
