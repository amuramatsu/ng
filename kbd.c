/* $Id: kbd.c,v 1.13.2.5 2006/01/11 14:47:34 amura Exp $ */
/*
 *		Terminal independent keyboard handling.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "ttydef.h"
#include "kbd.h"

#include "i_window.h"
#include "i_lang.h"
#include "i_buffer.h"
#include "in_code.h"

#include "autosave.h"
#include "echo.h"
#include "undo.h"
#include "ttyio.h"
#define EXTERN
#include "key.h"
#include "line.h"
#include "macro.h"
#include "extend.h"
#include "cinfo.h"

#ifdef CANNA
#include <canna/jrkanji.h>
extern jrKanjiStatus ks;
int henkan();
#endif

#ifdef MOUSE
int allow_mouse_event = FALSE; /* allow mouse event */
#endif

#ifdef DO_METAKEY

int use_metakey = TRUE;
/*
 * Toggle the value of use_metakey
 */
int
do_meta(f, n)
int f, n;
{
    if (f & FFARG)
	use_metakey = n > 0;
    else
	use_metakey = !use_metakey;
    ewprintf("Meta keys %sabled", use_metakey ? "en" : "dis");
#if defined(MSDOS)&&defined(PC9801)
    if (use_metakey)
	setezkey();
    else
	resetezkey();
#endif
    return TRUE;
}
#endif

#ifdef BSMAP
static int bs_map = BSMAP;
/*
 * Toggle backspace mapping
 */
int
bsmap(f, n)
int f, n;
{
    if (f & FFARG)
	bs_map = n > 0;
    else
	bs_map = ! bs_map;
    ewprintf("Backspace mapping %sabled", bs_map ? "en" : "dis");
    return TRUE;
}
#endif

#ifndef NO_DPROMPT
#define PROMPTL		80
char prompt[PROMPTL], *promptp;
#endif

static int pushed = FALSE;
static int pushedc;

VOID
ungetkey(c)
int c;
{
#ifdef	DO_METAKEY
    if (use_metakey && pushed && c==CCHR('['))
	pushedc |= METABIT;
    else
#endif
    pushedc = c;
#ifdef 	BSMAP
    if (bs_map) {
	if (pushedc==CCHR('H'))
	    pushedc=CCHR('?');
	else if(pushedc==CCHR('?'))
	    pushedc=CCHR('H');
    }
#endif
    pushed = TRUE;
}

int
getkey(flag)
int flag;
{
    int c;

#ifndef NO_DPROMPT
    if (flag && !pushed) {
	if (prompt[0]!='\0' && ttwait()) {
	    ewprintf("%s", prompt);	/* avoid problems with % */
	    update();			/* put the cursor back	 */
	    epresf = KPROMPT;
	}
	if (promptp > prompt)
	    *(promptp-1) = ' ';
    }
#endif
#ifdef AUTOSAVE
    autosave_check(TRUE);
#endif
    if (pushed) {
	c = pushedc;
	pushed = FALSE;
    }
    else
	c = kgetkey();
#ifdef BSMAP
    if (bs_map) {
	if (c==CCHR('H'))
	    c = CCHR('?');
	else if (c==CCHR('?'))
	    c = CCHR('H');
    }
#endif
#ifdef DO_METAKEY
    if (use_metakey && (c&METABIT)) {
	pushedc = c & ~METABIT;
	pushed = TRUE;
	c = CCHR('[');
    }
#endif
#ifndef NO_DPROMPT
    if (flag && promptp < &prompt[PROMPTL - 5]) {
	promptp = keyname(promptp, c);
	*promptp++ = '-';
	*promptp = '\0';
    }
#endif
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
    autosave_check(FALSE);
#endif	/* AUTOSAVE */
    return c;
}

/*
 * doscan scans a keymap for a keyboard character and returns a pointer
 * to the function associated with that character.  Sets ele to the
 * keymap element the keyboard was found in as a side effect.
 */

MAP_ELEMENT *ele;

PF
doscan(map, c)
register KEYMAP *map;
register int c;
{
    register MAP_ELEMENT *elec = &map->map_element[0];	/* local register copy for faster access */
    register MAP_ELEMENT *last = &map->map_element[map->map_num];
    PF funct;

    while (elec < last && c > elec->k_num)
	elec++;
    ele = elec;			/* used by prefix and binding code	*/
    if (elec >= last || c < elec->k_base)
	funct = map->map_default;
    else
	funct = elec->k_funcp[c - elec->k_base];
    return funct;
}

