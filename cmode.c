/* $Id: cmode.c,v 1.4.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 *		C code editing commands
 *		There are only used when C_MODE is #defined.
 *
 *		Coded by Kazuhiro Takano
 */

/*
 * $Log: cmode.c,v $
 * Revision 1.4.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.4  2001/05/25 17:53:33  amura
 * change LABEL: check routine
 *
 * Revision 1.3  2000/12/14 18:12:13  amura
 * use alloca() and more memory secure
 *
 * Revision 1.2  2000/09/18 10:19:52  amura
 * calc_indent() is fixed
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 * Revision 1.2  2000/03/10  21:25:51  amura
 * Almost Rewrite calc_indent.
 *
 * Revision 1.1  1999/05/14  22:43:36  amura
 * Initial revision
 *
 */
/* 90.07.24	Created by K.Takano */

/* #define CMODE_DEBUG */
#include "config.h"	/* 90.12.20  by S.Yoshida */

#ifdef	C_MODE
#include "def.h"
#ifdef	UNDO
#include "undo.h"
#endif

static int indent_level			=  2;
			/* Indentation of C statements with respect to	*
			 * containing block.				*/
static int brace_imaginary_offset	=  0;
			/* Imagined indentation of a C open brace that	*
			 * actually follows a statement			*/
static int brace_offset			=  0;
			/* Extra indentation for braces, compared with	*
			 * other text in same context.			*/
static int argdecl_indent		=  5;
		/* not support yet */
			/* Indentation level of declarations of		*
			 * C function arguments.			*/
static int label_offset			= -2;
			/* Offset of C label lines and case statements	*
			 * relative to usual indentation.		*/
static int continued_statement_offset	=  2;
			/* Extra indent for lines not starting new	*
			 * statements.					*/
static int continued_brace_offset	=  0;
			/* Extra indent for substatements that start	*
			 * with open-braces. This is in addition to	*
			 * c-continued-statement-offset.		*/

static int auto_newline			=  FALSE;
		/* not support yet */
			/* Non-nil means automatically newline before	*
			 * and after braces, and after colons and	*
			 * semicolons, inserted in C code.		*
			 * Here, FALSE mean nil. (91.01.15 by S.Yoshida) */
static int tab_always_indent		=  TRUE;
			/* Non-nil means TAB in C mode should always	*
			 * reindent the current line, regardless of	*
			 * where in the line point is when the TAB	*
			 * command is used.				*
			 * Here, TRUE mean t. (91.01.15 by S.Yoshida)	*/

/* 91.02.06  Add static declaration for some compiler. by S.Yoshida */
static int	calc_indent();
static int	adjust_spc();
static int	check_bal();
static int	count_column();
extern int	getnum();
extern int	blinkmatch();
static int	do_cm_brace();	/* Nov 1991. bsh */

/*
 * COMMAND: use-c-mode
 */
int	flag_use_c_mode	= TRUE;

/*ARGSUSED*/
cm_use_c_mode(f, n)
{
    register int	s;
    char	buf[NINPUT];

    if ((f & FFARG) == 0) {
	if ((s = ereply("use-c-mode : ", buf, sizeof(buf))) != TRUE)
	    return (s);
	if (ISDIGIT(buf[0]) || buf[0] == '-')
	    n = (atoi(buf) > 0);
	else if (buf[0] == 't' || buf[0] == 'T')
	    n = TRUE;
	else	n = FALSE;
    }
    flag_use_c_mode = n;
    return (TRUE);
}

/*
 * COMMAND: electric-c-brace
 */
/*ARGSUSED*/
cm_brace(f, n)
{
    return do_cm_brace(f,n,0);
}

/*
 * COMMAND: electric-c-brace-blink
 *   Nov 1991. Added by bsh.
 */
/*ARGSUSED*/
cm_brace_blink(f, n)
{
    return do_cm_brace(f,n,1);
}

static int
do_cm_brace(f, n, blink)
{    
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY)	/* If this buffer is read-only, */
    {
	warnreadonly();		/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    if (n < 0)
	return (FALSE);
    
    while( n-- > 0 ) {
	if(!selfinsert(f|FFRAND,1))
	    return FALSE;
	cm_indent(FFRAND,1);
	if( blink )
	    blinkmatch( curwp->w_dotp, curwp->w_doto-1 );
    }
    return TRUE;
}


/*
 * COMMAND: electric-c-semi
 */
