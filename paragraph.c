/* $Id: paragraph.c,v 1.7.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 * Code for dealing with paragraphs and filling. Adapted from MicroEMACS 3.6
 * and GNU-ified by mwm@ucbvax.	 Several bug fixes by blarson@usc-oberon.
 */

/*
 * $Log: paragraph.c,v $
 * Revision 1.7.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.7  2001/07/18 14:56:09  amura
 * fix silly bug
 *
 * Revision 1.6  2001/06/19 15:23:19  amura
 * to make uniform all indent
 *
 * Revision 1.5  2001/05/25 15:36:53  amura
 * now buffers have only one mark (before windows have one mark)
 *
 * Revision 1.4  2000/10/02 16:24:42  amura
 * bugfix by Tillanosoft(Ng for Win32)
 *
 * Revision 1.3  2000/07/16 15:44:41  amura
 * undo bug on autofill fixed
 *
 * Revision 1.2  2000/06/27 01:49:44  amura
 * import to CVS
 *
 * Revision 1.1  2000/06/01  05:34:38  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#ifdef	UNDO
#include	"undo.h"
#endif

static int	fillcol = 70 ;
#define MG_MAXWORD 256

#ifdef	FILLPREFIX	/* 90.12.30  by S.Yoshida */
static int	fillprefix_col = 0;	/* fill-prefix column length. */
static int	fillprefix_len = 0;	/* fill-prefix byte length. */
#define PREFIXLENGTH 40
static char fillprefix[PREFIXLENGTH] = { '\0' };
#endif	/* FILLPREFIX */

/*
 * go back to the begining of the current paragraph
 * here we look for a <NL><NL> or <NL><TAB> or <NL><SPACE>
 * combination to delimit the begining of a paragraph
#ifdef	FILLPREFIX
 * (91.01.01  Add comment by S.Yoshida)
 * We also look for a line starting without fill-prefix strings.
 * If line is started with fill-prefix string, we strip it to
 * look for a <NL><NL> etc.
#endif
 */
/*ARGSUSED*/
gotobop(f, n)
{
	if (n < 0)	/* the other way...*/
		return gotoeop(f, -n);

	while (n-- > 0) {	/* for each one asked for */

		/* first scan back until we are in a word */
		
		while (backchar(FFRAND, 1) && !inword()) {}
		curwp->w_doto = 0;	/* and go to the B-O-Line */

		/* and scan back until we hit a <NL><SP> <NL><TAB> or <NL><NL> */
		while (lback(curwp->w_dotp) != curbp->b_linep)
#ifdef	FILLPREFIX	/* 91.01.01  by S.Yoshida */
			/* We strip fill-prefix strings to scan back. */
			if (llength(lback(curwp->w_dotp)) > fillprefix_len
			    && strncmp(ltext(curwp->w_dotp),
				       fillprefix, fillprefix_len) == 0
			    && lgetc(curwp->w_dotp,fillprefix_len) != ' '
			    && lgetc(curwp->w_dotp,fillprefix_len) != '\t')
#else	/* NOT FILLPREFIX */
			if (llength(lback(curwp->w_dotp))
			    && lgetc(curwp->w_dotp,0) != ' '
			    && lgetc(curwp->w_dotp,0) != '\t')
#endif	/* FILLPREFIX */
				curwp->w_dotp = lback(curwp->w_dotp);
			else
				break;
	}
	curwp->w_flag |= WFMOVE;	/* force screen update */
	return TRUE;
}

/*
 * go forword to the end of the current paragraph
 * here we look for a <NL><NL> or <NL><TAB> or <NL><SPACE>
 * combination to delimit the begining of a paragraph
#ifdef	FILLPREFIX
 * (91.01.01  Add comment by S.Yoshida)
 * We also look for a line starting without fill-prefix strings.
 * If line is started with fill-prefix string, we strip it to
 * look for a <NL><NL> etc.
#endif
 */
