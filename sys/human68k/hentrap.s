* $Id: hentrap.s,v 1.2 2003/02/22 08:09:47 amura Exp $
*
*	Public Domain Software
*
*
*	Written by Masatoshi Yoshizawa (Yoz.) for GAPO jstevie Human68k
*	Added abort trap by Sawayanagi Yosirou <willow@saru.cc.u-tokyo.ac.jp>
*
*

		.globl	_hentrap,_iskmode

		.text
_hentrap:
		tst.l	4(SP)
		beq	hentrap0

		clr.l	-(sp)
		move.l	#1,-(sp)
		dc.w	$ff22		* KNJCTRL(1,0)
		addq.l	#8,sp

		clr.l	henflag

		pea	hentrapmain(pc)
		move.w	#$ff18,-(sp)
		dc.w	$ff25		* intvcs
		addq.l	#6,sp

		move.l	d0,oldvect

		pea	aborthentrap
		move.w	#$fff2,-(sp)
		dc.w	$ff25		* intvcs
		addq.l	#6,sp

		move.l	d0,abortvect

		rts

hentrap0:
		move.l	abortvect,-(sp)
		move.w	#$fff2,-(sp)
		dc.w	$ff25		* intvcs
		addq.l	#6,sp

		move.l	oldvect,-(sp)
		move.w	#$ff18,-(sp)
		dc.w	$ff25		* intvcs
		addq.l	#6,sp

		rts

_iskmode:
		move.l	henflag,d0
		rts

hentrapmain:
		move.w	(A6),D0
		beq	henopen		* mode window open
		cmp.w	#3,D0
		beq	henclose	* mode window close
hencont:
		move.l	oldvect,A0
		jmp	(A0)

henopen:
		move.l	#-1,henflag
		bra	hencont

henclose:
		clr.l	henflag
		bra	hencont

aborthentrap:
		bsr	hentrap0
		move.l	abortvect,A0
		jmp	(A0)

		.bss
henflag:
		ds.l	1
oldvect:
		ds.l	1
abortvect:
		ds.l	1

		.end
