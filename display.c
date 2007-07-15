/* $Id: display.c,v 1.20.2.23 2007/07/15 20:31:44 amura Exp $ */
/*
 * The functions in this file handle redisplay. The
 * redisplay system knows almost nothing about the editing
 * process; the editing functions do, however, set some
 * hints to eliminate a lot of the grinding. There is more
 * that can be done; the "vtputc" interface is a real
 * pig. Two conditional compilation flags; the GOSLING
 * flag enables dynamic programming redisplay, using the
 * algorithm published by Jim Gosling in SIGOA. The MEMMAP
 * changes things around for memory mapped video. With
 * both off, the terminal is a VT52.
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "display.h"

#include "i_line.h"
#include "i_buffer.h"
#include "i_lang.h"
#include "i_window.h"
#include "i_tab.h"
#include "in_code.h"
#include "echo.h"
#include "line.h"
#include "kbd.h"
#include "tty.h"
#include "ttyio.h"
#include "lang.h"

/*
 * You can change these back to the types
 * implied by the name if you get tight for space. If you
 * make both of them "int" you get better code on the VAX.
 * They do nothing if this is not Gosling redisplay, except
 * for change the size of a structure that isn't used.
 * A bit of a cheat.
 */
/* These defines really belong in sysdef.h */
#ifndef XCHAR
#  define XCHAR		int
#  define XSHORT	int
#endif

#ifdef STANDOUT_GLITCH
extern int SG;				/* number of standout glitches	*/
#endif /* STANDOUT_GLITCH */
#ifdef GOSLING
# undef GOSLING				/* GOSLING too slow?		*/
#endif

/*
 * A video structure always holds
 * an array of characters whose length is equal to
 * the longest line possible. Only some of this is
 * used if "ncol" isn't the same as "NCOL".
 */
typedef struct {
    short v_hash;			/* Hash code, for compares.	*/
    short v_flag;			/* Flag word.			*/
    short v_color;			/* Color of the line.		*/
    XSHORT v_cost;			/* Cost of display.		*/
    LANG_MODULE *v_lang;		/* Language of this line	*/
#ifndef ZEROARRAY
    NG_WCHAR_t v_text[1];		/* The actual characters.	*/
#else
    NG_WCHAR_t v_text[ZEROARRAY];	/* The actual characters.	*/
#endif
} VIDEO;

#define	MEM_ROUND(n)	(((n)+7)&~7)	/* Memory round bound for 8 bytes*/
#define SIZEOF_VIDEO	MEM_ROUND(sizeof(VIDEO) + vncol*sizeof(NG_WCHAR_t))

#define VFCHG	0x0001			/* Changed.			*/
#define VFHBAD	0x0002			/* Hash and cost are bad.	*/
#define VFEXT	0x0004			/* extended line (beond ncol)	*/

/*
 * SCORE structures hold the optimal
 * trace trajectory, and the cost of redisplay, when
 * the dynamic programming redisplay code is used.
 * If no fancy redisplay, this isn't used. The trace index
 * fields can be "char", and the score a "short", but
 * this makes the code worse on the VAX.
 */
typedef struct {
    XCHAR s_itrace;			/* "i" index for track back.	*/
    XCHAR s_jtrace;			/* "j" index for trace back.	*/
    XSHORT s_cost;			/* Display cost.		*/
} SCORE;

int sgarbf = TRUE;			/* TRUE if screen is garbage.	*/
static int vtrow = 0;			/* Virtual cursor row.		*/
static int vtcol = 0;			/* Virtual cursor column.	*/
int tthue = CNONE;			/* Current color.		*/
int ttrow = HUGE;			/* Physical cursor row.		*/
int ttcol = HUGE;			/* Physical cursor column.	*/
int tttop = HUGE;			/* Top of scroll region.	*/
int ttbot = HUGE;			/* Bottom of scroll region.	*/

static int vncol = 0;
static int vnrow = 0;

static VIDEO **vscreen = NULL;		/* Edge vector, virtual.	*/
static VIDEO **pscreen = NULL;		/* Edge vector, physical.	*/
static VIDEO *video    = NULL;		/* Actual screen data.		*/
static VIDEO *blanks   = NULL;		/* Blank line image.		*/

/*
 * Some predeclerations to make ANSI compilers happy
 */
static VOID vtmove _PRO((int, int));
static VOID vtmarkyen _PRO((NG_WCHAR_ta));
static VOID vteeol _PRO((void));
static VOID ucopy _PRO((VIDEO *, VIDEO *));
static VOID uline _PRO((int, VIDEO *, VIDEO *));
static VOID modeline _PRO((WINDOW *));
static int vtputs _PRO((const NG_WCHAR_t *));
#ifdef ADDFUNC
static int windowpos _PRO((WINDOW *));
static VOID moderatio _PRO((WINDOW *));
#endif

#ifdef GOSLING
/*
 * This matrix is written as an array because
 * we do funny things in the "setscores" routine, which
 * is very compute intensive, to make the subscripts go away.
 * It would be "SCORE	score[vnrow][vnrow]" in old speak.
 * Look at "setscores" to understand what is up.
 */
static VOID hash _PRO((void));
static VOID setscores _PRO((void));
static VOID traceback _PRO((void));

static SCORE *score = NULL;
#endif

VOID
vtsetsize(col, row)
int col, row;
{
    register VIDEO *vp;
    register int i;
    
    if (col<=vncol && row<=vnrow)
	return;
    vncol = col;
    vnrow = row;
    
    if (vscreen != NULL)
	free(vscreen);
    if (pscreen != NULL)
	free(pscreen);
    if (video != NULL)
	free(video);
    if (blanks != NULL)
	free(blanks);
	
    vscreen = malloc(sizeof(VIDEO*)*(vnrow-1));
    pscreen = malloc(sizeof(VIDEO*)*(vnrow-1));
    video   = malloc(SIZEOF_VIDEO*2*(vnrow-1));
    blanks  = malloc(SIZEOF_VIDEO);
    if (vscreen==NULL || pscreen==NULL || video==NULL || blanks==NULL)
	panic("Cannot allocate video buffer");
    bzero(video, SIZEOF_VIDEO*2*(vnrow-1));
    bzero(blanks, SIZEOF_VIDEO);
#ifdef GOSLING
    if (score != NULL)
	free(score);
    score = malloc(sizeof(SCORE)*vnrow*vnrow);
    if (score == NULL)
	panic("Cannot allocate video buffer");
    bzero(score, sizeof(SCORE)*vnrow*vnrow);
#endif
    vp = video;
    for (i=0; i<vnrow-1; ++i) {
	vscreen[i] = (VIDEO*)vp;
	vp->v_lang = terminal_lang;
	vp = (VIDEO*)((char*)vp +SIZEOF_VIDEO);
	pscreen[i] = (VIDEO*)vp;
	vp->v_lang = terminal_lang;
	vp = (VIDEO*)((char*)vp +SIZEOF_VIDEO);
    }
    blanks->v_color = CTEXT;
    blanks->v_lang = terminal_lang;
    for (i=0; i<vncol; ++i)
	blanks->v_text[i] = NG_WSPACE;
    sgarbf = TRUE;
}

