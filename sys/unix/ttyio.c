/* $Id: ttyio.c,v 1.9.2.2 2003/02/23 13:58:14 amura Exp $ */
/*
 *	Unix terminal I/O. (for configure)
 * The functions in this file
 * negotiate with the operating system for
 * keyboard characters, and write characters to
 * the display in a barely buffered fashion.
 *
 * Create from BSD UNIX version by amura, 2000
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.9.2.2  2003/02/23 13:58:14  amura
 * canna_init() must be called after setttysize() on ttyio.c
 *
 * Revision 1.9.2.1  2002/02/10 12:34:38  amura
 * cannot compile problem with Cygwin is fixed
 *
 * Revision 1.9  2001/03/02 08:48:32  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.8  2001/02/18 19:28:12  amura
 * ttflush() is modified for Solaris
 *
 * Revision 1.7  2001/02/14 09:22:30  amura
 * always use select() even if ioctl() does NOT support FIONREAD
 *
 * Revision 1.6  2001/01/23 08:43:50  amura
 * reset terminal polling mode in ttwait()
 *
 * Revision 1.5  2001/01/20 15:48:47  amura
 * very big terminal supported
 *
 * Revision 1.4  2000/12/18 17:20:41  amura
 * edit for cygwin
 *
 * Revision 1.3  2000/12/01 10:12:55  amura
 * edit for adapting to POSIX
 *
 * Revision 1.2  2000/11/23 14:03:32  amura
 * some fix for FreeBSD's termios interface
 *
 * Revision 1.1  2000/11/19 18:35:00  amura
 * support GNU configure system
 *
 */

#include	"config.h"
#include	"def.h"

#include	<sys/types.h>
#ifdef	HAVE_TERMIOS_H
#include	<termios.h>
#else
#ifdef	HAVE_TERMIO_H
#include	<termio.h>
#else
#ifdef	HAVE_SGTTY_H
#include	<sgtty.h>
#else
#error "What tty do you use?"
#endif	/* HAVE_SGTTY_H */
#endif	/* HAVE_TERMIO_H */
#endif	/* HAVE_SGTTY_H */
#ifdef	HAVE_FCNTL_H
#include	<fcntl.h>
#endif
#ifdef	HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef	ADDFUNC	/* 90.02.14  by S.Yoshida */
#include	<signal.h>		/* 90.02.13: For SIGWINCH.	*/
#include	<sys/ioctl.h>		/* 00.04.03: amura		*/
#endif	/* ADDFUNC */

#define NOBUF	512			/* Output buffer size.		*/

char	obuf[NOBUF];			/* Output buffer.		*/
int	nobuf;

#if defined(HAVE_TERMIO_H)||defined(HAVE_TERMIOS_H)
static int ttyactivep = FALSE;		/* terminal in editor mode?	*/
static int ttysavedp = FALSE;		/* terminal state saved?	*/
#ifdef	HAVE_TERMIOS_H
static struct termios	ot;		/* entry state of the terminal	*/
static struct termios	nt;		/* editor's terminal state	*/
#else
static struct termio	ot;		/* entry state of the terminal	*/
static struct termio	nt;		/* editor's terminal state	*/
#endif
#else	/* not HAVE_TERMIO_H || HAVE_TERMIOS_H */
struct	sgttyb	oldtty;			/* V6/V7 stty data.		*/
struct	sgttyb	newtty;
struct	tchars	oldtchars;		/* V7 editing.			*/
struct	tchars	newtchars;
struct	ltchars oldltchars;		/* 4.2 BSD editing.		*/
struct	ltchars newltchars;
#endif	/* HAVE_TERMIO_H || HAVE_TERMIOS_H */

#ifdef	TIOCGWINSZ
struct	winsize winsize;		/* 4.3 BSD window sizing	*/
#endif

int	nrow;				/* Terminal size, rows.		*/
int	ncol;				/* Terminal size, columns.	*/

#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.13  by S.Yoshida */
RETSIGTYPE ttwinch();
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */

#ifndef HAVE_SELECT
/* These are used to implement typeahead on System V */
int kbdflgs;			/* saved keyboard fd flags	*/
int kbdpoll;			/* in O_NONBLOCK mode		*/
int kbdqp;			/* there is a char in kbdq	*/
char kbdq;			/* char we've already read	*/
#ifndef	O_NONBLOCK	/* for old system compat? */
#define O_NONBLOCK	O_NDELAY
#endif
#endif	/* HAVE_SELECT */

