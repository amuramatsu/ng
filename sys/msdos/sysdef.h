/* $Id: sysdef.h,v 1.9.2.1 2003/02/28 17:52:48 amura Exp $ */
/*
 *		MS-DOS based systems
 */
#ifndef __SYSDEF_H__
#define __SYSDEF_H__

#include <stdio.h>
#include <stdlib.h>
#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
#include <mem.h>
#include <dir.h>
extern void *alloca(int);		/* defind in alloca.asm		*/
#else	/* NOT __TURBOC__ */
#include <memory.h>			/* need to use memmove().	*/
#endif	/* __TURBOC__ */

#define NFILEN	80			/* Length, file name.		*/
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
#ifdef PC9801
#define HAVE_ORIGINAL_PUTLINE		/* Use port specified putline()	*/
#endif

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
#define	BDC1		'\\'		/* Buffer names.		*/

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	bcopy(s,d,n)	memmove(d,s,n)	/* copy memory area.		*/
#define	bcmp(s,d,n)	memcmp(s,d,n)	/* compare memory area.		*/
#define	bzero(s,n)	memset(s,0,n)	/* zero fill memory area.	*/
#define	fncmp(s,d)	strcmp(s,d)	/* file name comparison		*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/

#endif /* __SYSDEF_H__ */