/*ARGSUSED*/
gotoeop(f, n)
{
	if (n < 0)	/* the other way...*/
		return gotobop(f, -n);

	while (n-- > 0) {	/* for each one asked for */

		/* Find the first word on/after the current line */
		curwp->w_doto = 0;
		while(forwchar(FFRAND, 1) && !inword()) {}
		curwp->w_doto = 0;
		curwp->w_dotp = lforw(curwp->w_dotp);
		/* and scan forword until we hit a <NL><SP> or ... */
		while (curwp->w_dotp != curbp->b_linep) {
#ifdef	FILLPREFIX	/* 91.01.01  by S.Yoshida */
			/* We strip fill-prefix strings to scan forword. */
			if (llength(curwp->w_dotp) > fillprefix_len
			    && strncmp(ltext(curwp->w_dotp),
				       fillprefix, fillprefix_len) == 0
			    && lgetc(curwp->w_dotp,fillprefix_len) != ' '
			    && lgetc(curwp->w_dotp,fillprefix_len) != '\t')
#else	/* NOT FILLPREFIX */
			if (llength(curwp->w_dotp)
			    && lgetc(curwp->w_dotp,0) != ' '
			    && lgetc(curwp->w_dotp,0) != '\t')
#endif	/* FILLPREFIX */
				curwp->w_dotp = lforw(curwp->w_dotp);
			else
				break;
		}
		if(curwp->w_dotp == curbp->b_linep) {
			/* beond end of buffer, cleanup time */
			curwp->w_dotp = lback(curwp->w_dotp);
			curwp->w_doto = llength(curwp->w_dotp);
			break;			
		}
	}
	curwp->w_flag |= WFMOVE;	/* force screen update */
	return TRUE;
}

/*
 * Fill the current paragraph according to the current
 * fill column
#ifdef	FILLPREFIX
 * (91.01.01  Add comment by S.Yoshida)
 * with current fill-prefix strings.
#endif
 */
