/* $Id: kinsoku.c,v 1.4 2001/02/18 17:07:26 amura Exp $ */
/*
 *		Kinsoku char handling routines.
 *		These are only used when KANJI is #defined.
 *
 *		Coded by Shigeki Yoshida (shige@csk.CO.JP)
 */

/*
 * $Log: kinsoku.c,v $
 * Revision 1.4  2001/02/18 17:07:26  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.3  2001/02/11 15:40:25  amura
 * some function are changed to static for speed/size
 *
 * Revision 1.2  2000/11/16 14:31:13  amura
 * fix some typos which cause compile error when using
 * strict ANSI-C compiler (ex ACK, gcc-1.x)
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */
/* 90.01.29	Created by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
#include	"def.h"

#define	MAXBOLKC	128		/* Maximum number of BOL (begin	*/
					/* of line) KINSOKU chars.	*/
#define	MAXEOLKC	64		/* Maximum number of EOL (end	*/
					/* of line) KINSOKU chars.	*/

/* BOL KINSOKU char list (EUC).	*/
/* This table must be sorted.	*/
static unsigned short	bolkchar[MAXBOLKC] = {
	'!',	'\'',	')',	',',	'-',	/*  5 */
	'.',	':',	';',	'?',	']',	/* 10 */
	'_',	'}',	'~',	0xa1a2,	0xa1a3,	/* 15 */
	0xa1a4,	0xa1a5,	0xa1a6,	0xa1a7,	0xa1a8,	/* 20 */
	0xa1a9,	0xa1aa,	0xa1ab,	0xa1ac,	0xa1ad,	/* 25 */
	0xa1ae,	0xa1af,	0xa1b0,	0xa1b1,	0xa1b2,	/* 30 */
	0xa1b3,	0xa1b4, 0xa1b5,	0xa1b6,	0xa1b7,	/* 35 */
	0xa1b8,	0xa1b9,	0xa1ba,	0xa1bb,	0xa1bc,	/* 40 */
	0xa1bd,	0xa1be,	0xa1bf,	0xa1c0,	0xa1c1,	/* 45 */
	0xa1c2,	0xa1c3,	0xa1c4,	0xa1c5,	0xa1c7,	/* 50 */
	0xa1c9,	0xa1cb,	0xa1cd,	0xa1cf,	0xa1d1,	/* 55 */
	0xa1d3, 0xa1d5,	0xa1d7,	0xa1d9,	0xa1db,	/* 60 */
	0xa1eb,	0xa1ec,	0xa1ed, 0xa1ee,	0xa4a1,	/* 65 */
	0xa4a3,	0xa4a5,	0xa4a7,	0xa4a9,	0xa4c3,	/* 70 */
	0xa4e3,	0xa4e5,	0xa4e7,	0xa4ee,	0xa5a1,	/* 75 */
	0xa5a3,	0xa5a5,	0xa5a7,	0xa5a9,	0xa5c3,	/* 80 */
	0xa5e3,	0xa5e5,	0xa5e7,	0xa5ee,	0xa5f5,	/* 85 */
	0xa5f6
};

/* EOL KINSOKU char list (EUC).	*/
/* This table must be sorted.	*/
static unsigned short	eolkchar[MAXEOLKC] = {
	'(',	'[',	'{',	0xa1c6,	0xa1c8,	/*  5 */
	0xa1ca,	0xa1cc,	0xa1ce,	0xa1d0,	0xa1d2,	/* 10 */
	0xa1d4,	0xa1d6,	0xa1d8,	0xa1da,	0xa1eb,	/* 15 */
	0xa1ec,	0xa1ed,	0xa1ee,	0xa1f7,	0xa1f8	/* 20 */
};

static int	nbolkc = 86;		/* Number of BOL KINSOKU chars.	*/
static int	neolkc = 20;		/* Number of EOL KINSOKU chars.	*/

/*
 * FUNCTION: list-kinsoku-chars
 * Display a list of kinsoku-bol-chars and kinsoku-eol-chars values
 * in the *Kinsoku Chars* buffer.
 */
