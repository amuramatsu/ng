/* $Id: ttykbd.c,v 1.4.2.1 2006/01/14 19:59:59 amura Exp $ */
/*
 * Name:	MG 2a
 *		Termcap keyboard driver using key files
 * Created:	22-Nov-1987 Mic Kaczmarczik (mic@emx.cc.utexas.edu)
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */

#ifdef	XKEYS
#include "def.h"

#include "extend.h"
#include "echo.h"
#include "tty.h"

VOID
#ifdef ADDOPT
ttykeymapinit(ngrcfile)
char *ngrcfile;
#else
ttykeymapinit()
#endif
{
#ifndef	NO_STARTUP
    char *cp, *startupfile();

    if ((cp = gettermtype()) != NULL) {
#ifdef ADDOPT
	if (((cp = startupfile(ngrcfile, cp)) != NULL)
#else
	if (((cp = startupfile(cp)) != NULL)
#endif
	    && (load(cp) != TRUE))
	ewprintf("Error reading key initialization file");
    }
#endif
}

/*
 * Start keypad mode -- called by update() and spawncli()
 */
VOID
ttykeypadstart()
{
#ifndef	WITHOUT_TERMCAP
    extern char *KS;
    if (KS && *KS)			/* turn on keypad	*/
	putpad(KS, 1);
#endif
}

/*
 * Clean up the keyboard -- called by tttidy() and spawncli()
 */
VOID
ttykeymaptidy()
{
#ifndef	WITHOUT_TERMCAP
    extern char *KE;
    if (KE && *KE)
	putpad(KE, 1);	/* turn off keypad		*/
#endif
}

#endif /* XKEYS */
