/* $Id: spawn.c,v 1.2 2001/11/23 11:56:50 amura Exp $ */
/*
 *		Spawn CLI for MS-DOS. (Tested only at MS-DOS 3.1)
 *
 *		I make this file from UNIX System V spawn.c.
 */

/*
 * $Log: spawn.c,v $
 * Revision 1.2  2001/11/23 11:56:50  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#include <signal.h>
#include <process.h>
#inlcude <stdlib.h>

#if defined(__TURBOC__) && __TURBOC__ < 0x0200	/* 90.12.28  by Junn Ohta */
						/* For Turbo-C 1.5	  */
#include <errno.h>
static VOID (*oldfunc)();
static VOID (*curfunc)() = SIG_DFL;
extern void ctrlbrk(int (*fptr)(void));

static int sigfunc(void)
{
    if (curfunc == SIG_DFL)
	return 0;	/* abort */
    if (curfunc != SIG_IGN) {
	oldfunc = curfunc;
	curfunc = SIG_DFL;
	(*oldfunc)();
    }
    return 1;		/* resume */
}

VOID (*signal(int sig, int (*func)()))()
{
    if (sig != SIGINT) {
	errno = EINVAL;
	return SIG_ERR;
    }
    ctrlbrk(sigfunc);
    oldfunc = curfunc;
    curfunc = func;
    return oldfunc;
}
#endif	/* __TURBOC__ */

static char *shellp = NULL;		/* Saved "SHELL" name.		*/

/*
 * On MS-DOS, we no gots job control, so always run
 * a subshell using spawn. Bound to "C-C", and used
 * as a subcommand by "C-Z".
 *
 * Returns 0 if the shell executed OK, something else if
 * we couldn't start shell or it exited badly.
 */
/*ARGSUSED*/
int
spawncli(f, n)
int f, n;
{
    register VOID (*oisig)();
    int status;
    
    if (shellp == NULL) {
	shellp = getenv("SHELL");
	if (shellp == NULL)
	    shellp = getenv("shell");
	if (shellp == NULL)
	    shellp = getenv("COMSPEC");
	if (shellp == NULL)
	    shellp = getenv("comspec");
	if (shellp == NULL)
	    shellp = "command.com";	/* Safer.		*/
    }
    ttcolor(CTEXT);
    ttnowindow();
    ttmove(nrow-1, 0);
    if (epresf != FALSE) {
	tteeol();
	epresf = FALSE;
    }
    ttclose();
    sgarbf = TRUE;				/* Force repaint.	*/
    oisig = signal(SIGINT,  SIG_IGN);
#ifdef	EXTD_DIR
    dirend();
#endif
    if (spawnlp(P_WAIT, shellp, NULL) == -1)
	status = FALSE;
    else
	status = TRUE;
    signal(SIGINT,  oisig);
    ttopen();
    if (status == FALSE) {
	ewprintf("Failed to run %s", shellp);
	sleep(2);		/* Show this message for 2 sec	*/
				/* before refresh screen.	*/
    }
    return (status);
}

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
#if !defined(__TURBOC__) || __TURBOC__ >= 0x0200
#include <sys/types.h>
#endif	/* __TURBOC__ */
#include <sys/stat.h>
#include <io.h>

char tempfile[NFILEN];

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
    char buf[CMDLINELENGTH];
    char *tmp;
    int ostdin, ostdout, ostderr, in, out, s;
    char *temp_path;
    
    temp_path = getenv("TMP");
    if (temp_path == NULL)
	temp_path = getenv("tmp");
    if (temp_path == NULL)
	temp_path = getenv("TEMP");
    if (temp_path == NULL)
	temp_path = getenv("temp");

    tmp = tempfile;
    if (temp_path && *temp_path) {
	while (*tmp = *temp_path++)
	    tmp++;
	if (tmp[-1] != '/' && tmp[-1] != '\\')
	    *tmp++ = '/';
    }
    strcpy(tmp, "ngXXXXXX");
    if ((tmp = mktemp(tempfile)) == NULL)
	return NULL;
    if ((in = open(input ? input : "nul", 0)) < 0)
	return NULL;
    if ((out = creat(tmp, S_IREAD | S_IWRITE)) < 0) {
	close(in);
	return NULL;
    }
    ostdin = dup(0); ostdout = dup(1); ostderr = dup(2);
    if (ostdin < 0 || ostdout < 0 || ostderr < 0) {
	s = -1;
	goto skip;
    }
    dup2(in, 0);
    dup2(out, 1);
    dup2(out, 2);
    strcpy(buf, command);
#ifdef	EXTD_DIR
    ensurecwd();
#endif
    s = system(buf);
    close(in);
    close(out);
    dup2(ostdin, 0); dup2(ostdout, 1); dup2(ostderr, 2);
skip:
    close(ostdin); close(ostdout); close(ostderr);
    if (s == -1) {
	unlink(tmp);
	return NULL;
    }
    return tmp;
}
#endif	/* NO_SHELL */
