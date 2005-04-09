/* $Id: random.c,v 1.13.2.3 2005/04/09 06:26:14 amura Exp $ */
/*
 *		Assorted commands.
 * The file contains the command
 * processors for a large assortment of unrelated
 * commands. The only thing they have in common is
 * that they are all command processors.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */
/* 91.11.30     Modified by bsh */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "random.h"

#include "i_buffer.h"
#include "i_lang.h"
#include "i_window.h"
#include "basic.h"
#include "display.h"
#include "line.h"
#include "echo.h"
#include "undo.h"
#include "kbd.h"
#include "search.h"

/*
 * Display a bunch of useful information about
 * the current location of dot. The character under the
 * cursor (in octal), the current line, row, and column, and
 * approximate position of the cursor in the file (as a percentage)
 * is displayed. The column position assumes an infinite position
 * display; it does not truncate just because the screen does.
 * This is normally bound to "C-X =".
 */
/*ARGSUSED*/
int
showcpos(f, n)
int f, n;
{
    register LINE *clp;
    register long nchar;
    long cchar=0;
    register int nline, row;
    int cline=0;	/* Current line */
    NG_WCHAR_t cbyte=0;	/* Current char */
    int ratio;
    int x, y;

    clp = lforw(curbp->b_linep);	/* Collect the data.	*/
    nchar = 0;
    nline = 0;
    for (;;) {
	++nline;			/* Count this line	*/
	if (clp == curwp->w_dotp) {
	    cline = nline;		/* Mark line		*/
	    cchar = nchar + curwp->w_doto;
	    if (curwp->w_doto == llength(clp))
		cbyte = '\n';
	    else
		cbyte = lgetc(clp, curwp->w_doto);
	}
	nchar += llength(clp);		/* Now count the chars	*/
	clp = lforw(clp);
	if (clp == curbp->b_linep)
	    break;
	nchar++;			/* count the newline	*/
    }
    row = curwp->w_toprow + 1;		/* Determine row.	*/
    clp = curwp->w_linep;
    while (clp!=curbp->b_linep && clp!=curwp->w_dotp) {
	row += countlines(clp);
	clp = lforw(clp);
    }
    row += colrow(clp, curwp->w_doto, &x, &y);
    /*NOSTRICT*/
    ratio = nchar ? (100L*cchar) / nchar : 100;
    {
	char w[NG_WCHAR_STRLEN];
	NG_WCHAR_TO_STR(w, cbyte);
	ewprintf("Char: %s (0%o)(0x%x)  point=%ld(%d%%)  "
		 "line=%d  offset=%d  row=%d  col=%d",
		 w, cbyte, cbyte, cchar, ratio,
		 cline, getcolpos(), row, x+1);
    }
    return TRUE;
}

int
getcolpos()
{
    register int col, i, c;
#ifdef VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif /* VARIABLE_TAB */
    int (*lm_width)(NG_WCHAR_t) = curbp->b_lang->lm_width;
    col = 0;				/* Determine column.	*/
    
    for (i=0; i<curwp->w_doto; ++i) {
	c = lgetc(curwp->w_dotp, i);
	if (c == '\t' && !(curbp->b_flag & BFNOTAB)) {
#ifdef	VARIABLE_TAB
	    col = (col/tab + 1)*tab - 1;
#else
	    col |= 0x07;
#endif
	    ++col;
	}
	else
	    col += lm_width(c);
    }
    return col;
}

/*
 * Twiddle the two characters on either side of
 * dot. If dot is at the end of the line twiddle the
 * two characters before it. Return with an error if dot
 * is at the beginning of line; it seems to be a bit
 * pointless to make this work. This fixes up a very
 * common typo with a single stroke. Normally bound
 * to "C-T". This always works within a line, so
 * "WFEDIT" is good enough.
 */
