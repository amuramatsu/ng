/* $Id: shell.c,v 1.5.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 *		Shell commands.
 * The file contains the command
 * processor, shell-command
 *	written by Kaoru Maeda.
 */

#include "config.h"	/* 91.01.10  by S.Yoshida */
#include "def.h"


#ifndef NO_SHELL

#include "shell.h"
#include "buffer.h"
#include "echo.h"

/*ARGSUSED*/
int
shellcmnd(f, n)
int f, n;
{
    char buf[CMDLINELENGTH];
    char *result;
    extern char *call_process _PRO((char *, char *));
    extern int isetmark _PRO((void)), gotobob _PRO((int, int));
    int s;
    BUFFER *bp = NULL, *obp = NULL;
    WINDOW *wp = NULL, *owp = NULL;
    WINDOW *popbuf _PRO((BUFFER *));
    
    s = ereply("Shell command: ", buf, sizeof buf);
    if (s != TRUE)
	return s;
    if ((f & FFARG) == 0) {
	if ((bp = bfind("*Shell Command Output*", TRUE)) == NULL)
	    return FALSE;
	if ((wp = popbuf(bp)) == NULL)
	    return FALSE;
	bp->b_flag &= ~BFCHG;		/* Blow away old.	*/
	if (bclear(bp) != TRUE)
	    return FALSE;
	obp = curbp; owp = curwp;
	curbp = bp; curwp = wp;
    }
    if ((result = call_process(buf, NULL)) == NULL)
	return FALSE;
    isetmark();
    ewprintf(result);
    s = insertfile(result, (char *)NULL);
    if ((f & FFARG) == 0) {
	(VOID) gotobob(0, 1);
	bp->b_dotp = wp->w_dotp;
	bp->b_doto = wp->w_doto;
	curbp = obp;
	curwp = owp;
#ifdef AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
	bp->b_flag &= ~(BFCHG | BFACHG);
#else
	bp->b_flag &= ~BFCHG;
#endif /* AUTOSAVE	*/
    }
    unlink(result);
    return s;
}
#endif
