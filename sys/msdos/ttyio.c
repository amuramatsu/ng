/* $Id: ttyio.c,v 1.3 2001/03/02 08:48:32 amura Exp $ */
/*
 *		MS-DOS terminal I/O. (Tested only at MS-DOS 3.1)
 *		I make this file from BSD UNIX ttyio.c.
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.3  2001/03/02 08:48:32  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.2  2001/01/20 15:48:47  amura
 * very big terminal supported
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	<dos.h>
#ifndef	__TURBOC__	/* 90.04.06  by S.Yoshida */
#define MK_FP(seg,ofs)	((void far *) \
			   (((unsigned long)(seg) << 16) | (unsigned)(ofs)))

#define peek(a,b,c)	(*((int  far*)MK_FP((a),(b))))
#endif	/* __TURBOC__ */

#ifdef TCCONIO
#include	<conio.h>
#endif

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
#include	"fepctrl.h"
#endif

int	nrow;								/* Terminal size, rows.		*/
int	ncol;								/* Terminal size, columns.	*/
int	ospeed = 13;						/* We think 9600 bps is used.	*/

static	int	dosversion;					/* MS-DOS version number.	*/
static	int	breakstat;					/* BREAK check mode status.	*/
static	int	stdinstat;					/* stdin IOCTRL status.		*/
#ifdef TCCONIO
static	unsigned int oldmode;			/* save old textmode */
static	struct text_info tinfo;			/* Registers for textinfo(). */
#endif
#ifdef	DO_METAKEY
extern int use_metakey;
#endif

union	REGS	inregs, outregs;		/* Registers for intdos().	*/
static	int	dummy = 0;					/* Dummy value for bdos().	*/

#ifdef	PC9801	/* 90.03.06  by K.Takano */
static char keysave[10][6];
static int ezkey = FALSE;				/* EZkey flag */
#endif	/* PC9801 */

#ifdef FEPCTRL	/* 90.11.26  by K.Takano and 97.08.16 by A.Muramatsu */
static int fepctrl = FALSE;				/* FEP control enable flag	*/
static int fepmode = TRUE;				/* now FEP mode			*/
static int fepforce = 0;				/* force FEP to mode	*/
#endif

/*
 * This function gets called once, to set up
 * the terminal channel. 
 */
ttopen() {
#ifdef TCCONIO
	gettextinfo(&tinfo);
	oldmode = tinfo.currmode;
#if TCCONIOMODE
	textmode(TCCONIOMODE);
#endif
	directvideo = 0;
#else
	register char *tv_stype;
	char *getenv(), tcbuf[1024], err_str[72];

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
#endif	/* TCCONIO */
	if (ttraw() == FALSE)
		panic("aborting due to terminal initialize failure");

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_init();
#endif
}

/*
 * This function sets the Ctrl-C check function off.
 * This is called both by ttopen() above and by spawncli() to
 * get the current terminal settings and then change them to what
 * Ng expects.	Thus, stty changes done while spawncli() is in effect
 * will be reflected in Ng.
 */
ttraw() {
	inregs.h.ah = 0x30;		/* Check MS-DOS version.	*/
	intdos(&inregs, &outregs);
	dosversion = outregs.h.al;
	
	if (dosversion > 1) {
		inregs.h.ah = 0x33;	/* Get BREAK check status.	*/
		inregs.h.al = 0x00;
		intdos(&inregs, &outregs);
		breakstat = outregs.h.dl;
		inregs.h.al = 0x01;	/* Set BREAK check status to	*/
		inregs.h.dl = 0x00;	/* no BREAK checking.		*/
		intdos(&inregs, &outregs);
		if (outregs.h.al == 0xff) {
			return(FALSE);
		}

		inregs.h.ah = 0x44;	/* Get IOCTRL status.		*/
		inregs.h.al = 0x00;
		inregs.x.bx = 0x00;	/* 0 = stdin.			*/
		intdos(&inregs, &outregs);
		stdinstat = outregs.h.dl;
		inregs.x.dx = (outregs.x.dx | 0x0020) & 0x0027;	/* raw mode */
		inregs.h.al = 0x01;	/* Set IOCTRL to raw.		*/
		intdos(&inregs, &outregs);
		if (outregs.x.cflag != 0x00) {
			return(FALSE);
		}
	}
	setttysize();
#ifdef	IBMPC	/* 90.02.23  by S.Yoshida */
	assignkey();
	getcursor();	/* 91.01.11  Get cursor info. by S.Yoshida */
#endif	/* IBMPC */
#ifdef	PC9801	/* 90.03.06  by K.Takano */
	assignkey();
	if (use_metakey)
		setezkey();
#endif	/* PC9801 */
	return(TRUE);
}

