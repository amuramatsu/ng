/* $Id: ttyio.c,v 1.2 2001/01/20 15:48:46 amura Exp $ */
/*
 *		Ultrix-32 and Unix terminal I/O.
 * The functions in this file
 * negotiate with the operating system for
 * keyboard characters, and write characters to
 * the display in a barely buffered fashion.
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.2  2001/01/20 15:48:46  amura
 * very big terminal supported
 *
 * Revision 1.1.1.1  2000/06/27 01:48:02  amura
 * import to CVS
 *
 */
/* 90.02.05	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"

#include	<sgtty.h>
#ifdef	ADDFUNC	/* 90.02.14  by S.Yoshida */
#include	<signal.h>		/* 90.02.13: For SIGWINCH.	*/
#endif	/* ADDFUNC */
#include	<sys/ioctl.h>		/* 00.04.03: amura		*/

#define NOBUF	512			/* Output buffer size.		*/

char	obuf[NOBUF];			/* Output buffer.		*/
int	nobuf;
struct	sgttyb	oldtty;			/* V6/V7 stty data.		*/
struct	sgttyb	newtty;
struct	tchars	oldtchars;		/* V7 editing.			*/
struct	tchars	newtchars;
struct	ltchars oldltchars;		/* 4.2 BSD editing.		*/
struct	ltchars newltchars;
#ifdef	TIOCGWINSZ
struct	winsize winsize;		/* 4.3 BSD window sizing	*/
#endif
int	nrow;				/* Terminal size, rows.		*/
int	ncol;				/* Terminal size, columns.	*/
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.13  by S.Yoshida */
VOID	ttwinch();
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
#ifdef HAVE_GETSID
static int ttysavedp = FALSE;		/* terminal state saved?	*/
int	job_control;
#endif

/*
 * This function gets called once, to set up
 * the terminal channel. On Ultrix is's tricky, since
 * we want flow control, but we don't want any characters
 * stolen to send signals. Use CBREAK mode, and set all
 * characters but start and stop to 0xFF.
 */
ttopen() {
	register char *tv_stype;
	char *getenv(), *tgetstr(), tcbuf[1024], err_str[72];
#ifndef	SUPPORT_ANSI
	int sprintf();
#endif

#ifdef HAVE_GETSID
	if( !ttysavedp )
	{
		pid_t pid,pgid,sid;

		pid = getpid();
		pgid = getpgrp();
		sid = getsid(0);
		if (pid == pgid && pgid != sid)
			job_control = TRUE;
		else
			job_control = FALSE;
		ttysavedp = TRUE;
	}
#endif /* HAVE_GETSID */

/* do this the REAL way */
	if ((tv_stype = getenv("TERM")) == NULL)
	{
		puts("Environment variable TERM not defined!");
		exit(1);
	}

	if((tgetent(tcbuf, tv_stype)) != 1)
	{
		(void) sprintf(err_str, "Unknown terminal type %s!", tv_stype);
		puts(err_str);
		exit(1);
	}
	if (ttraw() == FALSE)
		panic("aborting due to terminal initialize failure");
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.13  by S.Yoshida */
	(void) signal(SIGWINCH, ttwinch);
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
}

/*
 * This function sets the terminal to RAW mode, as defined for the current
 * shell.  This is called both by ttopen() above and by spawncli() to
 * get the current terminal settings and then change them to what
 * mg expects.	Thus, stty changes done while spawncli() is in effect
 * will be reflected in mg.
 */
ttraw() {
	extern short ospeed;

	if (ioctl(0, TIOCGETP, (char *) &oldtty) < 0) {
		ewprintf("ttopen can't get sgtty");
		return(FALSE);
	}
	newtty.sg_ospeed = ospeed = oldtty.sg_ospeed;
	newtty.sg_ispeed = oldtty.sg_ispeed;
	newtty.sg_erase	 = oldtty.sg_erase;
	newtty.sg_kill	 = oldtty.sg_kill;
	newtty.sg_flags	 = oldtty.sg_flags;
	newtty.sg_flags &= ~(ECHO|CRMOD);	/* Kill echo, CR=>NL.	*/
#ifdef FLOWCONTROL
	newtty.sg_flags |= CBREAK;		/* Half-cooked mode.	*/
#else
	newtty.sg_flags |= RAW|ANYP;		/* raw mode for 8 bit path.*/
#endif
	if (ioctl(0, TIOCSETP, (char *) &newtty) < 0) {
		ewprintf("ttopen can't set sgtty");
		return(FALSE);
	}
	if (ioctl(0, TIOCGETC, (char *) &oldtchars) < 0) {
		ewprintf("ttopen can't get chars");
		return(FALSE);
	}
	newtchars.t_intrc  = 0xFF;		/* Interrupt.		*/
	newtchars.t_quitc  = 0xFF;		/* Quit.		*/
#if FLOWCONTROL
	newtchars.t_startc = 0x11;		/* ^Q, for terminal.	*/
	newtchars.t_stopc  = 0x13;		/* ^S, for terminal.	*/
#else
	newtchars.t_startc = 0xFF;		/* ^Q, for terminal.	*/
	newtchars.t_stopc  = 0xFF;		/* ^S, for terminal.	*/
#endif
	newtchars.t_eofc   = 0xFF;
	newtchars.t_brkc   = 0xFF;
	if (ioctl(0, TIOCSETC, (char *) &newtchars) < 0) {
		ewprintf("ttraw can't set chars");
		return(FALSE);
	}
	if (ioctl(0, TIOCGLTC, (char *) &oldltchars) < 0) {
		panic("ttraw can't get ltchars");
		return(FALSE);
	}
	newltchars.t_suspc  = 0xFF;		/* Suspend #1.		*/
	newltchars.t_dsuspc = 0xFF;		/* Suspend #2.		*/
	newltchars.t_rprntc = 0xFF;
	newltchars.t_flushc = 0xFF;		/* Output flush.	*/
	newltchars.t_werasc = 0xFF;
	newltchars.t_lnextc = 0xFF;		/* Literal next.	*/
	if (ioctl(0, TIOCSLTC, (char *) &newltchars) < 0) {
		ewprintf("ttraw can't set ltchars");
		return(FALSE);
	}
	setttysize() ;
	return(TRUE);
}

