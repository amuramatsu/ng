; $Id: rawgetc.asm,v 1.3 2001/11/24 08:24:29 amura Exp $
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
; Revision 1.3  2001/11/24 08:24:29  amura
; Rewrite all sources (for msdos port)
;
; Revision 1.2  2001/11/23 11:56:50  amura
; Rewrite all sources
;
; Revision 1.1.1.1  2000/06/27 01:47:58  amura
; import to CVS
; 

RAWGETC_TEXT	segment	byte public 'CODE'
	public	_rawgetc

	assume	cs:RAWGETC_TEXT,ds:NOTHING
_rawgetc	proc	far
		mov	ah, 06h
		mov	dl, 0ffh
		int     21h
		mov	ah, 0
		jnz	L1
		mov	ax, -1

ifdef	PC9801	; 1990.03.15  by K.Takano
		jmp	short done

	L1:	cmp	al, ' '
		jne	done
		mov	ah, 02h
		int	18h
		and	ax, 0010h
		xor	al, 10h
		shl	al, 1
	done:
		push	ax
		mov	ax, 040ah
		int	18h
		and	ah, 02h	; NFER key
		pop	ax
		jz	L2
		or	ax, 0100h ; METABIT
	L2:
endif		; 1990.03.15  by K.Takano

ifdef	IBMPC	; 1990.03.15  by S.Yoshida
		jmp	short done

	L1:	cmp	al, ' '
		jne	done
		mov	ah, 02h
		int	16h
		and	ax, 0004h
		xor	al, 04h
		jz	done
		mov	al, ' '
	done:
endif		; 1990.03.15  by S.Yoshida.

ifndef	PC9801
ifndef	IBMPC
	L1:
endif
endif
		ret
_rawgetc	endp
RAWGETC_TEXT	ends

end
