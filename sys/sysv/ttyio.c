/* $Id: ttyio.c,v 1.7 2001/01/23 08:54:31 amura Exp $ */
/*
 * Name:	MicroEMACS
 *		System V terminal I/O.
 * Version:	0
 * Last edit:	Tue Aug 26 23:57:57 PDT 1986
 * By:		gonzo!daveb
 *		{sun, amdahl, mtxinu}!rtech!gonzo!daveb
 *
 * The functions in this file
 * negotiate with the operating system for
 * keyboard characters, and write characters to
 * the display in a barely buffered fashion.
 *
 * This version goes along with tty/termcap/tty.c.
 * Terminal size is determined there, rather than here, and
 * this does not open the termcap file
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.7  2001/01/23 08:54:31  amura
 * remove terminal size check all
 *
 * Revision 1.6  2001/01/23 08:43:50  amura
 * reset terminal polling mode in ttwait()
 *
 * Revision 1.5  2001/01/20 15:48:47  amura
 * very big terminal supported
 *
 * Revision 1.4  2001/01/17 18:31:53  amura
 * fix typo POSIXTTY to POSIX_TTY
 *
 * Revision 1.3  2000/12/01 09:47:25  amura
 * fix ttraw() with termios
 * unset IEXTEN flag on c_lflag
 *
 * Revision 1.2  2000/11/16 14:32:03  amura
 * fix some typos which cause compile error when using
 * strict ANSI-C compiler (ex ACK, gcc-1.x)
 *
 * Revision 1.1.1.1  2000/06/27 01:47:59  amura
 * import to CVS
 *
 */
/* 90.02.05	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"

#include	<sys/types.h>
#include	<fcntl.h>
#ifdef	POSIXTTY		/* by S.Okamoto 93/03/16 */
#include	<termios.h>
#else
#include	<termio.h>
#endif
#ifdef	ADDFUNC	/* 93.07.08  by S.Yoshida */
#include	<signal.h>		/* 93.07.08: For SIGWINCH.	*/
#include	<sys/ioctl.h>		/* 00.04.03: amura		*/
#endif	/* ADDFUNC */

#define	NOBUF	512			/* Output buffer size.		*/

char	obuf[NOBUF];			/* Output buffer.		*/
int	nobuf;				/* buffer count			*/

#ifdef POSIXTTY			/* by S.Okamoto 93/03/16 */
static struct termios	ot;		/* entry state of the terminal	*/
static struct termios	nt;		/* editor's terminal state	*/
#else
static struct termio	ot;		/* entry state of the terminal	*/
static struct termio	nt;		/* editor's terminal state	*/
#endif

static int ttyactivep = FALSE;		/* terminal in editor mode?	*/
static int ttysavedp = FALSE;		/* terminal state saved?	*/

#ifdef	BUGFIX	/* 91.01.14  by S.Yoshida */
#ifdef	TIOCGWINSZ
struct	winsize winsize;		/* 4.3 BSD window sizing	*/
#endif
#endif	/* BUGFIX */

int	nrow;				/* Terminal size, rows.		*/
int	ncol;				/* Terminal size, columns.	*/

#ifdef	ADDFUNC		/* 93.07.08  by S.Yoshida */
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
VOID	ttwinch();
#endif
#endif
#ifdef HAVE_GETSID
int	job_control;
#endif

/* These are used to implement typeahead on System V */

int kbdflgs;			/* saved keyboard fd flags	*/
int kbdpoll;			/* in O_NDELAY mode			*/
int kbdqp;			/* there is a char in kbdq	*/
char kbdq;			/* char we've already read	*/

/*
 * This function gets called once, to set up
 * the terminal channel.  This version turns off flow
 * control.  This may be wrong for your system, but no
 * good solution has really been found (daveb).
 */
