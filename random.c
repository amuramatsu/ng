/* $Id: random.c,v 1.10.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 *		Assorted commands.
 * The file contains the command
 * processors for a large assortment of unrelated
 * commands. The only thing they have in common is
 * that they are all command processors.
 */

/*
 * $Log: random.c,v $
 * Revision 1.10.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.10  2001/07/23 17:12:02  amura
 * fix mark handling when make newline on the mark position
 *
 * Revision 1.9  2001/07/22 20:46:58  amura
 * before checkin has bug. now mark handling is fixed
 *
 * Revision 1.8  2001/06/19 15:15:32  amura
 * add correcting mark position when yank empty line
 *
 * Revision 1.7  2001/05/25 15:37:21  amura
 * now buffers have only one mark (before windows have one mark)
 *
 * Revision 1.6  2001/02/18 17:07:26  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.5  2001/01/05 14:07:04  amura
 * first implementation of Hojo Kanji support
 *
 * -- snip -- 
 *
 * Revision 1.1  2000/06/01  05:35:32  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */
/* 91.11.30     Modified by bsh */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#ifdef	UNDO
#include	"undo.h"
#endif

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
showcpos(f, n)
{
	register LINE	*clp;
	register long	nchar;
	long		cchar=0;
	register int	nline, row;
	int		cline=0, cbyte=0;	/* Current line/char/byte */
#ifdef	CHGMISC	/* 99.3.26 by M.Suzuki	*/
	int		cbyte2;
#endif	/* CHGMISC	*/
	int		ratio;
	int		x, y;

	clp = lforw(curbp->b_linep);		/* Collect the data.	*/
	nchar = 0;
	nline = 0;
	for (;;) {
		++nline;			/* Count this line	*/
		if (clp == curwp->w_dotp) {
			cline = nline;		/* Mark line		*/
			cchar = nchar + curwp->w_doto;
#ifdef	CHGMISC	/* 99.3.26 by M.Suzuki	*/
			if (curwp->w_doto == llength(clp)){
				cbyte = '\n';
				cbyte2 = 0;
			} else {
				cbyte = lgetc(clp, curwp->w_doto);
				cbyte2 = lgetc(clp, curwp->w_doto+1);
			}
#else
			if (curwp->w_doto == llength(clp))
				cbyte = '\n';
			else
				cbyte = lgetc(clp, curwp->w_doto);
#endif
		}
		nchar += llength(clp);		/* Now count the chars	*/
		clp = lforw(clp);
		if (clp == curbp->b_linep) break;
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
#ifdef	CHGMISC		/* 99.3.26 by M.Suzuki	*/
	if (ISKANJI(cbyte)){
		char w[3];
		w[0] = cbyte;
		w[1] = cbyte2;
		w[2] = '\0';
		ewprintf("Char: %s (0%o,0%o)(0x%x,0x%x)  point=%ld(%d%%)  "
			 "line=%d  offset=%d  row=%d  col=%d",
			 w, cbyte, cbyte2, cbyte, cbyte2, cchar, ratio,
			 cline, getcolpos(), row, x+1);
	} else
#endif	/* CHGMISC */
	ewprintf(
	"Char: %c (0%o)  point=%ld(%d%%)  line=%d  offset=%d  row=%d  col=%d",
		cbyte, cbyte, cchar, ratio, cline, getcolpos(), row, x+1);
	return TRUE;
}

getcolpos() {
	register int	col, i, c;
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
	int kan2nd = 0;
#endif  /* SS_SUPPORT */
#ifdef  VARIABLE_TAB
	int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */
#ifdef	BUGFIX	/* 90.04.10  by m.tei (Nikkei MIX) */
	col = 0;				/* Determine column.	*/
#else	/* NOT BUGFIX */
	col = 1;				/* Determine column.	*/
#endif	/* BUGFIX */

	for (i=0; i<curwp->w_doto; ++i) {
		c = lgetc(curwp->w_dotp, i);
		if (c == '\t' && !(curbp->b_flag & BFNOTAB)) {
#ifdef	VARIABLE_TAB
		    col = (col/tab + 1)*tab - 1;
#else
		    col |= 0x07;
#endif
#ifndef	BUGFIX	/* 90.04.10  by m.tei (Nikkei MIX) */
		    ++col;
#endif	/* BUGFIX */
		} else if (ISCTRL(c) != FALSE)
			++col;
		++col;
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		if (ISKANJI(c)) {
#ifdef	HOJO_KANJI
		    if (ISHOJO(c) && kan2nd==0) {
			kan2nd = 3;
			col--;
		    }
#endif
#ifdef	HANKANA
		    if (ISHANKANA(c) && kan2nd==0)
			col--;
#endif
		    if (kan2nd == 0)
			kan2nd = 1;
		    else
			kan2nd--;
		}
#endif  /* SS_SUPPORT */
	}
#ifdef	BUGFIX  /* 90.04.10  by m.tei (Nikkei MIX) */
	col++;
#endif	/* BUGFIX */
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
twiddle(f, n)
{
	register LINE	*dotp;
	register int	doto;
	register int	cr;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	cr2;
	register int	cr3;
	register int	cr4;
#endif	/* KANJI */
	VOID	 lchange();
#ifdef	UNDO
	UNDO_DATA	*undo;
#endif

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	dotp = curwp->w_dotp;
	doto = curwp->w_doto;
	if(doto==llength(dotp)) {
		if(--doto<=0) return FALSE;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (ISKANJI(lgetc(dotp, doto))) {
			if (--doto <= 0) return FALSE;
		}
#endif	/* KANJI */
	} else {
		if(doto==0) return FALSE;
		++curwp->w_doto;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (ISKANJI(lgetc(dotp, doto))) {
			++curwp->w_doto;
		}
#endif	/* KANJI */
	}
#ifdef	UNDO
	undo_setup(undo);
	if (isundo()) {
	    undo_bfree(undo);
	    undo->u_dotlno = get_lineno(curbp,dotp);
	    undo->u_doto = doto;
	    undo->u_type = UDTWIDDLE;
	    undo->u_used = 0;
	    undo_finish(&(undo->u_next));
	}
#endif	/* UNDO */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	cr = lgetc(dotp, doto);
	if (ISKANJI(cr)) {			/* It's KANJI 1st byte.	*/
		cr2 = lgetc(dotp, doto + 1);	/* Get KANJI 2nd byte.	*/
	} else {
		cr2 = '\0';			/* cr is ASCII.		*/
		/* 91.01.15  NULL -> '\0' */
	}
	cr3 = lgetc(dotp, --doto);
	if (ISKANJI(cr3)) {			/* It's KANJI 2nd byte.	*/
		cr4 = cr3;
		cr3 = lgetc(dotp, --doto);	/* Get KANJI 1st byte.	*/
	} else {
		cr4 = '\0';			/* cr3 is ASCII.	*/
		/* 91.01.15  NULL -> '\0' */
	}
	lputc(dotp, doto++, cr);		/* ASCII or KANJI 1st.	*/
	if (cr2 != '\0') {			/* This char is KANJI.	*/
		/* 91.01.15  NULL -> '\0' */
		lputc(dotp, doto++, cr2);	/* Put KANJI 2nd byte.	*/
	}
	lputc(dotp, doto++, cr3);		/* ASCII or KANJI 1st.	*/
	if (cr4 != '\0') {			/* This char is KANJI.	*/
		/* 91.01.15  NULL -> '\0' */
		lputc(dotp, doto++, cr4);
	}
#else	/* NOT KANJI */
	cr = lgetc(dotp, doto--);
	lputc(dotp, doto+1, lgetc(dotp, doto));
	lputc(dotp, doto, cr);
#endif	/* KANJI */
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
openline(f, n)
{
	register int	i;
	register int	s;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0)
		return FALSE;
	if (n == 0)
		return TRUE;
	i = n;					/* Insert newlines.	*/
	do {
		s = lnewline();
	} while (s==TRUE && --i);
	if (s == TRUE)				/* Then back up overtop */
		s = backchar(f | FFRAND, n);	/* of them all.		*/
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
newline(f, n)
{
#ifndef	BUGFIX	/* 90.02.14  by S.Yoshida */
	register LINE	*lp;
#endif	/* BUGFIX */
	register int	s;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0) return FALSE;
	while (n--) {
#ifndef	BUGFIX	/* 90.02.14  by S.Yoshida */
		lp = curwp->w_dotp;
		if (llength(lp) == curwp->w_doto
		&& lforw(lp) != curbp->b_linep
		&& llength(lforw(lp)) == 0) {
			if ((s=forwchar(FFRAND, 1)) != TRUE)
				return s;
		} else
#endif	/* BUGFIX */
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
deblank(f, n)
{
	register LINE	*lp1;
	register LINE	*lp2;
	register RSIZE	nld;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

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
justone(f, n) {
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */
	(VOID) delwhite(f, n);
	return linsert(1, ' ');
}
/*
 * Delete any whitespace around dot.
 */
/*ARGSUSED*/
delwhite(f, n)
{
	register int	col, c, s;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	col = curwp->w_doto;
	while (((c = lgetc(curwp->w_dotp, col)) == ' ' || c == '\t')
			&& col < llength(curwp->w_dotp))
		++col;
	do {
		if (curwp->w_doto == 0) {
			s = FALSE;
			break;
		}
		if ((s = backchar(FFRAND, 1)) != TRUE) break;
	} while ((c = lgetc(curwp->w_dotp, curwp->w_doto)) == ' ' || c == '\t');

	if (s == TRUE) (VOID) forwchar(FFRAND, 1);
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
indent(f, n)
{
	register int	nicol;
	register int	c;
	register int	i;
#ifdef  VARIABLE_TAB
	int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0) return (FALSE);
	while (n--) {
		nicol = 0;
		for (i=0; i<llength(curwp->w_dotp); ++i) {
			c = lgetc(curwp->w_dotp, i);
			if (c!=' ' && c!='\t')
				break;
			if (c == '\t')
#ifdef	VARIABLE_TAB
				nicol = (nicol/tab + 1)*tab - 1;
#else
				nicol |= 0x07;
#endif
			++nicol;
		}
		if (lnewline() == FALSE ||
		    ((curbp->b_flag&BFNOTAB) ?
			linsert(nicol, ' ') == FALSE : (
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
forwdel(f, n)
{
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

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
backdel(f, n)
{
	register int	s;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

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
killline(f, n) {
	register RSIZE	chunk;
	register LINE	*nextp;
	register int	i, c;
	VOID	 kdelete();

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
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
	} else if (n > 0) {
		chunk = llength(curwp->w_dotp)-curwp->w_doto+1;
		nextp = lforw(curwp->w_dotp);
		i = n;
		while (--i) {
			if (nextp == curbp->b_linep)
				break;
			chunk += llength(nextp)+1;
			nextp = lforw(nextp);
		}
	} else {				/* n <= 0		*/
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

killoneline(f, n) {
	register RSIZE	chunk;
	register LINE	*nextp;
	register int	i;
	VOID	 kdelete();
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if ((lastflag&CFKILL) == 0)		/* Clear kill buffer if */
		kdelete();			/* last wasn't a kill.	*/
	thisflag |= CFKILL;
	if (!(f & FFARG)) {
		curwp->w_doto = 0;
		chunk = llength(curwp->w_dotp) + 1;
	} else if (n > 0) {
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
	} else {				/* n <= 0		*/
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
yank(f, n)
{
	register int	c;
	register int	i;
	register LINE	*lp;
	register int	nline;
	VOID	 isetmark();
	int mark_adjust = FALSE;
#ifdef	UNDO
	int run_insert = FALSE;
#endif

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0) return FALSE;
#ifdef	CLIPBOARD
	if ( !receive_clipboard() ) {
		return FALSE ;
	}
#endif	/* CLIPBOARD */
	nline = 0;				/* Newline counting.	*/
	while (n--) {
		isetmark();			/* mark around last yank */
		i = 0;
#ifdef	UNDO
		if (isundo()) {
			extern int get_lineno pro((BUFFER*,LINE*));
			extern int set_lineno;
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
						LINE *lp=lback(curwp->w_dotp);
						curbp->b_markp  = lp;
						curbp->b_marko  = llength(lp);
						mark_adjust = FALSE;
					}
					++nline; set_lineno++;
					run_insert = FALSE;
				} else {
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
		} else
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
			} else {
				if (linsert(1, c) == FALSE)
					return FALSE;
			}
			++i;
		}
	}
	lp = curwp->w_linep;			/* Cosmetic adjustment	*/
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
space_to_tabstop(f, n)
int f, n;
{
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    if(n<0) return FALSE;
    if(n==0) return TRUE;
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
zaptochar(f,n)
{
	int c;
	RSIZE	nbytes;
	LINE *sp = curwp->w_dotp;                /* save dot */
	int so = curwp->w_doto;
	LINE *cp;
	int  co;
	
	ewprintf( "Zap to char: " );
	c = getkey(FALSE);
	if( c == CCHR('G') )			/* you can't zap to ^G */
		return FALSE;
	pat[0] = c;
#ifndef	KANJI
	pat[1] = '\0';
#else
	if( !ISKANJI(c) )
		pat[1] = '\0';
#ifdef	HOJO_KANJI
	else if( ISHOJO(c) ) {
		c = getkey(FALSE);
		if( !ISKANJI(c) )	return FALSE;
		pat[1] = c;
		c = getkey(FALSE);
		if( !ISKANJI(c) )	return FALSE;
		pat[2] = c;
		pat[3] = '\0';
	}
#endif
	else {
		c = getkey(FALSE);
		if( !ISKANJI(c) )
			return FALSE;
		pat[1] = c;
		pat[2] = '\0';
	}
#endif	/* KANJI */
	
	if( n == 0 ) n = 1;
	if( n < 0 ){
		while(n < 0 && backsrch())
			++n;
		if( n < 0 ){		       /* not found */
#ifdef	ZAPTOC_A
			curwp->w_dotp  = lforw(curbp->b_linep);
			curwp->w_doto  = 0;
#else		/* Safer */
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
		while(n > 0 && forwsrch())
			--n;
		if( n > 0 ){
#ifdef	ZAPTOC_A
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
	if( cp == sp ){			       /* on the same line */
		nbytes = so - co;
	}
	else {
		nbytes = llength(cp) - co + 1 + so;
		for(cp = lforw(cp); cp != sp; cp = lforw(cp))
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

#ifdef	ADDFUNC	/* 90.02.15  by S.Yoshida */
/*
 * Count lines in the current page. (Now, page is same as buffer)
 */
/*ARGSUSED*/
pagelines(f, n)
{
	register LINE	*lp;
	register int	prelines;
	register int	postlines;
	register int	totallines;

	prelines = 0;
	for (lp = lforw(curbp->b_linep); lp != curwp->w_dotp; lp = lforw(lp)) {
		prelines++;
	}
	if (curwp->w_doto > 0) {	/* "dot" is in the line. */
		prelines++;
	}
	postlines = 0;
	for (lp = curwp->w_dotp; lp != curbp->b_linep; lp = lforw(lp)) {
		postlines++;
	}
	if (curwp->w_dotp == lback(curbp->b_linep) &&	/* "dot" is at the */
	    curwp->w_doto == llength(curwp->w_dotp)) {	/* end of buffer.  */
		postlines = 0;
	} else if (llength(lback(curbp->b_linep)) == 0) { /* Last line has */
		                                          /* no text.      */
		postlines--;
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
regionlines(f, n)
{
	register LINE	*lp;
	register int	totallines;
	register int	counting;

	totallines = 0;
	if (curwp->w_dotp == curbp->b_markp) {
		if (curwp->w_doto != curbp->b_marko) {
			totallines = 1;
		}
	} else {
		counting = FALSE;
		for (lp = lforw(curbp->b_linep);
		     lp != curbp->b_linep; lp = lforw(lp)) {
			if (lp == curwp->w_dotp) {	
				if (counting) {	/* End of counting. */
					if (curwp->w_doto > 0) {
						totallines++;
					}
					break;
				} else {	/* Start of counting. */
					counting = TRUE;
				}
			} else if (lp == curbp->b_markp) {
				if (counting) {	/* End of counting. */
					if (curbp->b_marko > 0) {
						totallines++;
					}
					break;
				} else {	/* Start of counting. */
					counting = TRUE;
				}
			}
			if (counting) {
				totallines++;
			}
		}
	}
	ewprintf("Region has %d lines", totallines);
	return TRUE;
}

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

int
windowpos(wp)
register WINDOW *wp;
{
  extern int rowcol2offset pro((LINE *, int, int));
  LINE *lp, *targetlp;
  register BUFFER *bp = wp->w_bufp;
  int top = FALSE, bot = FALSE, off;
  int res = MG_RATIO_ALL;

  /* check if the beginning of top line is shown */
  if (wp->w_linep == lforw(bp->b_linep)) {
    if (wp->w_lines == 0) {
      top = TRUE;
    }
  }
  /* check if the end of the bottom line is shown */
  lp = wp->w_linep;
  off = rowcol2offset(lp, wp->w_lines + wp->w_ntrows, ncol);
  while (off < 0) {
    if (lforw(lp) != bp->b_linep) {
      lp = lforw(lp);
      off = rowcol2offset(lp, -off - 1, ncol);
    }
    else {
      bot = TRUE;
      break;
    }
  }
  if (top && bot) {
    res = MG_RATIO_ALL;
  }
  else if (top) {
    res = MG_RATIO_TOP;
  }
  else if (bot) {
    res = MG_RATIO_BOT;
  }
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
#endif	/* ADDFUNC */
