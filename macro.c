/* $Id: macro.c,v 1.3.2.1 2005/04/07 17:15:19 amura Exp $ */
/* keyboard macros for MicroGnuEmacs 1x */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#ifndef NO_MACRO
#include "def.h"

#include "key.h"
#define EXTERN
#define INIT(i) = (i)
#include "macro.h"
#include "echo.h"
#include "line.h"

/*ARGSUSED*/
int
definemacro(f, n)
int f, n;
{
    register LINE *lp1;
    LINE *lp2;

    macrocount = 0;
    if (macrodef) {
	ewprintf("already defining macro");
	return macrodef = FALSE;
    }
    /* free lines allocated for string arguments */
    if (maclhead != NULL) {
	for (lp1 = maclhead->l_fp; lp1 != maclhead; lp1 = lp2) {
	    lp2 = lp1->l_fp;
	    free((char *)lp1);
	}
	free((char *)lp1);
    }
    if ((maclhead = lp1 = lalloc(0)) == NULL) return FALSE;
    ewprintf("Defining Keyboard Macro...");
    maclcur = lp1->l_fp = lp1->l_bp = lp1;
    return macrodef = TRUE;
}

/*ARGSUSED*/
int
finishmacro(f, n)
int f, n;
{
    macrodef = FALSE;
    ewprintf("End Keyboard Macro Definition");
    return TRUE;
}

/*ARGSUSED*/
int
executemacro(f, n)
int f, n;
{
    int i, j;
    PF funct;
    int flag, num;
	
    if (macrodef ||
	(macrocount >= MAXMACRO && macro[MAXMACRO].m_funct != finishmacro))
	return FALSE;
    if (macrocount == 0)
	return TRUE;
    inmacro = TRUE;
    for (i = n; i > 0; i--) {
	maclcur = maclhead->l_fp;
	flag = 0;
	num = 1;
	for (j = 0; j < macrocount-1; j++) {
	    funct = macro[j].m_funct;
	    if (funct == universal_argument) {
		flag = FFARG;
		num = macro[++j].m_count;
		continue;
	    }
	    if ((*funct)(flag, num) != TRUE) {
		inmacro = FALSE;
		return FALSE;
	    }
	    lastflag = thisflag;
	    thisflag = 0;
	    flag = 0;
	    num = 1;
	}
    }
    inmacro = FALSE;
    return TRUE;
}
#endif /* !NO_MACRO */
