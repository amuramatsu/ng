/* $Id: sysdef.h,v 1.4 2001/02/18 19:29:03 amura Exp $ */
/*
 * Name:	MicroEMACS
 * Version:	MG 2a
 *		Commodore Amiga system header file.
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.4  2001/02/18 19:29:03  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.3  2001/01/05 14:07:06  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.2  2000/12/14 18:07:52  amura
 * filename length become flexible
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 */

#include	<stdio.h>	/* Dec. 16, 1992 by H.Ohkubo */

/* Neither can lattice 4 */
extern char *offset_dummy;		/* Manx 3.2 can't handle 0->	*/
#define OFFSET(type,member) \
 ((char *)&(((type *)offset_dummy)->member)-(char *)((type *)offset_dummy))

#ifdef	_DCC
#include <alloca.h>
#endif

#ifdef SAS6
# define LATTICE	1
#endif

#ifndef SUPPORT_ANSI
# ifdef LATTICE
#  define SUPPORT_ANSI	1
# endif
# ifdef _DCC
#  define SUPPORT_ANSI	1
# endif
# ifdef __GNUC__
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
# ifdef	BUGFIX	/* Dec. 15, 1992 by H.Ohkubo */
#  ifdef  SAS6
#   define	NULL	0L
#  else
#   define	NULL	((void *)0)
#  endif
# else	/* ORIGINAL Code */
# define NULL	((char *) 0)		/* These are crass, but		*/ 
# endif	/* BUGFIX */
#endif
#ifndef EOF
# define	EOF	-1		/* will work`			*/
#endif

#ifndef	NO_SHELL	/* Dec. 15, 1992 by H.Ohkubo */
#define	CMDLINELENGTH	NFILEN		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	BSMAP	FALSE	/* Dec.18,1992 Add by H.Ohkubo */
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
#define	bcopy(src,dest,len) movmem(src,dest,len)
#define	bzero(s,n)	memset(s,0,n)	/* Dec. 16, 1992 Add by H.Ohkubo */
#define	bcmp(s,d,n)	memcmp(s,d,n)	/* Dec. 16, 1992 Add by H.Ohkubo */
#endif

#define fncmp Strcmp

#ifndef NO_DIRED
#define rename(s1,s2) (Rename(s1,s2) == -1 ? 0 : -1)
#define unlinkdir(s1) (DeleteFile(s1) == -1 ? 0 : -1)
#endif
#ifndef	NO_DIR
#define	rchdir(dir)	chdir(dir)
#define	dirend()	(VOID)0
#endif

#ifdef	KANJI	/* Dec.17,1992 by H.Ohkubo */
#ifndef	AMIGA_STDIO
extern	int	FlushBuf();
typedef	struct	{
	int	*niobuf;
	int	bufmax;
	unsigned char	*iobuf;
}	My_FILE;
#define	FILE	My_FILE
#define	AMIGA_FAST_FILE
#define	Putc(c,fp)	{if (*(fp)->niobuf == (fp)->bufmax) FlushBuf();\
				(fp)->iobuf[(*(fp)->niobuf)++] = (c);}
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