/*ARGSUSED*/
int
twiddle(f, n)
int f, n;
{
    register LINE *dotp;
    register int doto;
    register NG_WCHAR_t cr;
#ifdef UNDO
    UNDO_DATA *undo = NULL;
#endif

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    dotp = curwp->w_dotp;
    doto = curwp->w_doto;
    if (doto==llength(dotp)) {
	if (--doto<=0)
	    return FALSE;
    }
    else {
	if (doto == 0)
	    return FALSE;
	++curwp->w_doto;
    }
#ifdef UNDO
    undo_setup(undo);
    if (isundo()) {
	undo_bfree(undo);
	undo->u_dotlno = get_lineno(curbp,dotp);
	undo->u_doto = doto;
	undo->u_type = UDTWIDDLE;
	undo->u_used = 0;
	undo_finish(&(undo->u_next));
    }
#endif /* UNDO */
    cr = lgetc(dotp, doto--);
    lputc(dotp, doto+1, lgetc(dotp, doto));
    lputc(dotp, doto, cr);
    lchange(WFEDIT);
    return TRUE;
}

/*
 * Open up some blank space. The basic plan
 * is to insert a bunch of newlines, and then back
 * up over them. Everything is done by the subcommand
 * procerssors. They even handle the looping. Normally
 * this is bound to "C-O".
 */
/*ARGSUSED*/
int
openline(f, n)
int f, n;
{
    register int i;
    register int s;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
	}
#endif /* READONLY */
    
    if (n < 0)
	return FALSE;
    if (n == 0)
	return TRUE;
    i = n;					/* Insert newlines.	*/
    do {
	s = lnewline();
    } while (s==TRUE && --i);
    if (s == TRUE)				/* Then back up overtop */
	s = backchar(f | FFRAND, n);		/* of them all.		*/
    return s;
}

/*
 * Insert a newline.
 * If you are at the end of the line and the
 * next line is a blank line, just move into the
 * blank line. This makes "C-O" and "C-X C-O" work
 * nicely, and reduces the ammount of screen
 * update that has to be done. This would not be
 * as critical if screen update were a lot
 * more efficient.
 */
/*ARGSUSED*/
int
newline(f, n)
int f, n;
{
    register int s;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    if (n < 0)
	return FALSE;
    while (n--) {
	if ((s=lnewline()) != TRUE)
	    return s;
    }
    return TRUE;
}

/*
 * Delete blank lines around dot.
 * What this command does depends if dot is
 * sitting on a blank line. If dot is sitting on a
 * blank line, this command deletes all the blank lines
 * above and below the current line. If it is sitting
 * on a non blank line then it deletes all of the
 * blank lines after the line. Normally this command
 * is bound to "C-X C-O". Any argument is ignored.
 */
/*ARGSUSED*/
int
deblank(f, n)
int f, n;
{
    register LINE *lp1;
    register LINE *lp2;
    register RSIZE nld;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    lp1 = curwp->w_dotp;
    while (llength(lp1)==0 && (lp2=lback(lp1))!=curbp->b_linep)
	lp1 = lp2;
    lp2 = lp1;
    nld = (RSIZE) 0;
    while ((lp2=lforw(lp2))!=curbp->b_linep && llength(lp2)==0)
	++nld;
    if (nld == 0)
	return (TRUE);
    curwp->w_dotp = lforw(lp1);
    curwp->w_doto = 0;
    return ldelete((RSIZE)nld, KNONE);
}

/*
 * Delete any whitespace around dot, then insert a space.
 */
int
justone(f, n)
int f, n;
{
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    (VOID) delwhite(f, n);
    return linsert(1, ' ');
}

/*
 * Delete any whitespace around dot.
 */
/*ARGSUSED*/
int
delwhite(f, n)
int f, n;
{
    register int col, c, s;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    col = curwp->w_doto;
    while (((c = lgetc(curwp->w_dotp, col)) == ' ' || c == '\t')
	   && col < llength(curwp->w_dotp))
	++col;
    do {
	if (curwp->w_doto == 0) {
	    s = FALSE;
	    break;
	}
	if ((s = backchar(FFRAND, 1)) != TRUE)
	    break;
    } while ((c = lgetc(curwp->w_dotp, curwp->w_doto)) == ' ' || c == '\t');

    if (s == TRUE)
	(VOID) forwchar(FFRAND, 1);
    (VOID) ldelete((RSIZE)(col - curwp->w_doto), KNONE);
    return TRUE;
}

