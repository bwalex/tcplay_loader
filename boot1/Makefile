WAS?=wasm
WCC?=wcl
RM?= rm -f

COPTFLAGS?=-osl -s
CWARNFLAGS?=-wx
CFLAGS?= -ms -ecd -3 -lr -zl $(COPTFLAGS) $(CWARNFLAGS) -zq -q -i=$(WATCOM)/h -zm
ASFLAGS?=-zq -i=$(WATCOM)/src/startup

ASM_OBJS= hdr.o startup.o
C_OBJS= boot1.o bios.o crc32_tiny.o aes256.o

boot1.bin: $(ASM_OBJS) $(C_OBJS)
	$(WCC) $(CFLAGS) -fe=$@ $^ @link.scr

$(ASM_OBJS): %.o: %.s
	$(WAS) $(ASFLAGS) $< -fo=$@

$(C_OBJS): %.o: %.c
	$(WCC) $(CFLAGS) -c $< -fo=$@

clean:
	$(RM) *.o
	$(RM) boot1.map boot1.err
	$(RM) boot1.bin
