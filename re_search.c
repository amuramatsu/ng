/* $Id: re_search.c,v 1.2 2000/10/02 16:11:46 amura Exp $ */
/*
 *		regular expression search commands for
 *			   MicroGnuEmacs
 *
 * This file contains functions to implement several of gnuemacs'
 * regular expression functions for MicroGnuEmacs.  Several of
 * the routines below are just minor rearrangements of the MicroGnuEmacs
 * non-regular expression search functions.  Hence some of them date back
 * in essential structure to the original MicroEMACS; others are modifications
 * of Rich Ellison's code.  I, Peter Newton, wrote about half from scratch.
 *
 * Although I have nothing to do with the GNU project, these functions
 * require the GNU project's regular expression package (files regex.c and
 * regex.h).  Hence, this file comes under the same copyright notice
 * as the GNU project's code.  As far as I know, the rest of MicroGnuEmacs
 * need not since it may be used independently of any GNU project code.	 In
 * any case, I certainly do not warrant either the correctness or utility
 * of this code.  The GNU project copyright notice follows.  Don't you
 * wish they would make it a bit shorter!
 */

/*
GNU Emacs copying permission notice Copyright (C) 1985 Richard M. Stallman
     Verbatim copies of this document, including its copyright notice,
     may be distributed by anyone in any manner.
     Distribution with modifications is not permitted.

GNU Emacs is distributed in the hope that it will be useful,
but without any warranty.  No author or distributor
accepts responsibility to anyone for the consequences of using it
or for whether it serves any particular purpose or works at all,
unless he says so in writing.

Everyone is granted permission to copy, modify and redistribute
GNU Emacs under the following conditions:

   Permission is granted to anyone to make or distribute verbatim copies
   of GNU Emacs source code as received, in any medium, provided that all
   copyright notices and permission and nonwarranty notices are preserved,
   and that the distributor grants the recipient permission
   for further redistribution as permitted by this document,
   and gives him and points out to him an exact copy of this document
   to inform him of his rights.

   Permission is granted to distribute modified versions
   of GNU Emacs source code, or of portions of it,
   under the above conditions, provided also that all
   changed files carry prominent notices stating who last changed them
   and that all the GNU-Emacs-derived material, including everything
   packaged together with it and not independently usable, is
   distributed under the conditions stated in this document.

   Permission is granted to distribute GNU Emacs in
   compiled or executable form under the same conditions applying
   for source code, provided that either
    A.	it is accompanied by the corresponding machine-readable
      source code, or
    B.	it is accompanied by a written offer, with no time limit,
      to give anyone a machine-readable copy of the corresponding
      source code in return for reimbursement of the cost of distribution.
      This written offer must permit verbatim duplication by anyone.
    C.	it is distributed by someone who received only the
      executable form, and is accompanied by a copy of the
      written offer of source code which he received along with it.

In other words, you are welcome to use, share and improve GNU Emacs
You are forbidden to forbid anyone else to use, share and improve
what you give them.   Help stamp out software-hoarding!
*/

/*
 * $Log: re_search.c,v $
 * Revision 1.2  2000/10/02 16:11:46  amura
 * small fix in re_searchagain()
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef	REGEX
#include	"def.h"
#include	"macro.h"

#define SRCH_BEGIN	(0)			/* Search sub-codes.	*/
#define SRCH_FORW	(-1)
#define SRCH_BACK	(-2)
#define SRCH_NOPR	(-3)
#define SRCH_ACCM	(-4)
#define SRCH_MARK	(-5)

char	re_pat[NPAT];			/* Regex pattern		*/
int	re_srch_lastdir = SRCH_NOPR;	 /* Last search flags. */
int	casefoldsearch = TRUE;		 /* Does search ignore case ? */

/* Indexed by a character, gives the upper case equivalent of the character */

