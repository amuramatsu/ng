/* $Id: sysdef.h,v 1.5.2.1 2003/02/23 13:58:14 amura Exp $ */
/*
 *	unix based systems (for configure)
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.5.2.1  2003/02/23 13:58:14  amura
 * canna_init() must be called after setttysize() on ttyio.c
 *
 * Revision 1.5  2001/03/02 08:48:32  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.4  2001/02/18 19:29:04  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.3  2000/12/14 18:14:12  amura
 * filename length become flexible
 *
 * Revision 1.2  2000/12/01 10:07:09  amura
 * edit for Minix
 *
 * Revision 1.1  2000/11/19 18:35:00  amura
 * support GNU configure system
 *
 */

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

/* for use alloca() */
#ifndef	__GNUC__
# ifdef HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
#   pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#   endif
#  endif
# endif
#endif	/* __GNUC__ */

#define	KBLOCK		8192		/* Kill grow.			*/
#define	GOOD		0		/* Good exit status.		*/
#ifndef	NO_SHELL
#define	CMDLINELENGTH	NFILEN		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	BSMAP		FALSE		/* Bs map feature can use.	*/
					/* (default mode is bsmap off)	*/
#define	MAKEBACKUP	TRUE		/* Making backup file is on.	*/
#define ITIMER		TRUE		/* Interval timer support	*/

typedef long	RSIZE;			/* Type for file/region sizes	*/
typedef short	KCHAR;			/* Type for internal keystrokes	*/

#ifndef FD_SET
#  define FD_SET(fd,fdsp)       ((*fdsp) |= (1<<(fd)))
#  define FD_ZERO(fdsp)		((*fdsp) = 0)
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

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	fncmp		strcmp		/* file name comparison		*/
#ifndef	HAVE_BCOPY
#define bcopy(s,d,n)	memcpy(d,s,n)
#endif
#ifndef	HAVE_BZERO
#define bzero(s,n)	memset(s,0,n)
#endif
#ifndef	HAVE_BCMP
#define bcmp(s,d,n)	memcmp(s,d,n)
#endif
#ifdef	HAVE_RMDIR
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
#endif
char *getenv();
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/
#ifndef	NO_DIR
#define	dirend()	(VOID)0
#endif
