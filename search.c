/* $Id: search.c,v 1.4 2001/05/25 15:37:21 amura Exp $ */
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

/*
 * $Log: search.c,v $
 * Revision 1.4  2001/05/25 15:37:21  amura
 * now buffers have only one mark (before windows have one mark)
 *
 * Revision 1.3  2001/01/05 14:07:05  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.2  2000/07/22 17:54:09  amura
 * fix typo bug
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */
/* 91.11.23	C-w command in isearch was added by bsh. You need to #define */
/*              IS_ENHANCE to enable this. */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#ifndef NO_MACRO
#include	"macro.h"
#endif

#define SRCH_BEGIN	(0)			/* Search sub-codes.	*/
#define SRCH_FORW	(-1)
#define SRCH_BACK	(-2)
#define SRCH_NOPR	(-3)
#define SRCH_ACCM	(-4)
#define SRCH_MARK	(-5)
#ifdef IS_ENHANCE
/* Search sub-codes for C-w in isearch.
 * It means LEN bytes of chars are added to pattern.
 */
#define SRCH_CHUNK_BASE	(-6)
#define	SRCH_CHUNK(len,fail)	(-(len*2)+SRCH_CHUNK_BASE-((fail)!=0))
#define	SRCH_CHUNK_LEN(code)	((-(code)+SRCH_CHUNK_BASE)/2)
#define	SRCH_CHUNK_FAIL(code)	((-(code)+SRCH_CHUNK_BASE)%2)
#endif

typedef struct	{
	int	s_code;
	LINE	*s_dotp;
	int	s_doto;
}	SRCHCOM;

static	SRCHCOM cmds[NSRCH];
static	int	cip;

int	srch_lastdir = SRCH_NOPR;		/* Last search flags.	*/

static VOID	is_cpush();
static VOID	is_lpush();
static VOID	is_pop();
static int	is_peek();
static VOID	is_undo();
static int	is_find();
static VOID	is_prompt();
static VOID	is_dspl();
static int	eq();

#ifndef	IS_ENHANCE
#define is_fail(code)	((code)==SRCH_ACCM)
#else
static	int	is_fail();
static  int	is_addword();
#endif

#ifdef	IS_ENHANCE
/* casefoldsearch: Does search ignore case ? */
#ifdef	REGEX
extern	int	casefoldsearch;	/* Defined in re_search.c */
#else
int	casefoldsearch = TRUE;
#endif
#endif

/*
 * Search forward.
 * Get a search string from the user, and search for it,
 * starting at ".". If found, "." gets moved to just after the
 * matched characters, and display does all the hard stuff.
 * If not found, it just prints a message.
 */
