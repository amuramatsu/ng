/* $Id: modes.c,v 1.3.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 * Commands to toggle modes. Without an argument, toggle mode.
 * Negitive or zero argument, mode off.	 Positive argument, mode on.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

/*
 * $Log: modes.c,v $
 * Revision 1.3.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.3  2000/12/14 18:12:14  amura
 * use alloca() and more memory secure
 *
 * Revision 1.2  2000/06/27 01:49:44  amura
 * import to CVS
 *
 * Revision 1.1  1999/05/19  04:22:31  amura
 * Initial revision
 *
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "kbd.h"

int defb_nmodes = 0;
MAPS *defb_modes[PBMODES] = {&map_table[0]};
int defb_flag = 0;

static int changemode(f, n, mode)
int f, n;
char *mode;
{
    register int i;
    MAPS *m;
    VOID upmodes();

    if((m = name_mode(mode)) == NULL) {
	ewprintf("Can't find mode %s", mode);
	return FALSE;
    }
    if(!(f & FFARG)) {
	for(i=0; i <= curbp->b_nmodes; i++)
	    if(curbp->b_modes[i] == m) {
		n = 0;			/* mode already set */
		break;
	    }
    }
    if(n > 0) {
	for(i=0; i <= curbp->b_nmodes; i++)
	    if(curbp->b_modes[i] == m) return TRUE;	/* mode already set */
	if(curbp->b_nmodes >= PBMODES-1) {
	    ewprintf("Too many modes");
	    return FALSE;
	}
	curbp->b_modes[++(curbp->b_nmodes)] = m;
    } else {
	/* fundamental is b_modes[0] and can't be unset */
	for(i=1; i <= curbp->b_nmodes && m != curbp->b_modes[i]; i++) {}
	if(i > curbp->b_nmodes) return TRUE;		/* mode wasn't set */
	for(; i < curbp->b_nmodes; i++)
	    curbp->b_modes[i] = curbp->b_modes[i+1];
	curbp->b_nmodes--;
    }
    upmodes(curbp);
    return TRUE;
}

indentmode(f, n)
{
    return changemode(f, n, "indent");
}

fillmode(f, n)
{
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    if(changemode(f, n, "fill") == FALSE) return FALSE;
    if(f & FFARG) {
	if(n <= 0) curbp->b_flag &= ~BFAUTOFILL;
	else curbp->b_flag |= BFAUTOFILL;
    } else curbp->b_flag ^= BFAUTOFILL;
    return TRUE;
#else	/* NOT KANJI */
    return changemode(f, n, "fill");
#endif	/* KANJI */
}

/*
 * Fake the GNU "blink-matching-paren" variable.
 */
blinkparen(f, n)
{
    return changemode(f, n, "blink");
}

notabmode(f, n)
{
    if(changemode(f, n, "notab") == FALSE) return FALSE;
    if(f & FFARG) {
	if(n <= 0) curbp->b_flag &= ~BFNOTAB;
	else curbp->b_flag |= BFNOTAB;
    } else curbp->b_flag ^= BFNOTAB;
    return TRUE;
}

#ifdef C_MODE	/* 90.07.24  by K.Takano */
cmode(f, n)
{
#ifdef VARIABLE_TAB
    extern int cmode_tab;
    int set_tabwidth();

    if (cmode_tab != 0)
	set_tabwidth(-1, cmode_tab);
#endif /* VARIABLE_TAB */
    return changemode(f, n, "C");
}
#endif

overwrite(f, n)
int f, n;
{
    if(changemode(f, n, "overwrite") == FALSE) return FALSE;
    if(f & FFARG) {
	if(n <= 0) curbp->b_flag &= ~BFOVERWRITE;
	else curbp->b_flag |= BFOVERWRITE;
    } else curbp->b_flag ^= BFOVERWRITE;
    return TRUE;
}

set_default_mode(f, n)
int f, n;
{
    register int i;
    register MAPS *m;
    char mode[NINPUT];

    if(eread("Set Default Mode: ", mode, sizeof(mode), EFNEW) != TRUE)
    	return ABORT;
    if((m = name_mode(mode)) == NULL) {
    	ewprintf("can't find mode %s", mode);
	return FALSE;
    }
    if(!(f & FFARG)) {
	for(i=0; i <= defb_nmodes; i++)
	    if(defb_modes[i] == m) {
		n = 0;			/* mode already set */
		break;
	    }
    }
    if(n > 0) {
	for(i=0; i <= defb_nmodes; i++)
	    if(defb_modes[i] == m) return TRUE;	/* mode already set */
	if(defb_nmodes >= PBMODES-1) {
	    ewprintf("Too many modes");
	    return FALSE;
	}
	defb_modes[++defb_nmodes] = m;
    } else {
	/* fundamental is defb_modes[0] and can't be unset */
	for(i=1; i <= defb_nmodes && m != defb_modes[i]; i++) {}
	if(i > defb_nmodes) return TRUE;		/* mode wasn't set */
	for(; i < defb_nmodes; i++)
	    defb_modes[i] = defb_modes[i+1];
	defb_nmodes--;
    }
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    if(strcmp(mode, "fill")==0)
    	if(n<=0) defb_flag &= ~BFAUTOFILL;
	else defb_flag |= BFAUTOFILL;
#endif	/* KANJI */
    if(strcmp(mode, "overwrite")==0)
    	if(n<=0) defb_flag &= ~BFOVERWRITE;
	else defb_flag |= BFOVERWRITE;
    if(strcmp(mode, "notab")==0)
    	if(n<=0) defb_flag &= ~BFNOTAB;
	else defb_flag |= BFNOTAB;
    return TRUE;
}
