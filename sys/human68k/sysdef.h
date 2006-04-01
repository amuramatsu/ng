/* $Id: sysdef.h,v 1.7.2.4 2006/04/01 17:15:15 amura Exp $ */
/*
 *		Human68k system definitions
 */
/* 90.11.10	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/*		MS-DOS system definitions */

#ifndef __SYSDEF_H__
#define __SYSDEF_H__

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>		/* with libc */
#include <sys/types.h>		/* with libc */

#define NFILEN		128		/* Length, file name.		*/
#define	KBLOCK		1024		/* Kill grow.			*/
#define	GOOD		0		/* Good exit status.		*/
#ifndef	NO_SHELL	/* 91.01.10  by K.Maeda */
#define	CMDLINELENGTH	NFILEN		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	NO_RESIZE			/* Screen size is constant.	*/
#define	BSMAP		TRUE		/* Bs map feature can use.	*/
					/* (default mode is bsmap on)	*/
#define	MAKEBACKUP	FALSE		/* Making backup file is off.	*/
#define	LOCAL_VARARGS
#define HAVE_ORIGINAL_PUTLINE		/* Use port specified putline()	*/
#ifdef DIRECT_IOCS
#define FASTTTY				/* Use original FastTTY routine */
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
#define	BDC1	'/'			/* Buffer names.		*/
#define	BDC2	'\\'			/* Buffer names.		*/

/* Grab memory as much as possible */
#define	SYSINIT		sysinit()

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#ifndef bcopy 
#define	bcopy(s,d,n)	memcpy(d,s,n)	/* copy memory area.		*/
#endif
#ifndef bzero
#define bzero(s,n)	memset(s,0,n)	/* 91.01.21  Add by H.Kaneko.	*/
#endif
#define bcmp(s,d,n)	memcmp(s,d,n)	/* 91.02.04  Add by Y.Nimura.	*/
					/* 91.02.04  strncmp -> memcmp	*/
					/*		by S.Yoshida	*/
#define	fncmp		h68kfncmp	/* file name comparison		*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/

#ifdef __GNUC__
#define alloca		__builtin_alloca
#endif


#ifdef __cplusplus
extern "C" {
#endif

void sysinit(void);
int h68kfncmp(const char *, const char *);

#ifdef __cplusplus
}
#endif

#endif /* __SYSDEF_H__ */
