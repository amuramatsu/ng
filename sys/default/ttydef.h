/* $Id: ttydef.h,v 1.3 2001/02/14 09:19:45 amura Exp $ */
/*
 *	Termcap terminal file, nothing special, just make it big
 *	enough for windowing systems.
 */

/*
 * $Log: ttydef.h,v $
 * Revision 1.3  2001/02/14 09:19:45  amura
 * code cleanup around putline() and NCOL/NROW
 *
 * Revision 1.2  2001/01/05 14:07:07  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.1.1.1  2000/06/27 01:48:02  amura
 * import to CVS
 *
 */
/* 90.11.09	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.05	Modified for Ng 1.0 by S.Yoshida */

#if defined(PC9801)||defined(WIN32)	/* 90.03.24  by A.Shirahashi */
#define	MEMMAP			/* Not memory mapped video.	*/
#else	/* not !PC9801 && !WIN32 */
#define GOSLING			/* Compile in fancy display.	*/
#endif	/* PC9801 */

#if !(defined(NROW)&&defined(NCOL))
#if defined(MSDOS)	/* 90.04.02  by S.Yoshida */
#define NROW	50			/* (default) Rows.		*/
#define NCOL	80			/* (default) Columns.		*/
#elif defined(HUMAN68K)	/* by Sawayanagi Yosirou */
#define NROW	32			/* (default) Rows.		*/
#define NCOL	96			/* (default) Columns.		*/
#else /* not MSDOS or HUMAN68K */
#define NROW	66			/* (default) Rows.		*/
#define NCOL	132			/* (default) Columns.		*/
#endif	/* MSDOS or HUMAN68K */
#endif	/* NROW && NCOL */
/* #define	MOVE_STANDOUT		/* don't move in standout mode	*/
#define STANDOUT_GLITCH			/* possible standout glitch	*/
#define TERMCAP				/* for possible use in ttyio.c	*/

#ifdef	DO_METAKEY
#ifndef METABIT
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#define METABIT 0x100
#else	/* NOT KANJI */
#define METABIT 0x80
#endif	/* KANJI */
#endif	/* METABIT */
#endif	/* DO_METAKEY */

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
