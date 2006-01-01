/* $Id: complt.c,v 1.11.2.6 2006/01/01 18:34:13 amura Exp $ */
/*
 *	Complete completion functions.
 */
/* 90.12.10  Created by Sawayanagi Yosirou */

#include "config.h"
#ifdef NEW_COMPLETE
#include "def.h"
#include "complt.h"

#include "i_buffer.h"
#include "i_window.h"
#include "i_lang.h"
#include "kbd.h"
#include "buffer.h"
#include "basic.h"
#include "echo.h"
#include "tty.h"
#include "ttyio.h"
#include "fileio.h"
#include "window.h"
#include "keymap.h"

#define LIST_COL	35

static BUFFER *bp = NULL;
static BUFFER *prev_bp = NULL;
static WINDOW *prev_wp = NULL;
static WINDOW prev_window;

static int complete_funcname _PRO((NG_WCHAR_t *));
static int complete_buffername _PRO((NG_WCHAR_t *));
static int complete_filename _PRO((NG_WCHAR_t *));
static int complete_list_funcnames _PRO((NG_WCHAR_t *, BUFFER *));
static int complete_list_buffernames _PRO((NG_WCHAR_t *, BUFFER *));
static int complete_list_filenames _PRO((NG_WCHAR_t *, BUFFER *));

/*
 * do some completion.
 */
int
complete(buf, flags)
NG_WCHAR_t *buf;
int flags;
{
    int res;
    
    switch (flags & EFAUTO) {
    case EFFUNC:
	res = complete_funcname (buf);
	break;
    case EFBUF:
	res = complete_buffername (buf);
	break;
    case EFFILE:
	res = complete_filename (buf);
	break;
    default:
	res = 0; /* dummy to prevent compile time warning */
	panic ("broken complete call: flags");
    }

    return (res);
}

char *
complete_message(matchnum)
int matchnum;
{
    char *msg;

    switch (matchnum) {
    case COMPLT_NOT_UNIQUE:
	msg = " [Complete, but not unique]";
	break;
    case COMPLT_AMBIGUOUS:
	msg = " [Ambiguous]";
	break;
    case COMPLT_SOLE:
	/* msg = "";*/
	msg = " [Sole completion]";
	break;
    case COMPLT_NO_MATCH:
	msg = " [No match]";
	break;
    default:
	msg = " [Internal error]";
    }
    
    return (msg);
}

/* complete function name */
static int
complete_funcname(wname)
NG_WCHAR_t *wname;
{
    int fnlen;
    char *name;
    int minlen = 0;
    int matchnum;
    int res;
    int i, j;
    char *cand;
    
    fnlen = wstrlen(wname);
    if ((name = (char *)alloca(fnlen+1)) == NULL)
	return -1;
    strlcpyw(name, wname, fnlen+1);
    
    /* compare names and make the common string of them */
    matchnum = 0;
    
    for (i = name_fent(name, TRUE); i < nfunct; i++) {
        cand = functnames[i].n_name;
	j = strncmp (cand, name, fnlen);
        if (j < 0)
	    continue;
        else if (j > 0)
	    break;	/* because functnames[] are in dictionary order */

	if (matchnum == 0) {
	    for (j = fnlen; cand[j] != '\0'; j++)
		name[j] = cand[j];
	    name[j] = '\0';
	    minlen = j;
	}
	else {
	    for (j = fnlen; name[j] != '\0'; j++) {
		if (cand[j] != name[j])
		    break;
	    }
	    name[j] = '\0';
	    if (cand[j] == '\0')
	        minlen = j;
	}
        matchnum++;
    }

    if (matchnum > 1)
	res = (minlen == (int) strlen (name)) ?
	    COMPLT_NOT_UNIQUE : COMPLT_AMBIGUOUS;
    else if (matchnum == 1)
	res = COMPLT_SOLE;
    else if (matchnum == 0)
	res = COMPLT_NO_MATCH;
    else
	res = -1;

    return (res);
}