/*
 * Initialize the data structures used
 * by the display code. The edge vectors used
 * to access the screens are set up. The operating
 * system's terminal I/O channel is set up. Fill the
 * "blanks" array with ASCII blanks. The rest is done
 * at compile time. The original window is marked
 * as needing full update, and the physical screen
 * is marked as garbage, so all the right stuff happens
 * on the first call to redisplay.
 */
VOID
vtinit()
{
    int col, row;
    ttopen();
    ttinit();
    col = ncol;
    row = nrow;
    if (col < NCOL)
	col = NCOL;
    if (row < NROW)
	row = NROW;
    vtsetsize(col, row);
}

/*
 * Tidy up the virtual display system
 * in anticipation of a return back to the host
 * operating system. Right now all we do is position
 * the cursor to the last line, erase the line, and
 * close the terminal channel.
 */
VOID
vttidy()
{
    ttcolor(CTEXT);
    ttnowindow();				/* No scroll window.	*/
    ttmove(nrow-1, 0);				/* Echo line.		*/
    tteeol();
    tttidy();
    ttflush();
    ttclose();
}

/*
 * Move the virtual cursor to an origin
 * 0 spot on the virtual display screen. I could
 * store the column as a character pointer to the spot
 * on the line, which would make "vtputc" a little bit
 * more efficient. No checking for errors.
 */
static VOID
vtmove(row, col)
int row, col;
{
    vtrow = row;
    vtcol = col;
}

/*
 * Write a character to the virtual display,
 * dealing with long lines and the display of unprintable
 * things like control characters. Also expand tabs every 8
 * columns. This code only puts printing characters into
 * the virtual display image. Special care must be taken when
 * expanding tabs. On a screen whose width is not a multiple
 * of 8, it is possible for the virtual cursor to hit the
 * right margin before the next tab stop is reached. This
 * makes the tab code loop if you are not careful.
 * Three guesses how we found this.
 */
VOID
vtputc(code)
register NG_WCHAR_ta code;
{
    register VIDEO *vp;
    int cwidth;
    register NG_WCHAR_t c = NG_WCODE(code);
    
    /* vtrow sometimes over-runs the vnrow -1.  In the case, vp at
     * following line becomes an uninitialized pointer.  Then core
     * dump or system error may occur.  To avoid the error.  Some
     * range confirmation should be needed. 
     * By Tillanosoft Sep 9, 2000.
     */
    if (vnrow - 1 <= vtrow)
	return;
    
    if (ISTAB(c) && !(curwp->w_bufp->b_flag & BFNOTAB)) {
#ifdef VARIABLE_TAB
	int tab = curwp->w_bufp->b_tabwidth;
#else
	int tab = 8;
#endif
	vp = vscreen[vtrow];
	if (tabnext(vtcol,tab) < ncol) {
	    do {
		vtputc(NG_WSPACE);
	    } while (vtcol<ncol && (vtcol%tab)!=0);
	}
	else {
	    vteeol();
	    vp->v_text[ncol-1] = NG_WBACKSL;
	    vtrow++;
	    vtcol = 0;
	}
	return;
    }
    vp = vscreen[vtrow];
    vp->v_lang = curwp->w_bufp->b_lang;
    cwidth = vp->v_lang->lm_width(c);
    if (vtcol+cwidth >= ncol) {
	vtmarkyen(NG_WBACKSL);
	vtrow++;
        vp = vscreen[vtrow];
        vp->v_lang = curwp->w_bufp->b_lang;
	vtcol = 0;
    }
    vp->v_lang->lm_displaychar(vp->v_text, vtcol, ncol, c);
    vtcol += cwidth;
}

#if defined(MEMMAP) && !defined(HAVE_ORIGINAL_PUTLINE)
VOID
putline(row, s, color, lang)
int row;
const NG_WCHAR_t *s;
int color;
LANG_MODULE *lang;
{
    char buf[NG_CODE_MAXLEN];
    int len;
    register int i;
    register int j;
    int oldrow = vtrow;
    int oldcol = vtcol;
    short disp_code;

    disp_code = NG_CODE_PASCII;
    if (vvp->v_lang == terminal_lang || IS_NG_CODE_GLOBAL(disp_code))
	disp_code = terminal_lang->lm_get_code(NG_CODE_FOR_DISPLAY);
    
    vtrow = row;
    vtcol = 0;
    ttmove(vtrow, 0);
    ttcolor(color);
    len = lang->lm_get_display_code(disp_code, NG_WSTART, buf, sizeof(buf));
    for (j = 0; j < len; j++)
	ttputc(buf[i]);
    for (i=0; i<ncol; i++, s++) {
	len = lang->lm_get_display_code(disp_code, *s, buf, sizeof(buf));
	for (j = 0; j < len; j++)
	    ttputc(buf[j]);
    }
    len = lang->lm_get_display_code(disp_code, NG_WFINISH, buf, sizeof(buf));
    for (j = 0; j < len; j++)
	ttputc(buf[j]);
    
    vtrow = oldrow;
    vtcol = oldcol;
    ttmove(vtrow, vtcol);
#ifndef MOVE_STANDOUT
    if (color != CTEXT)
	ttcolor(CTEXT);
#endif
    ttflush();
}
#endif /* MEMMAP && !HAVE_ORIGINAL_PUTLINE */

/* Mark '\\' end of line 
 * whether curcol is not on the top of line.
 */
static VOID
vtmarkyen(fillchar)
NG_WCHAR_ta fillchar;
{
    register VIDEO *vp;

    vp = vscreen[vtrow];
    if (vtcol > 0) {
	while (vtcol < ncol) {
	    vp->v_text[vtcol++] = fillchar;
	}
    }
}

/* Erase from the end of the
 * software cursor to the end of the
 * line on which the software cursor is
 * located. The display routines will decide
 * if a hardware erase to end of line command
 * should be used to display this.
 */
static VOID
vteeol()
{
    register VIDEO *vp;
    
    vp = vscreen[vtrow];
    /* clear fillers */
    if (vp->v_text[vtcol] == NG_WFILLER) {
	int i = vtcol - 1;
	while (i > 0 && vp->v_text[i] == NG_WFILLER)
	    vp->v_text[i--] = NG_WSPACE;
	vp->v_text[i] = NG_WSPACE; /* kill first char */
    }
    while (vtcol < ncol)
	vp->v_text[vtcol++] = NG_WSPACE;
}

