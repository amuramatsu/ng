/* $Id: spawn.c,v 1.3 2000/11/23 14:00:50 amura Exp $ */
/*
 * Name:	MicroGnuEmacs
 *		Spawn CLI for System V.
 *
 * Spawn for System V.
 */

/*
 * $Log: spawn.c,v $
 * Revision 1.3  2000/11/23 14:00:50  amura
 * fix for some strict compiler
 *
 * Revision 1.2  2000/06/27 01:59:43  amura
 * small bugfix
 *
 * Revision 1.1.1.1  2000/06/27 01:47:59  amura
 * import to CVS
 *
 */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"

#include	<signal.h>

char	*shellp	= NULL;			/* Saved "SHELL" program.	*/
char	*shname = NULL;			/* Saved shell name		*/

extern	char	*getenv();
#ifdef HAVE_GETSID
extern	int	job_control;
#endif

/*
 * On System V, we no gots job control, so always run
 * a subshell using fork/exec. Bound to "C-C", and used
 * as a subcommand by "C-Z". (daveb)
 *
 * Returns 0 if the shell executed OK, something else if
 * we couldn't start shell or it exited badly.
 */
/*ARGSUSED*/
spawncli(f, n)
{
	extern char	*strrchr();
	register int	pid;
	register int	wpid;
	register VOID	(*oqsig)();
	register VOID	(*oisig)();
#ifdef	ADDFUNC		/* 93.07.08  by S.Yoshida */
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
	register VOID	(*owsig)();
#endif
#ifdef	SIGTSTP	/* 93.07.08  by S.Yoshida */
	register int	omask;
#endif
#endif	/* ADDFUNC */
	int		status = FALSE;
	int		errp = FALSE;

#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
	ttykeymaptidy();
#endif /* XKEYS */
	if (shellp == NULL) {
		shellp = getenv("SHELL");
		if (shellp == NULL)
			shellp = getenv("shell");
		if (shellp == NULL)
			shellp = "/bin/sh";	/* Safer.		*/
		shname = strrchr( shellp, '/' ); 
		shname = shname ? shname+1 : shellp;
	}
	ttcolor(CTEXT);
	ttnowindow();
#ifdef	ADDFUNC	/* 93.07.08  by S.Yoshida */
#ifdef	SIGTSTP	/* 93.07.08  by S.Yoshida */
	if (strcmp(shellp, "/bin/csh") == 0) {
		if (epresf != FALSE) {
			ttmove(nrow-1, 0);
			tteeol();
			epresf = FALSE;
		}				/* Csh types a "\n"	*/
		ttmove(nrow-2, 0);		/* before "Stopped".	*/
	} else {
#endif
#endif
	ttmove(nrow-1, 0);
	if (epresf != FALSE) {
		tteeol();
		epresf = FALSE;
	}
#ifdef	ADDFUNC	/* 93.07.08  by S.Yoshida */
#ifdef	SIGTSTP	/* 93.07.08  by S.Yoshida */
	}
#endif
#endif
	ttclose();
	sgarbf = TRUE;				/* Force repaint.	*/
#ifdef	ADDFUNC	/* 93.07.08  by S.Yoshida */
#ifdef	SIGTSTP	/* 93.07.08  by S.Yoshida */
# ifdef HAVE_GETSID
	if (job_control) {
# else
	if (strcmp(shellp, "/bin/sh")!=0 ||
	    getenv("BASH_VERSION") || getenv("BASH"))	{
		/* C shell, ksh	or bash	*/
# endif
	    /*	omask = sigsetmask(0);  */
		oqsig = signal(SIGQUIT, SIG_IGN);
		oisig = signal(SIGINT,  SIG_IGN);
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
		owsig = signal(SIGWINCH, SIG_IGN);
#endif
		(void) kill(0, SIGTSTP);
	    /*	(void) sigsetmask(omask); */
		signal(SIGINT,  oisig);
		signal(SIGQUIT, oqsig);
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
		signal(SIGWINCH, owsig);
#endif
	} else {				/* Bourne shell.	*/
#endif	/* SIGTSTP */	
#endif	/* ADDFUNC */
	oqsig = signal(SIGQUIT, SIG_IGN);
	oisig = signal(SIGINT,  SIG_IGN);
#ifdef	ADDFUNC		/* 93.07.08  by S.Yoshida */
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
	owsig = signal(SIGWINCH, SIG_IGN);
#endif
#endif
	if ((pid=fork()) == 0) {
		(void) signal(SIGINT, oisig);
		(void) signal(SIGQUIT, oqsig);
#ifdef	ADDFUNC		/* 93.07.08  by S.Yoshida */
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
		(void) signal(SIGWINCH, owsig);
#endif
#endif
#ifdef	EXTD_DIR
		dirend();
#endif
		execlp(shellp, shname, "-i", (char *)NULL);
		_exit(1);			/* Should do better!	*/
	}
	else if (pid > 0) {
		while ((wpid=wait(&status))>=0 && wpid!=pid)
			;
	}
	else errp = TRUE;

	signal(SIGINT,  oisig);
	signal(SIGQUIT, oqsig);
#ifdef	ADDFUNC		/* 93.07.08  by S.Yoshida */
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
	signal(SIGWINCH, owsig);
#endif
#ifdef	SIGTSTP	/* 93.07.08  by S.Yoshida */
	}
#endif
#endif
	ttopen();
#ifdef SIGWINCH		/* by A.ITO  21 Jan. 1991 / by S.Yoshida */
	refresh(FFRAND, 0);		/* May be resized.	*/
#endif
	if(errp)
		ewprintf("Failed to create process");
#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
	ttykeypadstart();
#endif /* XKEYS */

	return !( errp | status );
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
	char *tmp;
	static char tmpbuf[20];
	int ostdin, ostdout, ostderr, in, out, s;
	extern char *mktemp();

	strcpy(tmpbuf, "/tmp/ngXXXXXX");
	if ((tmp = mktemp(tmpbuf)) == NULL)
		return NULL;
	if ((in = open(input ? input : "/dev/null", 0)) < 0)
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
#ifndef	SVR2	/* 91.02.04  SVR3 or later. by Junn Ohta */
	dup2(in, 0);
	dup2(out, 1);
	dup2(out, 2);
#else	/* SVR2 */
	close(0); dup(in);
	close(1); dup(out);
	close(2); dup(out);
#endif	/* SVR2 */
	strcpy(buf, command);
#ifdef	EXTD_DIR
	ensurecwd();
#endif
	s = system(buf);
	close(in);
	close(out);
#ifndef	SVR2	/* 91.02.04  SVR3 or later. by Junn Ohta */
	dup2(ostdin, 0); dup2(ostdout, 1); dup2(ostderr, 2);
#else	/* SVR2 */
	close(0); dup(ostdin);
	close(1); dup(ostdout);
	close(2); dup(ostderr);
#endif	/* SVR2 */
skip:	close(ostdin); close(ostdout); close(ostderr);
	if (s == -1) {
		unlink(tmp);
		return NULL;
	}
	return tmp;
}
#endif	/* NO_SHELL */
