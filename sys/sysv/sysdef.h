/* $Id: sysdef.h,v 1.2 2000/12/14 18:10:47 amura Exp $ */
/*
 *		System V system header file
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.2  2000/12/14 18:10:47  amura
 * filename length become flexible
 *
 * Revision 1.1.1.1  2000/06/27 01:47:59  amura
 * import to CVS
 *
 */

#include <stdio.h>
#include <unistd.h>		/* 00.04.15 by amura		*/

#define	HAVE_GETCWD

#define	KBLOCK	8192			/* Kill grow.			*/
#define	GOOD	0			/* Good exit status.		*/
#ifndef	NO_SHELL	/* 91.01.10  by K.Maeda */
#define	CMDLINELENGTH	NFILEN		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	BSMAP	FALSE			/* Bs map feature can use.	*/
					/* (default mode is bsmap off)	*/
#define	MAKEBACKUP	TRUE		/* Making backup file is on.	*/

typedef long	RSIZE;			/* Type for file/region sizes	*/
typedef short	KCHAR;			/* Type for internal keystrokes	*/

#if 0
#ifdef _POSIX_JOB_CONTROL		/* This is trick */
# if (0 - _POSIX_JOB_CONTROL - 1) != -1
#  define HAVE_GETSID
# endif
#endif
#endif

/*
 * Macros used by the buffer name making code.
 * Start at the end of the file name, scan to the left
 * until BDC1 (or BDC2, if defined) is reached. The buffer
 * name starts just to the right of that location, and
 * stops at end of string (or at the next BDC3 character,
 * if defined). BDC2 and BDC3 are mainly for VMS.
 */
#define	BDC1	'/'			/* Buffer names.		*/

#ifdef CANNA
#define SYSINIT canna_init()
#define SYSCLEANUP canna_end()
#endif

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	fncmp		strcmp		/* file name comparison		*/
#define bcopy(s,d,n)	memcpy(d,s,n)	/* memory-to-memory copy	*/
#define bzero(s,n)	memset(s,0,n)	/* 91.01.21  Add by H.Kaneko.	*/
#define bcmp(s,d,n)	memcmp(s,d,n)	/* 91.02.04  Add by Y.Nimura.	*/
					/* 91.02.04  strncmp -> memcmp	*/
					/*		by S.Yoshida	*/
#ifndef SVR2		/* SVID2(==SVR3) has rmdir. 00.04.03 by amura	*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
#endif
char *getenv();
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/
char *getcwd();