/*
 * This function gets called just
 * before we go back home to the shell. Put all of
 * the terminal parameters back.
 * Under MS-DOS this just calls ttcooked(), but the ttclose() hook is in
 * because vttidy() in display.c expects it for portability reasons.
 */
ttclose() {
	if (ttcooked() == FALSE)
		panic("");		/* ttcooked() already printf'd	*/

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_term();
#endif
#ifdef	TCCONIO
#ifdef	TCCONIOMODE
	textmode(oldmode);
#endif
#endif
}

/*
 * This function reset Ctrl-C check function on.
 */

ttcooked() {
	ttflush();
	if (dosversion > 1) {
		inregs.h.ah = 0x33;	/* Reset BREAK check status.	*/
		inregs.h.al = 0x01;
		inregs.h.dl = breakstat;
		intdos(&inregs, &outregs);
		if (outregs.h.al == 0xff) {
			ewprintf("ttclose can't set break check status");
			return(FALSE);
		}

		inregs.h.ah = 0x44;	/* Reset IOCTRL status.		*/
		inregs.h.al = 0x01;
		inregs.x.bx = 0x00;	/* 0 = stdin.			*/
		inregs.x.dx = stdinstat & 7;
		intdos(&inregs, &outregs);
		if (outregs.x.cflag != 0x00) {
			ewprintf("ttclose can't set IOCTRL status");
			return(FALSE);
		}
	}
#ifdef	IBMPC	/* 90.02.23  by S.Yoshida */
	cancelkey();
	resetcursor();	/* 91.01.05  Reset cursor by S.Yoshida */
#endif	/* IBMPC */
#ifdef	PC9801	/* 90.03.06  by K.Takano */
	cancelkey();
	if (use_metakey)
		resetezkey();
#endif	/* PC9801 */
	return(TRUE);
}

#ifdef	IBMPC	/* 90.02.23  by S.Yoshida */
/*
 * Assign IBMPC special keys to use Ng.
 */
assignkey()
{
	printf("\033[29;27p");		/* Assign C-[ key to ESC.	*/
	printf("\033[0;83;127p");	/* Assign Del key to DEL.	*/
	printf("\033[0;75;2p");		/* Assign <-  key to C-b.	*/
	printf("\033[0;77;6p");		/* Assign ->  key to C-f.	*/
	printf("\033[0;72;16p");	/* Assign up-arrow key to C-p.	*/
	printf("\033[0;80;14p");	/* Assign down-arrow key to C-n. */
	printf("\033[0;71;27;60p");	/* Assign Home  key to M-<.	*/
	printf("\033[0;73;27;118p");	/* Assign Pg-Up key to M-v.	*/
	printf("\033[0;81;22p");	/* Assign Pg-Dn key to C-v.	*/
	printf("\033[0;79;27;62p");	/* Assign End   key to M->.	*/
}

/*
 * Cancel J-3100 special key assign.
 */
cancelkey()
{
	printf("\033[29;29p");		/* Cancel C-[ key.	*/
	printf("\033[0;83;0;83p");	/* Cancel Del key.	*/
	printf("\033[0;75;0;75p");	/* Cancel <-  key.	*/
	printf("\033[0;77;0;77p");	/* Cancel ->  key.	*/
	printf("\033[0;72;0;72p");	/* Cancel up-arrow key.	*/
	printf("\033[0;80;0;80p");	/* Cancel down-arrow key. */
	printf("\033[0;71;0;71p");	/* Cancel Home  key.	*/
	printf("\033[0;73;0;73p");	/* Cancel Pg-Up key.	*/
	printf("\033[0;81;0;81p");	/* Cancel Pg-Dn key.	*/
	printf("\033[0;79;0;79p");	/* Cancel End   key.	*/
}