int
doin()
{
    KEYMAP *curmap;
    PF funct;
    int d;
#ifdef	UNDO
    int s;
    BUFFER *bp;
#endif

#ifndef NO_DPROMPT
    *(promptp = prompt) = '\0';
#endif
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_on();
#endif
#ifdef MOUSE
    allow_mouse_event = TRUE;
#endif
    curmap = curbp->b_modes[curbp->b_nmodes]->p_map;
    key.k_count = 0;
    d = getkey(TRUE);
#ifdef MOUSE
    allow_mouse_event = FALSE;
#endif
#ifdef CANNA
    if ((curbp->b_flag & BFCANNA) &&
        (ks.length != 0 || !(d==' '||ISCTRL(d)||ISKANJI(d))) )
	return henkan(d);
    else
#endif
    while ((funct=doscan(curmap,(key.k_chars[key.k_count++]=d)))
		== prefix) {
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_off();
#endif
	curmap = ele->k_prefmap;
	d = getkey(TRUE);
    }
#ifndef NO_MACRO
    if (macrodef && macrocount < MAXMACRO)
	macro[macrocount++].m_funct = funct;
#endif
    if (epresf == TRUE)
	eerase();
#ifdef	UNDO
    bp = curbp;
    ublock_open(bp);
    s = (*funct)(0, 1);
    ublock_close(bp);
    return s;
#else
    return (*funct)(0, 1);
#endif
}

int
rescan(f, n)
int f, n;
{
    int c;
    register KEYMAP *curmap;
    int i;
    PF	fp = (PF)NULL;
    int mode = curbp->b_nmodes;

    for (;;) {
	if (ISUPPER(key.k_chars[key.k_count-1])) {
	    c = TOLOWER(key.k_chars[key.k_count-1]);
	    curmap = curbp->b_modes[mode]->p_map;
	    for (i=0; i < key.k_count-1; i++) {
		if ((fp=doscan(curmap,(key.k_chars[i]))) != prefix)
		    break;
		curmap = ele->k_prefmap;
	    }
	    if (fp==prefix) {
		if ((fp = doscan(curmap, c)) == prefix) {
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
		    fepmode_off();
#endif
		    while((fp=doscan(curmap,key.k_chars[key.k_count++] =
				     getkey(TRUE))) == prefix)
			curmap = ele->k_prefmap;
		}
		if (fp!=rescan) {
#ifndef NO_MACRO
		    if (macrodef && macrocount <= MAXMACRO)
			macro[macrocount-1].m_funct = fp;
#endif
		    return (*fp)(f, n);
		}
	    }
	}
	/* try previous mode */
	if (--mode < 0)
	    return ABORT;
	curmap = curbp->b_modes[mode]->p_map;
	for (i=0; i < key.k_count; i++) {
	    if ((fp=doscan(curmap,(key.k_chars[i]))) != prefix)
		break;
	    curmap = ele->k_prefmap;
	}
	if (fp==prefix) {
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	    fepmode_off();
#endif
	    while ((fp=doscan(curmap,key.k_chars[i++]=getkey(TRUE)))
		   == prefix)
		curmap = ele->k_prefmap;
	    key.k_count = i;
	}
	if (fp!=rescan && i>=key.k_count-1) {
#ifndef NO_MACRO
	    if (macrodef && macrocount <= MAXMACRO)
		macro[macrocount-1].m_funct = fp;
#endif
	    return (*fp)(f, n);
	}
    }
}

int
universal_argument(f, n)
int f, n;
{
    int c, nn=4;
    KEYMAP *curmap;
    PF funct;

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    if (f&FFUNIV)
	nn *= n;
    for (;;) {
	key.k_chars[0] = c = getkey(TRUE);
	key.k_count = 1;
	if (c == '-')
	    return negative_argument(f, nn);
	if (c >= '0' && c <= '9')
	    return digit_argument(f, nn);
	curmap = curbp->b_modes[curbp->b_nmodes]->p_map;
	while ((funct=doscan(curmap,c)) == prefix) {
	    curmap = ele->k_prefmap;
	    key.k_chars[key.k_count++] = c = getkey(TRUE);
	}
	if (funct != universal_argument) {
#ifndef NO_MACRO
	    if (macrodef && macrocount < MAXMACRO-1) {
		if (f&FFARG)
		    macrocount--;
		macro[macrocount++].m_count = nn;
		macro[macrocount++].m_funct = funct;
	    }
#endif
	    return (*funct)(FFUNIV, nn);
	}
	nn <<= 2;
    }
}