/*ARGSUSED*/
cm_semi(f, n)
{
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY)	/* If this buffer is read-only, */
    {
	warnreadonly();		/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    if (auto_newline)
	return (cm_term(FFRAND, n));
    else
	return (selfinsert(FFRAND, n));
}


/*
 * COMMAND: electric-c-terminator
 */
/*ARGSUSED*/
cm_term(f, n)
{
    int c,i;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY)	/* If this buffer is read-only, */
    {
	warnreadonly();		/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    if (n == ';')
    {
	for (i = curwp->w_doto - 1; i >= 0; i--)
	{
	    c = lgetc(curwp->w_dotp, i);
	    if (c != ' ' && c != '\t')
		break;
	}
	i++;
	curwp->w_doto = i;
    }
    if (selfinsert(FFRAND, n) == FALSE)
	return (FALSE);
    if (cm_indent(FFRAND, n) == FALSE)
	return (FALSE);
    if (!auto_newline)
	return (TRUE);
    return newline(FFRAND, n);
}


/*
 * COMMAND: c-indent-command
 */
/*ARGSUSED*/
cm_indent(f, n)
{
    int i;
    char c;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY)
    {	/* If this buffer is read-only, */
	warnreadonly();		/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    if ((f & (FFRAND | FFARG)) == 0 && tab_always_indent == FALSE)
    {
	for (i = curwp->w_doto - 1; i >= 0; i--)
	{
	    c = lgetc(curwp->w_dotp, i);
	    if (c != ' ' && c != '\t')
		break;
	}
	if (i >= 0)
	    return (selfinsert(FFRAND, n));
    }

    i = calc_indent();

    if (i < 0 && !(f&FFRAND))
	return (selfinsert(FFRAND, n));
    else if (i < 0)
	return (TRUE);

    return (adjust_spc(i));
}


/*
 * COMMAND: c-newline-and-indent
 */
/*ARGSUSED*/
cm_lfindent(f, n)
{
    register int	i;
    register unsigned char c;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY)
    {				/* If this buffer is read-only, */
	warnreadonly();		/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */

    if (n < 0)
	return (FALSE);

    for (i = curwp->w_doto - 1; i >= 0; i--)
    {
	c = lgetc(curwp->w_dotp, i);
        if (c != ' ' && c != '\t')
	    break;
    }
    i++;
    curwp->w_doto = i;

    if (newline(FFRAND, n) == FALSE)
	return (FALSE);
    return (cm_indent(FFRAND, 1));
}


