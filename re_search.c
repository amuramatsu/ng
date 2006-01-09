/* $Id: re_search.c,v 1.6.2.4 2006/01/09 08:44:29 amura Exp $ */
/*
 *		Search commands with Regular Expression
 * The functions are remade from 'search.c' to help from GPL.
 * Now, all source code of Ng is not GPL except for win32 port.
 *
 * This file use T-Rex, the tiny regular expression routine
 * by Alberto Demichelis.
 */
/*
 *		Search commands.
 * The functions in this file implement the
 * search commands (both plain and incremental searches
 * are supported) and the query-replace command.
 *
 * The plain old search code is part of the original
 * MicroEMACS "distribution". The incremental search code,
 * and the query-replace code, is by Rich Ellison.
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */

#ifdef REGEX
#include "def.h"
#include "search.h"

#include "i_buffer.h"
#include "i_window.h"
#include "echo.h"
#include "kbd.h"
#include "basic.h"
#include "macro.h"
#include "line.h"	/* for lreplace() */
#include "word.h"	/* for forwword() */

#define ng
#include "trex.h"	/* use T-Rex as regex search routine */

#define SRCH_BEGIN	(0)			/* Search sub-codes.	*/
#define SRCH_FORW	(-1)
#define SRCH_BACK	(-2)
#define SRCH_NOPR	(-3)
#define SRCH_ACCM	(-4)
#define SRCH_MARK	(-5)

static int re_srch_lastdir = SRCH_NOPR;		/* Last search flags.	*/
static NG_WCHAR_t re_pat[NPAT];
static TRex *re_exp = NULL;

static int re_forwsrch _PRO((void));
static int re_backsrch _PRO((void));
static int re_doreplace _PRO((RSIZE, const NG_WCHAR_t *, int));
static int re_readpattern _PRO((const char *));
static int kill_match_lines _PRO((int));
static int count_match_lines _PRO((int));

/* casefoldsearch: Does search ignore case ? */
extern int casefoldsearch;

/*
 * Search forward.
 * Get a search string from the user, and search for it,
 * starting at ".". If found, "." gets moved to just after the
 * matched characters, and display does all the hard stuff.
 * If not found, it just prints a message.
 */
/*ARGSUSED*/
int
re_forwsearch(f, n)
int f, n;
{
    register int s;
    
    if ((s=re_readpattern("RE Search")) != TRUE)
	return s;
    if (re_forwsrch() == FALSE) {
	ewprintf("Search failed: \"%ls\"", re_pat);
	return FALSE;
    }
    re_srch_lastdir = SRCH_FORW;
    return TRUE;
}

/*
 * Reverse search.
 * Get a search string from the	 user, and search, starting at "."
 * and proceeding toward the front of the buffer. If found "." is left
 * pointing at the first character of the pattern [the last character that
 * was matched].
 */
/*ARGSUSED*/
int
re_backsearch(f, n)
int f, n;
{
    register int s;
    
    if ((s=re_readpattern("RE Search backward")) != TRUE)
	return (s);
    if (re_backsrch() == FALSE) {
	ewprintf("Search failed: \"%ls\"", re_pat);
	return FALSE;
    }
    re_srch_lastdir = SRCH_BACK;
    return TRUE;
}

/*
 * Search again, using the same search string
 * and direction as the last search command. The direction
 * has been saved in "srch_lastdir", so you know which way
 * to go.
 */
/*ARGSUSED*/
int
re_searchagain(f, n)
int f, n;
{
    if (re_srch_lastdir == SRCH_FORW) {
	if (re_forwsrch() == FALSE) {
	    ewprintf("Search failed: \"%ls\"", re_pat);
	    return FALSE;
	}
	return TRUE;
    }
    if (re_srch_lastdir == SRCH_BACK) {
	if (re_backsrch() == FALSE) {
	    ewprintf("Search failed: \"%ls\"", re_pat);
	    return FALSE;
	}
	return TRUE;
    }
    ewprintf("No last search");
    return FALSE;
}


/*
 * Query Replace.
 *	Replace strings selectively.  Does a search and replace operation.
 */