/*ARGSUSED*/
forwsearch(f, n)
{
	register int	s;

	if ((s=readpattern("Search")) != TRUE)
		return s;
	if (forwsrch() == FALSE) {
		ewprintf("Search failed: \"%s\"", pat);
		return FALSE;
	}
	srch_lastdir = SRCH_FORW;
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
backsearch(f, n)
{
	register int	s;

	if ((s=readpattern("Search backward")) != TRUE)
		return (s);
	if (backsrch() == FALSE) {
		ewprintf("Search failed: \"%s\"", pat);
		return FALSE;
	}
	srch_lastdir = SRCH_BACK;
	return TRUE;
}

/*
 * Search again, using the same search string
 * and direction as the last search command. The direction
 * has been saved in "srch_lastdir", so you know which way
 * to go.
 */
/*ARGSUSED*/
searchagain(f, n)
{
	if (srch_lastdir == SRCH_FORW) {
		if (forwsrch() == FALSE) {
			ewprintf("Search failed: \"%s\"", pat);
			return FALSE;
		}
		return TRUE;
	}
	if (srch_lastdir == SRCH_BACK) {
		if (backsrch() == FALSE) {
			ewprintf("Search failed: \"%s\"", pat);
			return FALSE;
		}
		return TRUE;
	}
	ewprintf("No last search");
	return FALSE;
}

/*
 * Use incremental searching, initially in the forward direction.
 * isearch ignores any explicit arguments.
 */
/*ARGSUSED*/
forwisearch(f, n)
{
	return isearch(SRCH_FORW);
}

/*
 * Use incremental searching, initially in the reverse direction.
 * isearch ignores any explicit arguments.
 */
/*ARGSUSED*/
backisearch(f, n)
{
	return isearch(SRCH_BACK);
}

/*
 * Incremental Search.
 *	dir is used as the initial direction to search.
 *	^S	switch direction to forward
 *	^R	switch direction to reverse
 *	^Q	quote next character (allows searching for ^N etc.)
 *	<ESC>	exit from Isearch
 *	<DEL>	undoes last character typed. (tricky job to do this correctly).
 *	^W	copy word from DOT.
 *	other ^ set mark, exit search
 *	Metakeys  set mark, exit search
 *	else	accumulate into search string
 */
isearch(dir) {
	register int	c;
	register LINE	*clp;
	register int	cbo;
	register int	success;
	int		pptr;
	char		opat[NPAT];
	VOID		ungetkey();
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	kanji1st = 0;	/* It is only KANJI 1st byte. */
#endif	/* KANJI */

#ifndef NO_MACRO
	if(macrodef) {
	    ewprintf("Can't isearch in macro");
	    return FALSE;
	}
#endif
	for (cip=0; cip<NSRCH; cip++)
		cmds[cip].s_code = SRCH_NOPR;
	(VOID) strcpy(opat, pat);
	cip = 0;
	pptr = -1;
	clp = curwp->w_dotp;
	cbo = curwp->w_doto;
	is_lpush();
	is_cpush(SRCH_BEGIN);
	success = TRUE;
	is_prompt(dir, TRUE, success);
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_off();
#endif
	for (;;) {
		update();
  	        c = getkey(FALSE);
		switch (c) {
		case CCHR('['):
#ifdef EMACS_QUERY_KEYS
		case CCHR('M'):
#endif /* EMACS_QUERY_KEYS */
			srch_lastdir = dir;
			curwp->w_bufp->b_markp = clp;
			curwp->w_bufp->b_marko = cbo;
			ewprintf("Mark set");
			return (TRUE);

		case CCHR('G'):
			if (success != TRUE) {
				while (is_fail(is_peek()))
					is_undo(&pptr, &dir);
				success = TRUE;
				is_prompt(dir, pptr < 0, success);
				break;
			}
			curwp->w_dotp = clp;
			curwp->w_doto = cbo;
			curwp->w_flag |= WFMOVE;
			srch_lastdir = dir;
			(VOID) ctrlg(FFRAND, 0);
			(VOID) strcpy(pat, opat);
			return ABORT;

		case CCHR(']'):
		case CCHR('S'):
			if (dir == SRCH_BACK) {
				dir = SRCH_FORW;
				is_lpush();
				is_cpush(SRCH_FORW);
				success = TRUE;
			}
			if (success==FALSE && dir==SRCH_FORW)
				break;
			is_lpush();
			pptr = strlen(pat);
			(VOID) forwchar(FFRAND, 1);
			if (is_find(SRCH_FORW) != FALSE) is_cpush(SRCH_MARK);
			else {
				(VOID) backchar(FFRAND, 1);
				ttbeep();
				success = FALSE;
			}
			is_prompt(dir, pptr < 0, success);
			break;

		case CCHR('R'):
			if (dir == SRCH_FORW) {
				dir = SRCH_BACK;
				is_lpush();
				is_cpush(SRCH_BACK);
				success = TRUE;
			}
			if (success==FALSE && dir==SRCH_BACK)
				break;
			is_lpush();
			pptr = strlen(pat);
			(VOID) backchar(FFRAND, 1);
			if (is_find(SRCH_BACK) != FALSE) is_cpush(SRCH_MARK);
			else {
				(VOID) forwchar(FFRAND, 1);
				ttbeep();
				success = FALSE;
			}
			is_prompt(dir, pptr < 0, success);
			break;

		case CCHR('H'):
		case CCHR('?'):
			is_undo(&pptr, &dir);
			if (!is_fail(is_peek())) success = TRUE;
			is_prompt(dir, pptr < 0, success);
			break;

		case CCHR('\\'):
		case CCHR('Q'):
			c = (char) getkey(FALSE);
			goto  addchar;
#ifndef EMACS_QUERY_KEYS
		case CCHR('M'):
			c = CCHR('J');
			goto  addchar;
#endif /* !EMACS_QUERY_KEYS */
#ifdef IS_ENHANCE

		case CCHR('W'):
		{
			int chunklen;
			int optr = pptr;

			if (pptr == -1)
				pptr = 0;
			if (pptr == 0)
				success = TRUE;
			
			if( (chunklen = is_addword(pptr,dir)) == 0 ){
				pptr = optr;
				ttbeep();
			}
			else {
				pptr += chunklen;
				is_lpush();
				if (!success)		/* already failed */
					is_cpush(SRCH_CHUNK(chunklen,1));
				else {
					if (is_find(dir))
						is_cpush(SRCH_CHUNK(chunklen,0));
					else {
						success = FALSE;
						ttbeep();
						is_cpush(SRCH_CHUNK(chunklen,1));
					}
				}
			}
			is_prompt(dir, FALSE, success);
		}
			break;
#endif
		default:
			if (ISCTRL(c)) {
				ungetkey(c);
				curwp->w_bufp->b_markp = clp;
				curwp->w_bufp->b_marko = cbo;
				ewprintf("Mark set");
				curwp->w_flag |= WFMOVE;
				return	TRUE;
			}	/* and continue */
			is_prompt(dir, FALSE, success);
		case CCHR('I'):
		case CCHR('J'):
		addchar:
			if (pptr == -1)
				pptr = 0;
			if (pptr == 0)
				success = TRUE;
			pat[pptr++] = c;
			if (pptr == NPAT) {
				ewprintf("Pattern too long");
				return FALSE;
			}
			pat[pptr] = '\0';
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			if (kanji1st != 0) {
				/* If the KANJI 1st byte exists,	*/
				/* we believe 'c' is KANJI 2nd byte.	*/
				kanji1st--;
			} else if (ISKANJI(c)) {
				/* When there is only KANJI 1st		*/
				/* byte, we don't search a patern.	*/
#ifdef	HOJO_KANJI
				if (ISHOJO(c))	kanji1st = 2;
				else
#endif				
				kanji1st = 1;	
				break;
			}
#endif	/* KANJI */
			is_lpush();
			if (success != FALSE) {
				if (is_find(dir) != FALSE)
					is_cpush(c);
				else {
					success = FALSE;
					ttbeep();
					is_cpush(SRCH_ACCM);
				}
			} else
				is_cpush(SRCH_ACCM);
			is_prompt(dir, FALSE, success);
		}
	}
	/*NOTREACHED*/
}

static VOID
is_cpush(cmd) register int cmd; {
	if (++cip >= NSRCH)
		cip = 0;
	cmds[cip].s_code = cmd;
}

static VOID
is_lpush() {
	register int	ctp;

	ctp = cip+1;
	if (ctp >= NSRCH)
		ctp = 0;
	cmds[ctp].s_code = SRCH_NOPR;
	cmds[ctp].s_doto = curwp->w_doto;
	cmds[ctp].s_dotp = curwp->w_dotp;
}

static VOID
is_pop() {
	if (cmds[cip].s_code != SRCH_NOPR) {
		curwp->w_doto  = cmds[cip].s_doto;
		curwp->w_dotp  = cmds[cip].s_dotp;
		curwp->w_flag |= WFMOVE;
		cmds[cip].s_code = SRCH_NOPR;
	}
	if (--cip <= 0)
		cip = NSRCH-1;
}

static int
is_peek() {
	return cmds[cip].s_code;
}

/* this used to always return TRUE (the return value was checked) */
static VOID
is_undo(pptr, dir) register int *pptr; register int *dir; {
	register int	redo = FALSE ;
	switch (cmds[cip].s_code) {
	case SRCH_BEGIN:
	case SRCH_NOPR:
		*pptr = -1;
	case SRCH_MARK:
		break;

	case SRCH_FORW:
		*dir = SRCH_BACK;
		redo = TRUE;
		break;

	case SRCH_BACK:
		*dir = SRCH_FORW;
		redo = TRUE;
		break;

	default:
#ifdef	IS_ENHANCE		
		if(cmds[cip].s_code < 0){
			int len = SRCH_CHUNK_LEN(cmds[cip].s_code);
			*pptr -= len;
			pat[*pptr] = '\0';
			break;
		}
#endif		
	case SRCH_ACCM:
		
		*pptr -= 1;
#ifdef	HUMAN68K	/* 90.11.16    Sawayanagi Yosirou */
		if (ISKANJI(pat[*pptr]))
		    *pptr -= 1;
#endif	/* HUMAN68K */
		if (*pptr < 0)
			*pptr = 0;
		pat[*pptr] = '\0';
		break;
	}
	is_pop();
	if (redo) is_undo(pptr, dir);
}

static int
is_find(dir) register int dir; {
	register int	plen, odoto;
	register LINE	*odotp ;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	clen;		/* Character length (not byte len). */
	register int	i;
#endif	/* KANJI */

	odoto = curwp->w_doto;
	odotp = curwp->w_dotp;
	plen = strlen(pat);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	for (i = 0, clen = 0; i < plen; i++, clen++) {
		if (ISKANJI(pat[i])) {
#ifdef	HOJO_KANJI
			if (ISHOJO(pat[i]))	i++;
#endif
			i++;
		}
	}
#endif	/* KANJI */
	if (plen != 0) {
		if (dir==SRCH_FORW) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			/* Backward char with "character"	*/
			/* length, not "byte" length.		*/
			for (i = 0; i < clen; i++) {
				(VOID) backchar(FFARG | FFRAND, 1);
			}
#else	/* NOT KANJI */
			(VOID) backchar(FFARG | FFRAND, plen);
#endif	/* KANJI */
			if (forwsrch() == FALSE) {
				curwp->w_doto = odoto;
				curwp->w_dotp = odotp;
				return FALSE;
			}
			return TRUE;
		}
		if (dir==SRCH_BACK) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			/* forward char with "character"	*/
			/* length, not "byte" length.		*/
			for (i = 0; i < clen; i++) {
				(VOID) forwchar(FFARG | FFRAND, 1);
			}
#else	/* NOT KANJI */
			(VOID) forwchar(FFARG | FFRAND, plen);
#endif	/* KANJI */
			if (backsrch() == FALSE) {
				curwp->w_doto = odoto;
				curwp->w_dotp = odotp;
				return FALSE;
			}
			return TRUE;
		}
		ewprintf("bad call to is_find");
		return FALSE;
	}
	return FALSE;
}

