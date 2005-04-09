/* $Id: main.c,v 1.11.2.2 2005/04/09 06:26:14 amura Exp $ */
/*
 *		Mainline
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "main.h"

#include "i_buffer.h"
#include "i_window.h"
#include "version.h"
#include "macro.h"
#include "echo.h"
#include "dir.h"
#include "dired.h"
#include "file.h"
#include "fileio.h"
#include "extend.h"
#include "buffer.h"
#include "kbd.h"
#include "display.h"
#include "tty.h"
#include "ttyio.h"

#include "basic.h" /* for gotoline() */

int thisflag;				/* Flags, this command		*/
int lastflag;				/* Flags, last command		*/
int curgoal;				/* Goal column			*/
BUFFER *curbp;				/* Current buffer		*/
WINDOW *curwp;				/* Current window		*/
BUFFER *bheadp;				/* BUFFER listhead		*/
WINDOW *wheadp = (WINDOW *)NULL;	/* WINDOW listhead		*/
char pat[NPAT];				/* Pattern			*/
#ifndef NO_DPROMPT
extern char prompt[], *promptp;		/* delayed prompting		*/
#endif

static VOID edinit _PRO((void));

#ifdef WIN32
VOID
Main(int argc, char**argv)
#else	/* WIN32 */
int
main(argc, argv)
int argc;
char **argv;
#endif	/* WIN32 */
{
#ifdef ADDOPT
    char *startupfunc = (char *)NULL;
    int line = 0;
    int fgoto = FALSE;
#ifndef	NO_STARTUP
    char *ngrcfile = (char *)NULL;
#endif
#endif
    char *cp;
    BUFFER *tmpbp;	/* 91.02.17 by N.Yuasa */
	
#ifdef	ADDFUNC	/* 90.12.28  by S.Yoshida */
    if (argc > 1 && argv[1][0] == '-') {
	switch (argv[1][1]) {
	case 'v':
	    printversion();
	    exit(GOOD);
	case 'c':
	    printversion();
	    printoptions();
	    exit(GOOD);
	case '-':
	    --argc;
	    argv++;
	    break;
#ifdef ADDOPT
#ifndef	NO_STARTUP
	case 'I':
	    if (argv[1][2])
		ngrcfile = &argv[1][2];
	    else {
		ngrcfile = argv[2];
		--argc;
		argv++;
	    }
	    --argc;
	    argv++;
	    break;
#endif /* NO_STARTUP */
	case 'f':
	    if (argv[1][2])
		startupfunc = &argv[1][2];
	    else {
		startupfunc = argv[2];
		--argc;
		argv++;
	    }
	    --argc;
	    argv++;
	    break;
#endif /* ADDOPT */
	default:
	    break;
	}
    }
#endif

#ifdef SYSINIT
    SYSINIT;				/* system dependent.	*/
#endif
    vtinit();				/* Virtual terminal.	*/
#ifndef NO_DIR
    dirinit();				/* Get current directory */
#endif
    edinit();				/* Buffers, windows.	*/
#ifdef ADDOPT
# ifndef NO_STARTUP
    ttykeymapinit(ngrcfile);		/* Symbols, bindings.	*/
# else
    ttykeymapinit();			/* Symbols, bindings.	*/
# endif
#else
    ttykeymapinit();			/* Symbols, bindings.	*/
#endif
    /* doing update() before reading files causes the error messages from
     * the file I/O show up on the screen.	(and also an extra display
     * of the mode line if there are files specified on the command line.)
     */
    update();
#ifndef NO_STARTUP				/* User startup file.	*/
    if ((cp = startupfile(ngrcfile, (char *)NULL)) != NULL)
	(VOID) load(cp);
#endif	/* NO_STARTUP */
    while (--argc > 0) {
#ifdef ADDOPT
	if ((!fgoto) && argv[1][0] == '+'){
	    line = atoi(&argv[1][1]);
	    ++argv;
	    fgoto = TRUE;
	    continue;
	}
#endif	/* ADDOPT */
#if defined(KANJI)&&(defined(MSDOS)||defined(HUMAN68K)||defined(WIN32))
	{
	    char argve[NFILEN];
	    strncpy(argve, *++argv, NFILEN);
	    argve[NFILEN-1] = 0;
	    bufstoe(argve, strlen(argve) + 1);
	    cp = adjustname(argve);
	}
#else
	cp = adjustname(*++argv);
#endif	/* KANJI and (MSDOS or HUMAN68K or WIN32) */

#ifndef NO_DIRED	/* 91.01.16  by S.Yoshida */
	if (ffisdir(cp)) {
	    eargset(cp);
	    (VOID) dired(0, 1);
#ifdef ADDOPT  /* 92.03.16  by Gen KUROKI */
	    if (fgoto) {
		gotoline(1,line);
		fgoto = FALSE;
	    }
#endif /* ADDOPT */
	    continue;
	}
#endif	/* NO_DIRED */
	if ((tmpbp = findbuffer(cp)) == NULL)	/* 91.02.17 by N.Yuasa */
	    break;
	else
	    curbp = tmpbp;
	(VOID) showbuffer(curbp, curwp, 0);
	(VOID) readin(cp);
#ifdef	READONLY	/* 91.01.16  by S.Yoshida */
	if (fchkreadonly(curbp->b_fname)) {
	    /* If no write permission, */
	    curbp->b_flag |= BFRONLY; /* mark as read-only.      */
	    ewprintf("File is write protected");
	}
#ifdef ADDOPT  /* 92.03.16  by Gen KUROKI */
	if (fgoto) {
	    gotoline(1,line);
	    fgoto = FALSE;
	}
#endif /* ADDOPT */
    }
#endif	/* READONLY */
    thisflag = 0;				/* Fake last flags.	*/

#ifdef ADDOPT
    if (startupfunc) {
	PF fn;
	fn = name_function(startupfunc);
	if (fn) {
	    (*fn)(FFRAND, 1);
	}
    }
#endif /* ADDOPT */
    for(;;) {
#ifdef C_ALLOCA
	(void)alloca(0);	/* for garbage correction */
#endif
#ifndef NO_DPROMPT
	*(promptp = prompt) = '\0';
	if (epresf == KPROMPT)
	    eerase();
#endif
	update();
	lastflag = thisflag;
	thisflag = 0;
	switch(doin()) {
	case TRUE:
	    break;
	case ABORT:
	    ewprintf("Quit");
	    /*FALLTHRU*/
	case FALSE:
	default:
	    ttbeep();
#ifdef KANJI
	    kgetkeyflush();
#endif
#ifndef NO_MACRO
	    macrodef = FALSE;
#endif
	    }
	}
#ifndef	WIN32
    return 0;
#endif
}