/*ARGSUSED*/
fillpara(f, n)
{
	register int	c;		/* current char durring scan	*/
	register int	wordlen;	/* length of current word	*/
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
	register int	wordcol;	/* column length of current word */
#endif	/* BUGFIX */
	register int	clength;	/* position on line during fill */
	register int	i;		/* index during word copy	*/
	register int	eopflag;	/* Are we at the End-Of-Paragraph? */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	c2;		/* KANJI 2nd byte.		*/
	int		lastsepchar;	/* last separater char.		*/
	int		ksepflag;	/* must separate with KANJI ?	*/
	int		kexist;		/* this word contain KANJI ?	*/
	int		kstart;		/* this word start with KANJI ?	*/
	int		kend;		/* last word end with KANJI ?	*/
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
	int		lastiseolkc;	/* last is EOL KINSOKU char ?	*/
	int		bolkclen;	/* BOL KINSOKU char byte length. */
#endif	/* KINSOKU */
#endif	/* KANJI */
	int		firstflag;	/* first word? (needs no space) */
	int		newlength;	/* tentative new line length	*/
	int		eolflag;	/* was at end of line		*/
	LINE		*eopline;	/* pointer to line just past EOP */
	char wbuf[MG_MAXWORD];		/* buffer for current word	*/
#ifdef  VARIABLE_TAB
	int		tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	/* record the pointer to the line just past the EOP */
	(VOID) gotoeop(FFRAND, 1);
	if(curwp->w_doto != 0)	{
		/* paragraph ends at end of buffer */
		(VOID) lnewline();
		/* In order to reduce the complexity, simply add a
		   newline (by the above code) so that the rest of the
		   source code will not be affected by the exceptional
		   paragraph.  This idea was brought to the source
		   code by the original authors, but there was a bug
		   which over-delete one word at the end of the
		   paragraph in the case the paragraph ends with EOB
		   and there is a certain FILLPREFIX string for the
		   paragraph.  This bug was brought to the source code
		   when FILLPREFIX feature had been introduced.  This
		   bug has been fixed by re-writing the way to get the
		   `eopline' here.  Tillanosoft Sep 4, 2000 */
		(VOID)gotoeop(FFRAND, 1);
	}
	eopline = curwp->w_dotp;

	/* and back top the begining of the paragraph */
	(VOID) gotobop(FFRAND, 1);

	/* initialize various info */
	while (!inword() && forwchar(FFRAND, 1)) {}
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
	for (clength = 0, i = 0; i < curwp->w_doto; i++) {
		c = lgetc(curwp->w_dotp, i);
		if (c == '\t' && !(curbp->b_flag & BFNOTAB))
#ifdef	VARIABLE_TAB
			clength = (clength/tab + 1)*tab - 1;
#else
			clength |= 0x07;
#endif
		else if (ISCTRL(c) != FALSE)
			++clength;
		++clength;
	}
	wordcol = 0;
#else	/* ORIGINAL */
	clength = curwp->w_doto;
#endif	/* BUGFIX */
	wordlen = 0;

	/* scan through lines, filling words */
	firstflag = TRUE;
	eopflag = FALSE;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	lastsepchar = '\0';	/* 91.01.16  NULL -> '\0' */
	kexist      = FALSE;
	kstart      = FALSE;
	kend        = FALSE;
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
	lastiseolkc = FALSE;
	bolkclen    = 0;
#endif	/* KINSOKU */
#endif	/* KANJI */
	while (!eopflag) {
		/* get the next character in the paragraph */
		if (eolflag=(curwp->w_doto == llength(curwp->w_dotp))) {
			c = ' ';
			if (lforw(curwp->w_dotp) == eopline)
				eopflag = TRUE;
		} else
			c = lgetc(curwp->w_dotp, curwp->w_doto);

#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		c2 = '\0';	/* 91.01.15  NULL -> '\0' */
		ksepflag = FALSE;
		if (ISKANJI(c)) {
			if (eolflag=((curwp->w_doto + 1) ==
				     llength(curwp->w_dotp))) {
				c = ' ';
				if (lforw(curwp->w_dotp) == eopline)
					eopflag = TRUE;
			} else {
				c2 = lgetc(curwp->w_dotp, curwp->w_doto + 1);
				if (wordlen > 0
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
				    && !lastiseolkc && !isbolkchar(c, c2)
#endif	/* KINSOKU */
				   ) {
					c2 = '\0'; /* 91.01.15  NULL -> '\0' */
					ksepflag = TRUE;
				} else {
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
					if (isbolkchar(c, c2)) {
						bolkclen += 2;
					}
					lastiseolkc = iseolkchar(c, c2);
#endif	/* KINSOKU */
					if (wordlen == 0) {
						kstart = TRUE;
					}
					kexist = TRUE;
				}
			}
		} else {
			if (0 < wordlen && ISKANJI(wbuf[wordlen - 1]) &&
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
			    !lastiseolkc && !isbolkchar(0, c) &&
#endif	/* KINSOKU */
			    c != ' ' && c != '\t') {
				ksepflag = TRUE;
			}
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
			if (kexist && isbolkchar(0, c)) {
				bolkclen++;
			}
			lastiseolkc = iseolkchar(0, c);
#endif	/* KINSOKU */
		}

		/* and then delete it */
		if (!ksepflag)
			if (ldelete((RSIZE) (c2 ? 2 : 1), KNONE) == FALSE &&
			    !eopflag)
#else	/* NOT KANJI */
		/* and then delete it */
		if (ldelete((RSIZE) 1, KNONE) == FALSE && !eopflag)
#endif	/* KANJI */
			return FALSE;

#ifdef	FILLPREFIX	/* 90.12.31  by S.Yoshida */
		/* and if EOL & not EOP, delete fill-prefix strings. */
		if (eolflag && !eopflag) {
			if (ldelete((RSIZE) fillprefix_len, KNONE) == FALSE)
				return FALSE;
		}
#endif	/* FILLPREFIX */

		/* if not a separator, just add it in */
#ifndef	KANJI	/* 90.01.29  by S.Yoshida */
		if (c != ' ' && c != '\t') {
#else	/* KANJI */
		if (c != ' ' && c != '\t' && !ksepflag) {
			if (c2 != '\0') {	/* 91.01.15  NULL -> '\0' */
				if (wordlen < MG_MAXWORD - 2) {
					wbuf[wordlen++] = c;
					wbuf[wordlen++] = c2;
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
					wordcol += 2;
#endif	/* BUGFIX */
				} else {
					ewprintf("Word too long!");
				}
			} else
#endif	/* KANJI */
			if (wordlen < MG_MAXWORD - 1) {
				wbuf[wordlen++] = c;
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
				if (c == '\t' && !(curbp->b_flag & BFNOTAB))
#ifdef	VARIABLE_TAB
					wordcol = (wordcol/tab + 1)*tab -1;
#else
					wordcol |= 0x07;
#endif
				else if (ISCTRL(c) != FALSE)
					++wordcol;
				++wordcol;
#endif	/* BUGFIX */
			} else {
				/* You loose chars beyond MG_MAXWORD if the word
				 * is to long. I'm to lazy to fix it now; it
				 * just silently truncated the word before, so
				 * I get to feel smug.
				 */
				ewprintf("Word too long!");
			}
		} else if (wordlen) {
			/* calculate tenatitive new length with word added */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			c2 = c;		/* save separate char.	*/
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
			newlength = clength + wordcol
#else	/* ORIGINAL */
			newlength = clength + wordlen
#endif	/* BUGFIX */
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
				- bolkclen
#endif	/* KINSOKU */
				+ ((!firstflag &&
				  (lastsepchar == ' ' ||
				   lastsepchar == '\t' ||
				   (lastsepchar == '\n' &&
				    !kend && !kstart)))
				 ? 1 : 0);
#else	/* NOT KANJI */
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
			newlength = clength + 1 + wordcol;
#else	/* ORIGINAL */
			newlength = clength + 1 + wordlen;
#endif	/* BUGFIX */
#endif	/* KANJI */
			/* if at end of line or at doublespace and previous
			 * character was one of '.','?','!' doublespace here.
			 */
			if((eolflag
			    || curwp->w_doto==llength(curwp->w_dotp)
			    || (c=lgetc(curwp->w_dotp,curwp->w_doto))==' '
			    || c=='\t')
			  && 0 < wordlen && ISEOSP(wbuf[wordlen - 1])
			  && wordlen<MG_MAXWORD-1) {
				wbuf[wordlen++] = ' ';
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
				++wordcol;
#endif	/* BUGFIX */
			}
			/* at a word break with a word waiting */
			if (newlength <= fillcol) {
				/* add word to current line */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
				if (!firstflag && (lastsepchar == ' ' ||
						   lastsepchar == '\t' ||
						   (lastsepchar == '\n' &&
						    !kend && !kstart))) {
#else	/* NOT KANJI */
				if (!firstflag) {
#endif	/* KANJI */
#ifdef	UNDO
					if (undoptr!=NULL && *undoptr!=NULL)
						(*undoptr)->u_type = UDNONE;
#endif
					(VOID) linsert(1, ' ');
					++clength;
				}
				firstflag = FALSE;
			} else {
				if(curwp->w_doto > 0 &&
				    lgetc(curwp->w_dotp,curwp->w_doto-1)==' ') {
					curwp->w_doto -= 1;
					(VOID) ldelete((RSIZE) 1, KNONE);
				}
				/* start a new line */
				(VOID) lnewline();
#ifdef	FILLPREFIX	/* 90.12.31  by S.Yoshida */
				/* and add the fill-prefix strings. */
				{
					char	*cp = fillprefix;

#ifdef	UNDO
				    if (undoptr != NULL) {
					if (*undoptr != NULL)
						(*undoptr)->u_type = UDNONE;
					if (*cp)
						if (linsert(1, *cp++) == FALSE)
							return FALSE;
					while (*cp) {
						undoptr = undobefore;
						if (linsert(1, *cp) == FALSE)
							return FALSE;
						cp++;
					}
				    } else
#endif
					while (*cp) {
						if (linsert(1, *cp) == FALSE)
							return FALSE;
						cp++;
					}
				}
				clength = fillprefix_col;
#else	/* NOT FILLPREFIX */
				clength = 0;
#endif	/* FILLPREFIX */
			}

			/* and add the word in in either case */
#ifdef	UNDO
			if (undoptr != NULL) {
				if (*undoptr != NULL)
					(*undoptr)->u_type = UDNONE;
				if (wordlen > 0) {
					(VOID) linsert(1, wbuf[0]);
				}
				for (i=1; i<wordlen; i++) {
					undoptr = undobefore;
					(VOID) linsert(1, wbuf[i]);
				}
			} else
#endif
			for (i=0; i<wordlen; i++) {
				(VOID) linsert(1, wbuf[i]);
#ifndef	BUGFIX	/* 91.01.02  by S.Yoshida */
				++clength;
#endif	/* BUGFIX */
			}
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
			clength += wordcol;
#endif	/* BUGFIX */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			lastsepchar = ksepflag ? '\0' : (eolflag ? '\n' : c2);
				/* 91.01.15  NULL -> '\0' */
			kexist = FALSE;
			kstart = FALSE;
			kend   = (0 < wordlen && ISKANJI(wbuf[wordlen - 1]));
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
			bolkclen = 0;
#endif	/* KINSOKU */
#endif	/* KANJI */
			wordlen = 0;
#ifdef	BUGFIX	/* 91.01.02  by S.Yoshida */
			wordcol = 0;
#endif	/* BUGFIX */
		}
	}
	/* and add a last newline for the end of our new paragraph */
	(VOID) lnewline();
	/* we realy should wind up where we started, (which is hard to keep
	 * track of) but I think the end of the last line is better than the
	 * begining of the blank line.	 */
	(VOID) backchar(FFRAND, 1);
	return TRUE;
}

/* delete n paragraphs starting with the current one */
/*ARGSUSED*/
killpara(f, n)
{
	register int status;	/* returned status of functions */

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	while (n--) {		/* for each paragraph to delete */

		/* mark out the end and begining of the para to delete */
		(VOID) gotoeop(FFRAND, 1);

		/* set the mark here */
		curbp->b_markp = curwp->w_dotp;
		curbp->b_marko = curwp->w_doto;

		/* go to the begining of the paragraph */
		(VOID) gotobop(FFRAND, 1);
		curwp->w_doto = 0;	/* force us to the begining of line */

		/* and delete it */
		if ((status = killregion(FFRAND, 1)) != TRUE)
			return status;

		/* and clean up the 2 extra lines */
		(VOID) ldelete((RSIZE) 1, KFORW);
	}
	return TRUE;
}

#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#ifndef	BUGFIX
#define	BUGFIX
#endif	/* BUGFIX */
#endif	/* KANJI */
/*
 * check to see if we're past fillcol, and if so,
 * justify this line. As a last step, justify the line.
#ifdef	FILLPREFIX
 * (91.01.01  Add comment by S.Yoshida)
 * When insert a newline, fill-prefix strings is added
 * at begin of the line.
#endif
 */
/*ARGSUSED*/
fillword(f, n)
{
	register char	c;
	register int	col, i, nce;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	kanji2nd = FALSE; /* Now we are on a KANJI 2nd byte. */
	int		kinserted = FALSE; /* KANJI 2nd byte has inserted. */
	extern int	inkfill;	/* Fill with KANJI 2nd byte.	*/
					/* This flag is defined at kbd.c. */
#endif	/* KANJI */
#ifdef  VARIABLE_TAB
	int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */


#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

#ifdef	BUGFIX	/* 90.01.29  by S.Yoshida */
	for (i = col = 0; col < fillcol; ++i, ++col) {
#else	/* ORIGINAL */
	for (i = col = 0; col <= fillcol; ++i, ++col) {
#endif	/* BUGFIX */
		if (i == curwp->w_doto) return selfinsert(f, n) ;
		c = lgetc(curwp->w_dotp, i);
		if (c == '\t'&& !(curbp->b_flag & BFNOTAB))
#ifdef	VARIABLE_TAB
			col = (col/tab + 1)*tab - 1;
#else
			col |= 0x07;
#endif
		else if (ISCTRL(c) != FALSE) ++col;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (kanji2nd) {		/* We think 'c' is KANJI 2nd byte. */
			kanji2nd = FALSE;
		} else if (ISKANJI(c)) { /* KANJI 1st byte.	*/
			kanji2nd = TRUE;
		}
#endif	/* KANJI */
	}
	if (curwp->w_doto != llength(curwp->w_dotp)) {
		(VOID) selfinsert(f, n);
		nce = llength(curwp->w_dotp) - curwp->w_doto;
#ifndef	KANJI	/* 90.01.29  by S.Yoshida */
	} else nce = 0;
#else	/* KANJI */
        } else {
		nce = 0;
		if (inkfill) {
			/* To check KINSOKU char, we need KANJI 2nd byte. */
			(VOID) selfinsert(f, n); /* Insert KANJI 2nd byte. */
		}
	}
	if (kanji2nd) {			/* When stop at KANJI 2nd byte, */
		i--;			/* Go to KANJI 1st byte.	*/
	}
#endif	/* KANJI */
	curwp->w_doto = i;

#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
	/* Skip BOL (begin of line) KINSOKU chars to not separate with it. */
	if (ISKANJI(c = lgetc(curwp->w_dotp, curwp->w_doto)) &&
	    isbolkchar(c, lgetc(curwp->w_dotp, curwp->w_doto + 1)) ||
	    !ISKANJI(c) && isbolkchar(0, c)) {
		do {
			(VOID) forwchar(FFRAND, 1);
		} while (curwp->w_doto < llength(curwp->w_dotp) &&
			 (ISKANJI(c = lgetc(curwp->w_dotp, curwp->w_doto)) &&
			 isbolkchar(c, lgetc(curwp->w_dotp, curwp->w_doto + 1))
			 || !ISKANJI(c) && isbolkchar(0, c)));
		if (curwp->w_doto == llength(curwp->w_dotp)) {
			if (inkfill) {
				curwp->w_doto -= nce;
			} else {
				(VOID) lnewline();
				curwp->w_doto = 0;
				curwp->w_flag |= WFMOVE;
			}
			return TRUE;
		}
	}
#endif	/* KINSOKU */

	if (curwp->w_doto < llength(curwp->w_dotp) && curwp->w_doto > 0 &&
	    ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' ' && c != '\t' &&
	    !ISKANJI(c) && !ISKANJI(lgetc(curwp->w_dotp, curwp->w_doto - 1))
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
	    || (!ISKANJI((c = lgetc(curwp->w_dotp, curwp->w_doto - 1))) &&
		iseolkchar(0, c) || curwp->w_doto > 1 && ISKANJI(c) &&
		iseolkchar(lgetc(curwp->w_dotp, curwp->w_doto - 2), c))
#endif	/* KINSOKU */
	    ))
#else	/* NOT KANJI */
#ifdef	BUGFIX	/* 90.01.29  by S.Yoshida */
	if (curwp->w_doto < llength(curwp->w_dotp) &&
	    (c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' ' && c != '\t')
#else	/* ORIGINAL */
	if ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' ' && c != '\t')
#endif	/* BUGFIX */
#endif	/* KANJI */
		do {
			(VOID) backchar(FFRAND, 1);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		} while (curwp->w_doto > 0 &&
			 ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' '
			 && c != '\t' && !ISKANJI(c) &&
			 !ISKANJI(lgetc(curwp->w_dotp, curwp->w_doto - 1))
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
			 || (!ISKANJI((c = lgetc(curwp->w_dotp,
						 curwp->w_doto - 1))) &&
			     iseolkchar(0, c) ||
			     curwp->w_doto > 1 && ISKANJI(c) &&
			     iseolkchar(lgetc(curwp->w_dotp,
					      curwp->w_doto - 2), c))
#endif	/* KINSOKU */
			 ));
#else	/* NOT KANJI */
		} while ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' '
		      && c != '\t' && curwp->w_doto > 0);
