; $Id: alloca.asm,v 1.2 2001/11/23 11:56:49 amura Exp $
;-------------------------------------------------------------
; alloca.asm: void * alloca(int)  for Turbo-C 2.0 (large model)
;
; CAUTION:
;	* Standard stack frame is required in caller function
;	  You had better use -k (Standard stack frame) option.
;
;-------------------------------------------------------------
; $Log: alloca.asm,v $
; Revision 1.2  2001/11/23 11:56:49  amura
; Rewrite all sources
;
; Revision 1.1.1.1  2000/06/27 01:47:58  amura
; import to CVS
; 

ALLOCA_TEXT	segment byte public 'CODE'
	public	_alloca
	extern _stklen:word

	asuume	cs:ALLOCA_TEXT,ds:NONE
_alloca		proc	far
		pop	cx		;retrun address
		pop	dx		;;return segment
		pop	ax		;argument
		or	ax, ax		;test if 0
		jz	eret		;no allocation required
		inc	ax		;
		and	al, 0FEh	;force Even
		mov	bx, sp		;caller function stack position
		sub	bx, ax		;get space
		jc	eret		;under flow!

		cmp	bx, 12		;;12 bytes use later
		jc	eret		;;
		mov	es, seg _strlen
		mov	ax, word ptr es:_stklen
		cmp	ax, bx		;;
		jb	eret		;;

		mov	ax, bx		; <- pointer to allocated memory
		xchg	sp, bx		; sp <- new pos / bx <- old pos
		push	ss:[bx+4]	; for safe..
		push	ss:[bx+2]	; for DI (may be)
		push	ss:[bx+0]	; for SI (may be)
		mov	bx, dx
		mov	dx, ss		;;allocated mem segment address
goback:		push	ax		; restore argument space
		push	bx		;;restor return segment
		push	cx		; restore return address
		ret			; bye-be

eret:		xor	ax, ax		; NULL
		mov	bx, dx		;;save return segment
		mov	dx, ax		;;NULL
		jmp	short goback	;

_alloca		endp
ALLOCA_TEXT	ends	
end