/* Calculate offset to col and row
*/
int
colrow(lp, offset_, curcol, lines)
const LINE *lp;
LINE_OFF_ta offset_;
int *curcol;
int *lines;
{
    register int i;
    register NG_WCHAR_t c;
    register int n;
    LINE_OFF_t offset = (LINE_OFF_t)offset_;
#ifdef VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif

    *curcol = 0;
    *lines = 0;
    for (i=0; i<offset; ++i) {
	c = lgetc(lp, i);
	if (ISTAB(c) && !(curbp->b_flag & BFNOTAB)) {
	    *curcol = tabnext(*curcol, tab);
	    if (*curcol >= ncol) {
		*curcol = 0;
		(*lines)++;
	    }
	}
	else {
	    n = curbp->b_lang->lm_width(c);
	    if (*curcol+n >= ncol
#ifdef NOWRAPMODE
		&& !(curbp->b_flag & BFNOWRAP)
#endif
		) {
		*curcol = n;
		++(*lines);
	    }
	    else
		*curcol += n;
	}
    }
    return *lines;
}

/*
 * Returns offset of specified location.  This will be mainly used to
 * respond to a mouse click on a screen.  In case the specified
 * location does not exist within the `lp', negative value will be
 * returned.  The absolute value of the negative result is the number
 * of rows left.
 *
 * This will return an offset right neighbor to the position, if the
 * character at the position is a wide character ,that is, the width
 * of the character is more than or equal to a double width character,
 * and the position is at the right half of the character.
 *
 * By Tillanosoft, March 21, 1999.
 */
LINE_OFF_t
colrow2offset(lp, col, row)
LINE *lp;
int col, row;
{
    register int i;
    register int curcol;
    register NG_WCHAR_t c;
    register int width;
    int (*lm_width)(NG_WCHAR_ta) = curbp->b_lang->lm_width;
#ifdef VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif /* VARIABLE_TAB */
  
    curcol = 0;
    for (i=0; i<llength(lp); i++) {
	if (row < 0 || (row == 0 && col < curcol))
	    return i;
	c = lgetc(lp, i);
	if (c == '\t' && !(curbp->b_flag & BFNOTAB) ) {
#ifdef VARIABLE_TAB
	    width = (curcol/tab + 1)*tab - curcol;
#else
	    width = (curcol | 0x07) +1 - curcol;
#endif
	    /*
	     * I should pay more care for the above line.
 	     * In case, wrapping arround occurs, where should I put the curcol
	     * in the next line?
	     * By Tillanosoft Mar 21, 1999
	     */
	}
	else
	    width = lm_width(c);
	if (row == 0 && col < curcol + (width+1)/2)
	    return i;
	curcol += width;
	if (curcol >= ncol-width
#ifdef NOLINEWRAP
	    && !(curbp->b_flag & BFNOWRAP)
#endif
	    ) {
	    curcol = curcol - ncol + width;
	    --row;
	}
    }
    if (row <= 0)
	return i;
    return -row;
}

/* Return offset of #th lines
 */
LINE_NO_t
skipline(lp, lines)
const LINE *lp;
int lines;
{
    register int i, n;
    register int curcol;
    register NG_WCHAR_t c;
    int (*lm_width)(NG_WCHAR_ta) = curbp->b_lang->lm_width;
#ifdef VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif /* VARIABLE_TAB */

    curcol = 0;
    for (i=0; i<llength(lp); ++i) {
	if (lines == 0)
	    return i;
	c = lgetc(lp, i);
	if (ISTAB(c) && !(curbp->b_flag & BFNOTAB))
	    n = tabnext(curcol, tab) - curcol;
	else
	    n = lm_width(c);

	if (curcol+n >= ncol
#ifdef NOWRAPMODE
	    && !(curbp->b_flag & BFNOWRAP)
#endif
	    ) {
	    curcol = n;
	    --lines;
	}
	else
	    curcol += n;
    }
    if (lines == 0)
	return i;
    ewprintf("Bug: skipline %d lines left",lines);
    return FALSE;
}

/* Count number of displayed lines on tty.
 * the line which is longer than ncol
 * returns value more than 2 lines
 */
int
countlines(lp)
const LINE *lp;
{
    register int i, n;
    register int curcol;
    register int lines;
    register NG_WCHAR_t c;
    int (*lm_width)(NG_WCHAR_ta) = curbp->b_lang->lm_width;
#ifdef VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif /* VARIABLE_TAB */

    curcol = 0;
    lines = 0;
    for (i=0; i<llength(lp); ++i) {
	c = lgetc(lp, i);
	if (ISTAB(c) && !(curbp->b_flag & BFNOTAB))
	    n = tabnext(curcol, tab) - curcol;
	else
	    n = lm_width(c);
	if (curcol+n >= ncol
#ifdef NOWRAPMODE
	    && !(curbp->b_flag & BFNOWRAP)
#endif
	    ) {
	    curcol = 0;
	    lines++;
	}
	else
	    curcol += n;
    }
    return lines+1;
}

/*
 * Make sure that the display is
 * right. This is a three part process. First,
 * scan through all of the windows looking for dirty
 * ones. Check the framing, and refresh the screen.
 * Second, make sure that "currow" and "curcol" are
 * correct for the current window. Third, make the
 * virtual and physical screens the same.
 */
