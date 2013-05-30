; Calls to these functions are generated for shift right and shift left
; on uint64_t's respectively by the Watcom Compiler.
; Getting it to link to the versions it ships with seems a major PITA,
; so here go my versions of it. Code is the same, because, well, there
; are no two ways to go about it.

U8SHIFT segment word public 'CODE'
__U8RS proc near public
	or si,si
	jz rshift_finish
rshift_bit:
	; shr puts the shifted out LSB into CF
	shr ax,1
	; rcr puts the CF into the MSB, then loads the shifted out LSB
	; into the CF
	rcr bx,1
	rcr cx,1
	rcr dx,1
	dec si
	jnz rshift_bit
rshift_finish:
	ret
__U8RS endp


__U8LS proc near public
	or si,si
	jz lshift_finish
lshift_bit:
	; shl puts the shifted out MSB into CF
	shl dx,1
	; rcl puts the CF into the LSB, then loads the shifted out MSB
	; into the CF
	rcl cx,1
	rcl bx,1
	rcl ax,1
	dec si
	jnz lshift_bit
lshift_finish:
	ret
__U8LS endp
U8SHIFT ends
end