/*ARGSUSED*/
kc_list_char(f, n)
{
	register unsigned short	*p;	/* KINSOKU char list pointer.	*/
	register unsigned short	*eop;	/* End of KINSOKU char list.	*/
	register char		c;
	register char		*l;	/* Display line buffer pointer.	*/
	register char		*eol;	/* End of display line buffer.	*/
	register BUFFER		*bp;
	register WINDOW		*wp;
#define	DISPLEN	64
	char	line[DISPLEN + 1];	/* Display line buffer.		*/

	if ((bp = bfind("*Kinsoku Chars*", TRUE)) == NULL) return FALSE;
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
	bp->b_flag &= ~(BFCHG | BFACHG);	/* Blow away old.	*/
#else
	bp->b_flag &= ~BFCHG;			/* Blow away old.	*/
#endif	/* AUTOSAVE	*/
	if (bclear(bp) != TRUE) return FALSE;

	strcpy(line, "kinsoku-bol-chars:"); /* BOL KINSOKU char list. */
	if (addline(bp, line) == FALSE) return FALSE;
	l   = line;
	*l++ = '\t';			/* List line start with TAB.	*/
	eol = &line[DISPLEN];
	p   = bolkchar;
	eop = &bolkchar[nbolkc];
	while (p < eop) {
		if (l >= eol) {
			*l = '\0';
			if (addline(bp, line) == FALSE) return FALSE;
			l = line;
			*l++ = '\t';	/* List line start with TAB.	*/
		} else {
			if ((c = (*p >> 8) & 0xff) != 0) {
				*l++ = c;
			}
			c = *p++ & 0xff;
			if (ISCTRL(c)) { /* This may be needless...	*/
				*l++ = '^';
				*l++ = CCHR(c);
			} else {
				*l++ = c;
			}
		}
	}
	if (l > line) {			/* Not shown line exists.	*/
		*l = '\0';
		if (addline(bp, line) == FALSE) return FALSE;
	}
	line[0] = '\0';
	if (addline(bp, line) == FALSE) return FALSE;

	strcpy(line, "kinsoku-eol-chars:"); /* EOL KINSOKU char list.	*/
	if (addline(bp, line) == FALSE) return FALSE;
	l   = line;
	*l++ = '\t';			/* List line start with TAB.	*/
	eol = &line[DISPLEN];
	p   = eolkchar;
	eop = &eolkchar[neolkc];
	while (p < eop) {
		if (l >= eol) {
			*l = '\0';
			if (addline(bp, line) == FALSE) return FALSE;
			l = line;
			*l++ = '\t';	/* List line start with TAB.	*/
		} else {
			if ((c = (*p >> 8) & 0xff) != 0) {
				*l++ = c;
			}
			c = *p++ & 0xff;
			if (ISCTRL(c)) { /* This may be needless...	*/
				*l++ = '^';
				*l++ = CCHR(c);
			} else {
				*l++ = c;
			}
		}
	}
	if (l > line) {			/* Not shown line exists.	*/
		*l = '\0';
		if (addline(bp, line) == FALSE) return FALSE;
	}

	if ((wp = popbuf(bp)) == NULL)	return FALSE;
	bp->b_dotp = lforw(bp->b_linep); /* put dot at beginning of buffer */
	bp->b_doto = 0;
	wp->w_dotp = bp->b_dotp;	/* fix up if window already on screen */
	wp->w_doto = bp->b_doto;
	return TRUE;
}

/*
 * FUNCTION: add-kinsoku-bol-chars
 * Add some BOL (begin of line) KINSOKU chars to a BOL KINSOKU char list
 * (kinsoku-bol-chars = bolkchar[]).
 */