/*ARGSUSED*/
int
re_queryrepl(f, n)
int f, n;
{
    register int s;
    register int rcnt = 0;	/* Replacements made so far	*/
    NG_WCHAR_t news[NPAT];	/* replacement string		*/
    
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
#ifndef NO_MACRO
    if (macrodef) {
	ewprintf("Can't query replace in macro");
	return FALSE;
    }
#endif
    /* Casefold check */
    if (!casefoldsearch)
        f = TRUE;

    if ((s=re_readpattern("RE Query replace")) != TRUE)
	return (s);
    if ((s=ereply("Query replace %ls with: ", news, NG_WCHARLEN(news), re_pat))
	== ABORT)
	return (s);
    if (s == FALSE)
	news[0] = '\0';
    ewprintf("Query replacing %ls with %ls:", re_pat, news);
    
    /*
     * Search forward repeatedly, checking each time whether to insert
     * or not.  The "!" case makes the check always true, so it gets put
     * into a tighter loop for efficiency.
     */
    
    while (re_forwsrch() == TRUE) {
	TRexMatch match;
	trex_getsubexp(re_exp, 0, &match);
retry:
	update();
	switch (getkey(FALSE)) {
	case ' ':
	case 'y':
	case 'Y':
	    if (re_doreplace(match.len, news, f) == FALSE)
		return FALSE;
	    rcnt++;
	    break;
	    
	case '.':
	    if (re_doreplace(match.len, news, f) == FALSE)
		return FALSE;
	    rcnt++;
	    goto stopsearch;
	    
	case CCHR('G'): /* ^G or ESC */
	    (VOID) ctrlg(FFRAND, 0);
	    /*FALLTHRU*/
	case CCHR('['):
	case CCHR('M'):
	    goto stopsearch;
	    
	case '!':
	    do {
		if (re_doreplace(match.len, news, f) == FALSE)
		    return (FALSE);
		rcnt++;
	    } while (forwsrch() == TRUE);
	    goto stopsearch;
	    
	case CCHR('H'):
	case CCHR('?'):		/* To not replace */
	case 'n':
	case 'N':
	    break;
	    
	default:
	    ewprintf("<SP> replace, [.] rep-end, "
		     "<DEL> don't, [!] repl rest <ESC> quit");
	    goto retry;
	}
    }
stopsearch:
    curwp->w_flag |= WFHARD;
    update();
    if (rcnt == 0)
	ewprintf("(No replacements done)");
    else if (rcnt == 1)
	ewprintf("(1 replacement done)");
    else
	ewprintf("(%d replacements done)", rcnt);
    return TRUE;
}

static int
re_doreplace(plen, st, f)
register RSIZE plen;			/* length to remove		*/
const NG_WCHAR_t *st;			/* replacement string		*/
int f;					/* case hack disable		*/
{
    NG_WCHAR_t newstr[NLINE];
    NG_WCHAR_t *p, *newstr_end;
    int num;
    register int state = 0;
    
    p = newstr;
    newstr_end = newstr + NG_WCHARLEN(newstr) - 1;
    while (*st != NG_EOS) {
	switch (state) {
	case 2:
	    if (ISDIGIT(*st)) {
		num = num*10 + (*st - NG_WCODE('0'));
		break;
	    }
	    else {
		TRexMatch match;
		if (trex_getsubexp(re_exp, num, &match) == TRex_True) {
		    const NG_WCHAR_t *sub = match.begin;
		    if (p + match.len >= newstr_end)
			return FALSE;
		    for (num=0; num<match.len; num++)
			*p++ = *sub++;
		}
		state = 0;
	    }
	    /*FALLTHRU*/
	    
	case 0:
	    if (*st == NG_WCODE('\\'))
		state = 1;
	    else if (p >= newstr_end)
		return FALSE;
	    else
		*p++ = *st;
	    break;

	case 1:
	    if (ISDIGIT(*st)) {
		num = *st - NG_WCODE('0');
		state = 2;
	    }
	    else if (p >= newstr_end)
		return FALSE;
	    else {
		*p++ = *st;
		state = 0;
	    }
	    break;
	}
	st++;
    }
    
    *p = NG_EOS;
    return lreplace(plen, newstr, f);
}

/*
 * This routine does the real work of a
 * forward search. The pattern is sitting in the external
 * variable "pat". If found, dot is updated, the window system
 * is notified of the change, and TRUE is returned. If the
 * string isn't found, FALSE is returned.
 */
static int
re_forwsrch()
{
    register LINE *clp;
    register int cbo;
    const NG_WCHAR_t *pp;
    
    clp = curwp->w_dotp;
    cbo = curwp->w_doto;
    if (cbo == clp->l_used) {
	if (clp != curbp->b_linep) {
            clp = lforw(clp);
            cbo = 0;
        }
    }

    for (;;) {
	if (trex_searchrange(re_exp,
			     ltext(clp) + cbo, ltext(clp) + llength(clp) + 1,
			     &pp, NULL) == TRex_True) {
	    curwp->w_dotp = clp;
	    curwp->w_doto = pp - ltext(clp);
	    curwp->w_flag |= WFMOVE;
	    return TRUE;
	}
	if ((clp = lforw(clp)) == curbp->b_linep)
	    break;
	cbo = 0;
    }
    return FALSE;
}

/*
 * This routine does the real work of a
 * backward search. The pattern is sitting in the external
 * variable "pat". If found, dot is updated, the window system
 * is notified of the change, and TRUE is returned. If the
 * string isn't found, FALSE is returned.
 */