static unsigned char upcase[0400] =
  { 000, 001, 002, 003, 004, 005, 006, 007,
    010, 011, 012, 013, 014, 015, 016, 017,
    020, 021, 022, 023, 024, 025, 026, 027,
    030, 031, 032, 033, 034, 035, 036, 037,
    040, 041, 042, 043, 044, 045, 046, 047,
    050, 051, 052, 053, 054, 055, 056, 057,
    060, 061, 062, 063, 064, 065, 066, 067,
    070, 071, 072, 073, 074, 075, 076, 077,
    0100, 0101, 0102, 0103, 0104, 0105, 0106, 0107,
    0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117,
    0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127,
    0130, 0131, 0132, 0133, 0134, 0135, 0136, 0137,
    0140, 0101, 0102, 0103, 0104, 0105, 0106, 0107,
    0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117,
    0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127,
    0130, 0131, 0132, 0173, 0174, 0175, 0176, 0177,
    0200, 0201, 0202, 0203, 0204, 0205, 0206, 0207,
    0210, 0211, 0212, 0213, 0214, 0215, 0216, 0217,
    0220, 0221, 0222, 0223, 0224, 0225, 0226, 0227,
    0230, 0231, 0232, 0233, 0234, 0235, 0236, 0237,
    0240, 0241, 0242, 0243, 0244, 0245, 0246, 0247,
    0250, 0251, 0252, 0253, 0254, 0255, 0256, 0257,
    0260, 0261, 0262, 0263, 0264, 0265, 0266, 0267,
    0270, 0271, 0272, 0273, 0274, 0275, 0276, 0277,
    0300, 0301, 0302, 0303, 0304, 0305, 0306, 0307,
    0310, 0311, 0312, 0313, 0314, 0315, 0316, 0317,
    0320, 0321, 0322, 0323, 0324, 0325, 0326, 0327,
    0330, 0331, 0332, 0333, 0334, 0335, 0336, 0337,
    0340, 0341, 0342, 0343, 0344, 0345, 0346, 0347,
    0350, 0351, 0352, 0353, 0354, 0355, 0356, 0357,
    0360, 0361, 0362, 0363, 0364, 0365, 0366, 0367,
    0370, 0371, 0372, 0373, 0374, 0375, 0376, 0377
  };

/*
 * Search forward.
 * Get a search string from the user, and search for it,
 * starting at ".". If found, "." gets moved to just after the
 * matched characters, and display does all the hard stuff.
 * If not found, it just prints a message.
 */
/*ARGSUSED*/
re_forwsearch(f, n) {
	register int	s;

	if ((s=re_readpattern("RE Search")) != TRUE)
		return (s);
	if (re_forwsrch() == FALSE) {
		ewprintf("Search failed: \"%s\"", re_pat);
		return (FALSE);
	}
	re_srch_lastdir = SRCH_FORW;
	return (TRUE);
}

/*
 * Reverse search.
 * Get a search string from the	 user, and search, starting at "."
 * and proceeding toward the front of the buffer. If found "." is left
 * pointing at the first character of the pattern [the last character that
 * was matched].
 */
/*ARGSUSED*/
re_backsearch(f, n) {
	register int	s;

	if ((s=re_readpattern("RE Search backward")) != TRUE)
		return (s);
	if (re_backsrch() == FALSE) {
		ewprintf("Search failed: \"%s\"", re_pat);
		return (FALSE);
	}
	re_srch_lastdir = SRCH_BACK;
	return (TRUE);
}



/*
 * Search again, using the same search string
 * and direction as the last search command. The direction
 * has been saved in "srch_lastdir", so you know which way
 * to go.
 */
/*ARGSUSED*/
/*  This code has problems-- some incompatibility(?) with
    extend.c causes match to fail when it should not.
 */
re_searchagain(f, n) {

  if (re_srch_lastdir == SRCH_NOPR) {
    ewprintf("No last search");
    return (FALSE);
  }

  if (re_srch_lastdir == SRCH_FORW) {
    if (re_forwsrch() == FALSE) {
      ewprintf("Search failed: \"%s\"", re_pat);
      return (FALSE);
    }
    return (TRUE);
  }
  if (re_srch_lastdir == SRCH_BACK) {
    if (re_backsrch() == FALSE) {
      ewprintf("Search failed: \"%s\"", re_pat);
      return (FALSE);
    }
    return (TRUE);
  }
  return (FALSE);
}

#ifdef REGEX_JAPANESE
#include "regex_j.h"
#else
#include "regex_e.h"
#endif

#define BYTEWIDTH 8

/* Compiled regex goes here-- changed only when new pattern read */
static struct re_pattern_buffer re_buff;
static char fastmap[(1 << BYTEWIDTH)];

/* regs holds boundaries of matched text */
static struct re_registers regs;

