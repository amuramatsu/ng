/* $Id: kbd.c,v 1.10 2001/02/18 17:07:26 amura Exp $ */
/*
 *		Terminal independent keyboard handling.
 */

/*
 * $Log: kbd.c,v $
 * Revision 1.10  2001/02/18 17:07:26  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.9  2001/01/05 14:07:04  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.8  2000/11/04 13:44:58  amura
 * undo memory exception is more safety
 *
 * Revision 1.7  2000/10/02 16:13:06  amura
 * ignore mouse event in minibuffer editing
 *
 * Revision 1.6  2000/09/21 17:28:30  amura
 * replace macro _WIN32 to WIN32 for Cygwin
 *
 * Revision 1.5  2000/09/13 21:03:53  amura
 * can compile option UNDO without KANJI
 *
 * Revision 1.4  2000/07/20 12:45:17  amura
 * support undo with auto-fill mode
 *
 * Revision 1.3  2000/07/16 15:44:41  amura
 * undo bug on autofill fixed
 *
 * Revision 1.2  2000/06/27 01:49:43  amura
 * import to CVS
 *
 * Revision 1.1  2000/06/01  05:29:03  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	"kbd.h"
#ifdef	UNDO
#include	"undo.h"
#endif

#define EXTERN
#include	"key.h"

#ifndef NO_MACRO
#include "macro.h"
#endif

#ifdef CANNA
#include    <canna/jrkanji.h>
extern jrKanjiStatus ks;
int henkan( );
#endif

#ifdef MOUSE
int allow_mouse_event = FALSE; /* allow mouse event */
#endif

#ifdef	DO_METAKEY
int use_metakey = TRUE;
/*
 * Toggle the value of use_metakey
 */
do_meta(f, n)
{
	if(f & FFARG)	use_metakey = n > 0;
	else		use_metakey = !use_metakey;
	ewprintf("Meta keys %sabled", use_metakey ? "en" : "dis");
#ifdef	PC9801
	if (use_metakey)
		setezkey();
	else
		resetezkey();
#endif
	return TRUE;
}
#endif

#ifdef	BSMAP
static int bs_map = BSMAP;
/*
 * Toggle backspace mapping
 */
bsmap(f, n)
{
	if(f & FFARG)	bs_map = n > 0;
	else		bs_map = ! bs_map;
	ewprintf("Backspace mapping %sabled", bs_map ? "en" : "dis");
	return TRUE;
}
#endif

#ifndef NO_DPROMPT
#define PROMPTL 80
  char	prompt[PROMPTL], *promptp;
#endif

static	int	pushed = FALSE;
static	int	pushedc;

VOID	ungetkey(c)
int	c;
{
#ifdef	DO_METAKEY
	if(use_metakey && pushed && c==CCHR('[')) pushedc |= METABIT;
	else
#endif
		pushedc = c;
#ifdef 	BSMAP
	if(bs_map)
		if(pushedc==CCHR('H')) pushedc=CCHR('?');
		else if(pushedc==CCHR('?')) pushedc=CCHR('H');
#endif	
	pushed = TRUE;
}

int getkey(flag)
int	flag;
{
	int	c;
	char	*keyname();

#ifndef NO_DPROMPT
	if(flag && !pushed) {
		if(prompt[0]!='\0' && ttwait()) {
			ewprintf("%s", prompt);	/* avoid problems with % */
			update();		/* put the cursor back	 */
			epresf = KPROMPT;
		}
		if(promptp > prompt) *(promptp-1) = ' ';
	}
#endif
#ifdef	AUTOSAVE
	autosave_check(TRUE);
#endif
	if(pushed) {
		c = pushedc;
		pushed = FALSE;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	} else	c = kgetkey();
#else	/* NOT KANJI */
	} else	c = getkbd();
#endif	/* KANJI */
#ifdef 	BSMAP
	if(bs_map)
		if(c==CCHR('H')) c=CCHR('?');
		else if(c==CCHR('?')) c=CCHR('H');
#endif	
#ifdef	DO_METAKEY
	if(use_metakey && (c&METABIT)) {
		pushedc = c & ~METABIT;
		pushed = TRUE;
		c = CCHR('[');
	}
