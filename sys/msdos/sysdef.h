/* $Id: sysdef.h,v 1.5 2001/05/20 20:22:51 amura Exp $ */
/*
 *		MS-DOS based systems
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.5  2001/05/20 20:22:51  amura
 * fix silly bug
 *
 * Revision 1.4  2001/05/08 17:58:57  amura
 * fix alloca() probrems with Turbo C
 *
 * Revision 1.3  2001/01/05 14:07:08  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.2  2000/12/14 18:10:47  amura
 * filename length become flexible
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */

#include <stdio.h>
#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
#include <mem.h>
extern void *alloca(int);		/* defind in alloca.asm		*/
#else	/* NOT __TURBOC__ */
#include <memory.h>			/* need to use memmove().	*/
#endif	/* __TURBOC__ */

#define	KBLOCK	1024			/* Kill grow.			*/
#define	GOOD	0			/* Good exit status.		*/
#ifndef	NO_SHELL	/* 91.01.10  by K.Maeda */
#define	CMDLINELENGTH	128		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	NO_RESIZE			/* Screen size is constant.	*/
#define	BSMAP	TRUE			/* Bs map feature can use.	*/
					/* (default mode is bsmap on)	*/
#define	MAKEBACKUP	FALSE		/* Making backup file is off.	*/
#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
#define	LOCAL_VARARGS
#endif	/* __TURBOC__ */

typedef long	RSIZE;			/* Type for file/region sizes	*/
typedef short	KCHAR;			/* Type for internal keystrokes	*/

/*
 * Macros used by the buffer name making code.
 * Start at the end of the file name, scan to the left
 * until BDC1 (or BDC2, if defined) is reached. The buffer
 * name starts just to the right of that location, and
 * stops at end of string (or at the next BDC3 character,
 * if defined). BDC2 and BDC3 are mainly for VMS.
 */
#define	BDC1	'\\'			/* Buffer names.		*/

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	bcopy(s,d,n)	memmove(d,s,n)	/* copy memory area.		*/
#define	fncmp		strcmp		/* file name comparison		*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
char *getenv();
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/
