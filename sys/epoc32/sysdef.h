/* $Id: sysdef.h,v 1.2 2001/11/28 19:02:10 amura Exp $ */
/*
 *		Epoc32 based systems
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.2  2001/11/28 19:02:10  amura
 * Small fixes arount termcap library.
 *
 * Revision 1.1  2001/09/30 15:59:12  amura
 * Initial EPOC32 commit.
 *
 * Currently this is not run yet. Some functions around tty are not implemented.
 *
 */

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
#define	BSMAP	TRUE			/* Bs map feature can use.	*/
					/* (default mode is bsmap on)	*/
#define	MAKEBACKUP	TRUE		/* Making backup file is off.	*/
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

#define SYSINIT epoc_init()
#define SYSCLEANUP epoc_cleanup()

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	bcopy(s,d,n)	memmove(d,s,n)	/* copy memory area.		*/
#define	bzero(s,n)	memset(s,0,n)	/* fill zero memory area.	*/
#define	bcmp(s,d,n)	memcmp(s,d,n)	/* compare memory area.		*/
#define	fncmp		strcmp		/* file name comparison		*/
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