/*
 * If called with "dir" not one of SRCH_FORW
 * or SRCH_BACK, this routine used to print an error
 * message. It also used to return TRUE or FALSE,
 * depending on if it liked the "dir". However, none
 * of the callers looked at the status, so I just
 * made the checking vanish.
 */
static VOID
is_prompt(dir, flag, success) {
	if (dir == SRCH_FORW) {
		if (success != FALSE)
			is_dspl("I-search", flag);
		else
			is_dspl("Failing I-search", flag);
	} else if (dir == SRCH_BACK) {
		if (success != FALSE)
			is_dspl("I-search backward", flag);
		else
			is_dspl("Failing I-search backward", flag);
	} else ewprintf("Broken call to is_prompt");
}

/*
 * Prompt writing routine for the incremental search.
 * The "prompt" is just a string. The "flag" determines
 * whether pat should be printed.
 */
static VOID
is_dspl(prompt, flag) char *prompt; {

	if (flag != FALSE)
		ewprintf("%s: ", prompt);
	else
		ewprintf("%s: %s", prompt, pat);
}

#ifdef	IS_ENHANCE
static int
is_fail(code)
{
	return code == SRCH_ACCM || code <= SRCH_CHUNK_BASE &&
		SRCH_CHUNK_FAIL(code);
}	
#endif

