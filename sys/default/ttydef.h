/* $Id: ttydef.h,v 1.1 2000/06/27 01:48:02 amura Exp $ */
/*
 *	Termcap terminal file, nothing special, just make it big
 *	enough for windowing systems.
 */

/*
 * $Log: ttydef.h,v $
 * Revision 1.1  2000/06/27 01:48:02  amura
 * Initial revision
 *
 */
/* 90.11.09	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.05	Modified for Ng 1.0 by S.Yoshida */

#ifdef	PC9801	/* 90.03.24  by A.Shirahashi */
#define	MEMMAP			/* Not memory mapped video.	*/
#else	/* NOT PC9801 */
#define GOSLING			/* Compile in fancy display.	*/
#endif	/* PC9801 */

#if !(defined(NROW)&&defined(NCOL))
#ifdef	MSDOS	/* 90.04.02  by S.Yoshida */
#define NROW	50			/* (maximum) Rows.		*/
#define NCOL	96			/* (maximum) Columns.		*/
#else	/* NOT MSDOS */
# ifdef HUMAN68K /* by Sawayanagi Yosirou */
#define NROW	32			/* (maximum) Rows.		*/
#define NCOL	96			/* (maximum) Columns.		*/
# else
#define NROW	66			/* (maximum) Rows.		*/
#define NCOL	132			/* (maximum) Columns.		*/
# endif	/* HUMAN68K */
#endif	/* MSDOS */
#endif
/* #define	MOVE_STANDOUT		/* don't move in standout mode	*/
#define STANDOUT_GLITCH			/* possible standout glitch	*/
#define TERMCAP				/* for possible use in ttyio.c	*/

#define getkbd()	(ttgetc())
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
#define	ungetkbd(c)	(ttungetc(c))
#endif	/* KANJI */

#ifndef XKEYS
# ifdef	ADDOPT
#define ttykeymapinit(ngrc) {}
# else
#define ttykeymapinit() {}
# endif
#endif

extern	int tputs();
#define	putpad(str, num)	tputs(str, num, ttputc)

#define	KFIRST	K00
#define	KLAST	K00