/* Changed 98/01/23 by amura for Variable Tab handling */
VOID
update()
{
    register LINE *lp;
    WINDOW *old_curwp;
    register WINDOW *wp;
    register VIDEO *vp1;
    VIDEO *vp2;
    register int i, j;
    register int hflag;
    register int currow;
    register int curcol;
#ifdef GOSLING
    register int offs;
    register int size;
#endif
    int	x,y;
    int	lines;
    /* 90.01.29  by S.Yoshida */
    
    if (typeahead() || kgetkey_continued())
	return;
    if (sgarbf) {			/* must update everything */
	wp = wheadp;
	while(wp != NULL) {
	    wp->w_flag |= WFMODE | WFHARD;
	    wp = wp->w_wndp;
	}
	/* 90.01.29  by S.Yoshida */
	kgetkey_flush();	/* Reset multibyte input condition. */
    } 
    
    old_curwp = curwp;
    hflag = FALSE;			/* Not hard.		*/
    wp = wheadp;
    while (wp != NULL) {
	if (wp == curwp && wp->w_flag == 0) {
	    lp = wp->w_linep;		/* Cursor location.	*/
	    i = -wp->w_lines;
	    while (lp != wp->w_dotp) {
		i += countlines(lp);
		lp = lforw(lp);
	    }
	    i += colrow(lp, wp->w_doto, &x, &y);
	    if (i < 0 || i >= wp->w_ntrows)
		wp->w_flag |= WFFORCE;
	}
	if (wp->w_flag != 0) {		/* Need update.		*/
	    if (wp->w_dotlines != countlines(wp->w_dotp))
		wp->w_flag |= WFHARD;
	    if ((wp->w_flag&WFFORCE) == 0) {
		lp = wp->w_linep;
		for (i = -wp->w_lines; i < wp->w_ntrows &&
			lp != wp->w_bufp->b_linep; ) {
		    if (lp == wp->w_dotp) {
			i += colrow(lp, wp->w_doto, &x, &y);
			if (i>=0 && i<wp->w_ntrows)
			    goto out;
			else
			    break;
		    }
		    i += countlines(lp);
		    lp = lforw(lp);
		}
	    }
	    i = wp->w_force;		/* Reframe this one.	*/
	    if (i > 0) {
		--i;
		if (i >= wp->w_ntrows)
		    i = wp->w_ntrows-1;
	    }
	    else if (i < 0) {
		i += wp->w_ntrows;
		if (i < 0)
		    i = 0;
	    }
	    else
		i = wp->w_ntrows/2;
	    lp = wp->w_dotp;
	    i -= colrow(lp, wp->w_doto, &x, &y);
	    while (i>0 && lback(lp)!=wp->w_bufp->b_linep) {
		lp = lback(lp);
		i -= countlines(lp);
	    }
	    if (i>0)
		i=0;
	    if (i<0)
		i = -i;
	    wp->w_linep = lp;
	    wp->w_lines = i;
	    wp->w_flag |= WFHARD;	/* Force full.		*/
out:
	    lp = wp->w_linep;		/* Try reduced update.	*/
	    i  = wp->w_toprow - wp->w_lines;
	    if ((wp->w_flag&~WFMODE) == WFEDIT) {
		while (lp != wp->w_dotp) {
		    i += countlines(lp);
		    lp = lforw(lp);
		}
		j = i + countlines(lp);
		if (j > wp->w_toprow + wp->w_ntrows) {
		    y = skipline(lp, wp->w_toprow + wp->w_ntrows - i);
		    j = wp->w_toprow + wp->w_ntrows;
		}
		else
		    y = llength(lp);
		if (i < wp->w_toprow) {
		    x = skipline(lp, wp->w_toprow - i);
		    i = wp->w_toprow;
		}
		else
		    x = 0;
		for (lines=i; lines<j; lines++) {
		    vscreen[lines]->v_color = CTEXT;
		    vscreen[lines]->v_lang = wp->w_bufp->b_lang;
		    vscreen[lines]->v_flag |= (VFCHG|VFHBAD);
		}
#ifdef NOWRAPMODE
		if (wp->w_bufp->b_flag & BFNOWRAP) {
		    curwp = wp;		/* for variable tab */
		    updext(i, wp->w_doto);
		    curwp = old_curwp;
		}
		else
#endif
		{
		    vtmove(i, 0);
		    curwp = wp;		/* for variable tab */
		    for (j=x; j<y; ++j)
			vtputc(lgetc(lp, j));
		    curwp = old_curwp;
		    if (y < llength(lp))
			vtmarkyen(NG_WBACKSL);
		    else
			vteeol();
		}
	    }
	    else if ((wp->w_flag&(WFEDIT|WFHARD)) != 0) {
		hflag = TRUE;
		while (i < wp->w_toprow + wp->w_ntrows) {
		    if (lp == wp->w_bufp->b_linep) {
			vtmove(i, 0);
			vscreen[i]->v_color =CTEXT;
			vscreen[i]->v_lang = wp->w_bufp->b_lang;
			vscreen[i]->v_flag |= (VFCHG|VFHBAD);
			vteeol();
			i++;
			continue;
		    }
		    j = i + countlines(lp);
		    if (j > wp->w_toprow + wp->w_ntrows){
			y = skipline(lp, wp->w_toprow + wp->w_ntrows - i);
			j = wp->w_toprow + wp->w_ntrows;
		    }
		    else
			y = llength(lp);
		    if (i < wp->w_toprow) {
			x = skipline(lp, wp->w_toprow - i);
			i = wp->w_toprow;
		    }
		    else
			x = 0;
		    for (lines=i; lines<j; lines++) {
			vscreen[lines]->v_color =CTEXT;
			vscreen[lines]->v_lang = wp->w_bufp->b_lang;
			vscreen[lines]->v_flag |= (VFCHG|VFHBAD);
		    }
#ifdef NOWRAPMODE
		    if (wp->w_bufp->b_flag & BFNOWRAP) {
			curwp = wp;		/* for variable tab */
			updext(i, wp->w_doto);
			curwp = old_curwp;
		    }
		    else
#endif
		    {
			vtmove(i, 0);
			curwp = wp;		/* for variable tab */
			for (j=x; j<y; ++j)
			    vtputc(lgetc(lp, j));
			curwp = old_curwp;
			if (y < llength(lp))
			    vtmarkyen(NG_WBACKSL);
			else
			    vteeol();
		    }
		    i = lines;
		    lp = lforw(lp);
		}
		/* Because vtputc clear modeline, rewrite it */
		if (vtrow >= wp->w_toprow + wp->w_ntrows)
		    wp->w_flag |= WFMODE;
	    }
	    if ((wp->w_flag&WFMODE) != 0)
		modeline(wp);
#ifdef ADDFUNC
	    else if ((wp->w_flag & WFHARD) != 0)
		/* to display the ratio in mode line */
		moderatio(wp);
#endif
	    wp->w_flag  = 0;
	    wp->w_force = 0;
	    wp->w_dotlines = countlines(wp->w_dotp);
	    }
	wp = wp->w_wndp;
    }
    lp = curwp->w_linep;			/* Cursor location.	*/
    currow = curwp->w_toprow - curwp->w_lines;
    while (lp != curwp->w_dotp) {
	currow += countlines(lp);
	lp = lforw(lp);
    }
    currow += colrow(lp, curwp->w_doto, &x, &y);
    curcol = x;
    /* make sure no lines need to be de-extended because the cursor is
	no longer on them */

    wp = wheadp;

    while (wp != NULL) {
        /* if garbaged then fix up mode lines */
	if (sgarbf != FALSE) {
	    vscreen[wp->w_toprow+wp->w_ntrows]->v_lang = wp->w_bufp->b_lang;
	    vscreen[wp->w_toprow+wp->w_ntrows]->v_flag |= VFCHG;
	}
	/* and onward to the next window */
	wp = wp->w_wndp;
    }

    if (sgarbf != FALSE) {		/* Screen is garbage.	*/
	sgarbf = FALSE;			/* Erase-page clears	*/
	epresf = FALSE;			/* the message area.	*/
	tttop  = HUGE;			/* Forget where you set */
	ttbot  = HUGE;			/* scroll region.	*/
	tthue  = CNONE;			/* Color unknown.	*/
	ttmove(0, 0);
	tteeop();
	for (i=0; i<nrow-1; ++i) {
	    uline(i, vscreen[i], blanks);
	    ucopy(vscreen[i], pscreen[i]);
	}
	ttmove(currow, curcol);
	ttflush();
#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
	ttykeypadstart();
#endif /* XKEYS */
	return;
    }
#ifdef GOSLING
    if (hflag != FALSE) {		/* Hard update?		*/
	for (i=0; i<nrow-1; ++i) {	/* Compute hash data.	*/
	    hash(vscreen[i]);
	    hash(pscreen[i]);
	}
	offs = 0;			/* Get top match.	*/
	while (offs != nrow-1) {
	    vp1 = vscreen[offs];
	    vp2 = pscreen[offs];
	    if (vp1->v_color != vp2->v_color ||  vp1->v_hash != vp2->v_hash)
		break;
	    uline(offs, vp1, vp2);
	    ucopy(vp1, vp2);
	    ++offs;
	}
	if (offs == nrow-1) {		/* Might get it all.	*/
	    ttmove(currow, curcol);
	    ttflush();
	    return;
	}
	size = nrow-1;			/* Get bottom match.	*/
	while (size != offs) {
	    vp1 = vscreen[size-1];
	    vp2 = pscreen[size-1];
	    if (vp1->v_color != vp2->v_color ||  vp1->v_hash != vp2->v_hash)
		break;
	    uline(size-1, vp1, vp2);
	    ucopy(vp1, vp2);
	    --size;
	}
	if ((size -= offs) == 0)	/* Get screen size.	*/
	    panic("Illegal screen size in update");
	setscores(offs, size);		/* Do hard update.	*/
	traceback(offs, size, size, size);
	for (i=0; i<size; ++i)
	    ucopy(vscreen[offs+i], pscreen[offs+i]);
	ttmove(currow, curcol);
	ttflush();
	return;
    }
#endif
    for (i=0; i<nrow-1; ++i) {		/* Easy update.		*/
	vp1 = vscreen[i];
	vp2 = pscreen[i];
	if ((vp1->v_flag&VFCHG) != 0) {
	    uline(i, vp1, vp2);
	    ucopy(vp1, vp2);
	}
    }
    ttmove(currow, curcol);
    ttflush();
}