static int
complete_buffername(wname)
NG_WCHAR_t *wname;
{
    int fnlen;
    char *name;
    int minlen = 0;
    int matchnum;
    int res;
    int j;
    char *cand;
    LIST *lh;
    
    fnlen = wstrlen(wname);
    if ((name = (char *)alloca(fnlen+1)) == NULL)
	return -1;
    strlcpyw(name, wname, fnlen+1);
    
    /* compare names and make the common string of them */
    matchnum = 0;
    for (lh = &(bheadp->b_list); lh != NULL; lh = lh->l_next) {
        cand = lh->l_name;
        if (strncmp (cand, name, fnlen) != 0)
	    continue;
	if (matchnum == 0) {
	    for (j = fnlen; cand[j] != '\0'; j++)
		name[j] = cand[j];
	    name[j] = '\0';
	    minlen = j;
	}
	else {
            for (j = fnlen; name[j] != '\0'; j++) {
                if (cand[j] != name[j])
		    break;
	    }
	    name[j] = '\0';
	    if (cand[j] == '\0')
	        minlen = j;
	}
        matchnum++;
    }

    if (matchnum > 1)
	res = (minlen == (int) strlen (name)) ?
	    COMPLT_NOT_UNIQUE : COMPLT_AMBIGUOUS;
    else if (matchnum == 1)
	res = COMPLT_SOLE;
    else if (matchnum == 0)
	res = COMPLT_NO_MATCH;
    else
	res = -1;

    return (res);
}

#ifndef NO_FILECOMP
static int
complete_filename(wname)
NG_WCHAR_t *wname;
{
    char *name;
    int fnlen;
    int minlen = 0;
    int matchnum;
    int res;
    int i, j;
    int fnnum;
    char *cand;
    char *filenames;

    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, wname, name);
    if (name == NULL)
	return -1;
    fnlen = strlen(name);
    
    if ((fnnum = fffiles (name, &filenames)) == -1)
	return (-1);    /* error */

    /* compare names and make a common string of them */
    matchnum = 0;
    cand = filenames;
    for (i = 0; i < fnnum; i++) {
	if (matchnum == 0) {
	    for (j = fnlen; cand[j] != '\0'; j++)
		name[j] = cand[j];
	    name[j] = '\0';
	    minlen = j;
	}
	else {
	    for (j = fnlen; name[j] != '\0'; j++) {
		if (cand[j] != name[j])
		    break;
	    }
	    name[j] = '\0';
	    if (cand[j] == '\0')
	        minlen = j;
	}
	matchnum++;
	cand += (strlen (cand) + 1);
    }
    free (filenames);

    if (matchnum > 1)
	res = (minlen == strlen (name)) ? COMPLT_NOT_UNIQUE : COMPLT_AMBIGUOUS;
    else if (matchnum == 1)
	res = COMPLT_SOLE;
    else if (matchnum == 0)
	res = COMPLT_NO_MATCH;
    else
	res = -1;

    return (res);
}
#endif	/* NO_FILECOMP */