static calc_indent()
{
    register LINE	*lp;
    register int	bo;
    unsigned char	bal1, bal2, c;
    LINE	*tlp;
    int	tbo, cbo;
    int	s;
    int	depth;
    int	in_paren = FALSE;
    int	in_comment = FALSE;
    int	null_line = TRUE;
    int	has_semi = FALSE;
    int	firstcheck = TRUE;
    int	in_strings = FALSE;	/* 91.01.13  by S.Yoshida */
    int	numesc = 0;		/* 91.01.13  by S.Yoshida */
    char	termchar;	/* 91.01.13  by S.Yoshida */
    int	indent = 0;
    int with_colon = FALSE;	/* 00.02.16  by amura */
#ifdef  VARIABLE_TAB
    int	tab = curwp->w_bufp->b_tabwidth;
#endif  /* VARIABLE_TAB */
#ifdef	CMODE_DEBUG
    char state[] = "    ";
#endif

/*-- Beggining of line check --*/
    lp = curwp->w_dotp;
    c = '\0';
    for (cbo = 0; cbo < llength(lp); cbo++)
    {
	c = lgetc(lp, cbo);
	if (c != ' ' && c != '\t')
	    break;
    }
#ifdef	CMODE_DEBUG
    ewprintf("");
#endif
    if (c == '#')			/* for pre-processer line	*/
	return (-1);			/* no indent			*/


/*-- Label check --*/	
    c = lgetc(lp, cbo);
    if (ISWORD(c) || c == '_' || c == '-')
    {
	for (bo = cbo+1; bo < llength(lp); bo++)
	{
	    /* 91.01.13  Modify to consider strings  by S.Yoshida */
	    c = lgetc(lp, bo);
	    if (c == ':')
	    {
		with_colon = TRUE;
		break;
	    }
	    if (c == '<' || c == '>' || c == '=' ||
		c == '[' || c == ']' || c == '\\')
		break;
	}
    }

/*-- Search before left palentheis --*/
    bo = 0;
    bal1 = bal2 = '\0';
    in_strings = FALSE;	/* 91.01.13  by S.Yoshida */
    numesc = 0;		/* 91.01.13  by S.Yoshida */
    termchar = '\0';	/* 91.01.13  by S.Yoshida */
    for (;;)
    {
	if (bo == 0)		/* beginning of line	*/
	{
	    if (firstcheck && !null_line)
	    {
		if (in_paren && bal1 != '{')
		    ;
		else if (!has_semi)
		{
		    with_colon = FALSE;
		    indent += continued_statement_offset;
#ifdef	CMODE_DEBUG
		    if (state[1]=='C' || state[1]=='2')
			state[1] = '2';
		    else
			state[1] = 'C';
#endif
		    if (lgetc(curwp->w_dotp, cbo) == '{')
		    {
			indent += continued_brace_offset;
#ifdef	CMODE_DEBUG
			state[2] = '{';
#endif
		    }
		}
		else
		    firstcheck = FALSE;
	    }
	    null_line = TRUE;
	    
	    lp = lback(lp);
	    if (lp == curbp->b_linep)		/* beginning of buffer	*/
		break;
	    bo = llength(lp);
	    for (s = 0; s < bo; s++)		/* skip preprocess line */
	    {
		c = lgetc(lp, s);
		if (c == '#')
		{
		    bo = 0;
		    continue;
		}
		else if (c != ' ' || c != '\t')
		    break;
	    }
	    c = lgetc(lp, 0);
	    for (s = 0; s < bo-1; s++)		/* skip B style comment */
		if (c == '/' && (c = lgetc(lp, s+1)) =='/')
		{
		    bo = s;
		    break;
		}
	    if (bo == 0)			/* skip null line	*/
		continue;
	}
	bo--;
	c = lgetc(lp, bo);			/* somewhere in middle	*/

	if (c == ' ' || c == '\t')		/* skip space		*/
	    continue;
	
	if (!in_strings && c == '*' && bo > 0 && lgetc(lp, bo-1) == '/')
	{					/* comment start mark	*/
	    if (in_comment)
	    {
		bo--;
		in_comment = FALSE;
		continue;
	    }
#ifdef	CMODE_DEBUG
	    ewprintf("in comment");
#endif
	    return (count_column(lp, bo));
	}
	if (in_comment)				/* skip comment		*/
	    continue;
	if (!in_strings && c == '/' && bo > 0 && lgetc(lp, bo-1) == '*')
	{					/* comment end mark	*/
	    bo--;
	    in_comment = TRUE;
	    continue;
	}
	null_line = FALSE;

	/* 91.01.13  Add to consider strings.  by S.Yoshida */
	if (in_strings)
	{
	    if (c == termchar)
	    {
		if (bo==0 || lgetc(lp,bo-1)!='\\')
		    in_strings = FALSE;
		else
		    bo--;
	    }
	    continue;
	}
	if (c == '"' || c == '\'')		/* Not in strings.	*/
	{
	    in_strings = TRUE;
	    termchar = c;
	    continue;
	}

	if (in_paren)
	{
	    if (bal2 == '{')
		has_semi = TRUE;
	    if (c == bal2)			/* left parenthesis	*/
	    {
		if (--depth == 0)
		    in_paren = FALSE;
	    }
	    else if (c == bal1)			/* right parenthesis	*/
		depth++;
	    continue;
	}

	if (c == ';' || c == ':')
	{
	    has_semi = TRUE;
	    continue;
	}
	
	bal1 = bal2 = c;
	s = check_bal(&bal2);
	if (s < 0)				/* right parenthesis	*/
	{
	    if (c == '}')
		has_semi = TRUE;
	    depth = 1;
	    in_paren = TRUE;	/* search match paren	*/
	    continue;
	}
	if (s > 0)				/* left parenthesis	*/
	    break;
    }
    tlp = lp; tbo = bo;

    if (lp == curbp->b_linep)		/* line is at top level	*/
    {
#ifdef	CMODE_DEBUG
	ewprintf("at top-level");
#endif
	return (0);
    }
    
    if (bal1 != '{')			/* line is expression	*/
    {
#ifdef	CMODE_DEBUG
	ewprintf("before is express parlentheis");
#endif
	return (count_column(tlp, tbo)+1);
    }

/*-- Check brace imaginary offset --*/
    for (bo--; bo >= 0; bo--)
    {
	c = lgetc(lp, bo);
	if (c != ' ' && c != '\t')
	{
	    indent += brace_imaginary_offset;
	    break;
	}
    }
    
/*-- Before left parrenthesis columns --*/
    s = 0;
    for (bo = 0; bo <= tbo; bo++)
    {
	c = lgetc(tlp, bo);
	if (c != ' ' && c != '\t')
	    break;
	if (c == '\t')
#ifdef VARIABLE_TAB
	    s = (s/tab + 1)*tab -1;
#else
	    s |= 0x07;
#endif
	s++;
    }
    if (s < 0)
	s = 0;

    in_strings = FALSE;	/* 91.01.13  by S.Yoshida */
    numesc = 0;		/* 91.01.13  by S.Yoshida */
    termchar = '\0';	/* 91.01.13  by S.Yoshida */
    lp = curwp->w_dotp;
    if (lgetc(lp, cbo) == '{')		/* left brace on line-top */
    {
	with_colon = FALSE;
	indent += brace_offset + indent_level;
    }
    
    indent += indent_level;
    
    for (bo = cbo; bo < llength(lp); bo++)
    {
	depth = 0;
	c = lgetc(lp, bo);
	/* 91.01.13  Modify & add to consider strings.  by S.Yoshida */
	if (c == '}' && !in_strings)
	{
	    if (depth == 0)		/* include close parenthesis	*/
	    {
		indent = 0;
		break;
	    }
	    depth--;
	} 
	else if (c == '{' && !in_strings)
	    depth++;
	else if (in_strings)
	{
	    if (c == '"' || c == '\'')
	    {
		if (c == termchar && (numesc % 2) == 0)
		    in_strings = FALSE;
		else
		    numesc = 0;
	    } 
	    else if (c == '\\')
		numesc++;
	    else
		numesc = 0;
	} 
	else if (c == '"' || c == '\'')	/* Not in_strings. */
	{
	    in_strings = TRUE;
	    termchar = c;
	    numesc = 0;
	}
    }
    
    if (with_colon)
    {
	indent += label_offset;
#ifdef	CMODE_DEBUG
	state[0] = 'L';
#endif
    }
#ifdef	CMODE_DEBUG
    ewprintf("%s before:%d append:%d '%c'", state, s, indent,
	     lgetc(tlp, tbo));
#endif
    return (s+indent)>=0 ? (s+indent) : 0;
}