/*
 * This function gets called once, to set up
 * the terminal channel. On Ultrix is's tricky, since
 * we want flow control, but we don't want any characters
 * stolen to send signals. Use CBREAK mode, and set all
 * characters but start and stop to 0xFF.
 */
ttopen() {
    register char *tv_stype;
    char *getenv(), *tgetstr(), tcbuf[2048], err_str[72];
#ifndef	SUPPORT_ANSI
    int sprintf();
#endif

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
#ifdef CANNA
    canna_init();
#endif
}

/*
 * This function sets the terminal to RAW mode, as defined for the current
 * shell.  This is called both by ttopen() above and by spawncli() to
 * get the current terminal settings and then change them to what
 * mg expects.	Thus, stty changes done while spawncli() is in effect
 * will be reflected in mg.
 */
ttraw() {
#if defined(HAVE_TERMIO_H)||defined(HAVE_TERMIOS_H)
    if (ttyactivep)
	return TRUE;
    if( !ttysavedp )
    {
#ifdef HAVE_TERMIOS_H
	if (tcgetattr(0, &ot) < 0) {
#else
	if (ioctl(0, TCGETA, &ot) < 0) {
#endif
	    ewprintf("ttopen can't get termio/termios");
	    return(FALSE);
	}
	
	nt = ot;		/* save entry state		*/
	nt.c_cc[VMIN] = 1;	/* one character read is OK	*/
	nt.c_cc[VTIME] = 0;	/* Never time out.		*/
	nt.c_iflag |= IGNBRK;
	nt.c_iflag &= ~( ICRNL | INLCR | ISTRIP | IXON | IXOFF );
	nt.c_oflag &= ~OPOST;
	nt.c_cflag |= CS8;	/* allow 8th bit on input	*/
	nt.c_cflag &= ~PARENB;	/* Don't check parity		*/
	nt.c_lflag &= ~( ECHO | ICANON | ISIG );
#ifdef	IEXTEN
	nt.c_lflag &= ~IEXTEN;
#endif
#ifndef HAVE_SELECT
	kbdpoll = (((kbdflgs = fcntl(0, F_GETFL, 0)) & O_NONBLOCK) != 0);
#endif
	ttysavedp = TRUE;
    }
#ifndef	HAVE_SELECT
    else
	kbdpoll = ((fcntl(0, F_GETFL, 0) & O_NONBLOCK) != 0);
#endif

#ifdef	HAVE_TERMIOS_H
    if (tcsetattr(0, TCSAFLUSH, &nt) < 0) {
#else
    if (ioctl(0, TCSETAF, &nt) < 0) {
#endif
	ewprintf("ttopen can't set termio/termios");
	return(FALSE);
    }
    ttyactivep = TRUE;
#else	/* not HAVE_TERMIO_H || HAVE_TERMIOS_H */
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
    newtty.sg_flags |= RAW|ANYP;	/* raw mode for 8 bit path.*/
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
	ewprintf("ttopen can't set chars");
	return(FALSE);
    }
    if (ioctl(0, TIOCGLTC, (char *) &oldltchars) < 0) {
	panic("ttopen can't get ltchars");
	return(FALSE);
    }
    newltchars.t_suspc  = 0xFF;		/* Suspend #1.		*/
    newltchars.t_dsuspc = 0xFF;		/* Suspend #2.		*/
    newltchars.t_rprntc = 0xFF;
    newltchars.t_flushc = 0xFF;		/* Output flush.	*/
    newltchars.t_werasc = 0xFF;
    newltchars.t_lnextc = 0xFF;		/* Literal next.	*/
    if (ioctl(0, TIOCSLTC, (char *) &newltchars) < 0) {
	ewprintf("ttopen can't set ltchars");
	return(FALSE);
    }
#endif	/* HAVE_TERMIO_H || HAVE_TERMIOS_H */
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
#ifdef	CANNA
    canna_end();
#endif
}

/*
 * This function restores all terminal settings to their default values,
 * in anticipation of exiting or suspending the editor.
 */

ttcooked() {
#if defined(HAVE_TERMIO_H)||defined(HAVE_TERMIOS_H)
    if(!ttysavedp || !ttyactivep)
	return(TRUE);
    ttflush();
#ifdef	HAVE_TERMIOS_H
    if (tcsetattr(0, TCSAFLUSH, &ot) < 0
#else
    if (ioctl(0, TCSETAF, &ot) < 0 
#endif
#ifndef	HAVE_SELECT
	|| fcntl( 0, F_SETFL, kbdflgs ) < 0
#endif
	) {
	ewprintf("ttclose can't set termio/termios");
	return(FALSE);
    }
    ttyactivep = FALSE;
#else	/* not HAVE_TERMIO_H || HAVE_TERMIOS_H */
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
#endif	/* HAVE_TERMIO_H || HAVE_TERMIOS_H */
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
    char *p = obuf;
    int outlen;
    if (nobuf != 0) {
	if ((outlen=write(1, p, nobuf)) != nobuf) {
	    p += nobuf;
	    nobuf -= outlen;
	}
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
#ifdef	HAVE_SELECT
    char buf[1];
#endif

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
    if (nkey > 0) {
	return (keybuf[--nkey]);
	}
#endif	/* KANJI */

#ifdef	HAVE_SELECT
    while (read(0, &buf[0], 1) != 1)
	;
    return (buf[0] & 0xFF);
#else	/* not HAVE_SELECT */
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
#endif	/* HAVE_SELECT */
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
RETSIGTYPE
ttwinch()
{
    refresh(FFRAND, 0);			/* Very easy way... */
#ifdef	CANNA
    canna_width();
#endif	/* CANNA */
    (void) signal(SIGWINCH, ttwinch);	/* for old scheme */
}
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
typeahead() {
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
    if (nkey > 0) {
	return (TRUE);
    }
#endif	/* KANJI */
#ifdef	HAVE_SELECT
#ifdef	FIONREAD
    {
	int	x;
	return ((ioctl(0, FIONREAD, (char *) &x) < 0) ? FALSE : x);
    }
#else	/* not FIONREAD */
    {
	struct timeval tmout;
	fd_set readfd;
	tmout.tv_sec = 0;
	tmout.tv_usec = 0;
	FD_ZERO(&readfd);
	FD_SET(0, &readfd);
	return ((select(1, &readfd, (fd_set *)0, (fd_set *)0, &tmout) == 0) ?
		FALSE : TRUE);
    }
#endif	/* FIONREAD */
#else	/* not HAVE_SELECT */
    if( !kbdqp )
    {
	if( !kbdpoll && fcntl( 0, F_SETFL, kbdflgs | O_NONBLOCK) < 0 )
	    abort();
	kbdpoll = TRUE;
	kbdqp = (1 == read( 0, &kbdq, 1 ));
    }
    return ( kbdqp );
#endif	/* HAVE_SELECT */
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
#ifdef	HAVE_SELECT
/*
 * A program to return TRUE if we wait for 2 seconds without anything
 * happening, else return FALSE.  Cribbed from mod.sources xmodem.
 */
int ttwait() {
    fd_set readfd;
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
    if ((select(1, &readfd, (fd_set *)0, (fd_set *)0, &tmout)) == 0)
	return(TRUE);
    return(FALSE);
}

#else	/* not HAVE_SELECT */

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
    VOID (*old_alrm)();
    unsigned int old_time;

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
    if (nkey > 0) {
	return (FALSE);
    }
#endif	/* KANJI */
    if (kbdqp)
	return FALSE;		/* already pending input	*/
    if (setjmp(tohere))
	return TRUE;		/* timeout on read if here	*/
    old_alrm = signal(SIGALRM, alrm);
    old_time = alarm(1);
    if (kbdpoll && fcntl( 0, F_SETFL, kbdflgs ) < 0)
	abort();
    kbdpoll = FALSE;
    kbdqp = (1 == read(0, &kbdq, 1));
    signal(SIGALRM, old_alrm);
    alarm(old_time);
    return FALSE;		/* successful read if here	*/
}
#endif	/* HAVE_SELECT */
#endif	/* NO_DPROMPT */

#ifdef	AUTOSAVE
VOID
itimer(func, sec)
VOID (*func)();
time_t sec;
{
    if (sec == 0)
	alarm(0);
    else
    {
	signal(SIGALRM, func);
	alarm(sec);
    }
}
#endif	/* AUTOSAVE */