/*
 * Re-Query Replace.
 *	Replace strings selectively.  Does a search and replace operation.
 */
/*ARGSUSED*/
re_queryrepl(f, n) {
	register int	s;
	register int	rcnt = 0;	/* Replacements made so far	*/
	register int	plen;		/* length of found string	*/
	char		news[NPAT];	/* replacement string		*/

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	/* Casefold check */
	if (!casefoldsearch) f = TRUE;

	if ((s=re_readpattern("RE Query replace")) != TRUE)
		return (s);
	if ((s=ereply("Query replace %s with: ",news, NPAT, re_pat)) == ABORT)
		return (s);
	if (s == FALSE)
		news[0] = '\0';
	ewprintf("Query replacing %s with %s:", re_pat, news);

	/*
	 * Search forward repeatedly, checking each time whether to insert
	 * or not.  The "!" case makes the check always true, so it gets put
	 * into a tighter loop for efficiency.
	 */

	while (re_forwsrch() == TRUE) {
	retry:
		update();
		switch (getkey(FALSE)) {
		case ' ':
#ifdef EMACS_QUERY_KEYS
		case 'y':
		case 'Y':
#endif /* EMACS_QUERY_KEYS */
			plen = regs.end[0] - regs.start[0];
			if (re_doreplace((RSIZE) plen, news, f) == FALSE)
				return (FALSE);
			rcnt++;
			break;

		case '.':
			plen = regs.end[0] - regs.start[0];
			if (re_doreplace((RSIZE) plen, news, f) == FALSE)
				return (FALSE);
			rcnt++;
			goto stopsearch;

		case CCHR('G'): /* ^G */
			(VOID) ctrlg(FFRAND, 0);
		case CCHR('['): /* ESC */
		case '`':
#ifdef EMACS_QUERY_KEYS
		case CCHR('M'):
#endif /* EMACS_QUERY_KEYS */
			goto stopsearch;

		case '!':
			do {
				plen = regs.end[0] - regs.start[0];
				if (re_doreplace((RSIZE) plen, news, f) == FALSE)
					return (FALSE);
				rcnt++;
			} while (re_forwsrch() == TRUE);
			goto stopsearch;

		case CCHR('?'):		/* To not replace */
#ifdef EMACS_QUERY_KEYS
		case 'n':
		case 'N':
#endif /* EMACS_QUERY_KEYS */
			break;

		default:
ewprintf("<SP> replace, [.] rep-end, <DEL> don't, [!] repl rest <ESC> quit");
			goto retry;
		}
	}
stopsearch:
	curwp->w_flag |= WFHARD;
	update();
	if (!inmacro) {
		if (rcnt == 0)
			ewprintf("(No replacements done)");
		else if (rcnt == 1)
			ewprintf("(1 replacement done)");
		else
			ewprintf("(%d replacements done)", rcnt);
	}
	return TRUE;
}



/* Routine re_doreplace calls lreplace to make replacements needed by
 * re_query replace.  Its reason for existence is to deal with \1,
 * \2. etc.
 */

/* Maximum length of replacement string */
#define REPLEN 256

re_doreplace(plen, st, f)
     register RSIZE  plen;		     /* length to remove	     */
     char	     *st;		     /* replacement string	     */
     int	     f;			     /* case hack disable	     */
{
  int s;
  int num, k;
  register int j;
  int more, state;
  LINE *clp;
  char repstr[REPLEN];

  clp = curwp->w_dotp;
  more = TRUE;
  j = 0;
  state = 0;

  /* The following FSA parses the replacement string */
  while (more) {
    switch (state) {

    case 0: if (*st == '\\') {
	      st++;
	      state = 1;
	    }
	    else if (*st == '\0')
	      more = FALSE;
	    else {
	      repstr[j] = *st;
	      j++; if (j >= REPLEN) return(FALSE);
	      st++;
	    }
	    break;
    case 1: if (*st >= '0' && *st <= '9') {
	      num = *st - '0';
	      st++;
	      state = 2;
	    }
	    else if (*st == '\0')
	      more = FALSE;
	    else {
	      repstr[j] = *st;
	      j++; if (j >= REPLEN) return(FALSE);
	      st++;
	      state = 0;
	    }
	    break;
    case 2: if (*st >= '0' && *st <= '9') {
	      num = 10*num + *st - '0';
	      st++;
	    }
	    else {
	      if (num >= RE_NREGS) return(FALSE);
	      k = regs.end[num] - regs.start[num];
	      if (j+k >= REPLEN) return(FALSE);
	      bcopy(&(clp->l_text[regs.start[num]]), &repstr[j], k);
	      j += k;
	      if (*st == '\0')
		more = FALSE;
	      if (*st == '\\') {
		st++;
		state = 1;
	      }
	      else {
		repstr[j] = *st;
		j++; if (j >= REPLEN) return(FALSE);
		st++;
		state = 0;
	      }
	    }
	    break;
	  } /* end case */
  } /* end while */

  repstr[j] = '\0';

  s = lreplace(plen, repstr, f);

  return(s);
}



