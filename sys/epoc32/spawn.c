/* $Id: spawn.c,v 1.2 2001/11/28 21:45:12 amura Exp $ */
/*
 *		Spawn CLI for EPOC32. (Tested only at Psion S5mx)
 *		!ALMOST FUNCTIONS ARE NOT IMPLEMENTED!
 */

/*
 * $Log: spawn.c,v $
 * Revision 1.2  2001/11/28 21:45:12  amura
 * Rewrite to new source code style
 *
 * Revision 1.1  2001/09/30 15:59:12  amura
 * Initial EPOC32 commit.
 *
 * Currently this is not run yet. Some functions around tty are not implemented.
 *
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#include <signal.h>
#include <process.h>
#include <errno.h>
#include <stdlib.h>

static char *shellp = NULL;		/* Saved "SHELL" name.		*/

/*
 * On EPOC32
 */
/*ARGSUSED*/
spawncli(f, n)
{
    /* IMPLEMENT ME */
    return FALSE;
}

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
#include <sys/types.h>
#include <sys/stat.h>

static char tempfile[CMDLINELENGTH];

/*
 *	Call process in subshell.
 * Execute COMMAND binding standard input to file INPUT.
 * NULL as INPUT means standard input should be bound to
 * /dev/null or whatever equivalent in your OS.
 * All output during the execution (including standard error output)
 * should go into a scratch file, whose name call_process() returns.
 * Return value NULL means error in some stage of the execution.
 * In that case, scratch file should be deleted.
 */
char *
call_process(command, input)
char *command;
char *input;
{
    /* IMPLEMENT ME */
    return NULL;
}
#endif	/* NO_SHELL */