int
re_backsrch()
{
    register LINE *clp;
    register int cbo;
    register int tbo;
    const NG_WCHAR_t *pp;
    
    clp = curwp->w_dotp;
    tbo = curwp->w_doto;
    
    /* Start search one position to the left of dot */
    tbo = tbo - 1;
    if (tbo < 0) {
        /* must move up one line */
        clp = lback(clp);
        tbo = llength(clp);
    }

    for (;;) {
	cbo = 0;
	pp = NULL;
	/* search last match */
	while (trex_searchrange(re_exp,
				ltext(clp) + cbo, ltext(clp) + tbo + 1,
				&pp, NULL) == TRex_True) {
	    cbo = pp - ltext(clp) + 1; /* search again from next char */
	}
	if (pp != NULL) {
	    curwp->w_dotp = clp;
	    curwp->w_doto = pp - ltext(clp);
	    curwp->w_flag |= WFMOVE;
	    return TRUE;
	}
	if ((clp = lback(clp)) == curbp->b_linep)
	    break;
	tbo = llength(clp);
    }
    return FALSE;
}

/*
 * Read a pattern.
 * Stash it in the external variable "pat". The "pat" is
 * not updated if the user types in an empty line. If the user typed
 * an empty line, and there is no old pattern, it is an error.
 * Display the old pattern, in the style of Jeff Lomicka. There is
 * some do-it-yourself control expansion.
 */
static int
re_readpattern(prompt)
const char *prompt;
{
    register int s;
    NG_WCHAR_t tpat[NPAT];
    const NG_WCHAR_t *errorp;
    
    if (re_pat[0] == NG_EOS)
	s = ereply("%ls: ", tpat, NG_WCHARLEN(tpat), prompt);
    else
	s = ereply("%ls: (default %ls) ", tpat, NG_WCHARLEN(tpat),
		   prompt, re_pat);
    
    if (s == TRUE) {				/* Specified		*/
	wstrcpy(re_pat, tpat);
	if (re_exp != NULL)
	    trex_free(re_exp);
	if ((re_exp = trex_compile(re_pat, &errorp)) == NULL) {
	    ewprintf("Regexp Error: %ls", errorp);
	    re_pat[0] = NG_EOS;
	    return FALSE;
	}
    }
    else if (s == FALSE && re_pat[0] != NG_EOS)	/* CR, but old one	*/
	s = TRUE;
    return s;
}

int
delmatchlines(f, n)
int f, n;
{
    int s;
    if ((s=re_readpattern("Flush lines (containing match for regexp)")) != TRUE)
	return s;
    return kill_match_lines(TRUE);
}


int
delnonmatchlines(f, n)
int f, n;
{
    int s;
    if ((s=re_readpattern("Keep lines (containing match for regexp)")) != TRUE)
	return s;
    return kill_match_lines(FALSE);
}

/* kill match lines */
static int
kill_match_lines(cond)
int cond;
{
    int count = 0;
    LINE *clp;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    clp = curwp->w_dotp;
    if (curwp->w_doto == llength(clp))
	clp = lforw(clp);
   
    while ((clp  = lforw(clp)) != curbp->b_linep) {
	TRexBool match;
        match = trex_searchrange(re_exp,
				 ltext(clp), ltext(clp) + llength(clp) + 1,
				 NULL, NULL);
	/* Delete line when appropriate */
	if ((cond == FALSE && match == TRex_False) ||
	    (cond == TRUE && match != TRex_True)) {
	    int s;
	    curwp->w_doto = 0;
	    curwp->w_dotp = clp;
	    count++;
	    s = ldelete(llength(clp)+1, KNONE);
	    clp = curwp->w_dotp;
	    curwp->w_flag |= WFMOVE;
	    if (s == FALSE)
		return FALSE;
	}
	else
	    clp = lforw(clp);
    }
    ewprintf("%d line(s) deleted", count);
    return TRUE;
}

int
cntmatchlines(f, n)
int f, n;
{
    int s;
    if ((s = re_readpattern("Count lines (matching regexp)")) != TRUE)
	return s;
    return count_match_lines(TRUE);
}


int
cntnonmatchlines(f, n)
int f, n;
{
    int s;
    if ((s = re_readpattern("Count lines (not matching regexp)")) != TRUE)
	return s;
    return count_match_lines(FALSE);
}


/* counting lines */
static int
count_match_lines(cond)
int cond;
{
    register int count = 0;
    register LINE *clp;
    
    clp = curwp->w_dotp;
    if (curwp->w_doto == llength(clp))
	clp = lforw(clp);
    do { 
	TRexBool match;
	match = trex_searchrange(re_exp,
				 ltext(clp), ltext(clp) + llength(clp) + 1,
				 NULL, NULL);
	if ((cond == FALSE && match == TRex_False) ||
	    (cond == TRUE && match != TRex_True))
	    count++;
    } while ((clp  = lforw(clp)) != curbp->b_linep);
    
    if (cond)
	ewprintf("Number of lines matching: %d", count);
    else
	ewprintf("Number of lines not matching: %d", count);
    return TRUE;
}

#endif /* REGEX */