#endif	/* KANJI */

	if (curwp->w_doto == 0)
		do {
			(VOID) forwchar(FFRAND, 1);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		} while (curwp->w_doto < llength(curwp->w_dotp) &&
			 ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' '
			  && c != '\t' && !ISKANJI(c) &&
			  !ISKANJI(lgetc(curwp->w_dotp, curwp->w_doto - 1))
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
			  || (!ISKANJI((c = lgetc(curwp->w_dotp,
						 curwp->w_doto - 1))) &&
			     iseolkchar(0, c) ||
			     curwp->w_doto > 1 && ISKANJI(c) &&
			     iseolkchar(lgetc(curwp->w_dotp,
					      curwp->w_doto - 2), c))
#endif	/* KINSOKU */
			));
#else	/* NOT KANJI */
		} while ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' '
		      && c != '\t' && curwp->w_doto < llength(curwp->w_dotp));
#endif	/* KANJI */

	(VOID) delwhite(FFRAND, 1);
	(VOID) lnewline();
#ifdef	FILLPREFIX	/* 90.12.31  by S.Yoshida */
	/* Add the fill-prefix strings at the begin of line. */
	{
		char	*cp = fillprefix;

#ifdef	UNDO
		if (isundo()) {
			if (*undoptr != NULL)
				(*undoptr)->u_type = UDNONE;
			if (*cp) {
				if (linsert(1, *cp++) == FALSE)
					return FALSE;
			}
			while (*cp) {
				undoptr = undobefore;
				if (linsert(1, *cp) == FALSE)
					return FALSE;
				cp++;
			}
		} else
#endif
		while (*cp) {
			if (linsert(1, *cp) == FALSE)
				return FALSE;
			cp++;
		}
	}