/* 91.01.11  by S.Yoshida */
static	int	old_blink_mode;		/* Cursor blink mode.	*/
static	int	old_start_line;		/* Start cursor line.	*/
static	int	old_end_line;		/* End cursor line.	*/
static	int	hold_new_cursor;	/* Hold new cursor at Ng exiting. */
static	int	new_blink_mode;		/* Cursor blink mode.	*/
static	int	new_start_line;		/* Start cursor line.	*/
static	int	new_end_line;		/* End cursor line.	*/

/*
 * 91.01.11  by S.Yoshida
 * Get old cursor info.
 */
getcursor()
{
	inregs.h.ah = 0x82;		/* Set/Get cursor mode.	*/
	inregs.h.al = 0x04;
	inregs.h.bl = -1;		/* Get mode.		*/
	int86(0x10, &inregs, &outregs);	/* INT 10h.		*/
	old_blink_mode = new_blink_mode = outregs.h.al;

	inregs.h.ah = 0x03;		/* Get cursor shape.	*/
	inregs.h.bh = 0;		/* Active page.		*/
	int86(0x10, &inregs, &outregs);	/* INT 10h.		*/
	old_start_line = new_start_line = outregs.h.ch;
					/* Start raster pos.	*/
	old_end_line   = new_end_line   = outregs.h.cl;
					/* End raster pos.	*/

	hold_new_cursor = 0;		/* Not hold new cursor at end. */
}

/*
 * 91.01.05  by S.Yoshida
 * Set cursor to specify shape.
 */
setcursor(s, e, b)
{
	inregs.h.ah = 0x82;		/* Set/Get cursor mode.	*/
	inregs.h.al = 0x04;
	inregs.h.bl = b;		/* Set blink mode.	*/
	int86(0x10, &inregs, &outregs);	/* INT 10h.		*/

	inregs.h.ah = 0x01;		/* Set cursor shape.	*/
	inregs.h.ch = s;		/* Start raster pos.	*/
	inregs.h.cl = e;		/* End raster pos.	*/
	int86(0x10, &inregs, &outregs);	/* INT 10h.		*/
}

/*
 * 91.01.11  by S.Yoshida
 * Reset cursor to old shape.
 */
resetcursor()
{
	if (!hold_new_cursor) {
		setcursor(old_start_line, old_end_line, old_blink_mode);
	}
}

/*
 * 91.01.11  by S.Yoshida
 * Get cursor mode parameter, and set it.
 */
/*ARGSUSED*/
j31_set_cursor(f, n)
{
	register int	s;
	register char	*p;
	register char	c;
	register int	pos = 0;
	register int	i;
	char	buf[NFILEN];

	if ((s = ereply("IBM PC Cursor: ", buf, NFILEN)) != TRUE) {
		return (s);
	}

	i = -1;

	for (p = buf; *p;) {
		c = *p++ & 0xff;
		switch (c) {
			case ',':
			case ':':
			if (i >= 0 && i <= 15) {
				if (pos == 0) {
					new_start_line = i;
				} else if (pos == 1) {
					new_end_line = i;
				}
			}
			i = -1;
			if (++pos > 3) {
				return TRUE;
			}
			break;
			case 't':
			case 'T':
			if (pos == 2) {
				new_blink_mode = 0;	/* Blink on. */
			} else if (pos == 3) {
				hold_new_cursor = 1;	/* Hold on. */
			}
			break;
			case 'n':
			case 'N':
			if (pos == 2) {
				new_blink_mode = 1;	/* Blink off. */
			} else if (pos == 3) {
				hold_new_cursor = 0;	/* Hold off. */
			}
			break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			if (i == -1) {
				i = 0;
			}
			i = i * 10 + c - '0';
			break;
		}
	}
	if (i >= 0 && i <= 15) {
		if (pos == 0) {
			new_start_line = i;
		} else if (pos == 1) {
			new_end_line = i;
		}
	}
	setcursor(new_start_line, new_end_line, new_blink_mode);
	return (TRUE);
}
#endif	/* IBMPC */

#ifdef	PC9801	/* 90.03.06  by K.Takano */
/*
 * Assign PC-9801 special keys to use Ng.
 */