/*
 * Initialize default buffer and window.
 */
static VOID
edinit()
{
    register BUFFER *bp;
    register WINDOW *wp;

    bheadp = NULL;
    bp = bfind("*scratch*", TRUE);		/* Text buffer.		*/
    wp = (WINDOW *)malloc(sizeof(WINDOW));	/* Initial window.	*/
    if (bp==NULL || wp==NULL)
	panic("edinit");
    curbp  = bp;				/* Current ones.	*/
    wheadp = wp;
    curwp  = wp;
    wp->w_wndp  = NULL;				/* Initialize window.	*/
    wp->w_bufp  = bp;
    bp->b_nwnd  = 1;				/* Displayed.		*/
    wp->w_linep = wp->w_dotp = bp->b_linep;
    wp->w_lines = 0;
    wp->w_doto  = 0;
    bp->b_markp = NULL;
    bp->b_marko = 0;
    wp->w_toprow = 0;
    wp->w_ntrows = nrow-2;			/* 2 = mode, echo.	*/
    wp->w_force = 0;
    wp->w_flag  = WFMODE|WFHARD;		/* Full.		*/
}

/*
 * Quit command. If an argument, always
 * quit. Otherwise confirm if a buffer has been
 * changed and not written out. Normally bound
 * to "C-X C-C".
 */
/*ARGSUSED*/
int
quit(f, n)
int f, n;
{
    register int s;

    if ((s = anycb(FALSE)) == ABORT)
	return ABORT;
    if (s == FALSE
	|| eyesno("Some modified buffers exist, really exit") == TRUE) {
	vttidy();
#ifndef NO_DIR
	dirend();
#endif
#ifdef	SYSCLEANUP
	SYSCLEANUP;
#endif
	exit(GOOD);
    }
    return TRUE;
}

/*
 * User abort. Should be called by any input routine that sees a C-g
 * to abort whatever C-g is aborting these days. Currently does
 * nothing.
 */
/*ARGSUSED*/
int
ctrlg(f, n)
int f, n;
{
    return ABORT;
}