static adjust_spc(nicol)
int nicol;
{
    register int	i;
    register unsigned char c;
    int	cbo;
#ifdef  VARIABLE_TAB
    int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

    cbo = curwp->w_doto;
    for (i = 0; i < llength(curwp->w_dotp); i++)
    {
	c = lgetc(curwp->w_dotp, i);
	if (c != ' ' && c != '\t')
	    break;
    }

    if ((cbo -= i) < 0)
	cbo = 0;
    curwp->w_doto = 0;		/* delete heading space */
    if (i != 0 && ldelete((RSIZE)i, KNONE) == FALSE)
	return (FALSE);

#ifdef	UNDO
    if (undoptr!=NULL && *undoptr!=NULL)
	(*undoptr)->u_type = UDNONE;
		/* insert space (and/or tab) */
    if ((curbp->b_flag & BFNOTAB) ?
	      (cbo += nicol, linsert(nicol, ' ') == FALSE) :
#ifdef  VARIABLE_TAB
	    ((i = nicol / tab)!=0 && (cbo += i, linsert(i, '\t') == FALSE)) ||
            ((i = nicol % tab)!=0 && (undoptr = undobefore,
				      cbo += i, linsert(i, ' ' ) == FALSE)))
#else
	    ((i = nicol / 8) != 0 && (cbo += i, linsert(i, '\t') == FALSE)) ||
	    ((i = nicol % 8) != 0 && (undoptr = undobefore,
				      cbo += i, linsert(i, ' ' ) == FALSE)))
#endif
	return (FALSE);
#else	/* NOT UNDO */
	/* insert space (and/or tab) */
    if ((curbp->b_flag & BFNOTAB) ?
	      (cbo += nicol, linsert(nicol, ' ') == FALSE) :
#ifdef  VARIABLE_TAB
	    ((i = nicol / tab)!=0 && (cbo += i, linsert(i, '\t') == FALSE)) ||
	    ((i = nicol % tab)!=0 && (cbo += i, linsert(i, ' ' ) == FALSE)))
