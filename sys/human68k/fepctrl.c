/* $Id: fepctrl.c,v 1.2 2001/11/23 11:56:47 amura Exp $ */
/*
 * fepctrl.c 1.2 1990/11/25	Public Domain.
 *
 * General purpose Japanese FEP control routines for Human68k.
 * Written for MS-DOS by Junn Ohta <ohta@src.ricoh.co.jp>
 * Adjusted to Human68k by Sawayanagi Yosirou <willow@saru.cc.u-tokyo.ac.jp>
 *
 *	int fep_init()
 *		checks FEP and turn it off, returns FEP type.
 *	void fep_term()
 *		restore the status of FEP saved by fep_init().
 *	void fep_on()
 *		restore the status of FEP saved by fep_off().
 *	void fep_off()
 *		save the status of FEP and turn it off.
 *	void fep_force_on()
 *		turn FEP on by its default "on" status.
 *	void fep_force_off()
 *		don't save the status of FEP and turn it off.
 */

/*
 * $Log: fepctrl.c,v $
 * Revision 1.2  2001/11/23 11:56:47  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */

#define	NG
#ifdef	NG
#include "config.h"
#endif

#ifdef	FEPCTRL

#include "fepctrl.h"

/*
 * default "on" status of FEP (used only in fep_force_on())
 */
static int fepon[NFEPS] = {
    /* FEP_NONE		*/  0,
    /* FEP_ASK68K	*/  1	/* 0=off, 1=on */
};

static int fep = FEP_NONE;
static int oldmode = 0;
static int keepmode = 0;

static VOID fep_open _PRO((void)), fep_close _PRO((void));
static int fep_find _PRO((void));
static int fep_mode _PRO((int));
static int ask68k_get_mode _PRO((void));
static int ask68k_set_mode _PRO((int));

int
fep_init()
{
    if (fep == FEP_NONE)
	fep = fep_find();
    fep_open();
    oldmode = keepmode = fep_mode(0);
    return fep;
}

void
fep_term()
{
    fep_mode(oldmode);
    fep_close();
}

void
fep_on()
{
    fep_mode(keepmode);
}

void
fep_off()
{
    keepmode = fep_mode(0);
}

void
fep_force_on()
{
    fep_mode(fepon[fep]);
}

void
fep_force_off()
{
	fep_mode(0);
}

int
fep_get_mode()
{
	return ask68k_get_mode();
}

/*--------------------------------------------------------------------*/
#ifdef TEST

static char *fepname[NFEPS] = {
    /* FEP_NONE		*/  "(none)",
    /* FEP_ASK68K	*/  "ASK68K"
};

static VOID
putstr(s)
char *s;
{
    while (*s)
	putch(*s++);
}

static VOID
echoline(s)
char *s;
{
    int c;
    putstr(s);
    while ((c = getch()) != '\r' && c != '\n')
	putch(c);
    putstr("\r\n");
}

int
main()
{
    putstr("fep = ");
    putstr(fepname[fep_init()]);
    putstr("\r\n");
    putstr("enter 4 lines of text\r\n");
    fep_force_on();
    echoline(" on: ");
    fep_off();
    echoline("off: ");
    fep_on();
    echoline(" on: ");
    fep_force_off();
    echoline("off: ");
    fep_term();
    exit(0);
}

#endif /* TEST */
/*--------------------------------------------------------------------*/

#ifdef __GNUC__
#define _asm		asm
#endif

static int
fep_find()
{
    /* it should be done in more exact way */
    int d0;
    
    _asm("	move.l	#2,-(sp)");
    _asm("	dc.w	$ff22");
    _asm("	addq.l	#4,sp");
    _asm("	move.l	d0,-4(a6)");
    
    if (d0 == -1)
	return FEP_NONE;
    else
	return FEP_ASK68K;
}

static VOID
fep_open()
{
    extern int hentrap _PRO((int));
    
    if (fep == FEP_ASK68K)
	hentrap(-1);
}

static VOID
fep_close()
{
    extern int hentrap _PRO((int));
    
    if (fep == FEP_ASK68K)
	hentrap(0);
}

static int
fep_mode(newmode)
int newmode;
{
    int curmode;
    
    switch (fep) {
    case FEP_ASK68K:
	/* mode: 0 = off, 1 = on */
	curmode = ask68k_get_mode();
	if (newmode != curmode)
	    ask68k_set_mode(newmode);
	return curmode;
    default:
	return 0;
    }
}

static int
ask68k_get_mode()
{
    extern int iskmode _PRO((void));
    
    if (iskmode() == 0)
	return 0;
    else
	return 1;
}

static int
ask68k_set_mode(flag)
int flag;
{
    if (flag) {
	/* mode = KNJCTRL(2) */
	_asm("	move.l	#2,-(sp)");
	_asm("	dc.w	$ff22");
	_asm("	addq.l	#4,sp");
	/* KNJCTRL(1, mode) */
	_asm("	move.l	d0,-(sp)");
	_asm("	move.l	#1,-(sp)");
	_asm("	dc.w	$ff22");
	_asm("	addq.l	#8,sp");
	return;    /* return d0 */
    }
    else {
	/* KNJCTRL(1, 0) */
	_asm("	clr.l	-(sp)");
	_asm("	move.l	#1,-(sp)");
	_asm("	dc.w	$ff22");
	_asm("	addq.l	#8,sp" );
	return;    /* return d0 */
    }
}
#endif /* FEPCTRL */