/*ARGSUSED*/
int
digit_argument(f, n)
int f, n;
{
    int nn, c;
    KEYMAP *curmap;
    PF funct;

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    nn = key.k_chars[key.k_count-1] - '0';
    for (;;) {
	c = getkey(TRUE);
	if (c < '0' || c > '9')
	    break;
	nn *= 10;
	nn += c - '0';
    }
    key.k_chars[0] = c;
    key.k_count = 1;
    curmap = curbp->b_modes[curbp->b_nmodes]->p_map;
    while ((funct=doscan(curmap,c)) == prefix) {
	curmap = ele->k_prefmap;
	key.k_chars[key.k_count++] = c = getkey(TRUE);
    }
#ifndef NO_MACRO
    if (macrodef && macrocount < MAXMACRO-1) {
	if (f&FFARG)
	    macrocount--;
	else
	    macro[macrocount-1].m_funct = universal_argument;
	macro[macrocount++].m_count = nn;
	macro[macrocount++].m_funct = funct;
    }
#endif
    return (*funct)(FFOTHARG, nn);
}

int
negative_argument(f, n)
int f, n;
{
    int nn = 0, c;
    KEYMAP *curmap;
    PF funct;

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    for (;;) {
	c = getkey(TRUE);
	if (c < '0' || c > '9')
	    break;
	nn *= 10;
	nn += c - '0';
    }
    if (nn)
	nn = -nn;
    else
	nn = -n;
    key.k_chars[0] = c;
    key.k_count = 1;
    curmap = curbp->b_modes[curbp->b_nmodes]->p_map;
    while ((funct=doscan(curmap,c)) == prefix) {
	curmap = ele->k_prefmap;
	key.k_chars[key.k_count++] = c = getkey(TRUE);
    }
#ifndef NO_MACRO
    if (macrodef && macrocount < MAXMACRO-1) {
	if (f&FFARG)
	    macrocount--;
	else
	    macro[macrocount-1].m_funct = universal_argument;
	macro[macrocount++].m_count = nn;
	macro[macrocount++].m_funct = funct;
    }
#endif
    return (*funct)(FFNEGARG, nn);
}

/*
 * Insert a character.	While defining a macro, create a "LINE" containing
 * all inserted characters.
 */
/* 90.01.29  by S.Yoshida */
int inkfill = FALSE;			/* Now we are in a fillword().	*/