/*
 * Update a saved copy of a line,
 * kept in a VIDEO structure. The "vvp" is
 * the one in the "vscreen". The "pvp" is the one
 * in the "pscreen". This is called to make the
 * virtual and physical screens the same when
 * display has done an update.
 */
static VOID
ucopy(vvp, pvp)
register VIDEO *vvp;
register VIDEO *pvp;
{
    vvp->v_flag &= ~VFCHG;			/* Changes done.	*/
    pvp->v_flag  = vvp->v_flag;			/* Update model.	*/
    pvp->v_hash  = vvp->v_hash;
    pvp->v_cost  = vvp->v_cost;
    pvp->v_color = vvp->v_color;
    bcopy(vvp->v_text, pvp->v_text, vncol*sizeof(NG_WCHAR_t));
}

#ifdef NOWRAPMODE
static VOID
vtpute(c, lbound)
int c;
int lbound;
{
    register VIDEO *vp;
    int cwidth;
#ifdef VARIABLE_TAB
    int tab = curwp->w_bufp->b_tabwidth;
#endif

    vp = vscreen[vtrow];
    vp->v_lang = curwp->w_bufp->b_lang;
    if (vtcol >= ncol)
	return;
    else if (ISTAB(c) && !(curbp->b_flag & BFNOTAB)) {
#ifdef VARIABLE_TAB
	int tab = curwp->w_bufp->b_tabwidth;
#else
	int tab = 8;
#endif
	vp = vscreen[vtrow];
	if (tabnext(vtcol, tab) <= ncol) {
	    do {
		vtpute(NG_WSPACE);
	    } while (vtcol<ncol && ((vtcol+lbound)%tab)!=0);
	}
	return;
    }
    else if (vtcol >= ncol)
	vp->v_text[ncol-1] = NG_WCODE('$');

    cwidth = vp->v_lang->lm_width(c);
    if (vtcol < 0) {
	if ((vtcol+cwidth) > 1) {
	    for (i=0; i<vtcol+cwidth; i++)
		vp->v_text[i] = NG_WCODE('$');
	}
	vtcol += cwidth;
    }
    vp = vscreen[vtrow];
    vp->v_lang->lm_displaychar(vp->v_text, vtcol, ncol, c);
    vtcol += cwidth;
}

/* updext: update the extended line which the cursor is currently
 * on at a column greater than the terminal width. The line
 * will be scrolled right or left to let the user see where
 * the cursor is
 */
static VOID
updext(currow, curcol)
int currow, curcol;
{
    register LINE *lp;			/* pointer to current line */
    register int j;			/* index into line */
    int lbound;				/* window left char offset */

    /* calculate what column the left bound should be */
    /* (force cursor into middle half of screen) */
    lbound = curcol - (curcol % (ncol>>1)) - (ncol>>2);
    if (lbound < 0)
	lbound = 0;
    /* scan through the line outputing characters to the virtual screen */
    /* once we reach the left edge */
    vtmove(currow, -lbound);			/* start scanning offscreen */
    lp = curwp->w_dotp;				/* line to output */
    for (j=0; j<llength(lp); ++j)		/* until the end-of-line */
	vtpute(lgetc(lp, j), lbound);
    vteeol();					/* truncate the virtual line */
    if (vscreen[currow]->v_text[ncol - 2] == NG_WFILLER) {
	j = ncol - 2;
	while (vscreen[currow]->v_text[j] == NG_WFILLER)
	    vscreen[currow]->v_text[j--] = NG_WCODE('$');
	vscreen[currow]->v_text[j] = NG_WCODE('$');
    }
}
#endif /* NOWRAPMODE */

/*
 * Update a single line. This routine only
 * uses basic functionality (no insert and delete character,
 * but erase to end of line). The "vvp" points at the VIDEO
 * structure for the line on the virtual screen, and the "pvp"
 * is the same for the physical screen. Avoid erase to end of
 * line when updating CMODE color lines, because of the way that
 * reverse video works on most terminals.
 */