/*
 * This function gets called just
 * before we go back home to the shell. Put all of
 * the terminal parameters back.
 *    Under UN*X this just calls ttcooked(), but the ttclose() hook is in
 * because vttidy() in display.c expects it for portability reasons.
 */
ttclose() {
	if (ttcooked() == FALSE)
		panic("");		/* ttcooked() already printf'd */
}

/*
 * This function restores all terminal settings to their default values,
 * in anticipation of exiting or suspending the editor.
 */

ttcooked() {
	ttflush();
	if (ioctl(0, TIOCSLTC, (char *) &oldltchars) < 0) {
		ewprintf("ttclose can't set ltchars");
		return(FALSE);
	}
	if (ioctl(0, TIOCSETC, (char *) &oldtchars) < 0) {
		ewprintf("ttclose can't set chars");
		return(FALSE);
	}
	if (ioctl(0, TIOCSETP, (char *) &oldtty) < 0) {
		ewprintf("ttclose can't set sgtty");
		return(FALSE);
	}
	return(TRUE);
}

/*
 * Write character to the display.
 * Characters are buffered up, to make things
 * a little bit more efficient.
 */
ttputc(c)
int c;
{
	if (nobuf >= NOBUF)
		ttflush();
	obuf[nobuf++] = c;
}

/*
 * Flush output.
 */
ttflush() {
	if (nobuf != 0) {
		if (write(1, obuf, nobuf) != nobuf)
			panic("ttflush write failed");
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
 */
ttgetc() {
	char	buf[1];

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return (keybuf[--nkey]);
	}
#endif	/* KANJI */
	while (read(0, &buf[0], 1) != 1)
		;
	return (buf[0] & 0xFF);
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
 * set the tty size. Functionized for 43BSD.
 */
setttysize() {

#ifdef	TIOCGWINSZ
	if (ioctl(0, TIOCGWINSZ, (char *) &winsize) != -1) {
		nrow = winsize . ws_row;
		ncol = winsize . ws_col;
	} else nrow = 0;
	if(nrow<=0 || ncol<=0)
#endif
	if ((nrow=tgetnum ("li")) <= 0
	|| (ncol=tgetnum ("co")) <= 0) {
		nrow = 24;
		ncol = 80;
	}
}

#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.13  by S.Yoshida */
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
}
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
typeahead() {
	int	x;

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return (TRUE);
	}
#endif	/* KANJI */
	return((ioctl(0, FIONREAD, (char *) &x) < 0) ? 0 : x);
}

/*
 * panic - just exit, as quickly as we can.
 */
panic(s) char *s; {
	(void) fputs("panic: ", stderr);
	(void) fputs(s, stderr);
	(void) fputc('\n', stderr);
	(void) fflush(stderr);
	abort();		/* To leave a core image. */
}
#ifndef NO_DPROMPT
#include <sys/time.h>
/*
 * A program to return TRUE if we wait for 2 seconds without anything
 * happening, else return FALSE.  Cribbed from mod.sources xmodem.
 */
int ttwait() {
	FDSET readfd;
	struct timeval tmout;

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return (FALSE);
	}
#endif	/* KANJI */
#ifdef	BUGFIX	/* 90.02.07  by S.Yoshida */
	tmout.tv_sec = 1;
#else	/* NOT BUGFIX */
	tmout.tv_sec = 2;
#endif	/* BUGFIX */
	tmout.tv_usec = 0;

	FD_ZERO(&readfd);
	FD_SET(0, &readfd);
	if ((select(1, &readfd, (FDSET *)0, (FDSET *)0, &tmout)) == 0)
 		return(TRUE);
	return(FALSE);
}
#endif