int
selfinsert(f, n)
int f, n;
{
    register int c;
    int count;
#ifndef NO_MACRO
    LINE *lp;
#endif
    
    if (n < 0)
	return FALSE;
    if (n == 0)
	return TRUE;
    c = key.k_chars[key.k_count-1];
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */

    /* 90.01.29  by S.Yoshida */
    /* In a autofill mode, normally fill trigger is only ' ' char. But	*/
    /* KANJI version require any KANJI char is also fill trigger.	*/
    /* This must be done at keymap.c to add KANJI fill trigger list in	*/
    /* a fillmap. But there are too many KANJI chars, so we use this	*/
    /* easy way.							*/
    if (curbp->b_flag & BFAUTOFILL && !inkfill) { /* Autofill mode and	*/
	int s;
#ifdef UNDO
	if (isundo()) {
	    if (lastflag & CFINS2) {
		if (undostart == undoptr) {
		    curbp->b_utop--;
		    if (curbp->b_utop > UNDOSIZE)
			curbp->b_utop = UNDOSIZE;
		    undostart = &curbp->b_ustack[curbp->b_utop];
		}
		if (undobefore == NULL)
		    panic("selfinsert: Run insert error");
		undoptr = undobefore;
	    }
	}
#endif
#if 0 /*XXX*/
	inkfill = TRUE;
	s = fillword(f, n);		/* fill word with KANJI char.	*/
	inkfill = FALSE;
#endif
	return (s);
    }					/* End of autofill mode add routine. */
#ifndef NO_MACRO
    if (macrodef && macrocount < MAXMACRO) {
	if (f & FFARG)
	    macrocount -= 2;
	if (lastflag & CFINS) {	/* last command was insert -- tack on end */
	    macrocount--;
	    if (maclcur->l_size < maclcur->l_used + n) {
		if ((lp = lallocx(maclcur->l_used + n)) == NULL)
		    return FALSE;
		lp->l_fp = maclcur->l_fp;
		lp->l_bp = maclcur->l_bp;
		lp->l_fp->l_bp = lp->l_bp->l_fp = lp;
		bcopy(maclcur->l_text, lp->l_text, maclcur->l_used);
		for (count = maclcur->l_used; count < lp->l_used; count++)
		    lp->l_text[count] = c;
		free((char *)maclcur);
		maclcur = lp;
	    }
	    else {
		maclcur->l_used += n;
		for (count = maclcur->l_used-n;
		     count < maclcur->l_used; count++)
		    maclcur->l_text[count] = c;
	    }
	}
	else {
	    macro[macrocount-1].m_funct = insert;
	    if ((lp = lallocx(n)) == NULL)
		return FALSE;
	    lp->l_bp = maclcur;
	    lp->l_fp = maclcur->l_fp;
	    maclcur->l_fp = lp;
	    maclcur = lp;
	    for (count = 0; count < n; count++)
		lp->l_text[count] = c;
	}
	thisflag |= CFINS;
    }
#endif
    if (c == '\n') {
	do {
	    count = lnewline();
	} while (--n && count==TRUE);
	return count;
    }
#ifdef	UNDO
    if (isundo()) {
	thisflag |= CFINS2;
	if (curbp->b_flag & BFOVERWRITE) {	/* Overwrite mode	*/
	    UNDO_DATA *undo;
	    if (lastflag & CFINS2) {
		if (!inkfill && undostart==undoptr) {
		    if (curbp->b_utop > 0)
			curbp->b_utop--;
		    else
			curbp->b_utop = UNDOSIZE;
		    undostart = &curbp->b_ustack[curbp->b_utop];
		}		
		if (undobefore == NULL)
		    panic("selfinsert: Run insert error");
		undoptr = undobefore;
		undo = *undoptr;
	    }
	    else {
		undo_setup(undo);
		if (!isundo())
		    goto noundo;
		undo->u_dotlno = get_lineno(curbp,curwp->w_dotp);
		undo->u_doto = curwp->w_doto;
		undo->u_type = UDOVER;
		undo->u_used = 0;
		undo->u_code = NG_EOS;
	    }
	    if (!undo_bgrow(undo, n))
		goto noundo;

	    lchange(WFEDIT);
	    if (undo->u_code != NG_EOS) {
		lputc(curwp->w_dotp, curwp->w_doto, undo->u_code);
	    }
	    count = undo->u_used;
	    while (curwp->w_doto < llength(curwp->w_dotp) && n--) {
		undo->u_buffer[count] =
		    lgetc(curwp->w_dotp, curwp->w_doto);
		lputc(curwp->w_dotp, curwp->w_doto++, c);
		count++;
	    }
	    undo->u_used = count;
	    undo_finish(&(undo->u_next));

	    if (!(lastflag & CFINS2)) {
		if (undoptr!=NULL && *undoptr!=NULL)
		    (*undoptr)->u_type = UDNONE;
	    }
	    if (n<=0) return TRUE;
	}
	else if (lastflag & CFINS2) {	/* not Overwrite mode */
	    if (!inkfill && undostart==undoptr) {
		if (curbp->b_utop > 0)
		    curbp->b_utop--;
		else
		    curbp->b_utop = UNDOSIZE;
		undostart = &curbp->b_ustack[curbp->b_utop];
	    }			
	    if (undobefore == NULL)
		panic("selfinsert: Run insert error");
	    undoptr = undobefore;
	}
	/* if this is NOT, somecase linsert() panic */
	else if (*undoptr != NULL)
	    (*undoptr)->u_type = UDNONE;
    } else
  noundo:
#endif	/* UNDO */
    if (curbp->b_flag & BFOVERWRITE) {		/* Overwrite mode	*/
	lchange(WFEDIT);
	while (curwp->w_doto < llength(curwp->w_dotp) && n--) {
	    lputc(curwp->w_dotp, curwp->w_doto++, c);
	}
	if (curwp->w_doto + curbp->b_lang->lm_width(c)
	      < llength(curwp->w_dotp)) {
	    lputc(curwp->w_dotp, curwp->w_doto, ' ');
	}
	if (n <= 0)
	    return TRUE;
    }
    return linsert(n, c);
}