/*
 * This routine does the real work of a
 * forward search. The pattern is sitting in the external
 * variable "pat". If found, dot is updated, the window system
 * is notified of the change, and TRUE is returned. If the
 * string isn't found, FALSE is returned.
 */
re_forwsrch() {

  register LINE *clp;
  register int tbo;
  int ntries;
  int i, plen;

  clp = curwp->w_dotp;
  tbo = curwp->w_doto;

  if (tbo == clp->l_used)
    /* Don't start matching off end of line-- must
     * move to beginning of next line, unless at end
     */
    if (clp != curbp->b_linep) {
      clp = lforw(clp);
      tbo = 0;
    }


  /* Note this loop does not process the last line, but this editor
     always makes the last line empty so this is good.
   */

  while (clp != (curbp->b_linep)) {

     ntries = llength(clp) - tbo;
     i = re_search (&re_buff, ltext(clp), llength(clp), tbo, ntries, &regs);

     if (i == -1) {
       clp = lforw(clp);
       tbo = 0;
     }
     else {
       curwp->w_doto = regs.end[0];
       curwp->w_dotp = clp;
       curwp->w_flag |= WFMOVE;
       return (TRUE);
     }

   }

  return(FALSE);

}


/*
 * This routine does the real work of a
 * backward search. The pattern is sitting in the external
 * variable "re_pat". If found, dot is updated, the window system
 * is notified of the change, and TRUE is returned. If the
 * string isn't found, FALSE is returned.
 */
re_backsrch() {

  register LINE *clp;
  register int tbo;
  int ntries;
  int i, startpos;
char m[1];

  clp = curwp->w_dotp;
  tbo = curwp->w_doto;

  /* Start search one position to the left of dot */
  tbo = tbo - 1;
  if (tbo < 0) {
    /* must move up one line */
    clp = lback(clp);
    tbo = llength(clp);
  }

  /* Note this loop does not process the last line, but this editor
     always makes the last line empty so this is good.
   */

  while (clp != (curbp->b_linep)) {

     ntries = tbo;
     i = re_search (&re_buff, ltext(clp), llength(clp), tbo, -ntries, &regs);

     if (i == -1) {
       clp = lback(clp);
       tbo = llength(clp);
     }
     else {
       curwp->w_doto = regs.start[0];
       curwp->w_dotp = clp;
       curwp->w_flag |= WFMOVE;
       return (TRUE);
     }

   }

  return(FALSE);

}


/*
 * Read a pattern.
 * Stash it in the external variable "re_pat". The "pat" is
 * not updated if the user types in an empty line. If the user typed
 * an empty line, and there is no old pattern, it is an error.
 * Display the old pattern, in the style of Jeff Lomicka. There is
 * some do-it-yourself control expansion.
 */
re_readpattern(prompt) char *prompt; {
	register int s;
	char tpat[NPAT];
	char *message;

	if (re_pat[0] == '\0') s = ereply("%s: ", tpat, NPAT, prompt);
	else s = ereply("%s: (default %s) ", tpat, NPAT, prompt, re_pat);

	if (s == TRUE) {
	  /* New pattern given */
	  (VOID) strcpy(re_pat, tpat);
	  re_buff.allocated = 40;
	  re_buff.buffer = (char *) malloc (re_buff.allocated);
	  re_buff.fastmap = fastmap;
	  if (casefoldsearch)
	    re_buff.translate = upcase;
	  else
	    re_buff.translate = '\0';
	  message = re_compile_pattern (re_pat, strlen(re_pat), &re_buff);
	  if (message != '\0') {
	    ewprintf("Regex Error: %s", message);
	    re_pat[0] = '\0';
	    return(FALSE);
	  }
	  re_compile_fastmap (&re_buff);
	}
	else if (s==FALSE && re_pat[0]!='\0')
	  /* Just using old pattern */
	  s = TRUE;
	return (s);
}



