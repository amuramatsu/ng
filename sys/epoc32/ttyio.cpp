/* $Id: ttyio.cpp,v 1.1 2001/09/30 15:59:12 amura Exp $ */
/*
 *		Epoc32 terminal I/O. (Tested only at Psion S5mx)
 *		I make this file from MSDOS ttyio.c.
 */

/*
 * $Log: ttyio.cpp,v $
 * Revision 1.1  2001/09/30 15:59:12  amura
 * Initial EPOC32 commit.
 *
 * Currently this is not run yet. Some functions around tty are not implemented.
 *
 */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	<e32base.h>
#include	<e32cons.h>

#include	"epoctty.h"

extern CTrapCleanup* cleanup;

int	nrow;				/* Terminal size, rows.		*/
int	ncol;				/* Terminal size, columns.	*/
int	setttysize();

#ifdef	DO_METAKEY
extern int use_metakey;
#endif

static EpocTty* epoctty;

/*
 * This function gets called once, to set up
 * the terminal channel. 
 */
int
ttopen()
{
    epoctty = EpocTty::NewL(_L("Ng for Epoc32 test implement"),
			    TSize(80, 25));
    CleanupStack::PushL(epoctty);
    setttysize();
    return 0;
}

/*
 * This function gets called just
 * before we go back home to the shell. Put all of
 * the terminal parameters back.
 * Under MS-DOS this just calls ttcooked(), but the ttclose() hook is in
 * because vttidy() in display.c expects it for portability reasons.
 */
int
ttclose()
{
    ttflush();
    CleanupStack::Pop();
    delete epoctty;
    return 0;
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
int
ttputc(int c)
{
#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
    if (nobuf > NOBUF)
	ttflush();
    obuf[nobuf++] = c;
#else
    epoctty->Putch(c);
#endif	/* TTFLUSH */
    return 0;
}

/*
 * Now ttflush() isn't needed. But some function call this,
 * so here is dummy.
 */
int
ttflush() 
{
#ifdef	TTFLUSH	/* 90.06.08  by A.Shirahashi */
    int i;
    for (i = 0; i < nobuf; i++) {
	epoctty->Putch(obuf[i]);
    }
    nobuf = 0;
#endif	/* TTFLUSH */
    return 0;
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
int
ttgetc() {
    int c;
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
    while ((c = epoctty->Getch()) == -1) {
#ifdef	AUTOSAVE
	autosave_handler();
#endif	
    }
    return (c);
}

#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
/*
 * Save pre-readed char to read again.
 */
int
ttungetc(int c)
{
    keybuf[nkey++] = c;
    return 0;
}
#endif	/* KANJI */

/*
 * set the tty size.
 */
int
setttysize()
{
    TSize size = epoctty->ScreenSize();
    ncol = size.iWidth;
    nrow = size.iHeight;
    return 0;
}

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
int
typeahead() {
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
    if (nkey > 0) {
	return(TRUE);
    }
#endif	/* KANJI */
    if (ahead != -1) {
	return(TRUE);
    }
    ahead = epoctty->Getch();
    return (ahead != -1);
}

/*
 * panic - just exit, as quickly as we can.
 */
int
panic(char *s)
{
    (void) fputs("panic: ", stderr);
    (void) fputs(s, stderr);
    (void) fputc('\n', stderr);
    (void) fflush(stderr);
    abort();		/* To leave a core image. */
    return 0;
}

#ifndef NO_DPROMPT
/*
 * A program to return TRUE if we wait for 1 seconds without anything
 * happening, else return FALSE.
 */
int
ttwait() {
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
    if (nkey > 0) {
	return(FALSE);
    }
#endif	/* KANJI */
    TTime nowTime;
    nowTime.UniversalTime();
    TTime endTime = nowTime + TTimeIntervalSeconds(1);
    do {
	if (typeahead()) {
	    return(FALSE);
	}
	nowTime.UniversalTime();
    } while (nowTime < endTime);
    return(TRUE);
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
    epoctty->SetPos(x, y);
}

void
epoc_tteeol()
{
    epoctty->ClearToEndOfLine();
}

#ifdef SS_SUPPORT
void
putline(int row, int column, unsigned char *s, unsigned char *t, short color)
{
    epoctty->PutLine(row, column, s, t, color);
}
#else
void
putline(int row, int column, unsigned char *s, short color)
{
    epoctty->PutLine(row, column, s, NULL, color);
}
#endif