/*
 * this could be implemented as a keymap with everthing defined
 * as self-insert.
 */
int
quote(f, n)
int f, n;
{
    register int c;

    key.k_count = 1;
    if ((key.k_chars[0] = getkey(TRUE)) >= '0' && key.k_chars[0] <= '7') {
	key.k_chars[0] -= '0';
	if ((c = getkey(TRUE)) >= '0' && c <= '7') {
	    key.k_chars[0] <<= 3;
	    key.k_chars[0] += c - '0';
	    if ((c = getkey(TRUE)) >= '0' && c <= '7') {
		key.k_chars[0] <<= 3;
		key.k_chars[0] += c - '0';
	    }
	    else
		ungetkey(c);
	}
	else
	    ungetkey(c);
    }
    return selfinsert(f, n);
}


/*
 * Input one byte from the keyboard with Multibyte code conversion.
 */
VOID
kgetkey_flush()
{
    terminal_lang->lm_get_keyin_code(NG_W_INPUTCONT);
}

static int kgetkey_continue = FALSE;
/*
 * Input one byte from the keyboard with Multibyte code conversion.
 */
int
kgetkey_continued()
{
    return kgetkey_continue;
}

int
kgetkey()
{
    register int c1, c2;
    kgetkey_continue = TRUE;
    do {
	c1 = getkbd();
#ifdef VTCURSOR /* 92.03.16 by Gen KUROKI, renamed by amura */
	if (c1 == NG_WESC) {
	    c1 = getkbd();
	    if (c1 == 'O' || c1 == '[') {
		c2 = getkbd();
		switch (c2) {
		case 'A': c1 = NG_W_UP; break;
		case 'B': c1 = NG_W_DOWN; break;
		case 'C': c1 = NG_W_RIGHT; break;
		case 'D': c1 = NG_W_LEFT; break;
		case 'P': c1 = NG_W_PF01; break;
		case 'Q': c1 = NG_W_PF02; break;
		case 'R': c1 = NG_W_PF03; break;
		case 'S': c1 = NG_W_PF04; break;
#if 0 /* vt100?, this codes are minor... */
		case 't': c1 = NG_W_PF05; break;
		case 'u': c1 = NG_W_PF06; break;
		case 'v': c1 = NG_W_PF07; break;
		case 'l': c1 = NG_W_PF08; break;
		case 'w': c1 = NG_W_PF09; break;
		case 'x': c1 = NG_W_PF10; break;
#endif
		case '1':
		case '2': {
		    int c3, n;
		    c3 = getkbd();
		    if ('0' < c3 || c3 > '9') {
			ungetkbd(c3);
			ungetkbd(c2);
			ungetkbd(c1);
			c1 = NG_WESC;
			break;
		    }
		    if ((n = getkbd()) != '~') {
			ungetkbd(n);
			ungetkbd(c3);
			ungetkbd(c2);
			ungetkbd(c1);
			c1 = NG_WESC;
			break;
		    }
		    n = (c2 - '0')*10 + c3 - '0';
		    switch (n) {
		    case 15: c1 = NG_W_PF05; break;
		    case 17: c1 = NG_W_PF06; break;
		    case 18: c1 = NG_W_PF07; break;
		    case 19: c1 = NG_W_PF08; break;
		    case 20: c1 = NG_W_PF09; break;
		    case 21: c1 = NG_W_PF10; break;
		    case 23: c1 = NG_W_PF12; break;
		    case 24: c1 = NG_W_PF13; break;
		    case 25: c1 = NG_W_PF14; break;
		    case 26: c1 = NG_W_PF15; break;
		    case 28: c1 = NG_W_PF16; break;
		    case 29: c1 = NG_W_PF17; break;
		    case 31: c1 = NG_W_PF18; break;
		    case 32: c1 = NG_W_PF19; break;
		    case 33: c1 = NG_W_PF20; break;
		    default:
			ungetkbd('~');
			ungetkbd(c3);
			ungetkbd(c2);
			ungetkbd(c1);
			c1 = NG_WESC;
			break;
		    }
		}
		default:
		    ungetkbd(c2);
		    ungetkbd(c1);
		    c1 = NG_WESC;
		    break;
		}
	    }
	}
#endif /* VTCURSOR */
    } while ((c1 = terminal_lang->lm_get_keyin_code(c1)) == NG_W_INPUTCONT);
    kgetkey_continue = FALSE;
    return c1;
}
