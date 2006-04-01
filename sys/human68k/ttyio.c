/* $Id: ttyio.c,v 1.12.2.2 2006/04/01 15:19:28 amura Exp $ */
/*
 *		Human68k terminal I/O
 */
/* 90.11.09	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */
/*		Terminal I/O for BSD UNIX */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#include <doslib.h>
#include <iocslib.h>
#include <time.h>
#include "tty.h"
#ifdef FEPCTRL
#include "fepctrl.h"
#endif
#include "i_lang.h"

#define	RAW_MODE	0x20
#define	COOKED_MODE	0x00
#define NOBUF		512

#define FNCKEY_DEL	24
#define FNCKEY_LEFT	26
#define FNCKEY_RIGHT	27
#define FNCKEY_UP	25
#define FNCKEY_DOWN	28
#define FNCKEY_ROLLUP	21
#define FNCKEY_ROLLDN	22
#define FNCKEY_HOME	31
#define FNCKEY_UNDO	32

int nrow;				/* Terminal size, rows.		*/
int ncol;				/* Terminal size, columns.	*/

static char obuf[NOBUF];		/* Output buffer.		*/
static int nobuf = 0;
static int stdinstat;			/* stdin IOCTRL status.		*/
static unsigned char fnckeybuf[9][6];	/* buffer for function keys */

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
static int fepctrl = FALSE;		/* FEP control enable flag	*/
static int fepmode = TRUE;		/* now FEP mode			*/
static int fepforce = 0;		/* force FEP to mode		*/
VOID fepmode_init _PRO((void));
VOID fepmode_term _PRO((void));
VOID fepmode_on _PRO((void));
VOID fepmode_off _PRO((void));
int fepmode_toggle _PRO((void));
#endif

static VOID assignkey _PRO((void));
static VOID cancelkey _PRO((void));
VOID setttysize _PRO((void));

/*
 * This function gets called once, to set up
 * the terminal channel. 
 */
