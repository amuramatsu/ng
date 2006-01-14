/* $Id: config.h,v 1.22.2.4 2006/01/14 19:59:59 amura Exp $ */
/*
 *		config.h - defines compile time options.
 */
/* 90.12.20	Created by S.Yoshida */

/*----------------------------------------------------------------------*/
/*	Features removeable to save space.				*/
/*----------------------------------------------------------------------*/

#undef	NO_BACKUP	/* Disable backup files when writing.	*/

#undef	NO_DIR		/* Disable dir change functions.	*/
			/* If defined, NO_STARTUP must be also. */

#undef	NO_DIRED	/* Disable "dired" mode.		*/
			/* If not defined, ADDFUNC must be also.*/

#undef	NO_DPROMPT	/* Disable delayed prompt on multi-key sequences. */

#undef	NO_FILECOMP	/* Disable file name completion.	*/

#undef	NO_HELP		/* Disable help, descibe-bindings,	*/
			/* describe-key-briefly, apropos  	*/

#undef	NO_MACRO	/* Disable keyboard macros.		*/
			/* If defined, NO_STARTUP must be also. */

#undef	NO_SHELL	/* Disable shell-command function.	*/

#undef	NO_STARTUP	/* Disable startup files, load, etc.	*/

#define	DO_METAKEY	/* Enable META key input.		*/

#define	REGEX		/* Enable regular expression functions.	*/

#define	PREFIXREGION	/* Enable function "prefix-region".	*/

#define	C_MODE		/* Enable c-mode functions.		*/

#define	FILLPREFIX	/* Enable fill prefix function.		*/

#define	READONLY	/* Enable read-only buffer mode.	*/

#define	NEW_COMPLETE	/* Enable new version completion.	*/
			/* If defined, ADDFUNC must be also.	*/

#define	VARIABLE_TAB	/* Enable buffer-local tabwidth	(by amura) */

#define BUFFER_MODE	/* Enable "Buffer Menu" mode for list-buffer. */
			/* (by Tillanosoft) */

#define	EXTD_DIR	/* buffer local current directly 	*/
			/* (by Tillanosoft, amura) */

#define	ADDOPT		/* Enable some commandline option	*/
			/* (by Gen KUROKI, amura) */

#define IS_ENHANCE	/* Incremantal search enhancement (by bsh) */

#define GOMATCH		/* goto matching fence (by bsh)		*/

#define ZAPTOCHAR	/* zap to char (by bsh) */
#undef	ZAPTOC_A	/* zap to char (GNU Emacs compatible) (by bsh) */

#define JUMPERR		/* jump to error function (by bsh)	*/
			/* If defined, ADDFUNC and REGEX must be also. */

#define	MINIBUF_EDIT	/* minibuffer edit like GNU emacs (by Kakugawa)	*/

#define	UNDO		/* enable buffer undo */

#define AUTOSAVE	/* enable autosaver (ported form M.Suziki's work) */

#define	KINSOKU		/* Create KINSOKU handling version. If defined, */
			/* KANJI must be also.				*/

#define	ADDFUNC		/* Create additional misc functions.	*/

#undef	INCLUDE_SKG	/* Enable SKG(Simple Kanji Generator System)	*/
			/* (by H.Konishi) */ 

#define	FKEYS		/* Enable to use function key */

/*----------------------------------------------------------------------*/
/*	SystemV / BSD machine dependent features.			*/
/*----------------------------------------------------------------------*/
/*................................................*/
#ifdef HAVE_CONFIG_H	/* Do not edit this line. */
#include "sysconfig.h"	/* Do not edit this line. */
#endif			/* Do not edit this line. */
#if defined(_WIN32)&&!defined(__CYGWIN__)&&!defined(WIN32)
#define	WIN32		/* Do not edit this line. */
#endif			/* Do not edit this line. */
#if defined(__XSDK__)&&!defined(EPOC32)
#define EPOC32		/* Do not edit this line. */
#endif			/* Do not edit this line. */
#ifndef	MSDOS		/* Do not edit this line. */
#ifndef	HUMAN68K	/* Do not edit this line. */
#ifndef	AMIGA		/* Do not edit this line. */
#ifndef WIN32		/* Do not edit this line. */
#ifndef EPOC32		/* Do not edit this line. */
/*................................................*/

