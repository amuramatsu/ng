/* $Id: ttydef.h,v 1.8.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 *	Termcap terminal file, nothing special, just make it big
 *	enough for windowing systems.
 */
/* 90.11.09	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.05	Modified for Ng 1.0 by S.Yoshida */

#ifdef HAVE_ORIGINAL_PUTLINE
# ifndef MEMMAP
#  define MEMMAP			/* Memory mapped video.	*/
# endif
#else	/* not !PC9801 && !WIN32 && !EPOC32 */
#define GOSLING			/* Compile in fancy display.	*/
#endif

typedef unsigned short DISP_ROW_t;

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
/* #define	MOVE_STANDOUT*/		/* don't move in standout mode	*/
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

#ifndef WITHOUT_TERMCAP
#ifdef __cplusplus
extern "C" {
#endif
#ifdef SUPPORT_ANSI
int tgetent(char *, char *);
int tgetflag(char *);
int tgetnum(char *);
char *tgetstr(char *, char **);
char *tgoto(char *, int, int);
int tputs(char *, int, void (*)(int));
#else
int tgetent();
int tgetflag();
int tgetnum();
char *tgetstr();
char *tgoto();
int tputs();
#endif
#ifdef __cplusplus
}
#endif
#define	putpad(str, num)	tputs(str, num, ttputc)
#endif

#define	KFIRST	K00
#define	KLAST	K00
