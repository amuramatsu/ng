/* $Id: ttyio.cpp,v 1.3.2.1 2006/01/14 23:43:38 amura Exp $ */
/*
 *		Epoc32 terminal I/O. (Tested only at Psion S5mx)
 *		I make this file from MSDOS ttyio.c.
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#include <e32base.h>
#include <e32cons.h>
#include "epoctty.h"

extern CTrapCleanup* cleanup;

int nrow;				/* Terminal size, rows.		*/
int ncol;				/* Terminal size, columns.	*/
void setttysize(void);

#ifdef	DO_METAKEY
extern int use_metakey;
#endif

static EpocTty* epoctty;

/*
 * This function gets called once, to set up
 * the terminal channel. 
 */
void
ttopen(void)
{
    epoctty = EpocTty::NewL(_L("Ng for Epoc32 test implement"),
			    TSize(80, 25));
    CleanupStack::PushL(epoctty);
    setttysize();
}

/*
 * This function gets called just
 * before we go back home to the shell. Put all of
 * the terminal parameters back.
 * Under MS-DOS this just calls ttcooked(), but the ttclose() hook is in
 * because vttidy() in display.c expects it for portability reasons.
 */
void
ttclose(void)
{
    ttflush();
    CleanupStack::Pop();
    delete epoctty;
}

#define TTFLUSH	/* 90.06.08  enable ttflush()  by A.Shirahashi */

#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
#define NOBUF 512
static unsigned char obuf[NOBUF];
static int nobuf = 0;
#endif

/*
 * Write character to the display without ^C check.
 */
void
ttputc(int c)
{
#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
    if (nobuf > NOBUF)
	ttflush();
    obuf[nobuf++] = c;
#else
    epoctty->Putch(c);
#endif	/* TTFLUSH */
}

/*
 * Now ttflush() isn't needed. But some function call this,
 * so here is dummy.
 */
void
ttflush() 
{
#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
    int i;
    for (i = 0; i < nobuf; i++)
	epoctty->Putch(obuf[i]);
    nobuf = 0;
#endif	/* TTFLUSH */
}

static	int	ahead = -1;		/* Typeahead charactor.		*/
static	int	nkey = 0;		/* The number of ungetc charactor. */
static	char	keybuf[4];		/* Ungetc charactors.		*/

/*
 * Read character from terminal without ^C check.
 * All 8 bits are returned, so that you can use
 * a multi-national terminal.
 */
int
ttgetc(void)
{
    int c;
    if (nkey > 0) {
	return keybuf[--nkey];
    }
    else if (ahead != -1) {
	c = ahead;
	ahead = -1;
	return c;
    }
    while ((c = epoctty->Getch()) == -1) {
#ifdef	AUTOSAVE
	autosave_handler();
#endif	
    }
    return c;
}

/*
 * Save pre-readed char to read again.
 */
void
ttungetc(int c)
{
    keybuf[nkey++] = c;
}

/*
 * set the tty size.
 */
void
setttysize(void)
{
    TSize size = epoctty->ScreenSize();
    ncol = size.iWidth;
    nrow = size.iHeight;
}

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
int
typeahead(void)
{
    if (nkey > 0)
	return TRUE;
    if (ahead != -1)
	return TRUE;
    ahead = epoctty->Getch();
    return ahead != -1;
}

/*
 * panic - just exit, as quickly as we can.
 */
void
panic(char *s)
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
ttwait(void)
{
    if (nkey > 0)
	return(FALSE);
    TTime nowTime;
    nowTime.UniversalTime();
    TTime endTime = nowTime + TTimeIntervalSeconds(1);
    do {
	if (typeahead())
	    return FALSE;
	nowTime.UniversalTime();
    } while (nowTime < endTime);
    return TRUE;
}
#endif

void
epoc_ttattr(int attribute)
{
    epoctty->SetColor(attribute);
}

void
epoc_ttmove(int x, int y)
{
    epoctty->SetPos(y, x);
}

void
epoc_tteeol(void)
{
    epoctty->ClearToEndOfLine();
}

void
putline(int row, const NG_WCHAR_t *s, int color)
{
    epoctty->PutLine(row, column, s, color);
}