/*
 * Insert a newline, then enough
 * tabs and spaces to duplicate the indentation
 * of the previous line. Assumes tabs are every eight
 * characters. Quite simple. Figure out the indentation
 * of the current line. Insert a newline by calling
 * the standard routine. Insert the indentation by
 * inserting the right number of tabs and spaces.
 * Return TRUE if all ok. Return FALSE if one
 * of the subcomands failed. Normally bound
 * to "C-J".
 */
/*ARGSUSED*/
int
indent(f, n)
int f, n;
{
    register int nicol;
    register int c;
    register int i;
#ifdef  VARIABLE_TAB
    int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */
    
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */
    
    if (n < 0)
	return (FALSE);
    while (n--) {
	nicol = 0;
	for (i=0; i<llength(curwp->w_dotp); ++i) {
	    c = lgetc(curwp->w_dotp, i);
	    if (c!=' ' && c!='\t')
		break;
	    if (c == '\t')
#ifdef VARIABLE_TAB
		nicol = (nicol/tab + 1)*tab - 1;
#else
	    nicol |= 0x07;
#endif
	    ++nicol;
	}
	if (lnewline() == FALSE || ((
		curbp->b_flag&BFNOTAB) ? linsert(nicol, ' ') == FALSE : (
		    ((i=nicol/8)!=0 && linsert(i, '\t')==FALSE) ||
		    ((i=nicol%8)!=0 && linsert(i,  ' ')==FALSE))))
	    return FALSE;
    }
    return TRUE;
}

/*
 * Delete forward. This is real
 * easy, because the basic delete routine does
 * all of the work. Watches for negative arguments,
 * and does the right thing. If any argument is
 * present, it kills rather than deletes, to prevent
 * loss of text if typed with a big argument.
 * Normally bound to "C-D".
 */
/*ARGSUSED*/
int
forwdel(f, n)
int f, n;
{
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    if (n < 0)
	return backdel(f | FFRAND, -n);
    if (f & FFARG) {			/* Really a kill.	*/
	if ((lastflag&CFKILL) == 0)
	    kdelete();
	thisflag |= CFKILL;
    }
    return ldelete((RSIZE) n, (f & FFARG) ? KFORW : KNONE);
}

/*
 * Delete backwards. This is quite easy too,
 * because it's all done with other functions. Just
 * move the cursor back, and delete forwards.
 * Like delete forward, this actually does a kill
 * if presented with an argument.
 */
/*ARGSUSED*/
int
backdel(f, n)
int f, n;
{
    register int s;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    if (n < 0)
	return forwdel(f | FFRAND, -n);
    if (f & FFARG) {			/* Really a kill.	*/
	if ((lastflag&CFKILL) == 0)
	    kdelete();
	thisflag |= CFKILL;
    }
    if ((s=backchar(f | FFRAND, n)) == TRUE)
	s = ldelete((RSIZE) n, (f & FFARG) ? KFORW : KNONE);
#ifdef	UNDO
    if (isundo() && undobefore!=NULL)
	(*undobefore)->u_type = UDBS;
#endif
    return s;
}

/*
 * Kill line. If called without an argument,
 * it kills from dot to the end of the line, unless it
 * is at the end of the line, when it kills the newline.
 * If called with an argument of 0, it kills from the
 * start of the line to dot. If called with a positive
 * argument, it kills from dot forward over that number
 * of newlines. If called with a negative argument it
 * kills any text before dot on the current line,
 * then it kills back abs(arg) lines.
 */