static VOID
uline(row, vvp, pvp)
int row;
VIDEO *vvp, *pvp;
{
#ifdef MEMMAP
    ttflush();	/* 90.06.09  by A.Shirahashi */
    putline(row, &vvp->v_text[0], vvp->v_color, vvp->v_lang);
#else  /* not MEMMAP */
    char buf[NG_CODE_MAXLEN];
    int len;
    register int i;
    register NG_WCHAR_t *cp1;
    register NG_WCHAR_t *cp2;
    register NG_WCHAR_t *cp3;
    NG_WCHAR_t *cp4;
    NG_WCHAR_t *cp5;
    register int nbflag;
    int w;
    short disp_code = NG_CODE_PASCII;
    LANG_MODULE *lang = vvp->v_lang;
    if (vvp->v_lang == terminal_lang || IS_NG_CODE_GLOBAL(disp_code))
	disp_code = terminal_lang->lm_get_code(NG_CODE_FOR_DISPLAY);
    
    if (vvp->v_color != pvp->v_color) {	/* Wrong color, do a	*/
	ttmove(row, 0);			/* full redraw.		*/
#ifdef	STANDOUT_GLITCH
	if (pvp->v_color != CTEXT && SG >= 0)
	    tteeol();
#endif
	ttcolor(vvp->v_color);
	len = lang->lm_get_display_code(disp_code, NG_WSTART, buf, sizeof(buf));
	for (i = 0; i < len; i++)
	    ttputc(buf[i]);
#ifdef	STANDOUT_GLITCH
	cp1 = &vvp->v_text[SG > 0 ? SG : 0];
	/* the odd code for SG==0 is to avoid putting the invisable
	 * glitch character on the next line.
	 * (Hazeltine executive 80 model 30)
	 */
	cp2 = &vvp->v_text[ncol - (SG >= 0 ? (SG!=0 ? SG : 1) : 0)];
#else	/* not STANDOUT_GLITCH */
	cp1 = &vvp->v_text[0];
	cp2 = &vvp->v_text[ncol];
#endif	/* STANDOUT_GLITCH */
	while (cp1 < cp2) {
	    if (*cp1 != NG_WFILLER) {
		len = lang->lm_get_display_code(disp_code, *cp1,
						buf, sizeof(buf));
		for (i = 0; i < len; i++)
		    ttputc(buf[i]);
		w = lang->lm_width(*cp1);
		ttcol += w;
		cp1 += w;
	    }
	}
	len = lang->lm_get_display_code(disp_code, NG_WFINISH,
					buf, sizeof(buf));
	for (i = 0; i < len; i++)
	    ttputc(buf[i]);
#ifndef MOVE_STANDOUT
	ttcolor(CTEXT);
#endif
	ttflush(); /* 90.06.09  by A.Shirahashi */
	return;
    }
    cp1 = &vvp->v_text[0];			/* Compute left match.	*/
    cp2 = &pvp->v_text[0];
    while (cp1 != &vvp->v_text[ncol] && *cp1 == *cp2) {
	++cp1;
	++cp2;
    }
    if (cp1 == &vvp->v_text[ncol]) {	/* All equal.		*/
	ttflush();
	return;
    }
    nbflag = FALSE;
    cp3 = &vvp->v_text[ncol];		/* Compute right match. */
    cp4 = &pvp->v_text[ncol];
    
    while (cp3[-1] == cp4[-1]) {
	--cp3;
	--cp4;
	if (cp3[0] != NG_WSPACE)	/* Note non-blanks in	*/
	    nbflag = TRUE;		/* the right match.	*/
    }
    cp5 = cp3;				/* Is erase good?	*/
    if (nbflag == FALSE && vvp->v_color == CTEXT) {
	while (cp5!=cp1 && cp5[-1]==NG_WSPACE)
	    --cp5;
	/* Alcyon hack */
	if ((int)(cp3-cp5) <= tceeol)
	    cp5 = cp3;
    }
    /* Alcyon hack */
    ttmove(row, (int)(cp1 - &vvp->v_text[0]));
#ifdef STANDOUT_GLITCH
    if (vvp->v_color != CTEXT && SG > 0) {
	if (cp1 < &vvp->v_text[SG])
	    cp1 = &vvp->v_text[SG];
	if (cp5 > &vvp->v_text[ncol-SG])
	    cp5 = &vvp->v_text[ncol-SG];
    }
    else if (SG < 0)
#endif /* STANDOUT_GLITCH */
	ttcolor(vvp->v_color);
    len = lang->lm_get_display_code(disp_code, NG_WSTART, buf, sizeof(buf));
    for (i = 0; i < len; i++)
	ttputc(buf[i]);
    while (cp1 < cp5) {
	if (*cp1 != NG_WFILLER) {
	    len = lang->lm_get_display_code(disp_code, *cp1, buf, sizeof(buf));
	    for (i = 0; i < len; i++)
		ttputc(buf[i]);
	    w = terminal_lang->lm_width(*cp1);
	    ttcol += w;
	    cp1 += w;
	}
    }
    len = lang->lm_get_display_code(disp_code, NG_WFINISH, buf, sizeof(buf));
    for (i = 0; i < len; i++)
	ttputc(buf[i]);
    if (cp5 != cp3)				/* Do erase.		*/
	tteeol();
#ifndef MOVE_STANDOUT	/* 90.03.21  by S.Yoshida */
    ttcolor(CTEXT);
#endif
    ttflush(); /* 90.06.09  by A.Shirahashi */
#endif  /* MEMMAP */
}

/*
 * Redisplay the mode line for
 * the window pointed to by the "wp".
 * This is the only routine that has any idea
 * of how the modeline is formatted. You can
 * change the modeline format by hacking at
 * this routine. Called by "update" any time
 * there is a dirty window.
 * Note that if STANDOUT_GLITCH is defined, first and last SG characters
 * may never be seen.
 */
static VOID
modeline(wp)
register WINDOW *wp;
{
    register int n;
    register BUFFER *bp;
    int mode;
#ifdef CANNA
    extern NG_WCHAR_t currentMode[];
    extern NG_WCHAR_t origMode[];
#endif

    n = wp->w_toprow+wp->w_ntrows;		/* Location.		*/
    vscreen[n]->v_color = CMODE;		/* Mode line color.	*/
    vscreen[n]->v_flag |= (VFCHG|VFHBAD);	/* Recompute, display.	*/
    vtmove(n, 0);				/* Seek to right line.	*/
    bp = wp->w_bufp;
    n = 0;
#ifdef CANNA
    if (bp->b_flag & BFCANNA) {
	if (bp == curbp)
	    n = vtputs(currentMode);
	else
	    n = vtputs(origMode);
    }
    else
	n = vtputs(_NG_WSTR("[ -- ]"));
#endif
    vtputc('-'); vtputc('-');
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if ((bp->b_flag&BFRONLY) != 0) {	/* "%" if read-only.	*/
	vtputc('%'); vtputc('%');
    }
    else
#endif /* READONLY */
    if ((bp->b_flag&BFCHG) != 0) {		/* "*" if changed.	*/
	vtputc('*'); vtputc('*');
    }
    else {
	vtputc('-'); vtputc('-');
    }
    vtputc('-');
    n += 5;
    n += vtputs(_NG_WSTR("Ng: "));
    if (bp->b_bname[0] != NG_EOS)
	n += vtputs(bp->b_bname);
    while (n < 30/*42*/) {			/* Pad out with blanks	*/
	vtputc(' ');
	++n;
    }
    vtputc('(');
    ++n;
    /* 90.01.29  by S.Yoshida */
    /* 90.12.28  Move to here like as Nemacs 3.3. by S.Yoshida */
    /* n += kdispbufcode(bp); XXX */
    for (mode=0;;) {
	NG_WCHAR_t wbuf[NLINE];
	wstrlcpya(wbuf, bp->b_modes[mode]->p_name, NG_WCHARLEN(wbuf));
	n += vtputs(wbuf);
	if (++mode > bp->b_nmodes)
	    break;
	vtputc('-');
	++n;
    }
    vtputc(')');
#ifdef ADDFUNC
    vtputc('-');
    vtputc('-');
    moderatio(wp);
#endif
    vtmarkyen('-');				/* Pad out.		*/
}

