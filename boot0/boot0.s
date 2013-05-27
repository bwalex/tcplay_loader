; Copyright (c) 2013 Alex Hornung <alex@alexhornung.com>.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
;
; 1. Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in
;    the documentation and/or other materials provided with the
;    distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
; ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
; FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
; COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
; INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
; BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
; AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
; OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
; SUCH DAMAGE.
;


[BITS 16]
[ORG 0x7C00]

%define OFF_LOADSZ	0x00
%define OFF_START	0x04
%define OFF_CRC		0x08
%define OFF_CRCSZ	0x0c
%define OFF_OFF		0x10
%define OFF_SKIP	0x14
%define OFF_ENTRY_OFF	0x18
%define OFF_ENTRY_SEG	0x1A

_start:
	; Save bios boot device number
	mov [biosdev], dl

	; Clear screen
	mov  ax, 0x0600
	int 0x10

	; Print banner
	mov  si, BANNERMSG
	call print

	; Load sector 2, which contains the table
	; with the size of the whole chunk.
	mov cl, 2
	mov al, 1
	mov bx, 0x50
	mov es, bx
	xor bx, bx
	call copy_sectors
	jc error_disk

	; Load size and determine number of sectors
	; to read, and read them.
	mov eax, [es:bx]
	shr eax, 9
	inc eax
	call copy_sectors
	jc error_disk

	; Calculate crc
	xor bx, bx
	mov ecx, [es:bx+OFF_CRCSZ]
	mov esi, [es:bx+OFF_SKIP]
	add esi, 0x500
	mov edi, [es:bx+OFF_CRC]
	call crc32
	cmp ebx, edi
	jne error_crc

	; Load ax <- offset, bx <- segment
	xor bx, bx
	mov ax, [es:bx+OFF_ENTRY_OFF]
	mov bx, [es:bx+OFF_ENTRY_SEG]

	; Set up a resonable stack pointer
	mov sp, 0x7400

	; Place biosdev back into dl for boot1
	mov dl, [biosdev]

	; Return far (to startup code of boot1)
	push bx
	push ax
	retf

end:
	jmp end

error_disk:
	mov si, DISKERRMSG
	call print	
	jmp end

error_crc:
	mov si, CRCERRMSG
	call print
	jmp end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Print string
; in:    si  nul-terminated string
; used:  ax, bx
print:
	lodsb
	or   al, al
	jz   print_ret
	call putc
	jmp  print
print_ret:
	ret

putc:
	mov ah, 0x0E
	mov bx, 0x0007
	int 0x10
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copy sector(s)
; in:     cl  sector number (start)
; in:     al  sector count
; in:  es:bx  destination buffer
; out:    ah  return code
; out:    al  actual sectors read
; out:    cf  set if error
copy_sectors:
	mov ah, 0x02
	xor ch, ch
	xor dx, dx
	mov dl, [biosdev]
	int 0x13
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Calculate CRC32
; in:    si  buffer
; in:    cx  size
; out:  ebx  crc32
; used: eax, edx
crc32:
        ;mov ebx, 0xffffffff
	xor ebx, ebx
	dec ebx

crc_loop:
	; Load next byte and XOR it into CRC
        xor eax, eax
        lodsb
        xor ebx, eax

	; Set inner loop counter
        mov  dl, 4
crc_inner_loop:
	; EAX <- CRC >> 2
        mov eax, ebx
        shr eax, 2

	; Load table entry at index CRC & 0x3
        and  bx, 3
        shl  bx, 2
        mov ebx, [crc_table+bx]

	; CRC <- table[CRC & 0x03] ^ (CRC >> 2)
        xor ebx, eax

	dec dl
        jnz crc_inner_loop

	loop crc_loop

        xor ebx, 0xffffffff
	ret

crc_table:
DD 0x00000000
DD 0x76dc4190
DD 0xedb88320
DD 0x9b64c2b0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Data
biosdev: DB 0


; Literal pool
BANNERMSG   DB 'tcplay boot0',      0x0D, 0x0A, 0
DISKERRMSG  DB 'Disk read error',   0x0D, 0x0A, 0
CRCERRMSG   DB 'CRC error',         0x0D, 0x0A, 0


; Pad with zeros and add signature
TIMES 510 - ($ - $$) DB 0
DW 0xAA55
