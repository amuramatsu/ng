/* $Id: sysdef.h,v 1.3 2000/12/14 18:10:46 amura Exp $ */
/*
 *		BSD unix based systems (sunos, ultrix)
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.3  2000/12/14 18:10:46  amura
 * filename length become flexible
 *
 * Revision 1.2  2000/06/27 01:59:42  amura
 * small bugfix
 *
 * Revision 1.1.1.1  2000/06/27 01:48:02  amura
 * import to CVS
 *
 */

#include <stdio.h>
#include <unistd.h>		/* 00.04.15 by amura		*/
#include <sys/param.h>

#define	KBLOCK	8192			/* Kill grow.			*/
#define	GOOD	0			/* Good exit status.		*/
#define	SYMBLINK	1		/* Handle symbolic links	*/
#ifndef	NO_SHELL	/* 91.01.10  by K.Maeda */
#define	CMDLINELENGTH	NFILEN		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	BSMAP	FALSE			/* Bs map feature can use.	*/
					/* (default mode is bsmap off)	*/
#define	MAKEBACKUP	TRUE		/* Making backup file is on.	*/

typedef int	RSIZE;			/* Type for file/region sizes	*/
typedef short	KCHAR;			/* Type for internal keystrokes	*/

#ifdef BSD4_4
#define HAVE_MKSTEMP
#define HAVE_GETCWD
#define HAVE_FDSET
#define HAVE_DIRENT
#else
#define NO_PID_TYPE
#define NO_SIG_TYPE
#endif

#if 0
#ifdef	__FreeBSD__
#if __FreeBSD__ >= 3
# define HAVE_GETSID
#endif
#else	/* NOT __FreeBSD__ */
#ifdef _POSIX_JOB_CONTROL		/* This is trick */
# if (0 - _POSIX_JOB_CONTROL - 1) != -1
#  define HAVE_GETSID
# endif
#endif	/* _POSIX_JOB_CONTROL */
#endif	/* __FreeBSD__ */
#endif

#ifdef NO_PID_TYPE
#  undef PID_T
#  define PID_T int
#else
#ifndef PID_T
#  define PID_T pid_t
#endif
#endif

#ifdef NO_SIG_TYPE
#  undef SIG_T
   typedef void (*SIG_T)();
/* If your compiler doesn't have void, rewrite this "int (*SIG_T)()"
   Cannot use VOID because it's defined later...  */
#else
#ifndef SIG_T
#  define SIG_T sig_t
#endif
#endif

#ifdef NO_FDSET
#  undef FDSET
#  define FDSET int
#  define FD_SET(fd,fdsp) ((*fdsp) |= (1<<(fd)))
#  define FD_ZERO(fdsp) ((*fdsp) = 0)
#else
#ifndef FDSET
#  define FDSET fd_set
#else
#include <sys/types.h>
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
#define	SYSINIT canna_init()
#define SYSCLEANUP canna_end()
#endif

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	fncmp		strcmp		/* file name comparison		*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
char *getenv();
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/

