/* $Id: fasttty.c,v 1.1.2.2 2006/01/14 21:39:51 amura Exp $ */
/*
 * IOCS direct display driver
 */
#include <iocslib.h>

#define BEL	0x07			/* BEL character.		*/

extern int ttrow;
extern int ttcol;
extern int tttop;
extern int ttbot;
extern int tthue;

int tceeol;			/* Costs are set later */
int tcinsl;
int tcdell;

#ifdef NO_RESIZE
static VOID setttysize _PRO((void));
#endif

VOID
ttinit()
{
    ttresize();			/* set nrow & ncol	*/
    tceeol = 1;
    tcinsl = 2;
    tcdell = 2;
}

/*
 * Clean up the terminal, in anticipation of
 * a return to the command interpreter. This is a no-op
 * on the ANSI display. On the SCALD display, it sets the
 * window back to half screen scrolling. Perhaps it should
 * query the display for the increment, and put it
 * back to what it was.
 */
VOID
tttidy()
{
    /* NOP */
}

/*
 * Move the cursor to the specified
 * origin 0 row and column position. Try to
 * optimize out extra moves; redisplay may
 * have left the cursor in the right
 * location last time!
 */
VOID
ttmove(row, col)
int row, col;
{
     if (ttrow!=row || ttcol!=col) {
	 ttflush();
	 B_LOCATE(col, row);
	 ttrow = row;
	 ttcol = col;
     }
}

/*
 * Erase to end of line.
 */
VOID
tteeol() {
    ttflush();
    B_ERA_ED();
}

/*
 * Erase to end of page.
 */
VOID
tteeop()
{
    ttflush();
    B_CLR_ED();
}

/*
 * Make a noise.
 */
VOID
ttbeep()
{
    ttputc(BEL);
    ttflush();
}

/*
 * Insert nchunk blank line(s) onto the
 * screen, scrolling the last line on the
 * screen off the bottom.  Use the scrolling
 * region if possible for a smoother display.
 * If no scrolling region, use a set
 * of insert and delete line sequences
 */
VOID
ttinsl(row, bot, nchunk)
int row, bot, nchunk;
{
    if (row == bot) {		/* Case of one line insert is	*/
	ttmove(row, 0);		/*	special			*/
	tteeol();
	return;
    }
    ttflush();
    B_LOCATE(0, 1+bot-nchunk);
    B_DEL(nchunk);
    B_LOCATE(0, row);
    B_INS(nchunk);
    ttrow = row;
    ttcol = 0;
}

/*
 * Delete nchunk line(s) from "row", replacing the
 * bottom line on the screen with a blank line.
 * Unless we're using the scrolling region, this is
 * done with a crafty sequences of insert and delete
 * lines.  The presence of the echo area makes a
 * boundry condition go away.
 */
VOID
ttdell(row, bot, nchunk)
int row, bot, nchunk;
{
    if (row == bot) {		/* One line special case	*/
	ttmove(row, 0);
	tteeol();
	return;
    }
    ttflush();
    B_LOCATE(0, row);
    B_DEL(nchunk);
    B_LOCATE(0, bot-nchunk);
    B_INS(nchunk);
    ttrow = bot-nchunk;
    ttcol = 0;
}

/*
 * This routine sets the scrolling window
 * on the display to go from line "top" to line
 * "bot" (origin 0, inclusive). The caller checks
 * for the pathalogical 1 line scroll window that
 * doesn't work right, and avoids it. The "ttrow"
 * and "ttcol" variables are set to a crazy value
 * to ensure that the next call to "ttmove" does
 * not turn into a no-op (the window adjustment
 * moves the cursor).
 *
 */
VOID
ttwindow(top, bot)
int top, bot;
{
    /* NOP */
}

/*
 * Switch to full screen scroll. This is
 * used by "spawn.c" just before is suspends the
 * editor, and by "display.c" when it is getting ready
 * to exit.  This function gets to full screen scroll
 * by telling the terminal to set a scrolling regin
 * that is LI or nrow rows high, whichever is larger.
 * This behavior seems to work right on systems
 * where you can set your terminal size.
 */
VOID
ttnowindow()
{
    /* NOP */
}

/*
 * Set the current writing color to the
 * specified color. Watch for color changes that are
 * not going to do anything (the color is already right)
 * and don't send anything to the display.
 * The rainbow version does this in putline.s on a
 * line by line basis, so don't bother sending
 * out the color shift.
 */
VOID
ttcolor(color)
register int color;
{
    if (color != tthue) {
	if (color == CTEXT) {		/* Normal video.	*/
	    ttflush();
	    B_COLOR(3);
	}
	else if (color == CMODE) {	/* Reverse video.	*/
	    ttflush();
	    B_COLOR(11);
	}
	tthue = color;			/* Save the color.	*/
    }
}

/*
 * This routine is called by the
 * "refresh the screen" command to try and resize
 * the display. The new size, which must be deadstopped
 * to not exceed the NROW and NCOL limits, it stored
 * back into "nrow" and "ncol". Display can always deal
 * with a screen NROW by NCOL. Look in "window.c" to
 * see how the caller deals with a change.
 */
VOID
ttresize()
{
    setttysize();			/* found in "ttyio.c",	*/
					/* ask OS for tty size	*/
    if (nrow < 1)			/* Check limits.	*/
	nrow = 1;
    if (ncol < 1)
	ncol = 1;
    vtsetsize(ncol, nrow);		/* found in "display.c" */
}

#ifdef NO_RESIZE
static VOID
setttysize()
{
    nrow = 31;
    ncol = 96;
}
#endif