#else
	    ((i = nicol / 8) != 0 && (cbo += i, linsert(i, '\t') == FALSE)) ||
	    ((i = nicol % 8) != 0 && (cbo += i, linsert(i, ' ' ) == FALSE)))
#endif
	return (FALSE);
#endif	/* UNDO */

    curwp->w_doto = cbo;
    return (TRUE);
}


static check_bal(balc)
unsigned char *balc;
{
    static struct {
	unsigned char left, right;
    } bal[] = {
	{ '(', ')' },
	{ '[', ']' },
	{ '{', '}' },
	{ '\0','\0'}
    };
    register int	i;

    for (i = 0; bal[i].right != '\0'; i++)
    {
	if (bal[i].right == *balc)
	{
	    *balc = bal[i].left;
	    return (-1);
	}
	if (bal[i].left == *balc)
	{
	    *balc = bal[i].right;
	    return (1);
	}
    }
    return (0);
}


static count_column(lp, bo)
LINE *lp;
int bo;
{
    register int i;
    register int col;
#ifdef  VARIABLE_TAB
    int	tab = curwp->w_bufp->b_tabwidth;
#endif  /* VARIABLE_TAB */

    col = 0;
    for (i = 0; i < bo; i++)
    {
	if (lgetc(lp, i) == '\t')
#ifdef  VARIABLE_TAB
	    col = (col/tab + 1)*tab -1;
#else
	    col |= 7;
#endif
	col++;
    }
    return (col);
}

/*
 *	Set and Display variables
 */

/*
 * COMMAND: set-c-indent-level
 */
/*ARGSUSED*/
cm_set_indent(f, n)
{
    if ((f & FFARG) == 0) {
    	if (getnum("c-indent-level", &n) == FALSE)
	    return (FALSE);
    }
    indent_level = n;
    return (TRUE);
}

/*
 * COMMAND: set-c-brace-imaginary-offset
 */
/*ARGSUSED*/
cm_set_imagin(f, n)
{
	if ((f & FFARG) == 0) {
		if (getnum("c-brace_imaginary-offset", &n) == FALSE)
			return (FALSE);
	}
	brace_imaginary_offset = n;
	return (TRUE);
}


/*
 * COMMAND: set-c-brace-offset
 */
/*ARGSUSED*/
cm_set_brace(f, n)
{
	if ((f & FFARG) == 0) {
		if (getnum("c-brace-offset", &n) == FALSE)
			return (FALSE);
	}
	brace_offset = n;
	return (TRUE);
}


/*
 * COMMAND: set-c-argdecl-indent
 */
/*ARGSUSED*/
cm_set_arg(f, n)
{
	if ((f & FFARG) == 0) {
		if (getnum("c-argdecl-indent", &n) == FALSE)
			return (FALSE);
	}
	argdecl_indent = n;
	return (TRUE);
}


/*
 * COMMAND: set-c-label-offset
 */
/*ARGSUSED*/
cm_set_label(f, n)
{
	if ((f & FFARG) == 0) {
		if (getnum("c-label-offset", &n) == FALSE)
			return (FALSE);
	}
	label_offset = n;
	return (TRUE);
}


/*
 * COMMAND: set-c-continued-statement-offset
 */
/*ARGSUSED*/
cm_set_cstat(f, n)
{
	if ((f & FFARG) == 0) {
		if (getnum("c-continued-statement-offset", &n) == FALSE)
			return (FALSE);
	}
	continued_statement_offset = n;
	return (TRUE);
}


/*
 * COMMAND: set-c-continued-brace-offset
 */
/*ARGSUSED*/
cm_set_cbrace(f, n)
{
	if ((f & FFARG) == 0) {
		if (getnum("c-continued-brace-offset", &n) == FALSE)
			return (FALSE);
	}
	continued_brace_offset = n;
	return (TRUE);
}


/*
 * COMMAND: set-c-auto-newline
 * 91.01.15  Modified by S.Yoshida
 * Add routine to consider "t" and "nil" argument.
 */
