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

EXTERN main_    : near
EXTERN _biosdev : word

DGROUP group CONST,_DATA,DATA,_BSS,STACK

STARTUP	segment	word public 'CODE'
_cstart_ proc near public
	; Set up segment regs (DS=ES=SS)
	mov ax, DGROUP
	mov ds, ax
	mov es, ax
	mov ss, ax

	; Set up stack pointer
	mov sp, offset DGROUP:end_of_stack

	; Zero BSS region
        mov cx, offset DGROUP:_end
        mov di, offset DGROUP:_edata
        sub cx, di
        xor al, al
        rep stosb

	; Store the bios boot dev
	xor dh, dh
	mov [_biosdev], dx

	; Pass control to C code
	call main_
	jmp $
_cstart_ endp
STARTUP ends

CONST segment word public 'DATA'
CONST ends

_DATA segment word public 'DATA'
_DATA ends

DATA segment word public 'DATA'
DATA ends

_BSS segment word public 'BSS'
        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)
_BSS ends


STACK_SIZE      equ     2048

STACK   segment para stack 'STACK'
        db      (STACK_SIZE) dup(?)
end_of_stack:
STACK   ends

end
