/* $Id: sysdef.h,v 1.9 2003/02/22 08:09:47 amura Exp $ */
/*
 * Name:	MicroEMACS
 * Version:	MG 2a
 *		Commodore Amiga system header file.
 */
#ifndef __SYSDEF_H__
#define __SYSDEF_H__

#include <stdio.h>		/* Dec. 16, 1992 by H.Ohkubo */
#include <stdlib.h>
#include <string.h>

/* Neither can lattice 4 */
extern char *offset_dummy;		/* Manx 3.2 can't handle 0->	*/
#define OFFSET(type,member) \
 ((char *)&(((type *)offset_dummy)->member)-(char *)((type *)offset_dummy))

#if defined(_DCC)
#include <alloca.h>
#elif defined(SAS6) || defined(VBCC)
void *alloca(int);
#endif

#ifdef SAS6
# define LATTICE	1
#endif
#ifndef SUPPORT_ANSI
# if defined(LATTICE) || defined(_DCC) || defined(__GNUC__) || defined(VBCC)
#  define SUPPORT_ANSI	1
# endif
#endif

#define	HAVE_GETCWD
#define	VARARGS
#define	DPROMPT				/* we always want delayed prompts */
#define	KBLOCK		4096		/* Kill grow.			*/
#define	GOOD		0		/* Good exit status.		*/
#define	SYSINIT		sysinit()	/* System-specific initialization */
#define SYSCLEANUP	syscleanup()	/* System-specific cleanup	*/
#define MALLOCROUND(m)	(m+=7,m&=~7)	/* Round up to 8 byte boundary	*/

#ifndef NULL
# ifdef  SAS6
#   define	NULL	0L
# else
#   define	NULL	((void *)0)
# endif
#endif
#ifndef EOF
# define	EOF	-1		/* will work`			*/
#endif

#ifndef	NO_SHELL	/* Dec. 15, 1992 by H.Ohkubo */
#define	CMDLINELENGTH	NFILEN		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	BSMAP		FALSE		/* Dec.18,1992 Add by H.Ohkubo */
#define WITHOUT_TERMCAP	TRUE		/* This port unused termcap lib */

/*
 * Macros used by the buffer name making code.
 * Start at the end of the file name, scan to the left
 * until BDC1 (or BDC2, if defined) is reached. The buffer
 * name starts just to the right of that location, and
 * stops at end of string (or at the next BDC3 character,
 * if defined). BDC2 and BDC3 are mainly for VMS.
 */
#define	BDC1	'/'			/* Buffer names.		*/
#define	BDC2	':'


/*
 * Typedefs for internal key type and how big a region can be.
 */

typedef short	KCHAR;	/* type used to represent Emacs characters */
typedef	long	RSIZE;	/* size of a region	*/

#ifndef __GNUC__
#define	bcopy(s,d,l)		memcpy(d,s,l)
#define	bzero(s,n)		memset(s,0,n)
#define	bcmp(s,d,n)		memcmp(s,d,n)
#endif

/* System includes */
#include <exec/types.h>
#ifdef INLINE
#include <pragmas/dos_pragmas.h>
#else
#include <clib/dos_protos.h>
#endif

#define fncmp(s1,s2)	Strcmp(s1,s2)
#define unlink(s1)	(DeleteFile(s1) == -1 ? 0 : -1)
#ifndef NO_DIRED
#define rename(s1,s2)	(Rename(s1,s2) == -1 ? 0 : -1)
#define unlinkdir(s1)	(DeleteFile(s1) == -1 ? 0 : -1)
#endif
#ifndef	NO_DIR
#define	rchdir(dir)	chdir(dir)
#define	dirend()	(VOID)0
#endif

#ifdef	SUPPORT_ANSI
extern int   Chdir(char *);
extern void  Putc(int);
extern void  sysinit(void);
extern void  syscleanup(void);
extern int   Strcmp(char *, char *);
#else
extern int   Chdir();
extern void  Putc();
extern void  sysinit();
extern void  syscleanup();
extern int   Strcmp();
#endif

#ifdef	KANJI	/* Dec.17,1992 by H.Ohkubo */
#ifndef	AMIGA_STDIO
# ifdef SUPPORT_ANSI
extern void FlushBuf(void);
# else
extern void FlushBuf();
# endif
typedef	struct	{
    int	*niobuf;
    int bufmax;
    unsigned char *iobuf;
} My_FILE;
#define	FILE		My_FILE
#define	AMIGA_FAST_FILE
#define	Putc(c,fp) do {				\
    if (*(fp)->niobuf == (fp)->bufmax)		\
	FlushBuf();				\
    (fp)->iobuf[(*(fp)->niobuf)++] = (c);	\
} while (/*CONSTCOND*/0)
#ifdef putc
# undef putc
#endif
#define	putc	Putc
/* #define	kputc	KPutc */
#endif	/* AMIGA_STDIO */
#endif	/* KANJI */

#define	chdir	Chdir	/* Dec.18,1992 Add by H.Ohkubo */

#ifdef	NEW_COMPLETE	/* Jan.5,1992 Add by H.Ohkubo */
#define	file_name_part	BaseName
#endif

#endif /* __SYSDEF_H__ */
