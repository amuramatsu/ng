/* $Id: ttyio.c,v 1.6.2.1 2003/03/02 17:36:42 amura Exp $ */
/*
 *		Human68k terminal I/O
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.6.2.1  2003/03/02 17:36:42  amura
 * add #include <iocslib.h>
 *
 * Revision 1.6  2001/03/09 15:53:14  amura
 * enable to really work autosave feature
 *
 * Revision 1.5  2001/03/02 08:48:31  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.4  2001/01/20 15:48:46  amura
 * very big terminal supported
 *
 * Revision 1.3  2001/01/05 14:07:07  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.2  2000/07/20 12:41:56  amura
 * enable to use XF1/2 key as META
 *
 * Revision 1.1.1.1  2000/06/27 01:47:57  amura
 * import to CVS
 *
 */
/* 90.11.09	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */
/*		Terminal I/O for BSD UNIX */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	<doslib.h>
#include	<iocslib.h>
#include	<time.h>

#define	RAW_MODE	0x20
#define	COOKED_MODE	0x00
#define NOBUF 512

#define FNCKEY_DEL	24
#define FNCKEY_LEFT	26
#define FNCKEY_RIGHT	27
#define FNCKEY_UP	25
#define FNCKEY_DOWN	28
#define FNCKEY_ROLLUP	21
#define FNCKEY_ROLLDN	22
#define FNCKEY_HOME	31
#define FNCKEY_UNDO	32

short	ospeed = 13;			/* We think 9600 bps is used.	*/
int	nrow;				/* Terminal size, rows.		*/
int	ncol;				/* Terminal size, columns.	*/

static char	obuf[NOBUF];	/* Output buffer.		*/
static int	nobuf = 0;
static int	stdinstat;		/* stdin IOCTRL status.		*/
static unsigned char	fnckeybuf[9][6];    /* buffer for function keys */

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
static int fepctrl = FALSE;		/* FEP control enable flag	*/
static int fepmode = TRUE;		/* now FEP mode			*/
static int fepforce = 0;		/* force FEP to mode		*/
#endif

/*
 * This function gets called once, to set up
 * the terminal channel. 
 */
ttopen() {
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
	if (ttraw() == FALSE)
		panic("aborting due to terminal initialize failure");
#ifdef	ADDFUNC		/* 90.02.14  by S.Yoshida */
#ifdef	SIGWINCH	/* 90.02.13  by S.Yoshida */
	(void) signal(SIGWINCH, ttwinch);
#endif	/* SIGWINCH */
#endif	/* ADDFUNC */
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_init();
#endif
}

/*
 * This function sets the terminal to RAW mode, as defined for the current
 * shell.  This is called both by ttopen() above and by spawncli() to
 * get the current terminal settings and then change them to what
 * Ng expects.	Thus, stty changes done while spawncli() is in effect
 * will be reflected in Ng.
 */
ttraw() {
	if ((stdinstat = IOCTRLGT(0)) < 0) {
		ewprintf("ttopen can't get IOCTRL status");
		return(FALSE);
	}
	if (IOCTRLST(0, RAW_MODE) < 0)
		return(FALSE);

	setttysize();
	assignkey();
	return(TRUE);
}

/*
 * This function gets called just
 * before we go back home to the shell. Put all of
 * the terminal parameters back.
 * Under Human68k this just calls ttcooked(), but the ttclose() hook is in
 * because vttidy() in display.c expects it for portability reasons.
 */
ttclose() {
	if (ttcooked() == FALSE)
		panic("");		/* ttcooked() already printf'd	*/
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_term();
#endif
}

/*
 * This function restores all terminal settings to their default values,
 * in anticipation of exiting or suspending the editor.
 */
ttcooked() {
	ttflush();
	if (IOCTRLST(0, stdinstat) < 0) {
		ewprintf("ttclose can't set IOCTRL status");
		return(FALSE);
	}
	cancelkey();
	return(TRUE);
}

/*
 * Assign Human68k special keys to use Ng.
 */