static int
is_addword(pptr,dir)
{
	int oo = curwp->w_doto;
	LINE *op = curwp->w_dotp;
	int co = oo;
	LINE *cp = op;
	int chunklen, len;
	
	if( dir == SRCH_BACK ){
		/* skip common prefix */
		int i;
		for(i=0; i < pptr; ++i ){
			if(curwp->w_doto >= llength(curwp->w_dotp)){
				/* meet EOL */
				if( pat[i] != CCHR('J') )
					break;
			}
			else {
				int c;

				c =lgetc(curwp->w_dotp,curwp->w_doto);
#ifdef	KANJI
#ifdef	HOJO_KANJI
				if( ISHOJO(c) ) {
				    if ( c != CHARMASK(pat[i]) ||
					lgetc(curwp->w_dotp, curwp->w_doto+1)
					!= CHARMASK(pat[i+1]) ||
					lgetc(curwp->w_dotp, curwp->w_doto+2)
					!= CHARMASK(pat[i+2]) )
					break;
				    i += 2;
				} else
#endif
				if( ISKANJI(c) ){
					if( c != CHARMASK(pat[i]) ||
					   lgetc(curwp->w_dotp, curwp->w_doto+1)
					   != CHARMASK(pat[i+1]) )
						break;
					++i;
				}
				else if( !eq(c,CHARMASK(pat[i])) )
					break;
#endif				
			}
			if( !forwchar(FFRAND,1) )
				break;
		}
		cp = curwp->w_dotp;
		co = curwp->w_doto;
	}

	if( !forwword(FFRAND, 1) )	/* Skip one word */
		return FALSE;


	/*
	 * Copy string between (cp,co) and dot to pat.
	 * There should be some utility subroutine to do it.
	 */
	chunklen = 0;
	while( cp != curwp->w_dotp ){
		len = llength(cp) - co;
		if( len > NPAT-pptr-2 )		/* room for '\n' and '\0' */
			goto patfull;
		strncpy(&pat[pptr], ltext(cp)+co, len);
		pptr += len;
		pat[pptr++] = '\n';
		chunklen += len+1;
		cp = lforw(cp);
		co = 0;
	}
	
	len = curwp->w_doto - co;
	if( len > NPAT-pptr-2 )
		goto patfull;
	strncpy(&pat[pptr], ltext(cp)+co, len);
	pptr += len;
	pat[pptr] = '\0';
	chunklen += len;
	curwp->w_doto = oo;
	curwp->w_dotp = op;
	
	return chunklen;

 patfull:
	curwp->w_doto = oo;
	curwp->w_dotp = op;
	ewprintf("Pattern too long");
	return 0;
}