#endif	/* FILLPREFIX */
	i = llength(curwp->w_dotp) - nce;
#ifdef	FILLPREFIX	/* 90.12.31  by S.Yoshida */
	curwp->w_doto = i>fillprefix_len ? i : fillprefix_len;
#else	/* NOT FILLPREFIX */
	curwp->w_doto = i>0 ? i : 0;
#endif	/* FILLPREFIX */
	curwp->w_flag |= WFMOVE;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
        if (!inkfill)			/* previously inserted.	*/
#endif	/* KANJI */
#ifdef	FILLPREFIX	/* 90.12.31  by S.Yoshida */
	if (nce == 0 && curwp->w_doto > fillprefix_len) return fillword(f, n);
#else	/* NOT FILLPREFIX */
	if (nce == 0 && curwp->w_doto != 0) return fillword(f, n);
#endif	/* FILLPREFIX */
	return TRUE;
}

/* Set fill column to n. */
setfillcol(f, n) {
	extern int	getcolpos() ;

#ifdef	BUGFIX	/* 90.02.14  by S.Yoshida */
	fillcol = ((f & FFARG) ? n : (getcolpos() - 1));
#else	/* ORIGINAL */
	fillcol = ((f & FFARG) ? n : getcolpos());
#endif	/* BUGFIX */
	ewprintf("Fill column set to %d", fillcol);
	return TRUE;
}