ttopen()
{
	register char	*cp;
	extern char	*getenv();

	if (ttyactivep)
		return;

	if( !ttysavedp )
	{
#ifdef POSIXTTY		/* by S.Okamoto 93/03/16 */
		if (tcgetattr(0, &ot) < 0)
#else
		if (ioctl(0, TCGETA, &ot) < 0) 
#endif
			abort();
		nt = ot;		/* save entry state		*/
		nt.c_cc[VMIN] = 1;	/* one character read is OK	*/
		nt.c_cc[VTIME] = 0;	/* Never time out.		*/
		nt.c_iflag |= IGNBRK;
		nt.c_iflag &= ~( ICRNL | INLCR | ISTRIP | IXON | IXOFF );
		nt.c_oflag &= ~OPOST;
		nt.c_cflag |= CS8;	/* allow 8th bit on input	*/
		nt.c_cflag &= ~PARENB;	/* Don't check parity		*/
		nt.c_lflag &= ~( ECHO | ICANON | ISIG );
#ifdef	POSIXTTY
		nt.c_lflag &= ~IEXTEN;
#endif
		kbdpoll = (((kbdflgs = fcntl(0, F_GETFL, 0)) & O_NDELAY) != 0);
#ifdef HAVE_GETSID
		{
		pid_t pid,pgid,sid;

		pid = getpid();
		pgid = getpgrp();
		sid = getsid(0);
		if (pid == pgid && pgid != sid)
			job_control = TRUE;
		else
			job_control = FALSE;
		}
#endif /* HAVE_GETSID */

		ttysavedp = TRUE;
	}
#ifdef BUGFIX
	else
		kbdpoll = ((fcntl(0, F_GETFL, 0) & O_NDELAY) != 0);
#endif

#ifdef POSIXTTY		/* by S.Okamoto 93/03/16 */
	if (tcsetattr(0, TCSAFLUSH, &nt) < 0)
#else
	if (ioctl(0, TCSETAF, &nt) < 0) 
#endif
		abort();

#ifdef	TIOCGWINSZ
	if (ioctl(0, TIOCGWINSZ, (char *) &winsize) != -1) {
		nrow = winsize . ws_row;
		ncol = winsize . ws_col;
	} else
	nrow = 0;
	if (nrow <= 0 || ncol <= 0)
#endif	/* TIOCGWINSZ */
	/* This really belongs in tty/termcap... */

	if ((cp=getenv("TERMCAP")) == NULL
	|| (nrow=getvalue(cp, "li")) <= 0
	|| (ncol=getvalue(cp, "co")) <= 0) {
		nrow = 24;
		ncol = 80;
	}
#ifdef  ADDFUNC		/* 93.07.08  by S.Yoshida */
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
	(void) signal(SIGWINCH, ttwinch);
#endif
#endif
	ttyactivep = TRUE;
}

/*
 * This routine scans a string, which is
 * actually the return value of a getenv call for the TERMCAP
 * variable, looking for numeric parameter "name". Return the value
 * if found. Return -1 if not there. Assume that "name" is 2
 * characters long. This limited use of the TERMCAP lets us find
 * out the size of a window on the X display.
 */
getvalue(cp, name)
register char	*cp;
register char	*name;
{
	for (;;) {
		while (*cp!=0 && *cp!=':')
			++cp;
		if (*cp++ == 0)			/* Not found.		*/
			return (-1);
		if (cp[0]==name[0] && cp[1]==name[1] && cp[2]=='#')
			return (atoi(cp+3));	/* Stops on ":".	*/
	}
}

/*
 * This function gets called just
 * before we go back home to the shell. Put all of
 * the terminal parameters back.
 */
ttclose()
{
	if(!ttysavedp || !ttyactivep)
		return;
	ttflush();
#ifdef POSIXTTY
	if (tcsetattr(0, TCSAFLUSH, &ot) < 0 ||
	    fcntl( 0, F_SETFL, kbdflgs ) < 0)
#else
	if (ioctl(0, TCSETAF, &ot) < 0 || fcntl( 0, F_SETFL, kbdflgs ) < 0)
#endif
		abort();
	ttyactivep = FALSE;
}

/*
 * Write character to the display.
 * Characters are buffered up, to make things
 * a little bit more efficient.
 */
ttputc(c)
{
	if (nobuf >= NOBUF)
		ttflush();
	obuf[nobuf++] = c;
}

/*
 * Flush output.
 */