/*ARGSUSED*/
int
killline(f, n)
int f, n;
{
    register RSIZE chunk;
    register LINE *nextp;
    register int i, c;
    
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */
    
    if ((lastflag&CFKILL) == 0)		/* Clear kill buffer if */
	kdelete();			/* last wasn't a kill.	*/
    thisflag |= CFKILL;
    if (!(f & FFARG)) {
	for (i = curwp->w_doto; i < llength(curwp->w_dotp); ++i)
	    if ((c = lgetc(curwp->w_dotp, i)) != ' ' && c != '\t')
		break;
	if (i == llength(curwp->w_dotp))
	    chunk = llength(curwp->w_dotp)-curwp->w_doto + 1;
	else {
	    chunk = llength(curwp->w_dotp)-curwp->w_doto;
	    if (chunk == 0)
		chunk = 1;
	}
    }
    else if (n > 0) {
	chunk = llength(curwp->w_dotp)-curwp->w_doto+1;
	nextp = lforw(curwp->w_dotp);
	i = n;
	while (--i) {
	    if (nextp == curbp->b_linep)
		break;
	    chunk += llength(nextp)+1;
	    nextp = lforw(nextp);
	}
    }
    else {				/* n <= 0		*/
	chunk = curwp->w_doto;
	curwp->w_doto = 0;
	i = n;
	while (i++) {
	    if (lback(curwp->w_dotp) == curbp->b_linep)
		break;
	    curwp->w_dotp = lback(curwp->w_dotp);
	    curwp->w_flag |= WFMOVE;
	    chunk += llength(curwp->w_dotp)+1;
	}
    }
    /*
     * KFORW here is a bug. Should be KBACK/KFORW, but we need to
     * rewrite the ldelete code (later)?
     */
    return (ldelete(chunk,	KFORW));
}

/*						*/
/*	Kill oneline by S.Sasaki May-05-1992	*/
/*						*/
int
killoneline(f, n)
int f, n;
{
    register RSIZE chunk;
    register LINE *nextp;
    register int i;
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    if ((lastflag&CFKILL) == 0)		/* Clear kill buffer if */
	kdelete();			/* last wasn't a kill.	*/
    thisflag |= CFKILL;
    if (!(f & FFARG)) {
	curwp->w_doto = 0;
	chunk = llength(curwp->w_dotp) + 1;
    }
    else if (n > 0) {
	curwp->w_doto = 0;
	chunk = llength(curwp->w_dotp)+1;
	nextp = lforw(curwp->w_dotp);
	i = n;
	while (--i) {
	    if (nextp == curbp->b_linep)
		break;
	    chunk += llength(nextp)+1;
	    nextp = lforw(nextp);
	}
    }
    else {				/* n <= 0		*/
	chunk = llength(curwp->w_dotp)+1;
	curwp->w_doto = 0;
	i = n;
	while (i++) {
	    if (lback(curwp->w_dotp) == curbp->b_linep)
		break;
	    curwp->w_dotp = lback(curwp->w_dotp);
	    curwp->w_flag |= WFMOVE;
	    chunk += llength(curwp->w_dotp)+1;
	}
    }
    /*
     * KFORW here is a bug. Should be KBACK/KFORW, but we need to
     * rewrite the ldelete code (later)?
     */
    return ldelete(chunk,KFORW);
}

/*
 * Yank text back from the kill buffer. This
 * is really easy. All of the work is done by the
 * standard insert routines. All you do is run the loop,
 * and check for errors. The blank
 * lines are inserted with a call to "newline"
 * instead of a call to "lnewline" so that the magic
 * stuff that happens when you type a carriage
 * return also happens when a carriage return is
 * yanked back from the kill buffer.
 * An attempt has been made to fix the cosmetic bug
 * associated with a yank when dot is on the top line of
 * the window (nothing moves, because all of the new
 * text landed off screen).
 */
