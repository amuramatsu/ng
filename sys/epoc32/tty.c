/* $Id: tty.c,v 1.2 2001/11/28 21:45:12 amura Exp $ */
/*
 * Epoc32 Tty display driver
 *
 * Functions in this source file are stub of EpocTty class.
 */

/*
 * $Log: tty.c,v $
 * Revision 1.2  2001/11/28 21:45:12  amura
 * Rewrite to new source code style
 *
 * Revision 1.1  2001/09/30 15:59:12  amura
 * Initial EPOC32 commit.
 *
 * Currently this is not run yet. Some functions around tty are not implemented.
 *
 */

#include "config.h"
#include "def.h"

#define BEL	0x07			/* BEL character.		*/

extern int ttrow;
extern int ttcol;
extern int tttop;
extern int ttbot;
extern int tthue;

#ifdef NO_RESIZE
static void setttysize(void);
#endif

void ttputc(int);

/* These are dummy variables */
int tceeol = 1;			/* Costs are set later */
int tcinsl = 1;
int tcdell = 1;
int SG = -1;/* number of glitches, 0 for invisible, -1 for none	*/

void
ttinit(void)
{
    ttresize();			/* set nrow & ncol	*/
}

/*
 * Clean up the terminal, in anticipation of
 * a return to the command interpreter. This is a no-op
 * on the ANSI display. On the SCALD display, it sets the
 * window back to half screen scrolling. Perhaps it should
 * query the display for the increment, and put it
 * back to what it was.
 */
void
tttidy()
{
}

/*
 * Move the cursor to the specified
 * origin 0 row and column position. Try to
 * optimize out extra moves; redisplay may
 * have left the cursor in the right
 * location last time!
 */
void
ttmove(int row, int col)
{
    epoc_ttmove(row, col);
    ttrow = row;
    ttcol = col;
}

/*
 * Erase to end of line.
 */
void
tteeol(void)
{
    ttflush();
    epoc_tteeol();
}

/*
 * Erase to end of page.
 */
void
tteeop(void)
{
    register int line;
    ttflush();
    epoc_tteeol();
    for (line = ttrow + 1; line <= nrow; ++line) {
	epoc_ttmove(0,line);
	epoc_tteeol();
    }
    ttrow = ttcol = HUGE;
}

/*
 * Make a noise.
 */
void
ttbeep(void)
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
void
ttinsl(int row, int bot, int nchunk)
{
    register int	i, nl;
    
    if (row == bot) {		/* Case of one line insert is	*/
	ttmove(row, 0);		/*	special			*/
	tteeol();
	return;
    }
    ttmove(1+bot-nchunk, 0);
    nl = nrow - ttrow;
    for (i=0; i<nchunk; i++)	/* For all lines in the chunk	*/
	/* putpad(DL, nl)*/ ;
    ttmove(row, 0);
    nl = nrow - ttrow;	/* ttmove() changes ttrow */
    for (i=0; i<nchunk; i++)	/* For all lines in the chunk	*/
	/* putpad(AL, nl) */;
    ttrow = HUGE;
    ttcol = HUGE;
}

/*
 * Delete nchunk line(s) from "row", replacing the
 * bottom line on the screen with a blank line.
 * Unless we're using the scrolling region, this is
 * done with a crafty sequences of insert and delete
 * lines.  The presence of the echo area makes a
 * boundry condition go away.
 */
void
ttdell(int row, int bot, int nchunk)
{
    register int	i, nl;
    
    if (row == bot) {		/* One line special case	*/
	ttmove(row, 0);
	tteeol();
	return;
    }
    ttmove(row, 0);			/* Else use insert/delete line	*/
    nl = nrow - ttrow;
    for (i=0; i<nchunk; i++)	/* For all lines in the chunk	*/
	/* putpad(DL, nl) */;
    ttmove(1+bot-nchunk,0);
    nl = nrow - ttrow;	/* ttmove() changes ttrow */
    for (i=0; i<nchunk; i++)	/* For all lines in the chunk	*/
	/* putpad(AL, nl) */;
    ttrow = HUGE;
    ttcol = HUGE;
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
void
ttwindow(int top, int bot)
{
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
void
ttnowindow(void)
{
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
void
ttcolor(register int color)
{
    if (color != tthue) {
	ttflush();
	epoc_ttattr(color);
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
void
ttresize(void)
{
    setttysize();			/* found in "ttyio.cpp",*/
					/* ask OS for tty size	*/
    if (nrow < 1)			/* Check limits.	*/
	nrow = 1;
    if (ncol < 1)
	ncol = 1;
    vtsetsize(ncol, nrow);		/* found in "display.c" */
}

#ifdef NO_RESIZE
static void
setttysize(void)
{
    nrow = 80;
    ncol = 25;
}
#endif