/* Cause case to not matter in searches.  This is the default.	If
 * called with argument cause case to matter.
 */
setcasefold(f, n) {

  if (f & FFARG) {
    casefoldsearch = FALSE;
    ewprintf("Case-fold-search unset");
  }
  else {
    casefoldsearch = TRUE;
    ewprintf("Case-fold-search set");
  }

  /* Invalidate the regular expression pattern since I'm too lazy
   * to recompile it.
   */

  re_pat[0] = '\0';

  return(TRUE);

} /* end setcasefold */


/* Delete all lines after dot that contain a string matching regex
 */
delmatchlines(f, n) {
  int s;

  if ((s=re_readpattern("Flush lines (containing match for regexp)")) != TRUE)
    return (s);

  s = killmatches(TRUE);

  return(s);
}



/* Delete all lines after dot that don't contain a string matching regex
 */
delnonmatchlines(f, n) {
  int s;


  if ((s=re_readpattern("Keep lines (containing match for regexp)")) != TRUE)
    return (s);

  s = killmatches(FALSE);

  return(s);
}



/* This function does the work of deleting matching lines */
killmatches(cond)
   int cond;
{
  int s, i;
  int count = 0;
  LINE	*clp;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
  if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
    warnreadonly();			/* do only displaying warning.	*/
    return TRUE;
  }
#endif	/* READONLY */

  clp = curwp->w_dotp;
  if (curwp->w_doto == llength(clp))
    /* Consider dot on next line */
    clp = lforw(clp);

  while (clp != (curbp->b_linep)) {

     /* see if line matches */
     i = re_search (&re_buff, ltext(clp), llength(clp), 0, llength(clp),
		    &regs);
     /* Delete line when appropriate */
     if ((cond == FALSE && i == -1) || (cond == TRUE && i != -1)) {
       curwp->w_doto = 0;
       curwp->w_dotp = clp;
       count++;
       s = ldelete(llength(clp)+1, KNONE);
       clp = curwp->w_dotp;
       curwp->w_flag |= WFMOVE;
       if (s == FALSE) return(FALSE);
     }
     else
       clp = lforw(clp);
   }

  ewprintf("%d line(s) deleted", count);
  if (count > 0) curwp->w_flag |= WFMOVE;

  return(TRUE);
}

#ifndef	BUGFIX	/* 91.01.09  by S.Yoshida */
petersfunc(f, n) {

  int s;
  LINE	*clp;
  char c;

  curwp->w_doto = 0;
  s = ldelete(llength(curwp->w_dotp)+1, KNONE);
  curwp->w_flag |= WFMOVE;
  return(s);

}
#endif	/* BUGFIX */

/* Count lines matching regex
 */
cntmatchlines(f, n) {
  int s;

  if ((s=re_readpattern("Count lines (matching regexp)")) != TRUE)
    return (s);

  s = countmatches(TRUE);

  return(s);
}



/* Count lines that fail to match regex
 */
cntnonmatchlines(f, n) {
  int s;


  if ((s=re_readpattern("Count lines (not matching regexp)")) != TRUE)
    return (s);

  s = countmatches(FALSE);

  return(s);
}



/* This function does the work of counting matching lines */
countmatches(cond)
   int cond;
{
  int s, i;
  int count = 0;
  LINE	*clp;

  clp = curwp->w_dotp;
  if (curwp->w_doto == llength(clp))
    /* Consider dot on next line */
    clp = lforw(clp);

  while (clp != (curbp->b_linep)) {

     /* see if line matches */
     i = re_search (&re_buff, ltext(clp), llength(clp), 0, llength(clp),
		    &regs);
     /*	 Count line when appropriate */
     if ((cond == FALSE && i == -1) || (cond == TRUE && i != -1)) count++;
     clp = lforw(clp);
   }

  if (cond)
     ewprintf("Number of lines matching: %d", count);
  else
     ewprintf("Number of lines not matching: %d", count);

  return(TRUE);
}
#endif
