/* $Id: sysdef.h,v 1.2 2000/12/01 10:07:09 amura Exp $ */
/*
 *	unix based systems (for configure)
 */

/*
 * $Log: sysdef.h,v $
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
#define	MAXPATH		256		/* Maximum length of path for chdir */
#ifndef	NO_SHELL
#define	CMDLINELENGTH	128		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	BSMAP		FALSE		/* Bs map feature can use.	*/
					/* (default mode is bsmap off)	*/
#define	MAKEBACKUP	TRUE		/* Making backup file is on.	*/

typedef long	RSIZE;			/* Type for file/region sizes	*/
typedef short	KCHAR;			/* Type for internal keystrokes	*/

#ifdef fd_set
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

#ifdef CANNA
#define	SYSINIT canna_init()
#define SYSCLEANUP canna_end()
#endif

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