#ifdef	FILLPREFIX	/* 90.12.30  by S.Yoshida */
/*
 * Set fill-prefix strings.
 */
setfillprefix(f, n) {
	register char	c;
	register int	col, i;
#ifdef  VARIABLE_TAB
	int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

	fillprefix_col = getcolpos() - 1;
	if (fillprefix_col == 0) {
		fillprefix_len = 0;
		fillprefix[0] = '\0';
		ewprintf("fill-prefix cancelled");
	} else if (fillprefix_col + 1 > PREFIXLENGTH) {
		ewprintf("fill-prefix is too long; unchanged (\"%s\")",
			 fillprefix);
	} else {
		for (i = col = 0; col < fillprefix_col; ++i, ++col) {
			c = lgetc(curwp->w_dotp, i);
			if (c == '\t' && !(curbp->b_flag & BFNOTAB) )
#ifdef	VARIABLE_TAB
				col = (col/tab + 1)*tab - 1;
#else
				col |= 0x07;
#endif
			else if (ISCTRL(c) != FALSE) ++col;
			fillprefix[i] = c;
		}
		fillprefix_len = i;
		fillprefix[i] = '\0';
		ewprintf("fill-prefix: \"%s\"", fillprefix);
	}
	return TRUE;
}
#endif	/* FILLPREFIX */
