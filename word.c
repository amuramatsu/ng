/* $Id: word.c,v 1.3 2001/01/05 14:07:06 amura Exp $ */
/*
 *		Word mode commands.
 * The routines in this file
 * implement commands that work word at
 * a time. There are all sorts of word mode
 * commands.
 */

/*
 * $Log: word.c,v $
 * Revision 1.3  2001/01/05 14:07:06  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.2  2000/06/27 01:49:45  amura
 * import to CVS
 *
 * Revision 1.1  2000/06/01  05:36:12  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#ifdef	UNDO
#include	"undo.h"
#endif

#ifdef	HOJO_KANJI
#define	CHAR_LENGTH()	(ishojo() ? 3 : (iskanji() ? 2 : 1))
#else
#define	CHAR_LENGTH()	(iskanji() ? 2 : 1)
#endif

/*
 * Move the cursor backward by
 * "n" words. All of the details of motion
 * are performed by the "backchar" and "forwchar"
 * routines.
 */
/*ARGSUSED*/
backword(f, n)
{
	if (n < 0) return forwword(f | FFRAND, -n);
	if (backchar(FFRAND, 1) == FALSE)
		return FALSE;
	while (n--) {
		while (inword() == FALSE) {
			if (backchar(FFRAND, 1) == FALSE)
				return TRUE;
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		initcategory(0);	/* Set category of start char. */
		while (inword() != FALSE && incategory()) {
#else	/* NOT KANJI */
		while (inword() != FALSE) {
#endif	/* KANJI */
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
forwword(f, n)
{
	if (n < 0)
		return backword(f | FFRAND, -n);
	while (n--) {
		while (inword() == FALSE) {
			if (forwchar(FFRAND, 1) == FALSE)
				return TRUE;
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		initcategory(1);	/* Set category of start char. */
		while (inword() != FALSE && incategory()) {
#else	/* NOT KANJI */
		while (inword() != FALSE) {
#endif	/* KANJI */
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
upperword(f, n)
{
	register int	c;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0) return FALSE;
#ifdef	UNDO
	undo_reset(curbp);		/* this function cannot undo */
#endif
	while (n--) {
		while (inword() == FALSE) {
			if (forwchar(FFRAND, 1) == FALSE)
				return TRUE;
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		initcategory(1);	/* Set category of start char. */
		while (inword() != FALSE && incategory()) {
#else	/* NOT KANJI */
		while (inword() != FALSE) {
#endif	/* KANJI */
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
lowerword(f, n)
{
	register int	c;

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0) return FALSE;
#ifdef	UNDO
	undo_reset(curbp);		/* this function cannot undo */
#endif
	while (n--) {
		while (inword() == FALSE) {
			if (forwchar(FFRAND, 1) == FALSE)
				return TRUE;
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		initcategory(1);	/* Set category of start char. */
		while (inword() != FALSE && incategory()) {
#else	/* NOT KANJI */
		while (inword() != FALSE) {
#endif	/* KANJI */
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
capword(f, n)
{
	register int	c;
	VOID		lchange();

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0) return FALSE;
#ifdef	UNDO
	undo_reset(curbp);		/* this function cannot undo */
#endif
	while (n--) {
		while (inword() == FALSE) {
			if (forwchar(FFRAND, 1) == FALSE)
				return TRUE;
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		initcategory(1);	/* Set category of start char. */
		if (inword() != FALSE && incategory()) {
#else	/* NOT KANJI */
		if (inword() != FALSE) {
#endif	/* KANJI */
			c = lgetc(curwp->w_dotp, curwp->w_doto);
			if (ISLOWER(c) != FALSE) {
				c = TOUPPER(c);
				lputc(curwp->w_dotp, curwp->w_doto, c);
				lchange(WFHARD);
			}
			if (forwchar(FFRAND, 1) == FALSE)
				return TRUE;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			while (inword() != FALSE && incategory()) {
#else	/* NOT KANJI */
			while (inword() != FALSE) {
#endif	/* KANJI */
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
delfword(f, n)
{
	register RSIZE	size;
	register LINE	*dotp;
	register int	doto;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register RSIZE	s;			/* Delete char size.	*/
#endif	/* KANJI */

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

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
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			s = CHAR_LENGTH();
#endif	/* KANJI */
			if (forwchar(FFRAND, 1) == FALSE)
				goto out;	/* Hit end of buffer.	*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			size += s;
#else	/* NOT KANJI */
			++size;
#endif	/* KANJI */
		}
#ifndef	KANJI	/* 90.01.29  by S.Yoshida */
		while (inword() != FALSE) {
#else	/* KANJI */
		initcategory(1);	/* Set category of start char. */
		while (inword() != FALSE && incategory()) {
			s = CHAR_LENGTH();
#endif	/* KANJI */
			if (forwchar(FFRAND, 1) == FALSE)
				goto out;	/* Hit end of buffer.	*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			size += s;
#else	/* NOT KANJI */
			++size;
#endif	/* KANJI */
		}
	}
out:
	curwp->w_dotp = dotp;
	curwp->w_doto = doto;
	return (ldelete(size, KFORW));
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
delbword(f, n)
{
	register RSIZE	size;
	VOID		kdelete();

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	if (n < 0) return FALSE;
	if ((lastflag&CFKILL) == 0)		/* Purge kill buffer.	*/
		kdelete();
	thisflag |= CFKILL;
	if (backchar(FFRAND, 1) == FALSE)
		return (TRUE);			/* Hit buffer start.	*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	size = CHAR_LENGTH();
#else	/* NOT KANJI */	
	size = 1;				/* One deleted.		*/
#endif	/* KANJI */
	while (n--) {
		while (inword() == FALSE) {
			if (backchar(FFRAND, 1) == FALSE)
				goto out;	/* Hit buffer start.	*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			size += CHAR_LENGTH();
#else	/* NOT KANJI */	
			++size;
#endif	/* KANJI */
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		initcategory(0);	/* Set category of start char. */
		while (inword() != FALSE && incategory()) {
#else	/* NOT KANJI */
		while (inword() != FALSE) {
#endif	/* KANJI */
			if (backchar(FFRAND, 1) == FALSE)
				goto out;	/* Hit buffer start.	*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			size += CHAR_LENGTH();
#else	/* NOT KANJI */	
			++size;
#endif	/* KANJI */
		}
	}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	size -= CHAR_LENGTH();
#endif	/* KANJI */
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
inword() {
/* can't use lgetc in ISWORD due to bug in OSK cpp */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	c;

	if (curwp->w_doto == llength(curwp->w_dotp)) {
		return FALSE;
	}
	c = lgetc(curwp->w_dotp, curwp->w_doto);
	if (ISKANJI(c)) {
#ifdef	HOJO_KANJI
		if (ISHOJO(c)) {
			curwp->w_doto++;
			c = lgetc(curwp->w_dotp, curwp->w_doto);
		}
#endif
		return(iskword(c, lgetc(curwp->w_dotp, curwp->w_doto + 1)));

	} else {
		return (ISWORD(c));
	}
#else	/* NOT KANJI */
	return curwp->w_doto != llength(curwp->w_dotp) && 
		ISWORD(curwp->w_dotp->l_text[curwp->w_doto]);
#endif	/* KANJI */
}