#define XKEYS		/* use numelic keypad (by Gen.KUROKI) */

#define VTCURSOR	/* Use DEC vt cursor key on JIS input mode */
			/* (by Gen.KUROKI) */

/*#define CANNA*/	/* use CANNA			*/

/*................................................*/
#endif			/* Do not edit this line. */
#endif			/* Do not edit this line. */
#endif			/* Do not edit this line. */
#endif			/* Do not edit this line. */
#endif			/* Do not edit this line. */
/*................................................*/

/*----------------------------------------------------------------------*/
/*	MS-DOS machine dependent features.				*/
/*----------------------------------------------------------------------*/
/*................................................*/
#ifdef	MSDOS		/* Do not edit this line. */
/*................................................*/

/*#undef TCCONIO*/	/* Use Turbo C console IO */

/*#undef IBMPC*/	/* Create Ng for IBM PC compatible	*/
			/* (include Toshiba J-3100)		*/

/*#undef PC9801*/	/* Create Ng for NEC PC-9801 series. */

#define BACKSLASH	/* Display backslash instead of \ (PC-9801 only) */

#define FEPCTRL		/* Enable FEP auto control. */

/*................................................*/
#endif			/* Do not edit this line. */
/*................................................*/

/*----------------------------------------------------------------------*/
/*	Win32 machine dependent features. by Eiichirou ITO,Tillanosoft	*/
/*----------------------------------------------------------------------*/
/*................................................*/
#ifdef WIN32		/* Do not edit this line. */
/*................................................*/
  
#define	JAPANESE_KEYBOARD /* keyboard layout is Japanese one	*/

#define FEPCTRL		/* Enable FEP auto control. */

#define CLIPBOARD	/* Enable Clipboard cut & paste */

#define MOUSE		/* Enable Mouse */

#define DROPFILES	/* Drag'n Drop file open */
			/* If defined, ADDFUNC must be also. */

#define	EMACS_BACKUP_STYLE
			/* Backup files like as GNU Emacs */
			/* If you use Win9x, this shold not set */

#undef COMMANDBANDS	/* COMMANDBAND for WinCE */

/*................................................*/
#endif			/* Do not edit this line. */
/*................................................*/

/*----------------------------------------------------------------------*/
/*	Human68k machine dependent features.				*/
/*----------------------------------------------------------------------*/
/*................................................*/
#ifdef	HUMAN68K	/* Do not edit this line. */
/*................................................*/

#define FEPCTRL		/* Enable FEP auto control. */

/*#undef DIRECT_IOCS*/	/* use IOCS to output directory */

#define	EMACS_BACKUP_STYLE
			/* Backup files like as GNU Emacs */

/*................................................*/
#endif			/* Do not edit this line. */
/*................................................*/

/*----------------------------------------------------------------------*/
/*	AmigaDos dependent features.	Dec.18,1992 Add by H.Ohkubo	*/
/*----------------------------------------------------------------------*/
/*................................................*/
#ifdef	AMIGA		/* Do not edit this line. */
/*................................................*/

#undef	V11		/* No Ver1.1 AmigaDOS */

#define V2		/* Don't support AmigaDOS 1.x */

#define	DO_MENU		/* Enable Menu selection */

#define	MOUSE		/* Enable mouse */

#define	DO_ICONIFY	/* Enable iconify */

#define	CHANGE_COLOR	/* Enable color setting */

#define	CHANGE_FONT	/* Enable font setting */

#define	INLINE_PRAGMAS	/* Use #pragma for Amiga System calls */

