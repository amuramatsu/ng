/* $Id: config.h,v 1.12.2.1 2003/03/08 01:11:49 amura Exp $ */
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
			/* If defined, ADDFUNC must be also. */

#define	MINIBUF_EDIT	/* minibuffer edit like GNU emacs (by Kakugawa)	*/

#define	NEXTLINE 1	/* next-line-add-newlines (by S.Yoshida) */

#define EMACS_QUERY_KEYS /* isearch/query-replace key bindings compatible */
			/* with GNU Emacs 19 and later (by Shimbo)*/

#define	UNDO		/* enable buffer undo */

#define AUTOSAVE	/* enable autosaver (ported form M.Suziki's work) */

/*----------------------------------------------------------------------*/
/*	Features for Ng/KANgee (KANJI version Mg2a).			*/
/*----------------------------------------------------------------------*/

#define	KANJI		/* Create KANJI handling version.	*/

#define	HANKANA		/* Create Hankaku KANA  handling version. */

#undef HOJO_KANJI	/* Create Hojo KANJI handling version. */

#define CURSOR_POS	/* Cursor locates on next CHR after POINT */

#define	KINSOKU		/* Create KINSOKU handling version. If defined, */
			/* KANJI must be also.				*/

#define	BUGFIX		/* Fix bugs in the original Mg2a.	*/

#define	ADDFUNC		/* Create additional misc functions.	*/

#define	REGEX_JAPANESE	/* Enable Japanese regular expression.	*/
			/* (by amura, Selow) */

#define	INCLUDE_SKG	/* Enable SKG(Simple Kanji Generator System)	*/
			/* (by H.Konishi) */ 

#define JISFIX		/* JIS code input fix (by Gen.KUROKI) */

#define VTCURSOR	/* Use DEC vt cursor key on JIS input mode */
			/* (by Gen.KUROKI) */

/*----------------------------------------------------------------------*/
/*	SystemV / BSD machine dependent features.			*/
/*----------------------------------------------------------------------*/
/*................................................*/
#ifdef HAVE_CONFIG_H	/* Do not edit this line. */
#include "sysconfig.h"	/* Do not edit this line. */
#else			/* Do not edit this line. */
#if defined(_WIN32)&&!defined(__CYGWIN__)&&!defined(WIN32)
#define	WIN32		/* Do not edit this line. */
#endif			/* Do not edit this line. */
#ifndef	MSDOS		/* Do not edit this line. */
#ifndef	HUMAN68K	/* Do not edit this line. */
#ifndef	AMIGA		/* Do not edit this line. */
#ifndef WIN32		/* Do not edit this line. */
/*................................................*/

#undef	SVR2		/* System V is Release 2.	*/
#undef	SVR3		/* System V is Release 3.x.	*/
#define	SVR4		/* System V is Release 4.x. (or linux)	*/

#define POSIXTTY	/* System V has POSIX termios */

#define XKEYS		/* use numelic keypad (by Gen.KUROKI) */

/*#define CANNA		/* use CANNA kana-kanji server (by Endo) */

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

/*#undef TCCONIO	/* Use Turbo C console IO  (by amura) */

/*#undef IBMPC		/* Create Ng for IBM PC compatible	*/
			/* (include Toshiba J-3100)		*/

/*#undef PC9801		/* Create Ng for NEC PC-9801 series. */

#define BACKSLASH	/* Display backslash instead of \ (PC-9801 only) */

#define HOMEDIR		/* Home directory support for MSDOS (by amura) */

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

#define DROPFILES	/* Drag'n Drop file open */
			/* If defined, ADDFUNC must be also. */

#define HOMEDIR		/* Home directory support */

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

#define HOMEDIR		/* Home directory support */

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

#define V2		/* Don't support AmigaDOS 1.x (by amura) */

#define	DO_MENU		/* Enable Menu selection */

#define	MOUSE		/* Enable mouse */

#define	FKEYS		/* Enable to use function key */

#define	DO_ICONIFY	/* Enable iconify */

#define	CHANGE_COLOR	/* Enable color setting */

#define	CHANGE_FONT	/* Enable font setting */

#undef	USE_ARP		/* Enable ARP Library */
			/* Undefined by H.Konishi for SAS */ 

#define	LAMIGA_META	/* Assume Left Amiga to META key (by amura) */

