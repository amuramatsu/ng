/* $Id: sysdef.h,v 1.8 2001/05/25 15:59:07 amura Exp $ */
/*
 *		Win32 based systems
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.8  2001/05/25 15:59:07  amura
 * WIN32 version support AUTOSAVE feature
 *
 * Revision 1.7  2001/02/18 19:29:05  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.6  2001/02/18 17:00:02  amura
 * some definitions are imported form def.h
 *
 * Revision 1.5  2001/01/17 18:34:53  amura
 * now compile successfull on VC++ and BC++
 *
 * Revision 1.4  2001/01/05 14:07:09  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.3  2000/12/14 18:10:48  amura
 * filename length become flexible
 *
 * Revision 1.2  2000/09/18 10:22:39  amura
 * some macro definition is changed
 *
 * Revision 1.1.1.1  2000/06/27 01:47:59  amura
 * import to CVS
 *
 */

#ifdef	_WIN32_WCE
#ifndef	NULL
#ifdef	__cplusplus
#define	NULL	0
#else
#define	NULL	((void *)0)
#endif
#endif
#else	/* _WIN32_WCE */
#include <stdio.h>
#include <memory.h>			/* need to use memmove().	*/
#endif	/* _WIN32_WCE */

#define	KBLOCK	1024			/* Kill grow.			*/
#define	GOOD	0			/* Good exit status.		*/
#ifndef	NO_SHELL	/* 91.01.10  by K.Maeda */
#define	CMDLINELENGTH	NFILEN		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	NO_RESIZE			/* Screen size is constant.	*/
#define	BSMAP	TRUE			/* Bs map feature can use.	*/
					/* (default mode is bsmap on)	*/

#ifndef _WIN32_WCE
#define NG_PROCESS_IME_MESSAGES 1
#endif

/* #define TARGET_WCEVER_IS_100 */ /* define this if appropriate */

#ifndef TARGET_WCEVER_IS_100
#ifdef	CTRLMAP
#undef	CTRLMAP			/* Build ctrlmap feature in as default off */
#endif
#endif
#define	MAKEBACKUP	FALSE		/* Making backup file is off.	*/

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
#define bzero(s,n)	memset(s,0,n)
#define bcmp(s,d,n)	memcmp(s,d,n)
#define	fncmp		strcmp		/* file name comparison		*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
char *getenv();
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/
#ifndef	NO_DIR
#define	dirend()	(VOID)0
#endif

#ifdef __BORLANDC__
#include <malloc.h>	/* for alloca() definition */
#else
#define alloca(n)	_alloca(n)
#endif

#ifdef	__cplusplus
extern "C" {
#endif
void	strcat_num(char *str, int num);
void	strcat_char(char *str, int c);
int	stricmp(const char *src, const char *dst);
int	chdir(const char *dir);
int	rchdir(char *dir);
int	Fputc(int c);
int	Sprintf(char *buf, const char *fmt, ... );
#ifdef	__cplusplus
}
#endif

#ifdef	putc
#undef	putc
#endif
#define	putc(c,fp)	Fputc(c)
#define	exit(rc)	Exit(rc)
#define	sprintf		Sprintf