assignkey()
{
	unsigned char buf[6];

	/* Assign Del key to DEL.	*/
	FNCKEYGT (FNCKEY_DEL, fnckeybuf[0]);
	buf[0] = '\177';
	buf[1] = '\0';
	FNCKEYST (FNCKEY_DEL, buf);	
	/* Assign <-  key to C-b.	*/
	FNCKEYGT (FNCKEY_LEFT, fnckeybuf[1]);
	buf[0] = '\002';
	buf[1] = '\0';
	FNCKEYST (FNCKEY_LEFT, buf);
	/* Assign ->  key to C-f.	*/
	FNCKEYGT (FNCKEY_RIGHT, fnckeybuf[2]);
	buf[0] = '\006';
	buf[1] = '\0';
	FNCKEYST (FNCKEY_RIGHT, buf);
	/* Assign up-arrow key to C-p.	*/
	FNCKEYGT (FNCKEY_UP, fnckeybuf[3]);
	buf[0] = '\020';
	buf[1] = '\0';
	FNCKEYST (FNCKEY_UP, buf);
	/* Assign down-arrow key to C-n. */
	FNCKEYGT (FNCKEY_DOWN, fnckeybuf[4]);
	buf[0] = '\016';
	buf[1] = '\0';
	FNCKEYST (FNCKEY_DOWN, buf);
	/* Assign Roll-Up key to M-v.	*/
	FNCKEYGT (FNCKEY_ROLLUP, fnckeybuf[5]);
	buf[0] = '\033';
	buf[1] = '\166';
	buf[2] = '\0';
	FNCKEYST (FNCKEY_ROLLUP, buf);
	/* Assign Roll-Dn key to C-v.	*/
	FNCKEYGT (FNCKEY_ROLLDN, fnckeybuf[6]);
	buf[0] = '\026';
	buf[1] = '\0';
	FNCKEYST (FNCKEY_ROLLDN, buf);
	/* Assign Home  key to M-<.	*/
	FNCKEYGT (FNCKEY_HOME, fnckeybuf[7]);
	buf[0] = '\033';
	buf[1] = '\074';
	buf[2] = '\0';
	FNCKEYST (FNCKEY_HOME, buf);
	/* Assign Ins key to M->.	*/
	FNCKEYGT (FNCKEY_UNDO, fnckeybuf[8]);
	buf[0] = '\033';
	buf[1] = '\076';
	buf[2] = '\0';
	FNCKEYST (FNCKEY_UNDO, buf);
}

/*
 * Cancel Human68k special key assign.
 */
cancelkey()
{
	FNCKEYST (FNCKEY_DEL, fnckeybuf[0]);
	FNCKEYST (FNCKEY_LEFT, fnckeybuf[1]);
	FNCKEYST (FNCKEY_RIGHT, fnckeybuf[2]);
	FNCKEYST (FNCKEY_UP, fnckeybuf[3]);
	FNCKEYST (FNCKEY_DOWN, fnckeybuf[4]);
	FNCKEYST (FNCKEY_ROLLUP, fnckeybuf[5]);
	FNCKEYST (FNCKEY_ROLLDN, fnckeybuf[6]);
	FNCKEYST (FNCKEY_HOME, fnckeybuf[7]);
	FNCKEYST (FNCKEY_UNDO, fnckeybuf[8]);
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
 * Now ttflush() isn't needed. But some function call this,
 * so here is dummy.
 */
ttflush() 
{
    int i;

    for (i = 0; i < nobuf; i++)
    {
        if (obuf[i] == 0x0a)
	    B_PUTC (0x0d);
        B_PUTC (obuf[i] & 0xff);
    }
    nobuf = 0;
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
	int    c;
/* Process OPT.1/OPT.2 as a META key.
 * 		89.??.??  by M.Kondo
 * 		91.01.14  by K.Maeda
 */
	register int shifts;
#ifdef DO_METAKEY
	extern int use_metakey;		/* set in the generic kbd.c */
#endif
#ifndef CTRL
#  define SHIFT		(0x01)
#  define CTRL		(0x02)
#  define OPT1		(0x04)
#  define OPT2		(0x08)
#  define CAPSLOCK	(0x80)
#endif /* CTRL */
#ifndef	XF1_3GROUP
#  define XF1_3GROUP	(0x0A)
#  define XF1		(0x20 << 16)
#  define XF2		(0x40 << 16)
#  define XF3		(0x80 << 16)
#endif	/* XF1_3GROUP */

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return(keybuf[--nkey]);
	}	/* 91.01.14  by K.Maeda ---remove else */
#endif	/* KANJI */
#ifdef	AUTOSAVE
	while (!kbhit())
		autosave_handler();	/* this is polling */
#endif
	c = FGETC (1);
 	shifts = K_SFTSNS() & 0xFFFF;
	shifts |= K_KEYBIT(XF1_3GROUP)<<16;
	if (c == ' ' && (shifts & CTRL))
		c = 0;
#ifdef DO_METAKEY
	else if (use_metakey == TRUE && (shifts & (OPT1|OPT2|XF1|XF2)))
		return ((KCHAR)(c | METABIT));
#endif /* DO_METAKEY */
	return ((KCHAR)c);
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
	if ((nrow=tgetnum ("li")) <= 0
	|| (ncol=tgetnum ("co")) <= 0) {
		nrow = 32;
		ncol = 96;
	}
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
#ifdef	FEPCTRL
	if (fep_get_mode())	/* This hack for ASK68k */
		return(FALSE);
#endif
	return (kbhit ());
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
	int start;
	int lap;

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return(FALSE);
	}
#endif	/* KANJI */
	start = ONTIME();
	lap = 0;
	while (lap < 100) {
		if (kbhit()) {
			return(FALSE);
		}
		lap = ONTIME() - start;
		if (lap < 0)
			lap += 8640000;
	}
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

#ifdef	AUTOSAVE
VOID
itimer(func, sec)
{
    /* VDISPST */
}
#endif	/* AUTOSAVE */
