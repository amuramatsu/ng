/* $Id: spawn.c,v 1.1 2000/06/27 01:48:02 amura Exp $ */
/*
 * Spawn. New version, which
 * interracts with the job control stuff
 * in the 4.X BSD C shell.
 * Last edit:  Wed Aug 27 11:16:07 PDT 1986
 * By:	       rtech!daveb, to use stop for ksh.
 */

/*
 * $Log: spawn.c,v $
 * Revision 1.1  2000/06/27 01:48:02  amura
 * Initial revision
 *
 */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"

#include	<sgtty.h>
#include	<signal.h>
#include	<sys/wait.h>

char	*shellp = NULL;			/* Saved "SHELL" name.		*/

extern	struct	sgttyb	oldtty;		/* There really should be a	*/
extern	struct	sgttyb	newtty;		/* nicer way of doing this, so	*/
extern	struct	sgttyb	oldtchars;	/* spawn does not need to know	*/
extern	struct	sgttyb	newtchars;	/* about the insides of the	*/
extern	struct	sgttyb	oldltchars;	/* terminal I/O code.		*/
extern	struct	sgttyb	newltchars;

extern	char	*getenv();
#ifdef HAVE_GETSID
extern	int	job_control;
#endif

/*
 * This code does a one of 2 different
 * things, depending on what version of the shell
 * you are using. If you are using the C shell, which
 * implies that you are using job control, then MicroEMACS
 * moves the cursor to a nice place and sends itself a
 * stop signal. If you are using the Bourne shell it runs
 * a subshell using fork/exec. Bound to "C-C", and used
 * as a subcommand by "C-Z".
 *
 * Daveb -- changed sense of test so that we only spawn if you
 *	    are explicitly using /bin/sh.  This makes it stop
 *	    work with the ksh.
 */
/*ARGSUSED*/
spawncli(f, n) {
	register PID_T	pid, wpid;
	register int	omask;
	register VOID	(*oqsig)(),(*oisig)();
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.14  by S.Yoshida */
	register VOID	(*owsig)();
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
	/* union wait	status; */ 	/* change for Digital UNIX */
	int status;

#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
	ttykeymaptidy();
#endif /* XKEYS */
	if (shellp == NULL) {
		shellp = getenv("SHELL");
		if (shellp == NULL)
			shellp = getenv("shell");
		if (shellp == NULL)
			shellp = "/bin/sh";	/* Safer.		*/
	}
	ttcolor(CTEXT);
	ttnowindow();
	if (strcmp(shellp, "/bin/csh") == 0) {
		if (epresf != FALSE) {
			ttmove(nrow-1, 0);
			tteeol();
			epresf = FALSE;
		}				/* Csh types a "\n"	*/
		ttmove(nrow-2, 0);		/* before "Stopped".	*/
	} else {
		ttmove(nrow-1, 0);
		if (epresf != FALSE) {
			tteeol();
			epresf = FALSE;
		}
	}
	if (ttcooked() == FALSE)
		return (FALSE);
#ifdef HAVE_GETSID
	if (job_control) {
#else
	if (strcmp(shellp, "/bin/sh")!=0 ||
	    getenv("BASH_VERSION") || getenv("BASH"))	{
		/* C shell, ksh	or bash	*/
#endif
		omask = sigsetmask(0);
		(void) kill(0, SIGTSTP);
		(void) sigsetmask(omask);
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.14  by S.Yoshida */
		refresh(FFRAND, 0);		/* May be resized.	*/
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
	} else {				/* Bourne shell.	*/
		oqsig = signal(SIGQUIT, SIG_IGN);
		oisig = signal(SIGINT,	SIG_IGN);
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.14  by S.Yoshida */
		owsig = signal(SIGWINCH, SIG_IGN);
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
		if ((pid=fork()) < 0) {
			(void) signal(SIGQUIT, oqsig);
			(void) signal(SIGINT,  oisig);
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.14  by S.Yoshida */
			(void) signal(SIGWINCH, owsig);
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
			ewprintf("Failed to create process");
			return (FALSE);
		}
		if (pid == 0) {
#ifdef	EXTD_DIR
			dirend();
#endif
			execl(shellp, "sh", "-i", NULL);
			_exit(0);		/* Should do better!	*/
		}
		while ((wpid=wait((int*)&status))>=0 && wpid!=pid)
			;
		(void) signal(SIGQUIT, oqsig);
		(void) signal(SIGINT,  oisig);
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.14  by S.Yoshida */
		(void) signal(SIGWINCH, owsig);
		refresh(FFRAND, 0);		/* May be resized.	*/
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
	}
	sgarbf = TRUE;				/* Force repaint.	*/
#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
	ttykeypadstart();
#endif /* XKEYS */
	return ttraw();
}

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
#include <sys/types.h>
#include <sys/stat.h>
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
	static char tmpbuf[20];
	char *tmp;
	int ostdin, ostdout, ostderr, in, out, s;
#ifndef	HAVE_MKSTEMP
	extern char *mktemp();
#endif

	strcpy(tmpbuf, "/tmp/ngXXXXXX");

	if ((in = open(input ? input : "/dev/null", 0)) < 0)
		return NULL;
#ifndef HAVE_MKSTEMP	
	if ((tmp = mktemp(tmpbuf)) == NULL) {
		close(in);
		return NULL;
	}
	if ((out = creat(tmp, S_IREAD | S_IWRITE)) < 0) {
		close(in);
		return NULL;
	}
#else
	if ((out = mkstemp(tmpbuf)) < 0) {
		close(in);
		return NULL;
	}
	tmp = tmpbuf;
#endif

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