int
complete_list_names(buf, flags)
NG_WCHAR_t *buf;
int flags;
{
    int res;
    int cur_row;
    int cur_col;
    WINDOW *wp;
    
    if ((bp = bfind ("*Completions*", TRUE)) == NULL)
	return (FALSE);
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
    bp->b_flag &= ~(BFCHG|BFACHG);    /* avoid recursive veread */
#else
    bp->b_flag &= ~BFCHG;    /* avoid recursive veread */
#endif	/* AUTOSAVE	*/
    if (bclear (bp) != TRUE)
	return (FALSE);
    
    if (addline(bp, _NG_WSTR("Possible completions are:")) == FALSE)
	return (FALSE);

    switch (flags & EFAUTO) {
    case EFFUNC:
	res = complete_list_funcnames (buf, bp);
	break;
    case EFBUF:
	res = complete_list_buffernames (buf, bp);
	break;
    case EFFILE:
	res = complete_list_filenames (buf, bp);
	break;
    default:
	panic ("broken complete_list_names call: flags");
    }

    cur_row = ttrow;
    cur_col = ttcol;
    
    bp->b_dotp = lforw (bp->b_linep);	/* put dot at beginning of buffer */
    bp->b_doto = 0;
    /* setup window */
    if (curwp->w_bufp != bp) {
	if (wheadp->w_wndp == NULL) {
	    if (splitwind (FFRAND, 0) == FALSE)
		return (FALSE);
	    prev_wp = wheadp;
	    curwp = wheadp->w_wndp;
	    prev_bp = NULL;
	}
	else {
            for (wp = wheadp; wp->w_wndp != NULL; wp = wp->w_wndp)
		;
            prev_wp = curwp;
	    curwp = wp;
	    prev_bp = curwp->w_bufp;
	    prev_window.w_dotp = curwp->w_dotp;
	    prev_window.w_doto = curwp->w_doto;
	    if (prev_window.w_bufp) {
		prev_window.w_bufp->b_markp = prev_bp->b_markp;
		prev_window.w_bufp->b_marko = prev_bp->b_marko;
	    }
	}
    }
    for (wp = wheadp; wp != NULL; wp = wp->w_wndp) {
        if (wp->w_bufp == bp) {
	    wp->w_flag |= WFMODE | WFFORCE | WFHARD;
	    wp->w_dotp = bp->b_dotp;
	    wp->w_doto = bp->b_doto;
	}
    }
    curbp = bp;
    if (showbuffer (bp, curwp, WFFORCE | WFHARD) == FALSE)
	return (FALSE);

    update ();
    ttmove (cur_row, cur_col);
    
    return (TRUE);
}

static int
complete_list_funcnames(wname, bp)
NG_WCHAR_t *wname;
BUFFER *bp;
{
    char *name;
    int fnlen;
    int i, j;
    char *cand;
    char line[NFILEN];
    NG_WCHAR_t wline[NFILEN];

    fnlen = wstrlen(wname);
    if ((name = (char *)alloca(fnlen+1)) == NULL)
	return -1;
    strlcpyw(name, wname, fnlen+1);
    
    line[0] = '\0';
    for (i = name_fent(name, TRUE); i < nfunct; i++) {
        cand = functnames[i].n_name;
	j = strncmp (cand, name, fnlen);
        if (j < 0)
	    continue;
        else if (j > 0)
	    break;	/* because functnames[] are in dictionary order */

	if (line[0] == '\0') {
	    if (strlen (cand) < LIST_COL)
		strcpy (line, cand);
	    else {
		wstrlcpya(wline, cand, NG_WCHARLEN(wline));
		addline (bp, wline);
	    }
	}
	else {
	    if (strlen (cand) < LIST_COL) {
		for (j = strlen (line); j < LIST_COL; j++)
		    line[j] = ' ';
		line[j] = '\0';
		strcat (line, cand);
		wstrlcpya(wline, line, NG_WCHARLEN(wline));
		addline (bp, wline);
	    }
	    else {
		wstrlcpya(wline, line, NG_WCHARLEN(wline));
		addline (bp, wline);
		wstrlcpya(wline, cand, NG_WCHARLEN(wline));
		addline (bp, wline);
	    }
	    line[0] = '\0';
	}
    }
    if (line[0] != '\0') {
	wstrlcpya(wline, line, NG_WCHARLEN(wline));
	addline (bp, wline);
    }
    return (TRUE);
}

static int
complete_list_buffernames(name, bp)
NG_WCHAR_t *name;
BUFFER *bp;
{
    int fnlen;
    int j;
    NG_WCHAR_t cand[NFILEN], line[NFILEN];
    LIST *lh;

    fnlen = wstrlen (name);

    line[0] = '\0';
    for (lh = &(bheadp->b_list); lh != NULL; lh = lh->l_next) {
	LM_IN_CONVERT2(bp->b_lang, lm_buffer_name_code, lh->l_name, cand);
	
        if (wstrncmp (cand, name, fnlen) != 0)
	    continue;

	if (line[0] == '\0') {
	    if (wstrlen (cand) < LIST_COL)
		wstrcpy (line, cand);
	    else
		addline (bp, cand);
	}
	else {
	    if (wstrlen (cand) < LIST_COL) {
		for (j = wstrlen (line); j < LIST_COL; j++)
		    line[j] = NG_WSPACE;
		line[j] = '\0';
		wstrcat (line, cand);
		addline (bp, line);
	    }
	    else {
		addline (bp, line);
		addline (bp, cand);
	    }
	    line[0] = '\0';
	}
    }
    if (line[0] != '\0')
	addline (bp, line);
    return (TRUE);
}

