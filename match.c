/* $Id: match.c,v 1.3.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 * Name:	MicroEMACS
 *		Limited parenthesis matching routines
 *
 * The hacks in this file implement automatic matching
 * of (), [], {}, and other characters.	 It would be
 * better to have a full-blown syntax table, but there's
 * enough overhead in the editor as it is.
 *
 * Since I often edit Scribe code, I've made it possible to
 * blink arbitrary characters -- just bind delimiter characters
 * to "blink-matching-paren-hack"
 */

/*
 * $Log: match.c,v $
 * Revision 1.3.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.3  2001/02/11 15:38:05  amura
 * bugfix on VARIABLE_TAB suggested by ng1.3.1L6
 *
 * Revision 1.2  2000/11/16 14:31:13  amura
 * fix some typos which cause compile error when using
 * strict ANSI-C compiler (ex ACK, gcc-1.x)
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */
/* Nov 1991. Modified by bsh to add electric-c-brace-blink and
 *           goto-matching-fence.
 */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	"key.h"

static VOID	displaymatch();
static int	getmatch();     /* Nov 91. Added by bsh */

/* Balance table. When balance() encounters a character
 * that is to be matched, it first searches this table
 * for a balancing left-side character.	 If the character
 * is not in the table, the character is balanced by itself.
 * This is to allow delimiters in Scribe documents to be matched.
 */

static struct balance {
	char left, right;
} bal[] = {
	{ '(', ')' },
	{ '[', ']' },
	{ '{', '}' },
	{ '<', '>' },
	{ '\0','\0'}
};

/*
 * Self-insert character, then show matching character,
 * if any.  Bound to "blink-matching-paren-command".
 */

showmatch(f, n)
{
	register int  i, s;
	LINE *clp;
	int cbo;

	for (i = 0; i < n; i++) {
		if ((s = selfinsert(FFRAND|f, 1)) != TRUE)
			return s;
	        if( !getmatch(curwp->w_dotp, curwp->w_doto - 1, &clp, &cbo, -1))
		        ttbeep();
	        else
		        displaymatch(clp,cbo);
	}
	return TRUE;
}


/*
 * goto-matching-fence a la kemacs.
 *  Nov 1991. Added by bsh.
 */
gotomatch(f,n)
{
	LINE *clp;
	int cbo;
	int dir = 0;

	if( f & FFNEGARG ) dir = -1;
	else if( f & FFUNIV ) dir = n;

	if( !getmatch(curwp->w_dotp, curwp->w_doto, &clp, &cbo, dir)){
		ttbeep();
		return FALSE;
	}

	curwp->w_dotp = clp;
	curwp->w_doto = cbo;
	curwp->w_flag |= WFMOVE;
	return TRUE;
}


/* bsh: Mainly for cm_brace_blink */
blinkmatch( clp, cbo )
LINE *clp;
int cbo;
{
    LINE *mlp;
    int  mbo;
    
    if( !getmatch(clp,cbo,&mlp,&mbo,-1) )
	return FALSE;
    displaymatch(mlp,mbo);
    return TRUE;
}

static
getmatch( clp, cbo, mlp, mbo, dir )
register LINE *clp;
register int cbo;
LINE     **mlp;
int      *mbo;
int	dir;   /* Default direction if not decided from the char at dot*/
{
	
	int i, rbal, lbal;
	int depth, c;

	rbal = lbal = cbo >=llength(clp) ? '\n' : lgetc(clp,cbo);
	for (i = 0; bal[i].right != '\0'; i++)
		if (bal[i].right == rbal) {
			lbal = bal[i].left;
			dir = -1;	/* Backward */
			break;
		}
		else if(bal[i].left == lbal){
			rbal = bal[i].right;
			dir = 1;	/* Forward */
			break;
		}
	

	if( dir == 0 ) return FALSE;
	depth = 0;
	if( dir < 0 )	for (;;) {
		if (cbo == 0) {			/* beginning of line	*/
			clp = lback(clp);
			if (clp == curbp->b_linep)
				return (FALSE);
			cbo = llength(clp);	/* End of line */
			c = '\n';
		}
		else {
			--cbo;
			c = lgetc(clp,cbo);	/* somewhere in middle	*/
		}
		
		
		/* Check for a matching character.  If still in a nested */
		/* level, pop out of it and continue search.  This check */
		/* is done before the nesting check so single-character	 */
		/* matches will work too.				 */
		if (c == lbal) {
			if (depth == 0) {
				*mlp = clp;
				*mbo = cbo;
				return (TRUE);
			}
			else
				depth--;
		}
		/* Check for another level of nesting.	*/
		if (c == rbal)
			depth++;
	}
	else for(++cbo;;++cbo){
		if( cbo > llength(clp) ){
			if ((clp= lforw(clp)) == curbp->b_linep) return FALSE;
			cbo = 0;
		}
		if( cbo == llength(clp) ) c = '\n';
		else  c = lgetc(clp,cbo);
		if( c == rbal ){
			if( depth == 0 ){
				*mlp = clp;
				*mbo = cbo;
				return  TRUE;
			}
			--depth;
		}
		if( c == lbal ) ++depth;
	}

	return FALSE;
}
/*
 * Display matching character.
 * Matching characters that are not in the current window
 * are displayed in the echo line. If in the current
 * window, move dot to the matching character,
 * sit there a while, then move back.
 */

static VOID displaymatch(clp, cbo)
register LINE *clp;
register int  cbo;
{
	register LINE	*tlp;
	register int	tbo;
	register int	cp;
	register int	bufo;
	register int	c;
	int		inwindow;
	char		buf[NLINE];

	/* Figure out if matching char is in current window by	*/
	/* searching from the top of the window to dot.		*/

	inwindow = FALSE;
	for (tlp = curwp->w_linep; tlp != lforw(curwp->w_dotp); tlp = lforw(tlp))
		if (tlp == clp)
			inwindow = TRUE;

	if (inwindow == TRUE) {
		tlp = curwp->w_dotp;	/* save current position */
		tbo = curwp->w_doto;

		curwp->w_dotp  = clp;	/* move to new position */
		curwp->w_doto  = cbo;
		curwp->w_flag |= WFMOVE;

		update();		/* show match */
#ifdef	ADDFUNC	/* 91.01.23  by Sawayanagi Yosirou */
		ttwait();		/* wait 1 sec. or key press. */
#else	/* NOT ADDFUNC */
		sleep(1);		/* wait a bit */
#endif	/* ADDFUNC */

		curwp->w_dotp	= tlp;	/* return to old position */
		curwp->w_doto	= tbo;
		curwp->w_flag  |= WFMOVE;
		update();
	}
	else {	/* match not in this window so display line in echo area */
		bufo = 0;
		for (cp = 0; cp < llength(clp); cp++) { /* expand tabs	*/
			c = lgetc(clp,cp);
			if (c != '\t' || (curbp->b_flag & BFNOTAB))
				if(ISCTRL(c)) {
				    buf[bufo++] = '^';
				    buf[bufo++] = CCHR(c);
				} else buf[bufo++] = c;
			else
				do {
					buf[bufo++] = ' ';
#ifdef	VARIABLE_TABWIDTH
				} while (bufo % curbp->b_tabwidth);
#else
				} while (bufo & 7);
#endif
		}
		buf[bufo++] = '\0';
		ewprintf("Matches %s",buf);
	}
}