/*ARGSUSED*/
kc_add_bol(f, n)
{
	register int	s;
	register short	c;
	register char	*p;
	char	kchar[NFILEN];

	if ((s = ereply("Kinsoku Chars : ", kchar, NFILEN)) != TRUE) {
		return (s);
	}

	for (p = kchar; *p;) {
		c = *p++ & 0xff;
		if (ISKANJI(c)) {
			c = (c << 8) | (*p++ & 0xff);
		}
		if (nbolkc < MAXBOLKC) {
			if (kcinsert(bolkchar, c, nbolkc)) {
				nbolkc++;
			}
		} else {
			ewprintf("Too many kinsoku-bol-chars!");
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * FUNCTION: delete-kinsoku-bol-chars
 * Delete some BOL (begin of line) KINSOKU chars from a BOL KINSOKU char list
 * (kinsoku-bol-chars = bolkchar[]).
 */
/*ARGSUSED*/
kc_del_bol(f, n)
{
	register int	s;
	register short	c;
	register char	*p;
	char	kchar[NFILEN];

	if ((s = ereply("Kinsoku Chars : ", kchar, NFILEN)) != TRUE) {
		return (s);
	}

	for (p = kchar; *p;) {
		c = *p++ & 0xff;
		if (ISKANJI(c)) {
			c = (c << 8) | (*p++ & 0xff);
		}
		if (nbolkc > 0) {
			if (kcdelete(bolkchar, c, nbolkc)) {
				nbolkc--;
			}
		} else {
			ewprintf("No kinsoku-bol-chars!");
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * FUNCTION: add-kinsoku-eol-chars
 * Add some EOL (end of line) KINSOKU chars to a EOL KINSOKU char list
 * (kinsoku-eol-chars = eolkchar[]).
 */
/*ARGSUSED*/
kc_add_eol(f, n)
{
	register int	s;
	register short	c;
	register char	*p;
	char	kchar[NFILEN];

	if ((s = ereply("Kinsoku Chars : ", kchar, NFILEN)) != TRUE) {
		return (s);
	}

	for (p = kchar; *p;) {
		c = *p++ & 0xff;
		if (ISKANJI(c)) {
			c = (c << 8) | (*p++ & 0xff);
		}
		if (neolkc < MAXEOLKC) {
			if (kcinsert(eolkchar, c, neolkc)) {
				neolkc++;
			}
		} else {
			ewprintf("Too many kinsoku-eol-chars!");
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * FUNCTION: delete-kinsoku-eol-chars
 * Delete some EOL (end of line) KINSOKU chars from a EOL KINSOKU char list
 * (kinsoku-eol-chars = eolkchar[]).
 */
/*ARGSUSED*/
kc_del_eol(f, n)
{
	register int	s;
	register short	c;
	register char	*p;
	char	kchar[NFILEN];

	if ((s = ereply("Kinsoku Chars : ", kchar, NFILEN)) != TRUE) {
		return (s);
	}

	for (p = kchar; *p;) {
		c = *p++ & 0xff;
		if (ISKANJI(c)) {
			c = (c << 8) | (*p++ & 0xff);
		}
		if (neolkc > 0) {
			if (kcdelete(eolkchar, c, neolkc)) {
				neolkc--;
			}
		} else {
			ewprintf("No kinsoku-eol-chars!");
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * Insert one KINSOKU char in a KINSOKU char list.
 */
int
#ifdef SUPPORT_ANSI /* for strict compiler */
kcinsert(unsigned short *kclist, unsigned short kc, int nkc)
#else
kcinsert(kclist, kc, nkc)
unsigned short	*kclist;		/* KINSOKU char list.	*/
unsigned short	kc;			/* Target KINSOKU char.	*/
int		nkc;			/* Current number of KINSOKU chars. */
#endif
{
	unsigned short	*p = kclist;	/* Start of KINSOKU char list.	  */
	unsigned short	*eop = &kclist[nkc]; /* End of KINSOKU char list. */
	unsigned short	*pp;

	for (; p < eop; p++) {
		if (kc < *p) {
			break;
		} else if (kc == *p) {	/* Already exist.	*/
			return FALSE;
		}
	}
	if (p < eop) {
		pp = eop;
		for (; pp > p; pp--) {
			*pp = pp[-1];
		}
	}
	*p = kc;
	return TRUE;
}

/*
 * Delete one KINSOKU char form a KINSOKU char list.
 */
int
#ifdef SUPPORT_ANSI /* for strict compiler */
kcdelete(unsigned short *kclist, unsigned short kc,int nkc)
#else
kcdelete(kclist, kc, nkc)
unsigned short	*kclist;		/* KINSOKU char list.	*/
unsigned short	kc;			/* Target KINSOKU char.	*/
int		nkc;			/* Current number of KINSOKU chars. */
#endif
{
	unsigned short	*p = kclist;	/* Start of KINSOKU char list.	  */
	unsigned short	*eop = &kclist[nkc]; /* End of KINSOKU char list. */

	for (; p < eop; p++) {
		if (kc == *p) {
			break;
		}
	}
	if (p == eop) {			/* Not exist that char.	*/
		return FALSE;
	}
	for (; p < eop; p++) {
		*p = p[1];
	}
	return TRUE;
}

/*
 * Is this BOL (begin of line) KINSOKU char ?
 * c1 must be KANJI 1st byte or 0 (when c2 is ASCII).
 */
isbolkchar(c1, c2)
int	c1;
int	c2;
{
	register unsigned short	c = ((c1 & 0xff) << 8) | (c2 & 0xff);
	register unsigned short	*p = &bolkchar[0];
	register unsigned short	*eop = &bolkchar[nbolkc];

	if (c < *p || c > eop[-1]) {
		return FALSE;
	}
	while (p < eop) {
		if (c == *p++) {
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Is this EOL (end of line) KINSOKU char ?
 * c1 must be KANJI 1st byte or 0 (when c2 is ASCII).
 */
iseolkchar(c1, c2)
int	c1;
int	c2;
{
	register unsigned short	c = ((c1 & 0xff) << 8) | (c2 & 0xff);
	register unsigned short	*p = &eolkchar[0];
	register unsigned short	*eop = &eolkchar[neolkc];

	if (c < *p || c > eop[-1]) {
		return FALSE;
	}
	while (p < eop) {
		if (c == *p++) {
			return TRUE;
		}
	}
	return FALSE;
}
#endif	/* KINSOKU */