assignkey()
{
	static char keytbl[10][6] = {
		"\x16\0\0\0\0",		/* Assign ROLLUP key to C-v.	*/
		"\x1bv\0\0\0",		/* Assign ROLLDOWN key to C-v.	*/
		"\0\0\0\0\0",
		"\x7f\0\0\0\0",		/* Assign Del key to DEL.	*/
		"\x10\0\0\0\0",		/* Assign up-arrow key to C-p.	*/
		"\x02\0\0\0\0",		/* Assign <-  key to C-b.	*/
		"\x06\0\0\0\0",		/* Assign ->  key to C-f.	*/
		"\x0e\0\0\0\0",		/* Assign down-arrow key to C-n. */
		"\x1b<\0\0\0",		/* Assign HOME key to M-<.	*/
		"\x1b>\0\0\0"		/* Assign HELP key to M->.	*/
	};
	union REGS regs;
	register int i;

	for (i = 0; i < 10; i++) {
		regs.x.ax = 0x15 + i;
		regs.h.cl = 0x0c;
		regs.x.dx = (int)keysave[i];
		int86(0xdc, &regs, &regs);

		regs.x.ax = 0x15 + i;
		regs.h.cl = 0x0d;
		regs.x.dx = (int)keytbl[i];
		int86(0xdc, &regs, &regs);
	}
}

/*
 * Restore PC-9801 special key assign.
 */
cancelkey()
{
	union REGS regs;
	register int i;

	for (i = 0; i < 10; i++) {
		regs.x.ax = 0x15 + i;
		regs.h.cl = 0x0d;
		regs.x.dx = (int)keysave[i];
		int86(0xdc, &regs, &regs);
	}
}

#ifdef DO_METAKEY
/*
 * Set EZ-key mode to VZ-mode
 */
setezkey()
{
	union REGS regs;
	struct SREGS sregs;

	regs.x.ax = 0x35e0;
	int86x(0x21, &regs, &regs, &sregs);
	if (peek(sregs.es, regs.x.bx-2) == 0x5a45)
		ezkey = TRUE;
	else
		ezkey = FALSE;

	if (ezkey)
	{
		regs.h.al = 1;
		regs.h.ah = 2;
		int86(0xe0, &regs, &regs);
	}
}

/*
 * Unset EZ-key mode to VZ-mode
 */
resetezkey()
{
	union REGS regs;

	if (ezkey)
	{
		regs.h.al = 0;
		regs.h.ah = 2;
		int86(0xe0, &regs, &regs);
		ezkey = FALSE;
	}
}
#endif  /* DO_METAKEY */ 
#endif	/* PC9801 */

#define TTFLUSH	/* 90.06.08  enable ttflush()  by A.Shirahashi */

#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
#define NOBUF 512
static unsigned char obuf[NOBUF];
static int nobuf = 0;
#endif

/*
 * Write character to the display without ^C check.
 */
ttputc(c)
int c;
{
#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
	if (nobuf > NOBUF)
		ttflush();
	obuf[nobuf++] = c;
#else
# if defined(TCCONIO)
	putch(c);
# elif	defined(PC9801) && defined(__TURBOC__)	/* 90.03.23  by A.Shirahashi */
	union REGS regs;
	struct SREGS sregs;
	
	regs.h.cl = 0x10;
	regs.h.ah = 0x00;
	regs.h.dl = c;
	sregs.ds = _DS;
	sregs.es = _ES;
	int86x(0xdc, &regs, &regs, &sregs);
# else	/* NOT PC9801 && __TURBOC__ */
	bdos(6, c & 0xff, dummy);
# endif	/* TTCONIO || PC9801 && __TURBOC__ */
#endif	/* TTFLUSH */
}

/*
 * Now ttflush() isn't needed. But some function call this,
 * so here is dummy.
 */
ttflush() 
{
#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
	int i;

	for (i = 0; i < nobuf; i++) {
# if defined(TCCONIO)
		putch(obuf[i]);
# elif defined(PC9801) && defined(__TURBOC__)	/* 90.03.23  by A.Shirahashi */
		union REGS regs;
		struct SREGS sregs;
		
		regs.h.cl = 0x10;
		regs.h.ah = 0x00;
		regs.h.dl = obuf[i];
		sregs.ds = _DS;
		sregs.es = _ES;
		int86x(0xdc, &regs, &regs, &sregs);
# else	/* NOT PC9801 && __TURBOC__ */
		bdos(6, obuf[i], dummy);
# endif	/* TCCONIO || PC9801 && __TURBOC__ */
	}

	nobuf = 0;
#endif	/* TTFLUSH */
}