/*ARGSUSED*/
int
yank(f, n)
int f, n;
{
    register int c;
    register int i;
    register LINE *lp;
    register int nline;
    int mark_adjust = FALSE;
#ifdef UNDO
    int run_insert = FALSE;
#endif
    
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    if (n < 0)
	return FALSE;
#ifdef CLIPBOARD
    if (!receive_clipboard())
	return FALSE ;
#endif
    nline = 0;				/* Newline counting.	*/
    while (n--) {
	isetmark();			/* mark around last yank */
	i = 0;
#ifdef UNDO
	if (isundo()) {
	    set_lineno = get_lineno(curbp, curwp->w_dotp);
	    while ((c=kremove(i)) >= 0) {
		if (c == '\n') {
		    if (i == 0 && curbp->b_marko == 0)
			mark_adjust = TRUE;
		    if (newline(FFRAND, 1) == FALSE) {
			set_lineno = -1;
			return FALSE;
		    }
		    /* Mark position correction.	*/
		    if (mark_adjust) {
			LINE *lp = lback(curwp->w_dotp);
			curbp->b_markp  = lp;
			curbp->b_marko  = llength(lp);
			mark_adjust = FALSE;
		    }
		    ++nline; set_lineno++;
		    run_insert = FALSE;
		}
		else {
		    if (run_insert)
			undoptr = undobefore;
		    else if (*undoptr != NULL)
			(*undoptr)->u_type = UDNONE;
		    if (linsert(1, c) == FALSE) {
			set_lineno = -1;
			return FALSE;
		    }
		    run_insert = TRUE;
		}
		++i;
	    }
	    set_lineno = -1;
	}
	else
#endif
	    while ((c=kremove(i)) >= 0) {
		if (c == '\n') {
		    if (i == 0 && curbp->b_marko == 0)
			mark_adjust = TRUE;
		    if (newline(FFRAND, 1) == FALSE)
			return FALSE;
		    /* Mark position correction.	*/
		    if (mark_adjust) {
			LINE *lp=lback(curwp->w_dotp);
			curbp->b_markp  = lp;
			curbp->b_marko  = llength(lp);
			mark_adjust = FALSE;
		    }
		    ++nline;
		}
		else {
		    if (linsert(1, c) == FALSE)
			return FALSE;
		}
		++i;
	    }
    }
    lp = curwp->w_linep;		/* Cosmetic adjustment	*/
    if (curwp->w_dotp == lp) {		/* if offscreen insert. */
	while (nline > 0 && lback(lp)!=curbp->b_linep) {
	    lp = lback(lp);
	    nline --;
	}
	curwp->w_linep = lp;		/* Adjust framing.	*/
	curwp->w_lines = 0;
	curwp->w_flag |= WFHARD;
    }
    return TRUE;
}

/*ARGSUSED*/
int
space_to_tabstop(f, n)
int f, n;
{
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    if (n < 0)
	return FALSE;
    if (n == 0)
	return TRUE;
    return linsert((n<<3) - (curwp->w_doto & 7), ' ');
}

#ifdef ZAPTOC_A
# define ZAPTOCHAR
#endif

#ifdef ZAPTOCHAR		/* Nov 91, bsh */
/*
 * Zap to ARG'th occurence of char, but not including it.
 * Goes backward if ARG is negative.
 * #ifdef ZAPTOC_A
 *    Goes to end of buffer if char not found.
 * #else
 *    do nothing if it can't reach to ARG'th occurence of char.
 * #endif
 *
 * Real work to search CHAR is done by forwsrch, which does real work
 * of forwsearch() (M-x search-forward). Global buffer `pat' is used
 * for interface.
 */