ttflush()
{
	if (nobuf != 0) {
		write(1, obuf, nobuf);
		nobuf = 0;
	}
}

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
static	int	nkey = 0;		/* The number of ungetc charactor. */
static	int	keybuf[4];		/* Ungetc charactors.		*/
#endif	/* KANJI */

/*
 * Read character from terminal.
 * All 8 bits are returned, so that you can use
 * a multi-national terminal.
 *
 * If keyboard 'queue' already has typeahead from a typeahead() call,
 * just return it.  Otherwise, make sure we are in blocking i/o mode
 * and read a character.
 */
ttgetc()
{
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return (keybuf[--nkey]);
	}
#endif	/* KANJI */
	if( kbdqp )
		kbdqp = FALSE;
	else
	{
		if( kbdpoll && fcntl( 0, F_SETFL, kbdflgs ) < 0 )
			abort();
		kbdpoll = FALSE;
		while (read(0, &kbdq, 1) != 1)
			;
	}
	return ( kbdq & 0xff );
}

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
/*
 * Save pre-readed char to read again.
 */
ttungetc(c)
int	c;
{
	keybuf[nkey++] = c;
}
#endif	/* KANJI */

/*
 * Return non-FALSE if typeahead is pending.
 *
 * If already got unread typeahead, do nothing.
 * Otherwise, set keyboard to O_NDELAY if not already, and try
 * a one character read.
 */
typeahead()
{
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return (TRUE);
	}
#endif	/* KANJI */
	if( !kbdqp )
	{
		if( !kbdpoll && fcntl( 0, F_SETFL, kbdflgs | O_NDELAY ) < 0 )
			abort();
		kbdpoll = TRUE;
		kbdqp = (1 == read( 0, &kbdq, 1 ));
	}
	return ( kbdqp );
}


/*
 * panic:  print error and die, leaving core file.
 * Don't know why this is needed (daveb).
 */
panic(s)
char *s;
{
	fprintf(stderr, "%s\r\n", s);
	abort();
}


/*
** This should check the size of the window, and reset if needed.
*/

setttysize()
{
#ifdef	TIOCGWINSZ
	if (ioctl(0, TIOCGWINSZ, (char *) &winsize) != -1) {
		nrow = winsize . ws_row;
		ncol = winsize . ws_col;
	} else
#ifdef	BUGFIX	/* 93.07.08  by S.Yoshida */
	nrow = 0;
	if (nrow <= 0 || ncol <= 0)
#endif	/* BUGFIX */
#endif
	if ((nrow=tgetnum ("li")) <= 0
	|| (ncol=tgetnum ("co")) <= 0) {
		nrow = 24;
		ncol = 80;
	}
}

#ifdef	ADDFUNC		/* 93.07.08  by S.Yoshida */
#ifdef	SIGWINCH	/* 93.07.08  by S.Yoshida */
/*
 * Signal handler when window size has changed.
 */
VOID
ttwinch()
{
	refresh(FFRAND, 0);			/* Very easy way... */
#ifdef	CANNA
	canna_width();
#endif	/* CANNA */
	(void) signal(SIGWINCH, ttwinch);
}
#endif	/* ADDFUNC */
#endif	/* SIGWINCH */


#ifndef NO_DPROMPT
#include <signal.h>
#include <setjmp.h>

static jmp_buf tohere;

static VOID alrm()
{
	longjmp(tohere, -1);
}

/*
 * Return TRUE if we wait without doing anything, else return FALSE.
 */

ttwait()
{
	VOID alrm();

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return (FALSE);
	}
#endif	/* KANJI */
	if (kbdqp)
		return FALSE;		/* already pending input	*/
	if (setjmp(tohere))
		return TRUE;		/* timeout on read if here	*/
#ifdef	BUGFIX	/* 90.02.07  by S.Yoshida */
	signal(SIGALRM, alrm); alarm(1);
#else	/* NOT BUGFIX */
	signal(SIGALRM, alrm); alarm(2);
#endif	/* BUGFIX */
	if (kbdpoll && fcntl( 0, F_SETFL, kbdflgs ) < 0)
		abort();
	kbdpoll = FALSE;
	kbdqp = (1 == read(0, &kbdq, 1));
	alarm(0);
	return FALSE;			/* successful read if here	*/
}
#endif /* NO_DPROMPT */
