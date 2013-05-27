CAT?=	cat
RM?=	rm -f
CP?=	cp

all: clean_bootimg bootimg.bin

bootimg.bin:
	@echo ------------------------------------------------
	make -C genhdr
	@echo ------------------------------------------------
	make -C boot0
	@echo ------------------------------------------------
	make -C boot1
	@echo ------------------------------------------------
	$(CP) boot1/boot1.bin boot1.bin
	genhdr/genhdr boot1.bin
	$(CAT) boot0/boot0.bin boot1.bin dummy512k > bootimg.bin

clean_bootimg:
	$(RM) bootimg.bin
	$(RM) boot1.bin

clean: clean_bootimg
	make -C genhdr clean
	make -C boot0 clean
	make -C boot1 clean