int
zaptochar(f, n)
int f, n;
{
    int c;
    RSIZE nbytes;
    LINE *sp = curwp->w_dotp;			/* save dot */
    int so = curwp->w_doto;
    LINE *cp;
    int  co;
    
    ewprintf( "Zap to char: " );
    c = getkey(FALSE);
    if (c == CCHR('G'))				/* you can't zap to ^G */
	return FALSE;
    pat[0] = c;
#ifndef	KANJI
    pat[1] = '\0';
#else
    if (!ISKANJI(c))
	pat[1] = '\0';
    else {
	c = getkey(FALSE);
	if (!ISKANJI(c))
	    return FALSE;
	pat[1] = c;
	pat[2] = '\0';
    }
#endif	/* KANJI */
    
    if (n == 0)
	n = 1;
    if (n < 0) {
	while (n < 0 && backsrch())
	    ++n;
	if (n < 0) {		       /* not found */
#ifdef ZAPTOC_A
	    curwp->w_dotp  = lforw(curbp->b_linep);
	    curwp->w_doto  = 0;
#else /* Safer */
	    goto notfound;
#endif		    
	}
	else {
	    /* Not including last occurence of CHR */
	    forwchar( FFRAND, 1 );
	}
	cp = curwp->w_dotp;
	co = curwp->w_doto;
    }
    else {
	while (n > 0 && forwsrch())
	    --n;
	if (n > 0) {
#ifdef ZAPTOC_A
	    curwp->w_dotp  = lback(curbp->b_linep);
	    curwp->w_doto  = llength(curwp->w_dotp);
#else
	    goto notfound;
#endif
	}
	else {
	    /* Not including last occurence of CHAR */
	    backchar( FFRAND, 1 );
	}
	co = so;
	cp = sp;
	so = curwp->w_doto;	/* Swap dot and previous point */
	sp = curwp->w_dotp;
	curwp->w_doto = co;	/* Previous position */
	curwp->w_dotp = cp;
    }
    
    /* Region to kill is (dot) .. (cp,co).
     * Count up chars and let ldelete() to do real work
     */
    if (cp == sp) {			       /* on the same line */
	nbytes = so - co;
    }
    else {
	nbytes = llength(cp) - co + 1 + so;
	for (cp = lforw(cp); cp != sp; cp = lforw(cp))
	    nbytes += llength(cp)+1;
    }
    if ((lastflag&CFKILL) == 0)		/* This is a kill type	*/
	kdelete();			/* command, so do magic */
    thisflag |= CFKILL;			/* kill buffer stuff.	*/
    return (ldelete(nbytes,KFORW));
    
notfound:
    ewprintf( "Not found" );
    curwp->w_dotp = sp;
    curwp->w_doto = so;
    return FALSE;
}
#endif

#ifdef ADDFUNC	/* 90.02.15  by S.Yoshida */
/*
 * Count lines in the current page. (Now, page is same as buffer)
 */
/*ARGSUSED*/
int
pagelines(f, n)
int f, n;
{
    register LINE *lp;
    register int prelines;
    register int postlines;
    register int totallines;
    
    prelines = 0;
    for (lp = lforw(curbp->b_linep); lp != curwp->w_dotp; lp = lforw(lp))
	prelines++;
    if (curwp->w_doto > 0)	/* "dot" is in the line. */
	prelines++;
    postlines = 0;
    for (lp = curwp->w_dotp; lp != curbp->b_linep; lp = lforw(lp))
	postlines++;
    if (curwp->w_dotp == lback(curbp->b_linep) &&	/* "dot" is at the */
	curwp->w_doto == llength(curwp->w_dotp)) {	/* end of buffer.  */
	postlines = 0;
    }
    else if (llength(lback(curbp->b_linep)) == 0) {	/* Last line has */
	postlines--;					/* no text.      */
    }
    totallines = prelines + postlines;
    if (curwp->w_doto > 0 &&
	!(curwp->w_dotp == lback(curbp->b_linep) &&
	  curwp->w_doto == llength(curwp->w_dotp))) {
	totallines--;
    }
    ewprintf("Page has %d lines (%d + %d)",
	     totallines, prelines, postlines);
    return TRUE;
}

/*
 * Count lines in the current region.
 */
/*ARGSUSED*/
int
regionlines(f, n)
int f, n;
{
    register LINE *lp;
    register int totallines;
    register int counting;
    
    totallines = 0;
    if (curwp->w_dotp == curbp->b_markp) {
	if (curwp->w_doto != curbp->b_marko) {
	    totallines = 1;
	}
    }
    else {
	counting = FALSE;
	for (lp = lforw(curbp->b_linep);
	     lp != curbp->b_linep; lp = lforw(lp)) {
	    if (lp == curwp->w_dotp) {	
		if (counting) {	/* End of counting. */
		    if (curwp->w_doto > 0)
			totallines++;
		    break;
		}
		else {		/* Start of counting. */
		    counting = TRUE;
		}
	    }
	    else if (lp == curbp->b_markp) {
		if (counting) {	/* End of counting. */
		    if (curbp->b_marko > 0)
			totallines++;
		    break;
		}
		else {		/* Start of counting. */
		    counting = TRUE;
		}
	    }
	    if (counting)
		totallines++;
	}
    }
    ewprintf("Region has %d lines", totallines);
    return TRUE;
}

#endif /* ADDFUNC */
