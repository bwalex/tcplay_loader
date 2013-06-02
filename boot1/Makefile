WAS?=wasm
WCC?=wcl
RM?= rm -f

COPTFLAGS?=-osl -s
CWARNFLAGS?=-wx
CFLAGS?= -ms -ecd -3 -lr -zl $(COPTFLAGS) $(CWARNFLAGS) -zq -q -i=$(WATCOM)/h -zm
ASFLAGS?=-zq -i=$(WATCOM)/src/startup

ASM_OBJS= hdr.o startup.o u8shift.o
C_OBJS=  boot1.o bios.o fun.o crc32_tiny.o malloc.o
C_OBJS+= aes256.o rmd160.o sha512.o pbkdf2.o generic_xts.o

boot1.bin: $(ASM_OBJS) $(C_OBJS)
	$(WCC) $(CFLAGS) -fe=$@ $^ @link.scr

$(ASM_OBJS): %.o: %.s
	$(WAS) $(ASFLAGS) $< -fo=$@

$(C_OBJS): %.o: %.c
	$(WCC) $(CFLAGS) -c $< -fo=$@

clean:
	$(RM) *.o
	$(RM) *.err
	$(RM) boot1.map boot1.err
	$(RM) boot1.bin
