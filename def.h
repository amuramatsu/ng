/* $Id: def.h,v 1.21.2.5 2006/01/15 01:14:06 amura Exp $ */
/*
 * This file is the general header file for all parts
 * of the MicroEMACS display editor. It contains all of the
 * general definitions and macros. It also contains some
 * conditional compilation flags. All of the per-system and
 * per-terminal definitions are in special header files.
 * The most common reason to edit this file would be to zap
 * the definition of CVMVAS or BACKUP.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#ifndef __NG_DEF_H__
#define __NG_DEF_H__

#include <stdlib.h>
#include <string.h>
#include "sysdef.h"		/* Order is critical.		*/
#include "ttydef.h"
#include "chrdef.h"

/*
 * If your system and/or compiler does not support the "void" type
 * then define NO_VOID_TYPE in sysdef.h.  In the absence of some
 * other definition for VOID, the default in that case will be to
 * turn it into an int, which works with most compilers that don't
 * support void.  In the absence of any definition of VOID or
 * NO_VOID_TYPE, the default is to assume void is supported, which
 * should be the case for most modern C compilers.
 */

#ifdef NO_VOID_TYPE
#  undef VOID
#  define VOID int			/* Default for no void is int */
#  define VOIDptr char*
#else
#ifndef VOID
#  define VOID void			/* Just use normal void */
#  define VOIDptr void*
#endif /* VOID */
#endif /* NO_VOID_TYPE */

#ifdef SUPPORT_ANSI
#define _PRO(x) x
#else
#define _PRO(x) ()
#endif

typedef int (*PF) _PRO((int, int)); /* generaly useful type */

/*
 * All repeated structures are kept as linked lists of structures.
 * All of these start with a LIST structure (except lines, which
 * have their own abstraction). This will allow for
 * later conversion to generic list manipulation routines should
 * I decide to do that. it does mean that there are four extra
 * bytes per window. I feel that this is an acceptable price,
 * considering that there are usually only one or two windows.
 */
struct WINDOW;
struct BUFFER;
struct LANG_MODULE;
struct LIST;
#include "in_code.h"
typedef struct LIST {
    union {
	struct WINDOW *l_wp;
	struct BUFFER *x_bp;	/* l_bp is used by LINE */
	struct LANG_MODULE *l_mp;
	struct LIST *l_nxt;
    } l_p;
    NG_WCHAR_t *l_name;
} LIST;

/*
 * Usual hack - to keep from uglifying the code with lotsa
 * references through the union, we #define something for it.
 */
#define l_next	l_p.l_nxt
/*
 * Table sizes, etc.
 */
#ifndef NFILEN /* Maybe Win32 or UN*X */
#define NFILEN		1024		/* Length, file name.		*/
#endif
#define NBUFN		32		/* Length, buffer name.		*/
#define NLINE		256		/* Length, line.		*/
#define NINPUT		32		/* Length, small minibuf input	*/
#define TERMCAP_BUF_LEN	2048		/* Length, termcap lib buffer	*/
#define PBMODES		8		/* modes per buffer		*/
#define NKBDM		256		/* Length, keyboard macro.	*/
#define NPAT		80		/* Length, pattern.		*/
#define HUGE		1000		/* A rather large number.	*/
#define NSRCH		128		/* Undoable search commands.	*/
#define NXNAME		64		/* Length, extended command.	*/
#define NKNAME		20		/* Length, key names		*/
#define NUNDO	 	10		/* Undo buffer size		*/
#define MB_NHISTS	10		/* Minibuffer History size	*/

/*
 * Universal.
 */
#define FALSE	0			/* False, no, bad, etc.		*/
#define TRUE	1			/* True, yes, good, etc.	*/
#define ABORT	2			/* Death, ^G, abort, etc.	*/

#define KPROMPT 2			/* keyboard prompt		*/

/*
 * These flag bits keep track of
 * some aspects of the last command. The CFCPCN
 * flag controls goal column setting. The CFKILL
 * flag controls the clearing versus appending
 * of data in the kill buffer.
 */
#define CFCPCN	0x0001			/* Last command was C-P, C-N	*/
#define CFKILL	0x0002			/* Last command was a kill	*/
#define CFINS	0x0004			/* Last command was self-insert */
#define CFINS2	0x0008			/* Last command was insert	*/

/*
 * File I/O.
 */
#define FIOSUC	0			/* Success.			*/
#define FIOFNF	1			/* File not found.		*/
#define FIOEOF	2			/* End of file.			*/
#define FIOERR	3			/* Error.			*/
#define FIOLONG 4			/* long line partially read	*/

/*
 * Directory I/O.
 */
#define DIOSUC	0			/* Success.			*/
#define DIOEOF	1			/* End of file.			*/
#define DIOERR	2			/* Error.			*/

/*
 * Display colors.
 */
#define CNONE	0			/* Unknown color.		*/
#define CTEXT	1			/* Text color.			*/
#define CMODE	2			/* Mode line color.		*/

/* Flags for keyboard involked functions */

#define FFUNIV		1		/* universal argument		*/
#define FFNEGARG	2		/* negitive only argument	*/
#define FFOTHARG	4		/* other argument		*/
#define FFARG		7		/* any argument			*/
#define FFRAND		8		/* Called by other function	*/

/*
 * Flags for "eread".
 */
#define EFFUNC	0x0001			/* Autocomplete functions.	*/
#define EFBUF	0x0002			/* Autocomplete buffers.	*/
#define EFFILE	0x0004			/* " files (maybe someday)	*/
#define EFAUTO	0x0007			/* Some autocompleteion on	*/
#define EFNEW	0x0008			/* New prompt.			*/
#define EFCR	0x0010			/* Echo CR at end; last read.	*/

/*
 * Flags for "ldelete"/"kinsert"
 */

#define KNONE	0
#define KFORW	1
#define KBACK	2

#ifdef ADDFUNC
#define MG_RATIO_ALL -1 /* used at dotpos() to return value */
#define MG_RATIO_TOP -2 /* used at dotpos() to return value */
#define MG_RATIO_BOT -3 /* used at dotpos() to return value */
#endif

#include "extern.h"			/* include extern definitions */

#endif /* __NG_DEF__ */
