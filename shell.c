/* $Id: shell.c,v 1.5.2.6 2006/01/04 17:00:40 amura Exp $ */
/*
 *		Shell commands.
 * The file contains the command
 * processor, shell-command
 *	written by Kaoru Maeda.
 */

#include "config.h"	/* 91.01.10  by S.Yoshida */

#ifndef NO_SHELL
#include "def.h"
#include "shell.h"

#include "i_buffer.h"
#include "i_window.h"
#include "i_lang.h"
#include "buffer.h"
#include "basic.h"
#include "file.h"
#include "echo.h"

/*ARGSUSED*/
int
shellcmnd(f, n)
int f, n;
{
    NG_WCHAR_t buf[CMDLINELENGTH];
    char *result;
    int s;
    BUFFER *bp = NULL, *obp = NULL;
    WINDOW *wp = NULL, *owp = NULL;
    
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
    {
	/* code conversion */
	char *tmp;
	LM_OUT_CONVERT_TMP2(curbp->b_lang, NG_CODE_FOR_IO, buf, tmp);
	if (tmp == NULL)
	    return FALSE;
	if ((result = call_process(tmp, NULL)) == NULL)
	    return FALSE;
    }
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
