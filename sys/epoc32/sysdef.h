/* $Id: sysdef.h,v 1.5 2003/02/22 08:09:47 amura Exp $ */
/*
 *		Epoc32 based systems
 */

#ifndef __SYSDEF_H__
#define __SYSDEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>

#define	KBLOCK	1024			/* Kill grow.			*/
#define	GOOD	0			/* Good exit status.		*/
#ifndef	NO_SHELL
#define	CMDLINELENGTH	128		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	NO_RESIZE			/* Screen size is constant.	*/
#define	BSMAP		TRUE		/* Bs map feature can use.	*/
					/* (default mode is bsmap on)	*/
#define	MAKEBACKUP	FALSE		/* Making backup file is off.	*/
#define HAVE_ORIGINAL_PUTLINE		/* Use port specified putline()	*/
#define WITHOUT_TERMCAP	TRUE		/* This port unused termcap lib */

typedef int	RSIZE;			/* Type for file/region sizes	*/
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

#define SYSINIT		epoc_init()
#define SYSCLEANUP	epoc_cleanup()

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	bcopy(s,d,n)	memcpy(d,s,n)	/* copy memory area.		*/
#define	bzero(s,n)	memset(s,0,n)	/* fill zero memory area.	*/
#define	bcmp(s,d,n)	memcmp(s,d,n)	/* compare memory area.		*/
#define	fncmp(s,d)	strcmp(s,d)	/* file name comparison		*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/

/*
 * These are defined in ttyio.cpp, 
 */
#ifdef __cplusplus
extern "C" {
#endif
void epoc_init(void);
void epoc_cleanup(void);
void epoc_ttattr(int color);
void epoc_ttmove(int, int);
void epoc_tteeol();
#ifdef __cplusplus
}
#endif

#endif /* __SYSDEF_H__ */
