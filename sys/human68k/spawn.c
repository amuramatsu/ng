/* $Id: spawn.c,v 1.1 2000/06/27 01:47:57 amura Exp $ */
/*
 *		Spawn CLI for Human68k
 */

/*
 * $Log: spawn.c,v $
 * Revision 1.1  2000/06/27 01:47:57  amura
 * Initial revision
 *
 */
/* 90.11.10	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */
/*		Spawn CLI for UNIX System V */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"

#include	<signal.h>
#include	<process.h>

char	*shellp = NULL;			/* Saved "SHELL" name.		*/

extern	char	*getenv();

/*
 * On Human68k, we no gots job control, so always run
 * a subshell using spawn. Bound to "C-C", and used
 * as a subcommand by "C-Z".
 *
 * Returns 0 if the shell executed OK, something else if
 * we couldn't start shell or it exited badly.
 */
/*ARGSUSED*/
spawncli(f, n) {
	register void	(*oisig)();
	int		status;
#ifdef	EXTD_DIR
	ensurecwd();
#endif

	if (shellp == NULL) {
		shellp = getenv("SHELL");
		if (shellp == NULL)
			shellp = getenv("shell");
		if (shellp == NULL)
			shellp = "command.x";	/* Safer.		*/
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
	if (spawnlp(P_WAIT, shellp, shellp, NULL) == -1) {
		status = FALSE;
	} else {
		status = TRUE;
	}
	(VOID) signal(SIGINT,  oisig);
	ttopen();
	if(status == FALSE) {
		ewprintf("Failed to run %s", shellp);
		sleep(2);		/* Show this message for 2 sec	*/
					/* before refresh screen.	*/
	}
	dirinit();
	return (status);
}

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */

char temp_file[128];

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
	char buf[256];
	char *tmp;
	int ostdin, ostdout, ostderr, in, out, s;
	extern char *mktemp();
	char *temp_path;

	temp_path = getenv("TMP");
	if (temp_path == NULL)
		temp_path = getenv("tmp");
	if (temp_path == NULL)
		temp_path = getenv("TEMP");
	if (temp_path == NULL)
		temp_path = getenv("temp");

	tmp = temp_file;
	if (temp_path && *temp_path) {
		while (*tmp = *temp_path++) {
			tmp++;
		}
		if (tmp[-1] != '/' && tmp[-1] != '\\') {
			*tmp++ = '/';
		}
	}
	strcpy(tmp, "ngXXXXXX");
	if ((tmp = mktemp(temp_file)) == NULL) {
		return NULL;
	}
	if ((in = open(input ? input : "nul", 1)) < 0) {
		return NULL;
	}
	if ((out = creat(tmp, 0x20)) < 0) {
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
skip:	close(ostdin); close(ostdout); close(ostderr);
	if (s == -1) {
		unlink(tmp);
		return NULL;
	}
	return tmp;
}
#endif	/* NO_SHELL */