static	int	ahead = -1;		/* Typeahead charactor.		*/
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
static	int	nkey = 0;		/* The number of ungetc charactor. */
static	char	keybuf[4];		/* Ungetc charactors.		*/
#endif	/* KANJI */

/*
 * Read character from terminal without ^C check.
 * All 8 bits are returned, so that you can use
 * a multi-national terminal.
 */
ttgetc() {
    int	c;
    union REGS regs;
    
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
    if (nkey > 0) {
	return(keybuf[--nkey]);
    } else
#endif	/* KANJI */
	if (ahead != -1) {
	    c = ahead;
	    ahead = -1;
	    return(c);
	}
    while ((c = rawgetc()) == -1)
    {
#ifdef	AUTOSAVE
	autosave_handler();
#endif	
    }

#ifdef	DO_METAKEY
#ifdef	PC9801
    if (c<0x20 && use_metakey && ezkey) {
	int shift;
	
	if (c == CCHR('_'))    /* NFER code */
	    return (CCHR('['));
#ifdef __TURBOC__
	_AH = 0x02;
	geninterrupt(0x18);
	shift = _AL;
#else
	regs.h.ah = 0x02;
	int86(0x18, &regs, &regs);
	shift = regs.h.al;
#endif
	if (shift & 0x08)
	{
	    if (~(shift & 0x10))
		c += 0x40;
	    c &= 0x7F;
	    c |= METABIT;
	}
    }
#endif	/* PC9801 */
#endif	/* DO_METAKEY */
    return (c);
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
 * set the tty size.
 */
setttysize() {
#ifdef TCCONIO
	gettextinfo(&tinfo);
	nrow = tinfo.screenheight;
	ncol = tinfo.screenwidth;
#else
	if ((nrow=tgetnum ("li")) <= 0
		|| (ncol=tgetnum ("co")) <= 0) {
		nrow = 24;
		ncol = 80;
	}
#endif
}

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
typeahead() {
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return(TRUE);
	}
#endif	/* KANJI */
	if (ahead != -1) {
		return(TRUE);
	}
	ahead = rawgetc();
	return(ahead != -1);
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
/*
 * A program to return TRUE if we wait for 1 seconds without anything
 * happening, else return FALSE.
 */
int ttwait() {
	register int	s, ss, se;

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return(FALSE);
	}
#endif	/* KANJI */
	se = (ss = syssec()) + 100;
	do {
		if (typeahead()) {
			return(FALSE);
		}
		s = syssec();
		if (se >= 6000 && s < ss) {
			se -= 6000;
		}
	} while (s < se);
	return(TRUE);
}
#endif

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
fepmode_init()
{
	if (fepctrl)
		fep_init();
}


fepmode_term()
{
	if (fepctrl)
		fep_term();
}


fepmode_on()
{
	if (fepctrl) {
		if (fepforce) {
			if (fepforce == 1)
				fep_force_on();
			else
				fep_force_off();
			fepforce = 0;
			fepmode = TRUE;
			}
		else if (!fepmode) {
			fep_on();
			fepmode = TRUE;
		}
	}
}


fepmode_off()
{
	if (fepctrl && fepmode) {
		fep_off();
		fepmode = FALSE;
	}
}

fepmode_toggle()
{
	if (fepctrl) {
		if (fep_get_mode())
			fepforce = -1;
		else
			fepforce = 1;
		return (TRUE);
	}
	return (FALSE);
}

fepmode_set(f, n)
{
	register int s;
	char buf[NFILEN];

	if (f & FFARG) {
		n = (n > 0);
	}
	else {
		if ((s = ereply("FEP Control: ", buf, NFILEN)) != TRUE)
			return (s);
		if (ISDIGIT(buf[0]) || buf[0] == '-')
			n = (atoi(buf) > 0);
		else if (buf[0] == 't' || buf[0] == 'T')
			n = TRUE;
		else /* if (buf[0] == 'n' || buf[0] == 'N') */
			n = FALSE;
	}

	if (!fepctrl && n) {
		fep_init();
		fepmode = TRUE;
	}
	else if (fepctrl && !n)
		fep_term();
	fepctrl = n;

	return (TRUE);
}


fepmode_chg(f, n)
{
	if (fepctrl = !fepctrl) {
		fep_init();
		fepmode = TRUE;
	}
	else
		fep_term();
	return (TRUE);
}
#endif /* FEPCTRL */
