; $Id: alloca.asm,v 1.1 2000/06/27 01:47:58 amura Exp $
;-------------------------------------------------------------
; alloca.asm: void * alloca(int)  for Turbo-C 2.0
;
; CAUTION:
;	* Standard stack frame is required in caller function
;	  You had better use -k (Standard stack frame) option.
;
; $Header: /var/cvsroot/ng/sys/msdos/alloca.asm,v 1.1 2000/06/27 01:47:58 amura Exp $
;-------------------------------------------------------------
; $Log: alloca.asm,v $
; Revision 1.1  2000/06/27 01:47:58  amura
; Initial revision
; 

	include	rules.asi

	Header@
	CSeg@

PubProc@  alloca, __CDECL__
	pop cx			;retrun address
	if LPROG
		pop dx		;;return segment
	endif
	pop ax			;argument
	or ax, ax		;test if 0
	jz eret			;no allocation required
	inc ax			;
	and al, 0FEh		;force Even
	mov bx, sp		;caller function stack position
	sub bx, ax		;get space
	jc eret			;under flow!

	if LDATA
		ExtSym@ _stklen, word, __CDECL__
		cmp bx, 12		;;12 bytes use later
		jc eret			;;
;		ifdef __HUGE__		;; old!
;			mov ax, seg _stklen@
;			mov es, ax	;;
;			mov ax, word ptr es:_stklen@
;		else			;;
		mov ax, word ptr DGROUP:_stklen@
		cmp ax, bx		;;
		jb eret			;;
	else				;;
		ExtSym@ __brklvl, word, __CDECL__
		mov ax, word ptr DGROUP:__brklvl@
		add ax, 10		;; 12 bytes usr later
		cmp ax, bx		;;
		jnc eret		;;
	endif				;;

	mov ax, bx		; <- pointer to allocated memory
	xchg sp, bx		; sp <- new pos / bx <- old pos
	push ss:[bx+4]		; for safe..
	push ss:[bx+2]		; for DI (may be)
	push ss:[bx+0]		; for SI (may be)
	if LPROG
		mov bx, dx	;;save return segment
	endif
	if LDATA
		mov dx, ss	;;allocated mem segment address
	endif			;;
goback:	push ax			; restore argument space
	if LPROG		;;
		push bx		;;restor return segment
	endif			;;
	push cx			; restore return address
	ret			; bye-be
eret:	xor ax, ax		; NULL
	if LPROG		;;
		mov bx, dx	;;save return segment
	endif			;;
	if LDATA		;;
		mov dx, ax	;;NULL
	endif			;;
	jmp short goback	;

EndProc@  alloca, __CDECL__

	CSegEnd@
end
