/* $Id: version.c,v 1.15.2.5 2006/01/14 19:59:59 amura Exp $ */
/*
 * This file contains the string that get written
 * out by the emacs-version command.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "version.h"

#include "i_buffer.h"
#include "i_window.h"
#include "echo.h"
#include "buffer.h"
#include "window.h"

#define	VERSION		"1.99.-1"

# define PROGNAME	"Ng"
# define FORMERLY	"[Mg++ Next Generation]"

#if defined(MSDOS)
# if defined(IBMPC)
#  ifdef TCCONIO
#   define TARGET	"for IBM PC/TCCONIO"
#  else
#   define TARGET	"for IBM PC"
#  endif
# elif defined(PC9801)
#  ifdef TCCONIO
#   define TARGET	"for PC-9801/TCCONIO"
#  else
#   define TARGET	"for PC-9801"
#  endif
# else
#  define TARGET	"for MS-DOS"
# endif
#elif defined(AMIGA)
# if defined(V2)
#  define TARGET	"for AmigaDOS 2+"
# elif defined(V11)
#  define TARGET	"for AmigaDOS 1.1+"
# else
#  define TARGET	"for AmigaDOS 1+"
# endif
#elif defined(WIN)
# define TARGET		"for Win32 Ver.0.5.24"
#endif

#ifdef TARGET
char version[] = PROGNAME " " VERSION " " TARGET " " FORMERLY ;
#else
char version[] = PROGNAME " " VERSION " " FORMERLY ;
#endif

/*
 * Display the version. All this does
 * is copy the version string onto the echo line.
 */
/*ARGSUSED*/
int
showversion(f, n)
int f, n;
{
    ewprintf(version);
    return TRUE;
}

static char *compile_opts[] = {
#ifdef ADDFUNC	/* 90.12.28  by S.Yoshida */
#ifndef	NO_BACKUP
    "BACKUP\t\t(Enable backup files)",
#endif
#ifdef C_MODE
    "C_MODE\t\t(Enable \"c-mode\" functions)",
#endif
#ifdef BUFFER_MODE
    "BUFFER_MODE\t(Enable \"Buffer Menu\" mode)",
#endif
#ifdef NEW_COMPLETE
    "COMPLETE\t(Enable new version completion)",
#endif
#ifndef	NO_DIR
# ifdef EXTD_DIR
    "DIR\t\t(Enable buffer local directory change)",
# else
    "DIR\t\t(Enable directory change functions)",
# endif
#endif
#endif	/* ADD_FUNC */
#ifndef	NO_DIRED
    "DIRED\t\t(Enable \"dired\" mode functions)",
#endif
#ifndef	NO_DPROMPT
    "DPROMPT\t\t(Enable delayed prompt)",
#endif
#ifdef FEPCTRL
    "FEPCTRL\t\t(Enable FEP auto control)",
#endif
#ifndef	NO_FILECOMP
    "FILECOMP\t(Enable file name completion)",
#endif
#ifdef FILLPREFIX
    "FILLPREFIX\t(Enable fill prefix function)",
#endif
#ifndef	NO_HELP
    "HELP\t\t(Enable help)",
#endif
#ifdef KINSOKU
    "KINSOKU\t\t(Enable KINSOKU handling)",
#endif
#ifndef NO_MACRO
    "MACRO\t\t(Enable keyboard macros)",
#endif
#ifdef DO_METAKEY
    "METAKEY\t\t(Enable META key input)",
#endif
#ifdef PREFIXREGION
    "PREFIXREGION\t(Enable \"prefix-region\")",
#endif
#ifdef READONLY
    "READONLY\t(Enable read-only buffer mode)",
#endif
#ifdef REGEX
    "REGEX\t\t(Enable regular expression functions)",
#endif
#ifndef NO_SHELL
    "SHELL\t\t(Enable shell-command function)",
#endif
#ifndef NO_STARTUP
    "STARTUP\t\t(Enable startup files)",
#endif
#ifdef ADDFUNC
    "ADDFUNC\t\t(Enable misc functions)",
#endif
#ifdef VARIABLE_TAB
    "VARIABLE_TAB\t(Enable buffer-local tabwidth)",
#endif
#ifdef ADDOPT
    "ADDOPT\t\t(Enable additional startup option)",
#endif
#ifdef CLIPBOARD
    "CLIPBOARD\t(Enable yank-buffer to/from clipboard)",
#endif
#ifdef MINIBUF_EDIT
    "MINIBUF_EDIT\t(Enable minibuffer edit)",
#endif
#ifdef CANNA
    "CANNA\t\t(Enable CANNA)",
#endif
#ifdef UNDO
    "UNDO\t\t(Enable undo)",
#endif
#ifdef DROPFILES
    "DROPFILES\t(Enable Drag&Drop)",
#endif
#ifdef DO_MENU
    "DO_MENU\t\t(Enable menu selection)",
#endif
#ifdef MOUSE
    "MOUSE\t\t(Enable to use mouse)",
#endif
#ifdef FKEYS
    "FKEYS\t\t(Enable to use function keys)",
#endif
#ifdef DO_ICONIFY
    "DO_ICONIFY\t(Enable iconify)",
#endif
#ifdef CHANGE_COLOR
    "COLOR\t\t(Enable to color setting)",
#endif
#ifdef CHANGE_FONT
    "FONT\t\t(Enable to font setting)",
#endif
#ifdef USE_ARP
    "ARP\t\t(using ARP.Library)",
#endif
#ifdef REXX
    "REXX\t\t(Enable to use ARexx)",
#endif
#ifdef ASL
    "ASL\t\t(Enable to use ASL Filerequester)",
#endif
};
#define COMPILE_OPTS_NUM	(sizeof(compile_opts)/sizeof(char *))