/*
 * Redisplay the mode line  "raito" ONLY for
 * the window pointed to by the "wp".
 * This is the only routine that has any idea
 * of how the modeline is formatted. You can
 * change the modeline format by hacking at
 * this routine. Called by "update" any time
 * there is a dirty window.
 * Note that if STANDOUT_GLITCH is defined, first and last SG characters
 * may never be seen.
 */
static VOID
moderatio(wp)
register WINDOW *wp;
{
    register int n;
    register BUFFER *bp;
    int mode, l;
    extern int line_number_mode;		/* Defined in basic.c	*/

    l = wp->w_toprow+wp->w_ntrows;		/* Location.		*/
    vscreen[l]->v_color = CMODE;		/* Mode line color.	*/
    vscreen[l]->v_flag |= (VFCHG|VFHBAD);	/* Recompute, display.	*/
    bp = wp->w_bufp;
#ifdef CANNA
    n  = 46 + 6;
#else
    n  = 46;
#endif
    /* 90.12.28  Move to here like as Nemacs 3.3. by S.Yoshida */
#if 0 /* XXX */
    n += 4;					/* Show code.	*/
#endif
    for (mode=0;;) {	/* skip mode names */
	n += strlen(bp->b_modes[mode]->p_name);
	if (++mode > bp->b_nmodes)
	    break;
	++n;
    }
    vtmove(l, n);				/* Seek to right line.	*/
    if (line_number_mode) {
	NG_WCHAR_t linestr[NINPUT];	/* XXX now, support only 32bit int */
	wsnprintf(linestr, NG_WCHARLEN(linestr),
		  "L%d", get_lineno(bp, wp->w_dotp)+1);
	vtputs(linestr);
    }
    {
	int ratio = windowpos(wp);
	
	vtputc('-'); vtputc('-');
	switch (ratio) {
	case MG_RATIO_ALL:
	    vtputs(_NG_WSTR("All"));
	    break;
	    
	case MG_RATIO_TOP:
	    vtputs(_NG_WSTR("Top"));
	    break;
	  
	case MG_RATIO_BOT:
	    vtputs(_NG_WSTR("Bot"));
	    break;
	
	default:
	    if ((ratio / 10) == 0) {
		vtputc(' ');
	    }
	    else {
		vtputc('0' + (ratio / 10));
	    }
	    vtputc('0' + (ratio % 10));
	    vtputc('%');
	}
	vtputc('-');vtputc('-');
    }
}

/*
 * output a string to the mode line, report how long it was.
 */
static int
vtputs(s)
register const NG_WCHAR_t *s;
{
    int bvtrow = vtrow;
    while (*s != '\0')
	vtputc(*s++);
    return vtrow - bvtrow;
}

#ifdef GOSLING
/*
 * Compute the hash code for
 * the line pointed to by the "vp". Recompute
 * it if necessary. Also set the approximate redisplay
 * cost. The validity of the hash code is marked by
 * a flag bit. The cost understand the advantages
 * of erase to end of line. Tuned for the VAX
 * by Bob McNamara; better than it used to be on
 * just about any machine.
 */
static VOID
hash(vp)
register VIDEO *vp;
{
    register int i;
    register int n;
    register char *s;

    if ((vp->v_flag&VFHBAD) != 0) {		/* Hash bad.		*/
	s = &vp->v_text[ncol-1];
	for (i=ncol; i!=0; --i, --s)
	    if (*s != ' ')
		break;
	n = ncol-i;				/* Erase cheaper?	*/
	if (n > tceeol)
	    n = tceeol;
	vp->v_cost = i+n;			/* Bytes + blanks.	*/
	for (n=0; i!=0; --i, --s)
	    n = (n<<5) + n + *s;
	vp->v_hash = n;				/* Hash code.		*/
	vp->v_flag &= ~VFHBAD;			/* Flag as all done.	*/
    }
}

/*
 * Compute the Insert-Delete
 * cost matrix. The dynamic programming algorithm
 * described by James Gosling is used. This code assumes
 * that the line above the echo line is the last line involved
 * in the scroll region. This is easy to arrange on the VT100
 * because of the scrolling region. The "offs" is the origin 0
 * offset of the first row in the virtual/physical screen that
 * is being updated; the "size" is the length of the chunk of
 * screen being updated. For a full screen update, use offs=0
 * and size=nrow-1.
 *
 * Older versions of this code implemented the score matrix by
 * a two dimensional array of SCORE nodes. This put all kinds of
 * multiply instructions in the code! This version is written to
 * use a linear array and pointers, and contains no multiplication
 * at all. The code has been carefully looked at on the VAX, with
 * only marginal checking on other machines for efficiency. In
 * fact, this has been tuned twice! Bob McNamara tuned it even
 * more for the VAX, which is a big issue for him because of
 * the 66 line X displays.
 *
 * On some machines, replacing the "for (i=1; i<=size; ++i)" with
 * i = 1; do { } while (++i <=size)" will make the code quite a
 * bit better; but it looks ugly.
 */