#endif
#ifndef NO_DPROMPT
	if(flag && promptp < &prompt[PROMPTL - 5]) {
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

PF	doscan(map, c)
register KEYMAP *map;
register int	c;
{
    register MAP_ELEMENT *elec = &map->map_element[0];	/* local register copy for faster access */
    register MAP_ELEMENT *last = &map->map_element[map->map_num];
    PF funct;

    while(elec < last && c > elec->k_num) elec++;
    ele = elec;			/* used by prefix and binding code	*/
    if(elec >= last || c < elec->k_base)
	funct = map->map_default;
    else
	funct = elec->k_funcp[c - elec->k_base];
    return funct;
}

doin()
{
    KEYMAP	*curmap;
    PF	funct;
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
    d=getkey(TRUE);
#ifdef MOUSE
    allow_mouse_event = FALSE;
#endif
#ifdef CANNA
    if( (curbp->b_flag & BFCANNA) &&
        (ks.length != 0 || !(d==' '||ISCTRL(d)||ISKANJI(d))) )
	return henkan(d);
    else
#endif
    while((funct=doscan(curmap,(key.k_chars[key.k_count++]=d)))
		== prefix) {
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_off();
#endif
	curmap = ele->k_prefmap;
	d = getkey(TRUE);
    }
#ifndef NO_MACRO
    if(macrodef && macrocount < MAXMACRO)
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

rescan(f, n)
int f, n;
{
    int c;
    register KEYMAP *curmap;
    int i;
    PF	fp = (PF)NULL;
    int mode = curbp->b_nmodes;

    for(;;) {
	if(ISUPPER(key.k_chars[key.k_count-1])) {
	    c = TOLOWER(key.k_chars[key.k_count-1]);
	    curmap = curbp->b_modes[mode]->p_map;
	    for(i=0; i < key.k_count-1; i++) {
		if((fp=doscan(curmap,(key.k_chars[i]))) != prefix) break;
		curmap = ele->k_prefmap;
	    }
	    if(fp==prefix) {
		if((fp = doscan(curmap, c)) == prefix) {
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
		    fepmode_off();
#endif
		    while((fp=doscan(curmap,key.k_chars[key.k_count++] =
			    getkey(TRUE))) == prefix)
			curmap = ele->k_prefmap;
		}
		if(fp!=rescan) {
#ifndef NO_MACRO
		    if(macrodef && macrocount <= MAXMACRO)
			macro[macrocount-1].m_funct = fp;
#endif
		    return (*fp)(f, n);
		}
	    }
	}
	/* try previous mode */
	if(--mode < 0) return ABORT;
	curmap = curbp->b_modes[mode]->p_map;
	for(i=0; i < key.k_count; i++) {
	    if((fp=doscan(curmap,(key.k_chars[i]))) != prefix) break;
	    curmap = ele->k_prefmap;
	}
	if(fp==prefix) {
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	    fepmode_off();
#endif
	    while((fp=doscan(curmap,key.k_chars[i++]=getkey(TRUE)))
		    == prefix)
		curmap = ele->k_prefmap;
	    key.k_count = i;
	}
	if(fp!=rescan && i>=key.k_count-1) {
#ifndef NO_MACRO
	    if(macrodef && macrocount <= MAXMACRO)
		macro[macrocount-1].m_funct = fp;
#endif
	    return (*fp)(f, n);
	}
    }
}

int universal_argument pro((int, int));

universal_argument(f, n)
int f, n;
{
    int c, nn=4;
    KEYMAP *curmap;
    PF	funct;

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    if(f&FFUNIV) nn *= n;
    for(;;) {
	key.k_chars[0] = c = getkey(TRUE);
	key.k_count = 1;
	if(c == '-') return negative_argument(f, nn);
	if(c >= '0' && c <= '9') return digit_argument(f, nn);
	curmap = curbp->b_modes[curbp->b_nmodes]->p_map;
	while((funct=doscan(curmap,c)) == prefix) {
	    curmap = ele->k_prefmap;
	    key.k_chars[key.k_count++] = c = getkey(TRUE);
	}
	if(funct != universal_argument) {
#ifndef NO_MACRO
	    if(macrodef && macrocount < MAXMACRO-1) {
		if(f&FFARG) macrocount--;
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
digit_argument(f, n)
int f, n;
{
    int nn, c;
    KEYMAP *curmap;
    PF	funct;

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    nn = key.k_chars[key.k_count-1] - '0';
    for(;;) {
	c = getkey(TRUE);
	if(c < '0' || c > '9') break;
	nn *= 10;
	nn += c - '0';
    }
    key.k_chars[0] = c;
    key.k_count = 1;
    curmap = curbp->b_modes[curbp->b_nmodes]->p_map;
    while((funct=doscan(curmap,c)) == prefix) {
	curmap = ele->k_prefmap;
	key.k_chars[key.k_count++] = c = getkey(TRUE);
    }
#ifndef NO_MACRO
    if(macrodef && macrocount < MAXMACRO-1) {
	if(f&FFARG) macrocount--;
	else macro[macrocount-1].m_funct = universal_argument;
	macro[macrocount++].m_count = nn;
	macro[macrocount++].m_funct = funct;
    }
#endif
    return (*funct)(FFOTHARG, nn);
}

negative_argument(f, n)
int f, n;
{
    int nn = 0, c;
    KEYMAP *curmap;
    PF	funct;

#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    for(;;) {
	c = getkey(TRUE);
	if(c < '0' || c > '9') break;
	nn *= 10;
	nn += c - '0';
    }
    if(nn) nn = -nn;
    else nn = -n;
    key.k_chars[0] = c;
    key.k_count = 1;
    curmap = curbp->b_modes[curbp->b_nmodes]->p_map;
    while((funct=doscan(curmap,c)) == prefix) {
	curmap = ele->k_prefmap;
	key.k_chars[key.k_count++] = c = getkey(TRUE);
    }
#ifndef NO_MACRO
    if(macrodef && macrocount < MAXMACRO-1) {
	if(f&FFARG) macrocount--;
	else macro[macrocount-1].m_funct = universal_argument;
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
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
int	no_k2nd = 0;			/* We have no KANJI 2nd byte	*/
int	inkfill = FALSE;		/* Now we are in a fillword().	*/
#endif	/* KANJI */

selfinsert(f, n)
int f, n;
{
    register int c;
    int count;
    VOID lchange();
#ifndef NO_MACRO
    LINE *lp;
    int insert pro((int, int));
#endif
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    int	lkanji2nd;			/* For over write mode.	*/
#endif	/* KANJI */

    if (n < 0)	return FALSE;
    if (n == 0) return TRUE;
    c = key.k_chars[key.k_count-1];
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif	/* READONLY */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    /* In a autofill mode, normally fill trigger is only ' ' char. But	*/
    /* KANJI version require any KANJI char is also fill trigger.	*/
    /* This must be done at keymap.c to add KANJI fill trigger list in	*/
    /* a fillmap. But there are too many KANJI chars, so we use this	*/
    /* easy way.							*/
    if(curbp->b_flag & BFAUTOFILL && 		/* Autofill mode and	*/
       !inkfill && no_k2nd!=0) {		/* KANJI 2nd byte.	*/
	    int	s;
#ifdef	UNDO
	    if (isundo()) {
		if (lastflag & CFINS2) {
		    if (undostart == undoptr) {
			curbp->b_utop--;
			if (curbp->b_utop < 0)
			    curbp->b_utop = UNDOSIZE;
			undostart = &curbp->b_ustack[curbp->b_utop];
		    }
		    if (undobefore == NULL)
			panic("selfinsert: Run insert error");
		    undoptr = undobefore;
		}
	    }
#endif
	    inkfill = TRUE;
	    s = fillword(f, n);		/* fill word with KANJI char.	*/
	    inkfill = FALSE;
	    return (s);
    }					/* End of autofill mode add routine. */
    if (no_k2nd != 0) {			/* If there is only KANJI 1st byte, */
	    no_k2nd--;			/* we believe 'c' is KANJI 2nd byte.*/
    } else if (ISKANJI(c)) {
	    if ((n % 2) == 0) {		/* This is easy bug fix. */
		    n |= 0x01;
	    }
#ifdef	HOJO_KANJI
	    if (ISHOJO(c))	no_k2nd = 2;
	    else
#endif	    
	    no_k2nd = 1;		/* When there is no KANJI 2nd	*/
					/* byte, we don't do update().	*/
    }
#endif	/* KANJI */
#ifndef NO_MACRO
    if(macrodef && macrocount < MAXMACRO) {
	if(f & FFARG) macrocount -= 2;
	if(lastflag & CFINS) {	/* last command was insert -- tack on end */
	    macrocount--;
	    if(maclcur->l_size < maclcur->l_used + n) {
		if((lp = lallocx(maclcur->l_used + n)) == NULL)
		    return FALSE;
		lp->l_fp = maclcur->l_fp;
		lp->l_bp = maclcur->l_bp;
		lp->l_fp->l_bp = lp->l_bp->l_fp = lp;
		bcopy(maclcur->l_text, lp->l_text, maclcur->l_used);
		for(count = maclcur->l_used; count < lp->l_used; count++)
		    lp->l_text[count] = c;
		free((char *)maclcur);
		maclcur = lp;
	    } else {
		maclcur->l_used += n;
		for(count = maclcur->l_used-n; count < maclcur->l_used; count++)
		    maclcur->l_text[count] = c;
	    }
	} else {
	    macro[macrocount-1].m_funct = insert;
	    if((lp = lallocx(n)) == NULL) return FALSE;
	    lp->l_bp = maclcur;
	    lp->l_fp = maclcur->l_fp;
	    maclcur->l_fp = lp;
	    maclcur = lp;
	    for(count = 0; count < n; count++)
		lp->l_text[count] = c;
	}
	thisflag |= CFINS;
    }
#endif
    if(c == '\n') {
	do {
	    count = lnewline();
	} while (--n && count==TRUE);
	return count;
    }
#ifdef	UNDO
    if (isundo()) {
	thisflag |= CFINS2;
	if(curbp->b_flag & BFOVERWRITE) {	/* Overwrite mode	*/
	    UNDO_DATA *undo;
	    if (lastflag & CFINS2) {
#ifdef	KANJI
		if (!inkfill && undostart==undoptr) {
#else
		if (undostart==undoptr) {
#endif
		    curbp->b_utop--;
		    if (curbp->b_utop < 0)
			curbp->b_utop = UNDOSIZE;
		    undostart = &curbp->b_ustack[curbp->b_utop];
		}			
		if (undobefore == NULL)
		    panic("selfinsert: Run insert error");
		undoptr = undobefore;
		undo = *undoptr;
	    } else {
		undo_setup(undo);
		if (!isundo())
		    goto noundo;
		undo->u_dotlno = get_lineno(curbp,curwp->w_dotp);
		undo->u_doto = curwp->w_doto;
		undo->u_type = UDOVER;
		undo->u_used = 0;
		undo->u_code[0] = '\0';
	    }
	    if (!undo_bgrow(undo, n))
		goto noundo;

	    lchange(WFEDIT);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	    if (undo->u_code[0]) {
		lputc(curwp->w_dotp, curwp->w_doto, undo->u_code[0]);
		lkanji2nd = 1;
	    } else
		lkanji2nd = 0;
#endif	/* KANJI */
	    count = undo->u_used;
	    while(curwp->w_doto < llength(curwp->w_dotp) && n--) {
		undo->u_buffer[count] =
		    lgetc(curwp->w_dotp, curwp->w_doto);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (lkanji2nd != 0) {
		    lkanji2nd--;
		} else if (ISKANJI(undo->u_buffer[count])){
#ifdef	HOJO_KANJI
		    if (ISHOJO(undo->u_buffer[count])) lkanji2nd = 2;
		    else
#endif
		    lkanji2nd = 1;
		}
#endif	/* KANJI */
		lputc(curwp->w_dotp, curwp->w_doto++, c);
		count++;
	    }
	    undo->u_used = count;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	    if (lkanji2nd!=0 && curwp->w_doto < llength(curwp->w_dotp)) {
		undo->u_code[0] = lgetc(curwp->w_dotp, curwp->w_doto);
		lputc(curwp->w_dotp, curwp->w_doto, ' ');
	    } else
		undo->u_code[0] = '\0';
#endif	/* KANJI */
	    undo_finish(&(undo->u_next));

	    if (!(lastflag & CFINS2)) {
		if (undoptr!=NULL && *undoptr!=NULL)
		    (*undoptr)->u_type = UDNONE;
	    }
	    if (n<=0) return TRUE;
	} else if (lastflag & CFINS2) {/* not Overwrite mode */
#ifdef	KANJI
	    if (!inkfill && undostart==undoptr) {
#else
	    if (undostart==undoptr) {
#endif
		curbp->b_utop--;
		if (curbp->b_utop < 0)
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
    if(curbp->b_flag & BFOVERWRITE) {		/* Overwrite mode	*/
	lchange(WFEDIT);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	lkanji2nd = 0;
#endif	/* KANJI */
	while(curwp->w_doto < llength(curwp->w_dotp) && n--) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	    if (lkanji2nd != 0) {
		lkanji2nd--;
	    } else if (ISKANJI(lgetc(curwp->w_dotp, curwp->w_doto))) {
#ifdef	HOJO_KANJI
		if (ISHOJO(lgetc(curwp->w_dotp, curwp->w_doto)))
		    lkanji2nd = 2;
		else
#endif
		lkanji2nd = 1;
	    }
#endif	/* KANJI */
	    lputc(curwp->w_dotp, curwp->w_doto++, c);
	}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	if (lkanji2nd!=0 && curwp->w_doto < llength(curwp->w_dotp)) {
	    lputc(curwp->w_dotp, curwp->w_doto, ' ');
	}
#endif	/* KANJI */
	if(n<=0) return TRUE;
    }
    return linsert(n, c);
}

/*
 * this could be implemented as a keymap with everthing defined
 * as self-insert.
 */
quote(f, n)
{
    register int c;

    key.k_count = 1;
    if((key.k_chars[0] = getkey(TRUE)) >= '0' && key.k_chars[0] <= '7') {
	key.k_chars[0] -= '0';
	if((c = getkey(TRUE)) >= '0' && c <= '7') {
	    key.k_chars[0] <<= 3;
	    key.k_chars[0] += c - '0';
	    if((c = getkey(TRUE)) >= '0' && c <= '7') {
		key.k_chars[0] <<= 3;
		key.k_chars[0] += c - '0';
	    } else ungetkey(c);
	} else ungetkey(c);
    }
    return selfinsert(f, n);
}