VOID
ttopen()
{
    register char *tv_stype;
    char tcbuf[TERMCAP_BUF_LEN], err_str[72];

#ifndef DIRECT_IOCS
    /* do this the REAL way */
    if ((tv_stype = getenv("TERM")) == NULL) {
	puts("Environment variable TERM not defined!");
	exit(1);
    }

    if((tgetent(tcbuf, tv_stype)) != 1) {
	sprintf(err_str, "Unknown terminal type %s!", tv_stype);
	puts(err_str);
	exit(1);
    }
#endif
    if (ttraw() == FALSE)
	panic("aborting due to terminal initialize failure");
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
int
ttraw()
{
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
VOID
ttclose()
{
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
int
ttcooked()
{
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
static VOID
assignkey()
{
    int i;
    static struct {
	int  key_code;
	char key_data[6];
    } key_defs[] = {
	{ FNCKEY_DEL,	"\177" },	/* Assign Del key to DEL.	*/
	{ FNCKEY_LEFT,	"\002" },	/* Assign <-  key to C-b.	*/
	{ FNCKEY_RIGHT,	"\006" },	/* Assign ->  key to C-f.	*/
	{ FNCKEY_UP,	"\020" },	/* Assign up-arrow key to C-p.	*/
	{ FNCKEY_DOWN,	"\016" },	/* Assign down-arrow key to C-n. */
	{ FNCKEY_ROLLUP,"\033\166" },	/* Assign Roll-Up key to M-v.	*/
	{ FNCKEY_ROLLDN,"\026" },	/* Assign Roll-Dn key to C-v.	*/
	{ FNCKEY_HOME,	"\033\074" },	/* Assign Home  key to M-<.	*/
	{ FNCKEY_UNDO,	"\033\076" },	/* Assign Ins key to M->.	*/
    };

    for (i=0; i<9; i++) {
	FNCKEYGT(key_defs[i].key_code, fnckeybuf[i]);
	FNCKEYST(key_defs[i].key_code, key_defs[i].key_data);
    }
}

/*
 * Cancel Human68k special key assign.
 */
static VOID
cancelkey()
{
    FNCKEYST(FNCKEY_DEL, fnckeybuf[0]);
    FNCKEYST(FNCKEY_LEFT, fnckeybuf[1]);
    FNCKEYST(FNCKEY_RIGHT, fnckeybuf[2]);
    FNCKEYST(FNCKEY_UP, fnckeybuf[3]);
    FNCKEYST(FNCKEY_DOWN, fnckeybuf[4]);
    FNCKEYST(FNCKEY_ROLLUP, fnckeybuf[5]);
    FNCKEYST(FNCKEY_ROLLDN, fnckeybuf[6]);
    FNCKEYST(FNCKEY_HOME, fnckeybuf[7]);
    FNCKEYST(FNCKEY_UNDO, fnckeybuf[8]);
}

/*
 * Write character to the display.
 * Characters are buffered up, to make things
 * a little bit more efficient.
 */
VOID
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
VOID
ttflush() 
{
    int i;

    for (i = 0; i < nobuf; i++) {
        if (obuf[i] == 0x0a)
	    B_PUTC(0x0d);
	B_PUTC(obuf[i] & 0xff);
    }
    nobuf = 0;
}

static int nkey = 0;		/* The number of ungetc charactor. */
static int keybuf[4];		/* Ungetc charactors.		*/

/*
 * Read character from terminal.
 * All 8 bits are returned, so that you can use
 * a multi-national terminal.
 */
int
ttgetc()
{
    int c;
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

    if (nkey > 0)
	return(keybuf[--nkey]);
#ifdef	AUTOSAVE
    while (!kbhit())
	autosave_handler();	/* this is polling */
#endif
    c = FGETC(1);
    shifts = K_SFTSNS() & 0xFFFF;
    shifts |= K_KEYBIT(XF1_3GROUP)<<16;
    if (c == ' ' && (shifts & CTRL))
	c = 0;
#ifdef DO_METAKEY
    else if (use_metakey == TRUE && (shifts & (OPT1|OPT2|XF1|XF2)))
	return (KCHAR)(c | METABIT);
#endif /* DO_METAKEY */
    return (KCHAR)c;
}

/*
 * Save pre-readed char to read again.
 */
VOID
ttungetc(c)
int c;
{
    keybuf[nkey++] = c;
}

/*
 * set the tty size.
 */
VOID
setttysize()
{
#ifdef DIRECT_IOCS
    nrow = 31;
    ncol = 96;
#else
    if ((nrow=tgetnum ("li")) <= 0
	|| (ncol=tgetnum ("co")) <= 0) {
	nrow = 31;
	ncol = 96;
    }
#endif
}

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
int
typeahead()
{
    if (nkey > 0)
	return(TRUE);
#ifdef	FEPCTRL
    if (fep_get_mode())	/* This hack for ASK68k */
	return(FALSE);
#endif
    return (kbhit());
}

/*
 * panic - just exit, as quickly as we can.
 */
VOID
panic(s)
const char *s;
{
    fputs("panic: ", stderr);
    fputs(s, stderr);
    fputc('\n', stderr);
    fflush(stderr);
    abort();		/* To leave a core image. */
}
#ifndef NO_DPROMPT
/*
 * A program to return TRUE if we wait for 1 seconds without anything
 * happening, else return FALSE.
 */
int
ttwait()
{
    int start;
    int lap;
    
    if (nkey > 0)
	return(FALSE);
    start = ONTIME();
    lap = 0;
    while (lap < 100) {
	if (kbhit())
	    return(FALSE);
	lap = ONTIME() - start;
	if (lap < 0)
	    lap += (100 * 60 * 60 * 24);
    }
    return TRUE;
}
#endif

VOID
putline(int row, const NG_WCHAR_t *s, int color)
{
    static char buf[NCOL + 1];
    char *p, *endp;
    register int i = 0;
    int attr = 3;

    if (color == CTEXT)
        attr = 3;
    else if (color == CMODE)
	attr = 11;
    p = buf;
    endp = &buf[NCOL];
    while (p < endp) {
	if (ISASCII(*s))
	    *p++ = *s++ & 0x7f;
	else {
	    if ((i=terminal_lang->lm_get_display_code(*s++, p, endp - p)) < 0)
	        break;
	    p += i;
	}
    }
    *p = '\0';
    B_CUROFF();
    B_PUTMES(attr, 0, row, NCOL, buf);
    B_CURON();
}

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
VOID
fepmode_init()
{
    if (fepctrl)
	fep_init();
}

VOID
fepmode_term()
{
    if (fepctrl)
	fep_term();
}

VOID
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

VOID
fepmode_off()
{
    if (fepctrl && fepmode) {
	fep_off();
	fepmode = FALSE;
    }
}

int
fepmode_toggle()
{
    if (fepctrl) {
	if (fep_get_mode())
	    fepforce = -1;
	else
	    fepforce = 1;
	return TRUE;
    }
    return FALSE;
}

int
fepmode_set(f, n)
int f, n;
{
    register int s;
    char buf[NFILEN];

    if (f & FFARG)
	n = (n > 0);
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
    
    return TRUE;
}

int
fepmode_chg(f, n)
int f, n;
{
    fepctrl = !fepctrl;
    if (fepctrl) {
	fep_init();
	fepmode = TRUE;
    }
    else
	fep_term();
    return TRUE;
}
#endif /* FEPCTRL */

#ifdef	AUTOSAVE
VOID
itimer(func, sec)
VOID (*func)();
int sec;
{
    /* VDISPST */
}
#endif	/* AUTOSAVE */