static VOID
setscores(offs, size)
int offs, size;
{
    register SCORE *sp;
    SCORE *sp1;
    register int tempcost;
    register int bestcost;
    register int i, j;
    register VIDEO **vp;
    VIDEO **pp, **vbase, **pbase;
    
    vbase = &vscreen[offs-1];			/* By hand CSE's.	*/
    pbase = &pscreen[offs-1];
    score[0].s_itrace = 0;			/* [0, 0]		*/
    score[0].s_jtrace = 0;
    score[0].s_cost = 0;
    sp = &score[1];				/* Row 0, inserts.	*/
    tempcost = 0;
    vp = &vbase[1];
    for (j=1; j<=size; ++j) {
	sp->s_itrace = 0;
	sp->s_jtrace = j-1;
	tempcost += tcinsl;
	tempcost += (*vp)->v_cost;
	sp->s_cost = tempcost;
	++vp;
	++sp;
    }
    sp = &score[vnrow];				/* Column 0, deletes.	*/
    tempcost = 0;
    for (i=1; i<=size; ++i) {
	sp->s_itrace = i-1;
	sp->s_jtrace = 0;
	tempcost  += tcdell;
	sp->s_cost = tempcost;
	sp += vnrow;
    }
    sp1 = &score[vnrow+1];			/* [1, 1].		*/
    pp = &pbase[1];
    for (i=1; i<=size; ++i) {
	sp = sp1;
	vp = &vbase[1];
	for (j=1; j<=size; ++j) {
	    sp->s_itrace = i-1;
	    sp->s_jtrace = j;
	    bestcost = (sp-vnrow)->s_cost;
	    if (j != size)			/* Cd(A[i])=0 @ Dis.	*/
		bestcost += tcdell;
	    tempcost = (sp-1)->s_cost;
	    tempcost += (*vp)->v_cost;
	    if (i != size)			/* Ci(B[j])=0 @ Dsj.	*/
		tempcost += tcinsl;
	    if (tempcost < bestcost) {
		sp->s_itrace = i;
		sp->s_jtrace = j-1;
		bestcost = tempcost;
	    }
	    tempcost = (sp-vnrow-1)->s_cost;
	    if ((*pp)->v_color != (*vp)->v_color
		||  (*pp)->v_hash  != (*vp)->v_hash)
		tempcost += (*vp)->v_cost;
	    if (tempcost < bestcost) {
		sp->s_itrace = i-1;
		sp->s_jtrace = j-1;
		bestcost = tempcost;
	    }
	    sp->s_cost = bestcost;
	    ++sp;				/* Next column.		*/
	    ++vp;
	}
	++pp;
	sp1 += vnrow;				/* Next row.		*/
    }
}

/*
 * Trace back through the dynamic programming cost
 * matrix, and update the screen using an optimal sequence
 * of redraws, insert lines, and delete lines. The "offs" is
 * the origin 0 offset of the chunk of the screen we are about to
 * update. The "i" and "j" are always started in the lower right
 * corner of the matrix, and imply the size of the screen.
 * A full screen traceback is called with offs=0 and i=j=nrow-1.
 * There is some do-it-yourself double subscripting here,
 * which is acceptable because this routine is much less compute
 * intensive then the code that builds the score matrix!
 */
static VOID
traceback(offs, size, i, j)
int offs, size, i, j;
{
    register int itrace;
    register int jtrace;
    register int k;
    register int ninsl;
    register int ndraw;
    register int ndell;
    
    if (i==0 && j==0)			/* End of update.	*/
	return;
    itrace = score[(vnrow*i) + j].s_itrace;
    jtrace = score[(vnrow*i) + j].s_jtrace;
    if (itrace == i) {			/* [i, j-1]		*/
	ninsl = 0;			/* Collect inserts.	*/
	if (i != size)
	    ninsl = 1;
	ndraw = 1;
	while (itrace!=0 || jtrace!=0) {
	    if (score[(vnrow*itrace) + jtrace].s_itrace != itrace)
		break;
	    jtrace = score[(vnrow*itrace) + jtrace].s_jtrace;
	    if (i != size)
		++ninsl;
	    ++ndraw;
	}
	traceback(offs, size, itrace, jtrace);
	if (ninsl != 0) {
	    ttcolor(CTEXT);
	    ttinsl(offs+j-ninsl, offs+size-1, ninsl);
	}
	do {				/* B[j], A[j] blank.	*/
	    k = offs+j-ndraw;
	    uline(k, vscreen[k], blanks);
	} while (--ndraw);
	return;
    }
    if (jtrace == j) {			/* [i-1, j]		*/
	ndell = 0;			/* Collect deletes.	*/
	if (j != size)
	    ndell = 1;
	while (itrace!=0 || jtrace!=0) {
	    if (score[(nrow*itrace) + jtrace].s_jtrace != jtrace)
		break;
	    itrace = score[(vnrow*itrace) + jtrace].s_itrace;
	    if (j != size)
		++ndell;
	}
	if (ndell != 0) {
	    ttcolor(CTEXT);
	    ttdell(offs+i-ndell, offs+size-1, ndell);
	}
	traceback(offs, size, itrace, jtrace);
	return;
    }
    traceback(offs, size, itrace, jtrace);
    k = offs+j-1;
    uline(k, vscreen[k], pscreen[offs+i-1]);
}
#endif

#ifdef ADDFUNC
/*
   Investigate the dot position to return a ratio value.  The ratio
   value will be between 0 and 100.  The following values are special
   case and all of them are negative value.

   MG_RATIO_ALL   All portion of the buffer is shown in the window.
   MG_RATIO_TOP   Top portion of the buffer is shown in the window.
   MG_RATIO_BOT   Bottom portion of the buffer is shown in the window.

   By Tillanosoft, Mar 21, 1999.
      patched by amura, 2 Apl 2000
 */
static int
windowpos(wp)
register WINDOW *wp;
{
    LINE *lp, *targetlp;
    register BUFFER *bp = wp->w_bufp;
    int top = FALSE, bot = FALSE, off = 0;
    int res = MG_RATIO_ALL;
    
    /* check if the beginning of top line is shown */
    if (wp->w_linep == lforw(bp->b_linep)) {
	if (wp->w_lines == 0)
	    top = TRUE;
    }
    /* check if the end of the bottom line is shown */
    lp = wp->w_linep;
    /* XXX off = rowcol2offset(lp, wp->w_lines + wp->w_ntrows, ncol); */
    while (off < 0) {
	if (lforw(lp) != bp->b_linep) {
	    lp = lforw(lp);
	    /* XXX off = rowcol2offset(lp, -off - 1, ncol); */
	}
	else {
	    bot = TRUE;
	    break;
	}
    }
    if (top && bot)
	res = MG_RATIO_ALL;
    else if (top)
	res = MG_RATIO_TOP;
    else if (bot)
	res = MG_RATIO_BOT;
    else {
	/* count the bytes of the dot and the end */
	long nchar = 0, cchar = 0;
	
	targetlp = wp->w_linep;
	lp = lforw(bp->b_linep);
	for (;;) {
	    if (lp == targetlp) {
		cchar = nchar + skipline(lp, wp->w_lines);
	    }
	    nchar += llength(lp); /* Now count the chars */
	    lp = lforw(lp);
	    if (lp == bp->b_linep) {
		break;
	    }
	    nchar++; /* count the newline */
	}
	res = (cchar * 100) / nchar;
	if (res >= 100) {
	    res = 99;
	}
    }
    return res;
}
#endif /* ADDFUNC */