#undef	USE_ARP		/* Enable ARP Library */
			/* Undefined by H.Konishi for SAS */ 

#define	LAMIGA_META	/* Assume Left Amiga to META key */

#undef	MODE_RENDITION	/* Set Default */
#undef	TEXT_RENDITION	/* Set Default */
#undef	TEXT_FG		/* Set Default */
#undef	TEXT_BG		/* Set Default */
#undef	MODE_FG		/* Set Default */
#undef	MODE_BG		/* Set Default */

#define ASL             /* Enable ASL Requester (by H.Konishi) */

#undef	AMIGA_STDIO	/* Using ANSI-Standard FILE IO library */
#define	CLIPBOARD	/* AMIGA Clipboard support */

/*................................................*/
#endif			/* Do not edit this line. */
/*................................................*/

/*----------------------------------------------------------------------*/
/*	Epoc32 machine dependent features.				*/
/*----------------------------------------------------------------------*/
/*................................................*/
#ifdef	EPOC32		/* Do not edit this line. */
/*................................................*/

/* This port have no options yet */

/*................................................*/
#endif			/* Do not edit this line. */
/*................................................*/

/************************************************************************/
/*	Do not edit following lines.					*/
/************************************************************************/

#ifdef NO_DIR
#  ifndef NO_STARTUP
#    define NO_STARTUP
#  endif
#  ifdef EXTD_DIR
#    undef EXTD_DIR
#  endif
#endif

#ifdef NO_MACRO
#  ifndef NO_STARTUP
#    define NO_STARTUP
#  endif
#endif

#ifdef FEPCTRL
#  ifndef KANJI
#    undef FEPCTRL
#  endif
#endif

#ifdef TCCONIO
#  ifndef WITHOUT_TERMCAP
#    define WITHOUT_TERMCAP
#  endif
#endif

#ifdef NEW_COMPLETE
#  ifndef ADDFUNC
#    define ADDFUNC
#  endif
#endif

#ifndef	NO_DIRED
#  ifndef ADDFUNC
#    define ADDFUNC
#  endif
#endif

#ifdef JUMPERR
#  ifndef ADDFUNC
#    define ADDFUNC
#  endif
#  ifndef REGEX
#    define REGEX
#  endif
#endif

#ifdef DROPFILES
#  ifndef ADDFUNC
#    define ADDFUNC
#  endif
#endif

#ifdef MSDOS
#  ifdef IBMPC
#    ifdef PC9801
#      undef PC9801
#    endif
#  endif
#endif

#ifdef SAS6
#  ifdef DO_ICONIFY
#    undef DO_ICONIFY
#  endif
#  ifdef USE_ARP
#    undef USE_ARP
#  endif
#endif

#ifdef BROWSER
#  ifndef  DO_MENU
#    define DO_MENU
#  endif
#endif

#ifdef DIRECT_IOCS
#  ifndef WITHOUT_TERMCAP
#    define WITHOUT_TERMCAP
#  endif
#endif

#ifdef CLIPBOARD
#  ifndef WIN32
#    ifndef AMIGA
#      undef CLIPBOARD
#     endif
#   endif
#endif

#ifdef MOUSE
#  ifndef WIN32
#    ifndef AMIGA
#      undef MOUSE
#     endif
#   endif
#endif

#ifdef V2
#  ifdef V11
#    undef V11
#  endif
#endif

#ifdef ASL
#  ifndef V2
#    undef ASL
#  else
#    ifndef DO_MENU
#      undef ASL
#    endif
#  endif
#endif

#ifndef	SUPPORT_ANSI
#  ifdef __STDC__
#    define SUPPORT_ANSI
#  else
#    ifdef __GNUC__ /* gcc support ANSI-C */
#      define SUPPORT_ANSI
#    else
#      ifdef WIN32 /* all compilers on Win32 are ANSI-C! */
#        define SUPPORT_ANSI
#      endif
#    endif
#  endif
#endif
