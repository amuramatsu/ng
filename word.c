/* $Id: word.c,v 1.5.2.3 2006/01/04 17:00:40 amura Exp $ */
/*
 *		Word mode commands.
 * The routines in this file
 * implement commands that work word at
 * a time. There are all sorts of word mode
 * commands.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "word.h"

#include "i_window.h"
#include "i_lang.h"
#include "basic.h"
#include "line.h"
#include "undo.h"

static VOID initcategory _PRO((int));
static int incategory _PRO((void));
#define FORWORD		1
#define BACKWORD	0

/*
 * Move the cursor backward by
 * "n" words. All of the details of motion
 * are performed by the "backchar" and "forwchar"
 * routines.
 */
/*ARGSUSED*/
int
backword(f, n)
int f, n;
{
    if (n < 0)
	return forwword(f | FFRAND, -n);
    if (backchar(FFRAND, 1) == FALSE)
	return FALSE;
    while (n--) {
	while (inword() == FALSE) {
	    if (backchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
	/* 90.01.29  by S.Yoshida */
	initcategory(BACKWORD);	/* Set category of start char. */
	while (inword() != FALSE && incategory()) {
	    if (backchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
    }
    return forwchar(FFRAND, 1);
}

/*
 * Move the cursor forward by
 * the specified number of words. All of the
 * motion is done by "forwchar".
 */
/*ARGSUSED*/
int
forwword(f, n)
int f, n;
{
    if (n < 0)
	return backword(f | FFRAND, -n);
    while (n--) {
	while (inword() == FALSE) {
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
	/* 90.01.29  by S.Yoshida */
	initcategory(FORWORD);	/* Set category of start char. */
	while (inword() != FALSE && incategory()) {
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
    }
    return TRUE;
}

/*
 * Move the cursor forward by
 * the specified number of words. As you move,
 * convert any characters to upper case.
 */
/*ARGSUSED*/
int
upperword(f, n)
int f, n;
{
    register int c;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    if (n < 0)
	return FALSE;
#ifdef UNDO
    undo_reset(curbp);			/* this function cannot undo */
#endif
    while (n--) {
	while (inword() == FALSE) {
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
	/* 90.01.29  by S.Yoshida */
	initcategory(BACKWORD);	/* Set category of start char. */
	while (inword() != FALSE && incategory()) {
	    c = lgetc(curwp->w_dotp, curwp->w_doto);
	    if (ISLOWER(c) != FALSE) {
		c = TOUPPER(c);
		lputc(curwp->w_dotp, curwp->w_doto, c);
		lchange(WFHARD);
	    }
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
    }
    return TRUE;
}

/*
 * Move the cursor forward by
 * the specified number of words. As you move
 * convert characters to lower case.
 */
/*ARGSUSED*/
int
lowerword(f, n)
int f, n;
{
    register int c;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */

    if (n < 0)
	return FALSE;
#ifdef UNDO
    undo_reset(curbp);			/* this function cannot undo */
#endif
    while (n--) {
	while (inword() == FALSE) {
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
	/* 90.01.29  by S.Yoshida */
	initcategory(FORWORD);	/* Set category of start char. */
	while (inword() != FALSE && incategory()) {
	    c = lgetc(curwp->w_dotp, curwp->w_doto);
	    if (ISUPPER(c) != FALSE) {
		c = TOLOWER(c);
		lputc(curwp->w_dotp, curwp->w_doto, c);
		lchange(WFHARD);
	    }
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
    }
    return TRUE;
}

/*
 * Move the cursor forward by
 * the specified number of words. As you move
 * convert the first character of the word to upper
 * case, and subsequent characters to lower case. Error
 * if you try and move past the end of the buffer.
 */
/*ARGSUSED*/
int
capword(f, n)
int f, n;
{
    register int c;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */

    if (n < 0)
	return FALSE;
#ifdef UNDO
    undo_reset(curbp);		/* this function cannot undo */
#endif
    while (n--) {
	while (inword() == FALSE) {
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	}
	/* 90.01.29  by S.Yoshida */
	initcategory(FORWORD);	/* Set category of start char. */
	if (inword() != FALSE && incategory()) {
	    c = lgetc(curwp->w_dotp, curwp->w_doto);
	    if (ISLOWER(c) != FALSE) {
		c = TOUPPER(c);
		lputc(curwp->w_dotp, curwp->w_doto, c);
		lchange(WFHARD);
	    }
	    if (forwchar(FFRAND, 1) == FALSE)
		return TRUE;
	    /* 90.01.29  by S.Yoshida */
	    while (inword() != FALSE && incategory()) {
		c = lgetc(curwp->w_dotp, curwp->w_doto);
		if (ISUPPER(c) != FALSE) {
		    c = TOLOWER(c);
		    lputc(curwp->w_dotp, curwp->w_doto, c);
		    lchange(WFHARD);
		}
		if (forwchar(FFRAND, 1) == FALSE)
		    return TRUE;
	    }
	}
    }
    return TRUE;
}

/*
 * Kill forward by "n" words.
 */
/*ARGSUSED*/
int
delfword(f, n)
int f, n;
{
    register RSIZE size;
    register LINE *dotp;
    register int doto;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    
    if (n < 0)
	return FALSE;
    if ((lastflag&CFKILL) == 0)		/* Purge kill buffer.	*/
	kdelete();
    thisflag |= CFKILL;
    dotp = curwp->w_dotp;
    doto = curwp->w_doto;
    size = 0;
    while (n--) {
	while (inword() == FALSE) {
	    if (forwchar(FFRAND, 1) == FALSE)
		goto out;	/* Hit end of buffer.	*/
	    ++size;
	}
	initcategory(FORWORD);	/* Set category of start char. */
	while (inword() != FALSE && incategory()) {
	    if (forwchar(FFRAND, 1) == FALSE)
		goto out;	/* Hit end of buffer.	*/
	    ++size;
	}
    }
out:
    curwp->w_dotp = dotp;
    curwp->w_doto = doto;
    return (ldelete(size, KBACK));
}

/*
 * Kill backwards by "n" words. The rules
 * for success and failure are now different, to prevent
 * strange behavior at the start of the buffer. The command
 * only fails if something goes wrong with the actual delete
 * of the characters. It is successful even if no characters
 * are deleted, or if you say delete 5 words, and there are
 * only 4 words left. I considered making the first call
 * to "backchar" special, but decided that that would just
 * be wierd. Normally this is bound to "M-Rubout" and
 * to "M-Backspace".
 */
/*ARGSUSED*/
int
delbword(f, n)
int f, n;
{
    register RSIZE size;

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */

    if (n < 0)
	return FALSE;
    if ((lastflag&CFKILL) == 0)		/* Purge kill buffer.	*/
	kdelete();
    thisflag |= CFKILL;
    if (backchar(FFRAND, 1) == FALSE)
	return (TRUE);			/* Hit buffer start.	*/
    size = 1;				/* One deleted.		*/
    while (n--) {
	while (inword() == FALSE) {
	    if (backchar(FFRAND, 1) == FALSE)
		goto out;	/* Hit buffer start.	*/
	    ++size;
	}
	initcategory(BACKWORD);	/* Set category of start char. */
	while (inword() != FALSE && incategory()) {
	    if (backchar(FFRAND, 1) == FALSE)
		goto out;	/* Hit buffer start.	*/
	    ++size;
	}
    }
    if (forwchar(FFRAND, 1) == FALSE)
	return FALSE;
    --size;					/* Undo assumed delete. */
out:
    return ldelete(size, KBACK);
}

/*
 * Return TRUE if the character at dot
 * is a character that is considered to be
 * part of a word. The word character list is hard
 * coded. Should be setable.
 */
int
inword()
{
    NG_WCHAR_t c;
    if (curwp->w_doto == llength(curwp->w_dotp))
	return FALSE;
    c = lgetc(curwp->w_dotp, curwp->w_doto);
    if ((curbp->b_lang->lm_get_category(c) & _NGC_W) != 0)
	return TRUE;
    return FALSE;
}


static int cur_cat;			/* Current char category.	*/
static int cur_dir;			/* Current search direction.	*/

/*
 * Set char category of search start position char,
 * and set search direction.
 */
static VOID
initcategory(dir)
int dir;
{
    NG_WCHAR_t c =  lgetc(curwp->w_dotp, curwp->w_doto);
    cur_cat = curbp->b_lang->lm_get_category(c);
    cur_dir = dir;
}

/*
 * Is it in a same category ?
 * return TRUE if category of current position char is same as
 * start position char's one (now in a word), or it is HIRAGANA
 * added to the end of that word.
 */
static int
incategory()
{
    register int cat;
    NG_WCHAR_t c =  lgetc(curwp->w_dotp, curwp->w_doto);
    
    if (curwp->w_doto == llength(curwp->w_dotp))
	return FALSE;
    cat = curbp->b_lang->lm_get_category(c);
    if (cur_dir == FORWORD) {	/* Direction is forward. */
	if (cat & _NGC_WS) { /* Now start added slave chars part. */
	    cur_cat = cat;
	}
    }
    else {			/* Direcrion is backward. */
	if (cur_cat & _NGC_WS && cat != cur_cat) {
	    /* Now end added slave part and start word part. */
	    cur_cat = cat;
	}
    }
    return(cat == cur_cat);
}