static int
complete_list_filenames(wname, bp)
NG_WCHAR_t *wname;
BUFFER *bp;
{
    int dnlen;
    int i, j;
    int fnnum;
    char *cand, *name;
    NG_WCHAR_t line[NFILEN], cand2[NFILEN];
    char *filenames;

    fnnum = wstrlen(wname);
    if ((name = (char *)alloca(fnnum)) == NULL)
	return FALSE;
    strlcpyw(name, wname, fnnum+1);
    dnlen = file_name_part (name) - name;

    if ((fnnum = fffiles (name, &filenames)) == -1)
	return (FALSE);    /* error */

    line[0] = '\0';
    cand = filenames;
    for (i = 0; i < fnnum; i++) {
	cand += dnlen;
	LM_IN_CONVERT2(bp->b_lang, lm_buffer_name_code, cand, cand2);
	if (line[0] == '\0') {
	    if (wstrlen (cand2) < LIST_COL)
		wstrcpy (line, cand2);
	    else
		addline (bp, cand2);
	}
	else {
	    if (wstrlen (cand2) < LIST_COL) {
		for (j = wstrlen (line); j < LIST_COL; j++)
		    line[j] = NG_WSPACE;
		line[j] = '\0';
		wstrcat (line, cand2);
		addline (bp, line);
	    }
	    else {
		addline (bp, line);
		addline (bp, cand2);
	    }
	    line[0] = '\0';
	}
	cand += (strlen (cand) + 1);
    }
    if (line[0] != '\0')
	addline (bp, line);
    free (filenames);
    return (TRUE);
}

int
complete_del_list ()
{
    int cur_row;
    int cur_col;

    if (bp == NULL)
	return (FALSE);
    cur_row = ttrow;
    cur_col = ttcol;
    if (prev_bp == NULL) {
        if (wheadp->w_wndp != NULL)
	  delwind (FFRAND, 0);
    }
    else {
	curbp = prev_bp;
	showbuffer (prev_bp, curwp, WFFORCE | WFHARD);
	curwp->w_dotp = prev_window.w_dotp;
	curwp->w_doto = prev_window.w_doto;
	curwp->w_flag |= WFMOVE;
	curwp = prev_wp;
	curbp = curwp->w_bufp;
	if (prev_window.w_bufp) {
	    curbp->b_markp = prev_window.w_bufp->b_markp;
	    curbp->b_marko = prev_window.w_bufp->b_marko;
	}
    }      
    bp = NULL;
    prev_wp = NULL;
    prev_bp = NULL;
    update ();
    ttmove (cur_row, cur_col);
    /* 91.01.17  Add to delete *Completions* buffer. by S.Yoshida */
    eargset(_NG_WSTR("*Completions*"));
    killbuffer(0, 1);

    return (TRUE);
}

int
complete_scroll_up ()
{
    int cur_row;
    int cur_col;

    if (bp == NULL)
	return (FALSE);
    cur_row = ttrow;
    cur_col = ttcol;
    backpage (FFRAND, 0);
    update ();
    ttmove (cur_row, cur_col);
    return (TRUE);
}

int
complete_scroll_down ()
{
    int cur_row;
    int cur_col;

    if (bp == NULL)
	return (FALSE);
    cur_row = ttrow;
    cur_col = ttcol;
    forwpage (FFRAND, 0);
    update ();
    ttmove (cur_row, cur_col);
    return (TRUE);
}
#endif	/* NEW_COMPLETE */