/*
 * Display the Ng version, compile time options.
 */
/*ARGSUSED*/
int
showngversion(f, n)
int f, n;
{
    register BUFFER *bp;
    register WINDOW *wp;
    NG_WCHAR_t line[80];
    int i;

    if ((bp = bfind("*" PROGNAME " Version*", TRUE)) == NULL)
	return FALSE;
    if (bclear(bp) != TRUE)
	return FALSE;
    bp->b_flag &= ~BFCHG;		/* Blow away old.	*/
#ifdef READONLY
    bp->b_flag |= BFRONLY;
#endif
    if (bclear(bp) != TRUE)
	return FALSE;
    
    wsnprintf(line, NG_WCHARLEN(line), PROGNAME " version:");
    if (addline(bp, line) == FALSE)
	return FALSE;
    wsnprintf(line, NG_WCHARLEN(line), "\t%s", version);
    if (addline(bp, line) == FALSE)
	return FALSE;
    wsnprintf(line, NG_WCHARLEN(line), "Compile time options:");
    if (addline(bp, line) == FALSE)
	return FALSE;
    
    for (i=0; i<COMPILE_OPTS_NUM; i++) {
	wsnprintf(line, NG_WCHARLEN(line), "\t%s", compile_opts[i]);
	if (addline(bp, line) == FALSE)
	    return FALSE;
    }
    
    if ((wp = popbuf(bp)) == NULL)
	return FALSE;
    bp->b_dotp = lforw(bp->b_linep); /* put dot at beginning of buffer */
    bp->b_doto = 0;
    wp->w_dotp = bp->b_dotp;	/* fix up if window already on screen */
    wp->w_doto = bp->b_doto;
    return TRUE;
}

/*
 * Print the Ng version to the stdout.
 */
int
printversion()
{
#ifdef WIN32
    char buf[128], *ptr = buf ;
    
    sprintf(ptr, PROGNAME " version:\n");
    ptr += strlen(ptr) ;
    sprintf(ptr, "\t%s\n", version);
    MessageOut(buf) ;
#else /* WIN32 */
    printf(PROGNAME " version:\n");
    printf("\t%s\n", version);
#endif /* WIN32 */
    return TRUE;
}

/*
 * Print the Ng compile time options.
 */
int
printoptions()
{
#ifndef	WIN32
    int i;
    
    printf("Compile time options:\n");
    for (i=0; i<COMPILE_OPTS_NUM; i++)
	printf("\t%s\n", compile_opts[i]);
#endif /* WIN32 */
    return TRUE;
}