/*
 * Query Replace.
 *	Replace strings selectively.  Does a search and replace operation.
 */
/*ARGSUSED*/
queryrepl(f, n)
{
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

#ifndef NO_MACRO
	if(macrodef) {
	    ewprintf("Can't query replace in macro");
	    return FALSE;
	}
#endif
	if ((s=readpattern("Query replace")) != TRUE)
		return (s);
	if ((s=ereply("Query replace %s with: ",news, NPAT, pat)) == ABORT)
		return (s);
	if (s == FALSE)
		news[0] = '\0';
	ewprintf("Query replacing %s with %s:", pat, news);
	plen = strlen(pat);

	/*
	 * Search forward repeatedly, checking each time whether to insert
	 * or not.  The "!" case makes the check always true, so it gets put
	 * into a tighter loop for efficiency.
	 */

	while (forwsrch() == TRUE) {
	retry:
		update();
		switch (getkey(FALSE)) {
		case ' ':
#ifdef EMACS_QUERY_KEYS
		case 'y':
		case 'Y':
#endif /* EMACS_QUERY_KEYS */
			if (lreplace((RSIZE) plen, news, f) == FALSE)
				return (FALSE);
			rcnt++;
			break;

		case '.':
			if (lreplace((RSIZE) plen, news, f) == FALSE)
				return (FALSE);
			rcnt++;
			goto stopsearch;

		case CCHR('G'): /* ^G or ESC */
			(VOID) ctrlg(FFRAND, 0);
		case CCHR('['):
#ifdef EMACS_QUERY_KEYS
		case CCHR('M'):
#endif /* EMACS_QUERY_KEYS */
			goto stopsearch;

		case '!':
			do {
				if (lreplace((RSIZE) plen, news, f) == FALSE)
					return (FALSE);
				rcnt++;
			} while (forwsrch() == TRUE);
			goto stopsearch;

		case CCHR('H'):
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
	if (rcnt == 0)
		ewprintf("(No replacements done)");
	else if (rcnt == 1)
		ewprintf("(1 replacement done)");
	else
		ewprintf("(%d replacements done)", rcnt);
	return TRUE;
}

/*
 * This routine does the real work of a
 * forward search. The pattern is sitting in the external
 * variable "pat". If found, dot is updated, the window system
 * is notified of the change, and TRUE is returned. If the
 * string isn't found, FALSE is returned.
 */
forwsrch() {
	register LINE	*clp;
	register int	cbo;
	register LINE	*tlp;
	register int	tbo;
	char		*pp;
	register int	c;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	kanji2nd = 0; /* Now on a KANJI 2nd byte. */
#endif	/* KANJI */

	clp = curwp->w_dotp;
	cbo = curwp->w_doto;
	for(;;) {
		if (cbo == llength(clp)) {
			if((clp = lforw(clp)) == curbp->b_linep) break;
			cbo = 0;
			c = CCHR('J');
		} else
			c = lgetc(clp, cbo++);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (kanji2nd != 0) {	/* We believe 'c' is KANJI 2nd byte. */
			kanji2nd--;
			continue;	/* Don't check with KANJI 2nd byte. */
		} else if (ISKANJI(c)) { /* 'c' is KANJI 1st byte.	*/
#ifdef	HOJO_KANJI
			if (ISHOJO(c))	kanji2nd = 2;
			else
#endif
			kanji2nd = 1; /* Next byte is KANJI 2nd.	*/
		}
#endif	/* KANJI */
		if (eq(c, pat[0]) != FALSE) {
			tlp = clp;
			tbo = cbo;
			pp  = &pat[1];
			while (*pp != 0) {
				if (tbo == llength(tlp)) {
					tlp = lforw(tlp);
					if (tlp == curbp->b_linep)
						goto fail;
					tbo = 0;
					c = CCHR('J');
				} else
					c = lgetc(tlp, tbo++);
				if (eq(c, *pp++) == FALSE)
					goto fail;
			}
			curwp->w_dotp  = tlp;
			curwp->w_doto  = tbo;
			curwp->w_flag |= WFMOVE;
			return TRUE;
		}
	fail:	;
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
backsrch() {
	register LINE	*clp;
	register int	cbo;
	register LINE	*tlp;
	register int	tbo;
	register int	c;
	register char	*epp;
	register char	*pp;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	kanji1st = 0; /* Now on a KANJI 1st byte. */
#endif	/* KANJI */

	for (epp = &pat[0]; epp[1] != 0; ++epp)
		;
	clp = curwp->w_dotp;
	cbo = curwp->w_doto;
	for (;;) {
		if (cbo == 0) {
			clp = lback(clp);
			if (clp == curbp->b_linep)
				return FALSE;
			cbo = llength(clp)+1;
		}
		if (--cbo == llength(clp))
			c = CCHR('J');
		else
			c = lgetc(clp,cbo);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (kanji1st != 0) {	/* We believe 'c' is KANJI 1st byte. */
			kanji1st--;
			continue;	/* Don't check with KANJI 1st byte. */
		} else if (ISKANJI(c)) { /* 'c' is KANJI 2nd byte.	*/
#ifdef	HOJO_KANJI
			if (ISHOJO(c))	kanji1st = 2;
			else
#endif
			kanji1st = 1; /* Next byte is KANJI 1st.	*/
		}
#endif	/* KANJI */
		if (eq(c, *epp) != FALSE) {
			tlp = clp;
			tbo = cbo;
			pp  = epp;
			while (pp != &pat[0]) {
				if (tbo == 0) {
					tlp = lback(tlp);
					if (tlp == curbp->b_linep)
						goto fail;
					tbo = llength(tlp)+1;
				}
				if (--tbo == llength(tlp))
					c = CCHR('J');
				else
					c = lgetc(tlp,tbo);
				if (eq(c, *--pp) == FALSE)
					goto fail;
			}
			curwp->w_dotp  = tlp;
			curwp->w_doto  = tbo;
			curwp->w_flag |= WFMOVE;
			return TRUE;
		}
	fail:	;
	}
	/*NOTREACHED*/
}

/*
 * Compare two characters.
 * The "bc" comes from the buffer.
 * It has its case folded out. The
 * "pc" is from the pattern.
 */
static int
eq(bc, pc)
register int bc, pc;
{
	bc = CHARMASK(bc);
	pc = CHARMASK(pc);
	if (bc == pc) return TRUE;
#ifdef	IS_ENHANCE
	if(!casefoldsearch) return FALSE;
#endif	
	if (ISUPPER(bc)) return TOLOWER(bc) == pc;
	if (ISUPPER(pc)) return bc == TOLOWER(pc);
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
readpattern(prompt) char *prompt; {
	register int	s;
	char		tpat[NPAT];

	if (pat[0] == '\0') s = ereply("%s: ", tpat, NPAT, prompt);
	else s = ereply("%s: (default %s) ", tpat, NPAT, prompt, pat);

	if (s == TRUE)				/* Specified		*/
		(VOID) strcpy(pat, tpat);
	else if (s==FALSE && pat[0]!=0)		/* CR, but old one	*/
		s = TRUE;
	return s;
}
