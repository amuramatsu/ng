; $Id: rawgetc.asm,v 1.1 2000/06/27 01:47:58 amura Exp $
;
;	int rawgetc();	Get one char from keyboard. If no keyin, return -1.
;
;	1990.02.11	Created by S.Yoshida.
;			With original idea from K.Takano.
;	1990.03.15	Modified for C-SPC by K.Takano & S.Yoshida
;	1990.03.31	Modified for Turbo-C by A.Shirahashi
;
;	A> masm [-DPC9801|-DIBMPC] -Mx rawgetc.asm,rawgetc.obj,nul,nul
;
; $Log: rawgetc.asm,v $
; Revision 1.1  2000/06/27 01:47:58  amura
; Initial revision
; 

;		(from)		1990.03.31  by A.Shirahashi
RAWGETC_TEXT	segment	byte public 'CODE'
DGROUP	group	_DATA,_BSS
	assume	cs:RAWGETC_TEXT,ds:DGROUP
RAWGETC_TEXT	ends
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
RAWGETC_TEXT	segment	byte public 'CODE'
_rawgetc	proc	far
;		(to)		1990.03.31  by A.Shirahashi

		mov	ah, 06h
		mov	dl, 0ffh
		int     21h
		mov	ah, 0
		jnz	L1
		mov	ax, -1

		ifdef	PC9801	; 1990.03.15  by K.Takano
		jmp	short done

L1:		cmp	al, ' '
		jne	done
		mov	ah, 02h
		int	18h
		and	ax, 0010h
		xor	al, 10h
		shl	al, 1
done:
;		(from)		1991.06.02  by Y.Koyanagi
		push	ax
		mov	ax, 040ah
		int	18h
		and	ah, 02h	; NFER key
		pop	ax
		jz	L2
		or	ax, 0100h ; METABIT
L2:
;		(to)		1991.06.02  by Y.Koyanagi
		endif		; 1990.03.15  by K.Takano

		ifdef	IBMPC	; 1990.03.15  by S.Yoshida
		jmp	short done

L1:		cmp	al, ' '
		jne	done
		mov	ah, 02h
		int	16h
		and	ax, 0004h
		xor	al, 04h
		jz	done
		mov	al, ' '
done:
		endif		; 1990.03.15  by S.Yoshida.

;		(from)		1990.03.31  by A.Shirahashi
		ifndef PC9801
		ifndef IBMPC
L1:
		endif
		endif
		ret
_rawgetc	endp
RAWGETC_TEXT	ends
_DATA	segment word public 'DATA'
s@	label	byte
_DATA	ends
RAWGETC_TEXT	segment	byte public 'CODE'
RAWGETC_TEXT	ends
	public	_rawgetc
		end
;		(to)		1990.03.31  by A.Shirahashi