#undef	MODE_RENDITION	/* Set Default */
#undef	TEXT_RENDITION	/* Set Default */
#undef	TEXT_FG		/* Set Default */
#undef	TEXT_BG		/* Set Default */
#undef	MODE_FG		/* Set Default */
#undef	MODE_BG		/* Set Default */

#define ASL             /* Enable ASL Requester (by H.Konishi) */

#undef	AMIGA_STDIO	/* Using ANSI-Standard FILE IO library (by amura) */
#define	CLIPBOARD	/* AMIGA Clipboard support (by amura) */

/*................................................*/
#endif			/* Do not edit this line. */
/*................................................*/

/************************************************************************/
/*	Do not edit following lines.					*/
/************************************************************************/

#ifdef KANJI
# ifdef NO_KANJI /* in order to use same source for Ng and MG, Tillanosoft */
#  undef KANJI
# endif
#else /* not KANJI */
# ifdef HANKANA
#  undef HANKANA
# endif
# ifdef HOJO_KANJI
#  undef HOJO_KANJI
# endif
#endif

#ifdef HANKANA
#  ifndef SS_SUPPORT
#    define SS_SUPPORT
#  endif
#endif

#ifdef HOJO_KANJI
#  ifndef SS_SUPPORT
#    define SS_SUPPORT
#  endif
#endif

#ifndef KANJI
# ifdef JAPANESE_KEYBOARD
#  undef JAPANESE_KEYBOARD
# endif
#endif

#ifdef CURSOR_POS
#  ifndef KANJI
#    undef CURSOR_POS
#  endif
#endif

#ifdef	NO_DIR
#   ifndef  NO_STARTUP
#	define	NO_STARTUP
#   endif
#   ifdef   EXTD_DIR
#	undef EXTD_DIR
#   endif
#endif

#ifdef	NO_MACRO
#   ifndef  NO_STARTUP
#	define	NO_STARTUP
#   endif
#endif

#ifdef	KINSOKU
#   ifndef  KANJI
#	undef	KINSOKU
#   endif
#endif

#ifdef FEPCTRL
# ifndef KANJI
#  undef FEPCTRL
# endif
#endif

#ifdef	NEW_COMPLETE
#   ifndef  ADDFUNC
#	define	ADDFUNC
#   endif
#endif

#ifndef	NO_DIRED
#   ifndef  ADDFUNC
#	define	ADDFUNC
#   endif
#endif

#ifdef	JUMPERR
#   ifndef  ADDFUNC
#	define	ADDFUNC
#   endif
#endif

#ifdef	DROPFILES
#   ifndef  ADDFUNC
#	define	ADDFUNC
#   endif
#endif

#ifdef	MSDOS
#   ifdef   J3100
#	ifdef   PC9801
#	    undef   PC9801
#	endif
#   endif
#endif

#ifdef	DO_ICONIFY
#   ifdef   SAS6
#	undef	DO_ICONIFY
#   endif
#endif

#ifdef	USE_ARP
#   ifdef   SAS6
#	undef	USE_ARP
#   endif
#endif

#ifdef	BROWSER
#   ifndef  DO_MENU
#	define	DO_MENU
#   endif
#endif

#ifdef	REGEX_JAPANESE
#   ifndef  REGEX
#	define	REGEX
#   endif
#   ifndef  KANJI
#	undef   REGEX_JAPANESE
#   endif
#endif

#ifndef	SUPPORT_ANSI
# ifdef __STDC__
#  define SUPPORT_ANSI
# endif
# ifdef WIN32
#  define SUPPORT_ANSI
# endif
#endif

#ifdef WIN32
#define MOUSE
#endif

#ifdef CLIPBOARD
# ifndef WIN32
#  ifndef AMIGA
#   undef CLIPBOARD
#  endif
# endif
#endif

#ifdef V2
# ifdef V11
#  undef V11
# endif
#endif

#ifdef ASL
# ifndef V2
#  undef ASL
# else
#  ifndef DO_MENU
#   undef ASL
#  endif
# endif
#endif

#ifdef SVR2
# ifdef SVR3
#  undef SVR3
# endif
# ifdef SVR4
#  undef SVR4
# endif
#endif

#ifdef SVR3
# ifdef SVR4
#  undef SVR4
# endif
#endif

#ifdef SVR4
# ifndef POSIXTTY
#  define POSIXTTY
# endif
#endif
