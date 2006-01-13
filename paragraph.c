/* $Id: paragraph.c,v 1.12.2.4 2006/01/13 18:07:38 amura Exp $ */
/*
 * Code for dealing with paragraphs and filling. Adapted from MicroEMACS 3.6
 * and GNU-ified by mwm@ucbvax.	 Several bug fixes by blarson@usc-oberon.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "paragraph.h"

#include "i_buffer.h"
#include "i_window.h"
#include "i_lang.h"
#include "basic.h"
#include "random.h"
#include "region.h"
#include "line.h"
#include "undo.h"
#include "echo.h"
#include "word.h"

static int fillcol = 70 ;
#define MG_MAXWORD 256

#ifdef	FILLPREFIX	/* 90.12.30  by S.Yoshida */
static int fillprefix_col = 0;	/* fill-prefix column length. */
static int fillprefix_len = 0;	/* fill-prefix byte length. */
#define PREFIXLENGTH 40
static NG_WCHAR_t fillprefix[PREFIXLENGTH] = { NG_EOS };
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
int
gotobop(f, n)
int f, n;
{
    if (n < 0)	/* the other way...*/
	return gotoeop(f, -n);

    while (n-- > 0) {	/* for each one asked for */
	
	/* first scan back until we are in a word */
	while (backchar(FFRAND, 1) && !inword())
	    ;
	curwp->w_doto = 0;	/* and go to the B-O-Line */

	/* and scan back until we hit a <NL><SP> <NL><TAB> or <NL><NL> */
	while (lback(curwp->w_dotp) != curbp->b_linep) {
#ifdef	FILLPREFIX	/* 91.01.01  by S.Yoshida */
	    /* We strip fill-prefix strings to scan back. */
	    if (llength(lback(curwp->w_dotp)) > fillprefix_len
		&& memcmp(ltext(curwp->w_dotp),
			  fillprefix,
			  sizeof(NG_WCHAR_t)*(1+fillprefix_len)) == 0
		&& lgetc(curwp->w_dotp,fillprefix_len) != NG_WSPACE
		&& lgetc(curwp->w_dotp,fillprefix_len) != NG_WTAB)
#else	/* NOT FILLPREFIX */
	    if (llength(lback(curwp->w_dotp))
		&& lgetc(curwp->w_dotp,0) != NG_WSPACE
		&& lgetc(curwp->w_dotp,0) != NG_WTAB)
#endif	/* FILLPREFIX */
		curwp->w_dotp = lback(curwp->w_dotp);
	    else
		break;
	}
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
int
gotoeop(f, n)
int f, n;
{
    if (n < 0)	/* the other way...*/
	return gotobop(f, -n);
    
    while (n-- > 0) {	/* for each one asked for */
	    
	/* Find the first word on/after the current line */
	curwp->w_doto = 0;
	while (forwchar(FFRAND, 1) && !inword())
	    ;
	curwp->w_doto = 0;
	curwp->w_dotp = lforw(curwp->w_dotp);
	/* and scan forword until we hit a <NL><SP> or ... */
	while (curwp->w_dotp != curbp->b_linep) {
#ifdef FILLPREFIX	/* 91.01.01  by S.Yoshida */
	    /* We strip fill-prefix strings to scan forword. */
	    if (llength(curwp->w_dotp) > fillprefix_len
		&& memcmp(ltext(curwp->w_dotp),
			  fillprefix,
			  sizeof(NG_WCHAR_t)*(1+fillprefix_len)) == 0
		&& lgetc(curwp->w_dotp,fillprefix_len) != NG_WSPACE
		&& lgetc(curwp->w_dotp,fillprefix_len) != NG_WTAB)
#else	/* NOT FILLPREFIX */
	    if (llength(curwp->w_dotp)
		&& lgetc(curwp->w_dotp,0) != NG_WSPACE
		&& lgetc(curwp->w_dotp,0) != NG_WTAB)
#endif	/* FILLPREFIX */
		curwp->w_dotp = lforw(curwp->w_dotp);
	    else
		break;
	}
	if (curwp->w_dotp == curbp->b_linep) {
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
int
fillpara(f, n)
int f, n;
{
    register int c;		/* current char durring scan	*/
    register int wordlen;	/* length of current word	*/
    register int wordcol;	/* column length of current word */
    register int clength;	/* position on line during fill */
    register int i;		/* index during word copy	*/
    register int eopflag;	/* Are we at the End-Of-Paragraph? */
    /* 90.01.29  by S.Yoshida */
    int lastsepchar;		/* last separater char.		*/
    int ksepflag;		/* must separate with KANJI ?	*/
    int kexist;			/* this word contain KANJI ?	*/
    int kstart;			/* this word start with KANJI ?	*/
    int kend;			/* last word end with KANJI ?	*/
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
    int lastiseolkc;		/* last is EOL KINSOKU char ?	*/
    int bolkclen;		/* BOL KINSOKU char byte length. */
#endif	/* KINSOKU */
    int firstflag;		/* first word? (needs no space) */
    int newlength;		/* tentative new line length	*/
    int eolflag;		/* was at end of line		*/
    LINE *eopline;		/* pointer to line just past EOP */
    NG_WCHAR_t wbuf[MG_MAXWORD];	/* buffer for current word	*/
    int (*lm_width)(NG_WCHAR_ta) = curbp->b_lang->lm_width;
    int cwidth;
#ifdef  VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    /* record the pointer to the line just past the EOP */
    (VOID) gotoeop(FFRAND, 1);
    if (curwp->w_doto != 0) {
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
    while (!inword() && forwchar(FFRAND, 1))
	;
    for (clength = 0, i = 0; i < curwp->w_doto; i++) {
	c = lgetc(curwp->w_dotp, i);
	if (c == NG_WTAB && !(curbp->b_flag & BFNOTAB)) {
#ifdef	VARIABLE_TAB
	    clength = (clength/tab + 1)*tab;
#else
	    clength |= 0x07;
	    clength++;
#endif
	}
	else
	    clength += lm_width(c);
    }
    wordcol = 0;
    wordlen = 0;
    
    /* scan through lines, filling words */
    firstflag = TRUE;
    eopflag = FALSE;
    lastsepchar = NG_EOS;
    kexist      = FALSE;
    kstart      = FALSE;
    kend        = FALSE;
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
    lastiseolkc = FALSE;
    bolkclen    = 0;
#endif /* KINSOKU */
    while (!eopflag) {
	/* get the next character in the paragraph */
	c = lgetc(curwp->w_dotp, curwp->w_doto);
	cwidth = lm_width(c);
	eolflag = ((curwp->w_doto + cwidth-1)== llength(curwp->w_dotp));
	if (eolflag) {
	    c = NG_WSPACE;
	    cwidth = 1;
	    if (lforw(curwp->w_dotp) == eopline)
		eopflag = TRUE;
	}
	
	if (!eopflag && ISBREAKABLE2(curbp->b_lang, c)) {
	    if (wordlen > 0
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
		&& lastiseolkc == NG_EOS && !isbolkchar(c)
#endif
		) {
		ksepflag = TRUE;
	    }
	    else {
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
		if (isbolkchar(c))
		    bolkclen += cwidth;
		lastiseolkc = iseolkchar(c);
#endif
		if (wordlen == 0)
		    kstart = TRUE;
		kexist = TRUE;
	    }
	}
	
	if (0 < wordlen && ISBREAKABLE2(curbp->b_lang, wbuf[wordlen]) &&
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
	    !lastiseolkc && !isbolkchar(c) &&
#endif /* KINSOKU */
	    c != NG_WSPACE && c != NG_WTAB) {
	    ksepflag = TRUE;
	}
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
	if (kexist && isbolkchar(c))
	    bolkclen++;
	lastiseolkc = iseolkchar(c);
#endif /* KINSOKU */
	/* and then delete it */
	if (!ksepflag) {
	    if (ldelete((RSIZE) 1, KNONE) == FALSE && !eopflag)
		return FALSE;
	}
	
#ifdef FILLPREFIX	/* 90.12.31  by S.Yoshida */
	/* and if EOL & not EOP, delete fill-prefix strings. */
	if (eolflag && !eopflag) {
	    if (ldelete((RSIZE) fillprefix_len, KNONE) == FALSE)
		return FALSE;
	}
#endif	/* FILLPREFIX */
	    
	/* if not a separator, just add it in */
	if (c != NG_WSPACE && c != NG_WTAB && !ksepflag) {
	    if (wordlen < NG_WCHARLEN(wbuf) - 1) {
		wbuf[wordlen++] = c;
#if 0
		if (c == NG_WTAB && !(curbp->b_flag & BFNOTAB)) {
#ifdef VARIABLE_TAB
		    wordcol = (wordcol/tab + 1)*tab;
#else
		    wordcol |= 0x07;
		    wordcol++;
#endif
		}
		else
		    wordcol += lm_width(c);
#else
		wordcol += lm_width(c);
#endif
	    }
	    else {
		/* You loose chars beyond MG_MAXWORD if the word
		 * is to long. I'm to lazy to fix it now; it
		 * just silently truncated the word before, so
		 * I get to feel smug.
		 */
		ewprintf("Word too long!");
	    }
	}
	else if (wordlen > 0) {
	    /* calculate tenatitive new length with word added */
	    newlength = clength + wordcol
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
		- bolkclen
#endif
		+ ((!firstflag &&
		    (lastsepchar == NG_WSPACE ||
		     lastsepchar == NG_WTAB ||
		     (lastsepchar == NG_WCODE('\n') && !kend && !kstart)))
		   ? 1 : 0);
	    /* if at end of line or at doublespace and previous
	     * character was one of '.','?','!' doublespace here.
	     */
	    if ((eolflag
		 || curwp->w_doto==llength(curwp->w_dotp)
		 || (c=lgetc(curwp->w_dotp,curwp->w_doto))==NG_WSPACE
		 || c==NG_WTAB)
		&& 0 < wordlen && ISEOSP(wbuf[wordlen - 1])
		&& wordlen<MG_MAXWORD-1) {
		wbuf[wordlen++] = NG_WSPACE;
		++wordcol;
	    }
	    /* at a word break with a word waiting */
	    if (newlength <= fillcol) {
		/* add word to current line */
		/* 90.01.29  by S.Yoshida */
		if (!firstflag &&
		    (lastsepchar == NG_WSPACE||lastsepchar == NG_WTAB ||
		     (lastsepchar == NG_WCODE('\n') && !kend && !kstart))) {
#ifdef UNDO
		    if (undoptr!=NULL && *undoptr!=NULL)
			(*undoptr)->u_type = UDNONE;
#endif
		    (VOID) linsert(1, NG_WSPACE);
		    ++clength;
		}
		firstflag = FALSE;
	    }
	    else {
		if (curwp->w_doto > 0 &&
		   lgetc(curwp->w_dotp,curwp->w_doto-1)==NG_WSPACE) {
		    curwp->w_doto -= 1;
		    (VOID) ldelete((RSIZE) 1, KNONE);
		}
		/* start a new line */
		(VOID) lnewline();
#ifdef FILLPREFIX	/* 90.12.31  by S.Yoshida */
		/* and add the fill-prefix strings. */
		{
		    NG_WCHAR_t *cp = fillprefix;
#ifdef UNDO
		    if (undoptr != NULL) {
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
		    }
		    else
#endif /* UNDO */
		    while (*cp) {
			if (linsert(1, *cp) == FALSE)
			    return FALSE;
			cp++;
		    }
		}
		clength = fillprefix_col;
#else /* NOT FILLPREFIX */
		clength = 0;
#endif /* FILLPREFIX */
	    }

	    /* and add the word in in either case */
#ifdef UNDO
	    if (undoptr != NULL) {
		if (*undoptr != NULL)
		    (*undoptr)->u_type = UDNONE;
		if (wordlen > 0)
		    (VOID) linsert(1, wbuf[0]);
		for (i=1; i<wordlen; i++) {
		    undoptr = undobefore;
		    (VOID) linsert(1, wbuf[i]);
		}
	    }
	    else
#endif
	    for (i=0; i<wordlen; i++)
		(VOID) linsert(1, wbuf[i]);
	    clength += wordcol;
	    lastsepchar = ksepflag ? NG_EOS : (eolflag ? NG_WCODE('\n'): c);
	    kexist = FALSE;
	    kstart = FALSE;
	    kend   = (0 < wordlen && ISBREAKABLE2(curbp->b_lang, wbuf[wordlen]));
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
	    bolkclen = 0;
#endif /* KINSOKU */
	    wordlen = 0;
	    wordcol = 0;
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
int
killpara(f, n)
int f, n;
{
    register int status;	/* returned status of functions */

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */

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
int
fillword(f, n)
int f, n;
{
    register char c;
    register int col, i, nce;
    int (*lm_width)(NG_WCHAR_ta) = curbp->b_lang->lm_width;
#ifdef  VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */
    
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    for (i = col = 0; col < fillcol; ++i) {
	if (i == curwp->w_doto)
	    return selfinsert(f, n) ;
	c = lgetc(curwp->w_dotp, i);
	if (c == NG_WTAB && !(curbp->b_flag & BFNOTAB)) {
#ifdef	VARIABLE_TAB
	    col = (col/tab + 1)*tab;
#else
	    col |= 0x07;
	    col++;
#endif
	}
	else
	    col += lm_width(c);
    }
    if (curwp->w_doto != llength(curwp->w_dotp)) {
	(VOID) selfinsert(f, n);
	nce = llength(curwp->w_dotp) - curwp->w_doto;
    }
    else
        nce = 0;
    
    curwp->w_doto = i;

#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
    /* Skip BOL (begin of line) KINSOKU chars to not separate with it. */
    if (isbolkchar(lgetc(curwp->w_dotp, curwp->w_doto))) {
	do {
	    (VOID) forwchar(FFRAND, 1);
	} while (curwp->w_doto < llength(curwp->w_dotp) &&
		 isbolkchar(lgetc(curwp->w_dotp, curwp->w_doto)));
	if (curwp->w_doto == llength(curwp->w_dotp)) {
	    if (inkfill)
		curwp->w_doto -= nce;
	    else {
		(VOID) lnewline();
		curwp->w_doto = 0;
		curwp->w_flag |= WFMOVE;
	    }
	    return TRUE;
	}
    }
#endif /* KINSOKU */

    if (curwp->w_doto < llength(curwp->w_dotp) && curwp->w_doto > 0 &&
	((c = lgetc(curwp->w_dotp, curwp->w_doto)) != NG_WSPACE
	 && c != NG_WTAB && !ISBREAKABLE2(curbp->b_lang, c)
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
	 || iseolkchar(lgetc(curwp->w_dotp, curwp->w_doto - 1))
#endif /* KINSOKU */
	)) {
	do {
	    (VOID) backchar(FFRAND, 1);
	} while (curwp->w_doto > 0 &&
		 ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != NG_WSPACE
		  && c != NG_WTAB && !ISBREAKABLE2(curbp->b_lang, c)
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
		  || iseolkchar(lgetc(curwp->w_dotp, curwp->w_doto - 1))
#endif /* KINSOKU */
		  ));
    }

    if (curwp->w_doto == 0) {
	do {
	    (VOID) forwchar(FFRAND, 1);
	} while (curwp->w_doto < llength(curwp->w_dotp) &&
		 ((c = lgetc(curwp->w_dotp, curwp->w_doto)) != NG_WSPACE
		  && c != NG_WTAB && !ISBREAKABLE2(curbp->b_lang, c)
#ifdef KINSOKU	/* 90.01.29  by S.Yoshida */
		  || iseolkchar(lgetc(curwp->w_dotp, curwp->w_doto - 1))
#endif /* KINSOKU */
		  ));
    } /* end of if (curwp->w_doto == 0) */
    (VOID) delwhite(FFRAND, 1);
    (VOID) lnewline();
#ifdef FILLPREFIX	/* 90.12.31  by S.Yoshida */
    /* Add the fill-prefix strings at the begin of line. */
    {
	NG_WCHAR_t *cp = fillprefix;

#ifdef UNDO
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
	}
	else
#endif
	while (*cp) {
	    if (linsert(1, *cp) == FALSE)
		return FALSE;
	    cp++;
	}
    }
#endif /* FILLPREFIX */
    i = llength(curwp->w_dotp) - nce;
#ifdef FILLPREFIX	/* 90.12.31  by S.Yoshida */
    curwp->w_doto = i>fillprefix_len ? i : fillprefix_len;
#else /* NOT FILLPREFIX */
    curwp->w_doto = i>0 ? i : 0;
#endif /* FILLPREFIX */
    curwp->w_flag |= WFMOVE;
#ifdef FILLPREFIX	/* 90.12.31  by S.Yoshida */
    if (nce == 0 && curwp->w_doto > fillprefix_len)
#else /* NOT FILLPREFIX */
    if (nce == 0 && curwp->w_doto != 0)
#endif /* FILLPREFIX */
	return fillword(f, n);
    return TRUE;
}

/* Set fill column to n. */
int
setfillcol(f, n)
int f, n;
{
    fillcol = ((f & FFARG) ? n : (getcolpos() - 1));
    ewprintf("Fill column set to %d", fillcol);
    return TRUE;
}

#ifdef FILLPREFIX	/* 90.12.30  by S.Yoshida */
/*
 * Set fill-prefix strings.
 */
int
setfillprefix(f, n)
int f, n;
{
    register NG_WCHAR_t c;
    register int col, i;
    int (*lm_width)(NG_WCHAR_ta) = curbp->b_lang->lm_width;
#ifdef  VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */
    
    fillprefix_col = getcolpos() - 1;
    if (fillprefix_col == 0) {
	fillprefix_len = 0;
	fillprefix[0] = '\0';
	ewprintf("fill-prefix cancelled");
    }
    else if (fillprefix_col + 1 > PREFIXLENGTH) {
	ewprintf("fill-prefix is too long; unchanged (\"%ls\")",
		 fillprefix);
    }
    else {
	for (i = col = 0; col < fillprefix_col; ++i) {
	    c = lgetc(curwp->w_dotp, i);
	    if (c == NG_WTAB && !(curbp->b_flag & BFNOTAB)) {
#ifdef	VARIABLE_TAB

		col = (col/tab + 1)*tab;
#else
	        col |= 0x07;
		col+=;
#endif
	    }
	    else
		col += lm_width(c);
	    fillprefix[i] = c;
	}
	fillprefix_len = i;
	fillprefix[i] = NG_EOS;
	ewprintf("fill-prefix: \"%ls\"", fillprefix);
    }
    return TRUE;
}
#endif	/* FILLPREFIX */