/*ARGSUSED*/
cm_set_newl(f, n)
{
	register int	s;
	char	buf[NINPUT];

	if ((f & FFARG) == 0) {
		if ((s = ereply("c-auto-newline : ", buf, sizeof(buf))) != TRUE)
			return (s);
		if (ISDIGIT(buf[0]) || buf[0] == '-')
			n = (atoi(buf) > 0);
		else if (buf[0] == 't' || buf[0] == 'T')
			n = TRUE;
		else /* if (buf[0] == 'n' || buf[0] == 'N') */
			n = FALSE;
	}
	auto_newline = n;
	return (TRUE);
}


/*
 * COMMAND: set-c-tab-always-indent
 * 91.01.15  Modified by S.Yoshida
 * Add routine to consider "t" and "nil" argument.
 */
/*ARGSUSED*/
cm_set_tab(f, n)
{
	register int	s;
	char	buf[NINPUT];

	if ((f & FFARG) == 0) {
		if ((s = ereply("c-tab-always-indent : ", buf, sizeof(buf))) != TRUE)
			return (s);
		if (ISDIGIT(buf[0]) || buf[0] == '-')
			n = (atoi(buf) > 0);
		else if (buf[0] == 't' || buf[0] == 'T')
			n = TRUE;
		else /* if (buf[0] == 'n' || buf[0] == 'N') */
			n = FALSE;
	}
	tab_always_indent = n;
	return (TRUE);
}


/*
 * COMMAND: list-c-mode-variables
 */
/*ARGSUSED*/
cm_list_var(f, n)
{
	register BUFFER	*bp;
	register WINDOW	*wp;
	char	 line[80];

	if ((bp = bfind("*C Mode Variables*", TRUE)) == NULL) return FALSE;
	bp->b_flag &= ~BFCHG;		/* Blow away old.	*/
	if (bclear(bp) != TRUE) return FALSE;

	strcpy(line, "* List of variables controlling indentation style *");
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-indent-level : %d", indent_level);
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-brace-imaginary-offset : %d", brace_imaginary_offset);
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-brace-offset : %d", brace_offset);
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-argdecl-indent : %d", argdecl_indent);
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-label-offset : %d", label_offset);
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-continued-statement-offset : %d", continued_statement_offset);
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-continued-brace-offset : %d", continued_brace_offset);
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-auto-newline : %s",
		auto_newline ? "T" : "NIL");
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\tc-tab-always-indent : %s",
		tab_always_indent ? "T" : "NIL");
	if (addline(bp, line) == FALSE) return FALSE;

	if ((wp = popbuf(bp)) == NULL)	return FALSE;
	bp->b_dotp = lforw(bp->b_linep); /* put dot at beginning of buffer */
	bp->b_doto = 0;
	wp->w_dotp = bp->b_dotp;	/* fix up if window already on screen */
	wp->w_doto = bp->b_doto;
	return TRUE;
}

/*
 * COMMAND: c-indent-region
 * 91.09.14  Modified by Y.Koyanagi
 * re-indent region.
 */
/*ARGSUSED*/
cm_indentregion(f, n)
{
	register LINE	*startp,*endp;
	register int	loffs;
	register int	c, cbo;
	register int	s;
	REGION		region;
	enum { WHITELINE, COMMENT, OTHERS } type;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if ((s=getregion(&region)) != TRUE)
		return s;
	startp = endp = region.r_linep;
	loffs = region.r_offset;
	while (region.r_size--) {
		if (loffs == llength(endp)) {
			endp = lforw(endp);
			loffs = 0;
		} else {
			++loffs;
		}
	}
	curwp->w_dotp = startp;
	curwp->w_doto = 0;
	curwp->w_flag |= WFMOVE;
	while (curwp->w_dotp != endp) {
		type = WHITELINE;
		for (cbo = 0; cbo < llength(curwp->w_dotp); cbo++) {
			c = lgetc(curwp->w_dotp, cbo);
			if (c == '\t' || c == ' ') {
				continue;
			} else if (c == '/') {
				if ((cbo+1) < llength(curwp->w_dotp) &&
				    lgetc(curwp->w_dotp, cbo+1) == '*') {
					type = COMMENT;
				} else {
					type = OTHERS;
				}
				break;
			} else {
				type = OTHERS;
				break;
			}
		}
		if (type == OTHERS) {
			if (cm_indent(FFRAND,1) != TRUE) return FALSE;
		}
		if (forwline(FFRAND,1) != TRUE) return FALSE;
	}
	return TRUE;
}
#endif	/* C_MODE */
