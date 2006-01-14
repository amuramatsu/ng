/* $Id: help.c,v 1.6.2.6 2006/01/14 13:10:05 amura Exp $ */
/* Help functions for MicroGnuEmacs 2 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#ifndef NO_HELP
#include "help.h"

#include "i_buffer.h"
#include "echo.h"
#include "kbd.h"
#include "key.h"
#include "macro.h"
#include "buffer.h"
#include "cinfo.h"

/* 91.02.06  Move static declaration to here for some compiler. by S.Yoshida */
static int showall _PRO((char *, KEYMAP *));
static VOID findbind _PRO((PF, char *, KEYMAP *));
static VOID bindfound _PRO((VOID));

/*
 * Read a key from the keyboard, and look it
 * up in the keymap.  Display the name of the function
 * currently bound to the key.
 */
/*ARGSUSED*/
int
desckey(f, n)
int f, n;
{
    register KEYMAP *curmap;
    register PF funct;
    register char *pep;
    char prompt[80];
    int c;
    int m;
    int i;

#ifndef NO_MACRO
    if (inmacro)
	return TRUE;		/* ignore inside keyboard macro */
#endif
    (VOID) strcpy(prompt, "Describe key briefly: ");
    pep = prompt + strlen(prompt);
    key.k_count = 0;
    m = curbp->b_nmodes;
    curmap = curbp->b_modes[m]->p_map;
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    for (;;) {
	for (;;) {
	    ewprintf("%s", prompt);
	    pep[-1] = ' ';
	    pep = keyname(pep, key.k_chars[key.k_count++] = c = getkey(FALSE));
	    if((funct = doscan(curmap, c)) != prefix) break;
	    *pep++ = '-';
	    *pep = '\0';
	    curmap = ele->k_prefmap;
	}
	if (funct != rescan)
	    break;
	if (ISUPPER(key.k_chars[key.k_count-1])) {
	    funct = doscan(curmap, TOLOWER(key.k_chars[key.k_count-1]));
	    if (funct == prefix) {
		*pep++ = '-';
		*pep = '\0';
		curmap = ele->k_prefmap;
		continue;
	    }
	    if (funct != rescan)
		break;
	}
nextmode:
	if (--m < 0)
	    break;
	curmap = curbp->b_modes[m]->p_map;
	for (i=0; i < key.k_count; i++) {
	    funct = doscan(curmap, key.k_chars[i]);
	    if (funct != prefix) {
		if(i == key.k_count - 1 && funct != rescan) goto found;
		funct = rescan;
		goto nextmode;
	    }
	    curmap = ele->k_prefmap;
	}
	*pep++ = '-';
	*pep = '\0';
    }
found:
    if (funct == rescan)
	ewprintf("%k is not bound to any function");
    else if((pep = function_name(funct)) != NULL)
	ewprintf("%k runs the command %s", pep);
    else
	ewprintf("%k is bound to an unnamed function");
    return TRUE;
}

/*
 * This function creates a table, listing all
 * of the command keys and their current bindings, and stores
 * the table in the *help* pop-up buffer.  This
 * lets MicroGnuEMACS produce it's own wall chart.
 */
static BUFFER	*bp;
static char buf[80];	/* used by showall and findbind */

/*ARGSUSED*/
int
wallchart(f, n)
int f, n;
{
    int m;
    NG_WCHAR_t locbind[80];
    
    if ((bp = bfind("*help*", TRUE)) == NULL)
	return FALSE;
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
    bp->b_flag &= ~(BFCHG | BFACHG);/* Blow away old.	*/
#else
    bp->b_flag &= ~BFCHG;		/* Blow away old.	*/
#endif	/* AUTOSAVE	*/
    if (bclear(bp) != TRUE)
	return FALSE;	/* Clear it out.	*/
    for (m=curbp->b_nmodes; m > 0; m--) {
	wsnprintf(locbind, NG_WCHARLEN(locbind),
		  "Local keybindings for mode %s:",
		  curbp->b_modes[m]->p_name);
	if ((addline(bp, locbind) == FALSE) ||
	    (showall(buf, curbp->b_modes[m]->p_map) == FALSE) ||
	    (addline(bp, NG_WSTR_NULL) == FALSE))
	    return FALSE;
    }
    wsnprintf(locbind, NG_WCHARLEN(locbind), "Global bindings:");
    if ((addline(bp, locbind) == FALSE) ||
	(showall(buf, map_table[0].p_map) == FALSE))
	return FALSE;
    return popbuftop(bp);
}

static int
showall(ind, map)
char *ind;
KEYMAP *map;
{
    register MAP_ELEMENT *ele;
    register int i;
    PF functp;
    char *cp;
    char *cp2;
    int last;
    NG_WCHAR_t *wbuf;

    i = strlen(ind) + 1;
    
    if (addline(bp, NG_WSTR_NULL) == FALSE)
	return FALSE;
    last = -1;
    for (ele = &map->map_element[0];
	 ele < &map->map_element[map->map_num] ; ele++) {
	if (map->map_default != rescan && ++last < ele->k_base) {
	    cp = keyname(ind, last);
	    if (last < ele->k_base - 1) {
		strlcpy(cp, " .. ", sizeof(buf));
		cp = keyname(cp + 4, ele->k_base - 1);
	    }
	    do {
		*cp++ = ' ';
	    } while(cp < &buf[16]);
	    (VOID) strcpy(cp, function_name(map->map_default));
	    wbuf = (NG_WCHAR_t *)alloca(sizeof(buf));
	    wstrlcpya(wbuf, buf, NG_WCHARLEN(wbuf));
	    if (addline(bp, wbuf) == FALSE)
		return FALSE;
	}
	last = ele->k_num;
	for (i=ele->k_base; i <= last; i++) {
	    functp = ele->k_funcp[i - ele->k_base];
	    if (functp != rescan) {
		if (functp != prefix)
		    cp2 = function_name(functp);
		else
		    cp2 = map_name(ele->k_prefmap);
		if (cp2 != NULL) {
		    cp = keyname(ind, i);
		    do {
			*cp++ = ' ';
		    } while(cp < &buf[16]);
		    (VOID) strcpy(cp, cp2);
		    wbuf = (NG_WCHAR_t *)alloca(sizeof(buf));
		    wstrlcpya(wbuf, buf, NG_WCHARLEN(wbuf));
		    if (addline(bp, wbuf) == FALSE)
			return FALSE;
		}
	    }
	}
    }
    for (ele = &map->map_element[0];
	 ele < &map->map_element[map->map_num]; ele++) {
	if (ele->k_prefmap != NULL) {
	    for (i = ele->k_base;
		 ele->k_funcp[i - ele->k_base] != prefix; i++) {
		if (i >= ele->k_num)  /* damaged map */
		    return FALSE;
	    }
	    cp = keyname(ind, i);
	    *cp++ = ' ';
	    if (showall(cp, ele->k_prefmap) == FALSE)
		return FALSE;
	}
    }
    return TRUE;
}

int
help_help(f, n)
int f, n;
{
    KEYMAP *kp;
    PF funct;

    if ((kp = name_map("help")) == NULL)
	return FALSE;
    ewprintf("a b c: ");
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    do {
	int c = getkey(FALSE);
	if (!ISASCII(c))
	    return ABORT;
	funct = doscan(kp, c);
    } while (funct==NULL || funct==help_help);
#ifndef NO_MACRO
    if (macrodef && macrocount < MAXMACRO)
	macro[macrocount-1].m_funct = funct;
#endif
    return (*funct)(f, n);
}

static NG_WCHAR_t buf2[128];
static NG_WCHAR_t *buf2p;

/*ARGSUSED*/
int
apropos_command(f, n)
int f, n;
{
    register char *cp1;
    register NG_WCHAR_t *cp2;
    NG_WCHAR_t string[NINPUT];
    FUNCTNAMES *fnp;
    BUFFER *bp;

    /* FALSE means we got a 0 character string, which is fine */
    if (eread("apropos: ", string, NG_WCHARLEN(string), EFNEW) == ABORT)
	return ABORT;
    if ((bp = bfind("*help*", TRUE)) == NULL)
	return FALSE;
    bp->b_flag &= ~BFCHG;		/* Blow away old.	*/
    if (bclear(bp) == FALSE)
	return FALSE;
    for (fnp = &functnames[0]; fnp < &functnames[nfunct]; fnp++) {
	for (cp1 = fnp->n_name; *cp1; cp1++) {
	    cp2 = string;
	    while (*cp2 && *cp1 == *cp2)
		cp1++, cp2++;
	    if (!*cp2) {
		wstrlcpya(buf2, fnp->n_name, NG_WCHARLEN(buf2));
		buf2p = &buf2[wstrlen(buf2)];
		findbind(fnp->n_funct, buf, map_table[0].p_map);
		if (addline(bp, buf2) == FALSE)
		    return FALSE;
		break;
	    }
	    else
		cp1 -= cp2 - string;
	}
    }
    return popbuftop(bp);
}

static VOID
findbind(funct, ind, map)
PF funct;
char *ind;
KEYMAP *map;
{
    register MAP_ELEMENT *ele;
    register int i;
    char *cp;
    int last;

    last = -1;
    for (ele = &map->map_element[0];
	 ele < &map->map_element[map->map_num]; ele++) {
	if (map->map_default == funct && ++last < ele->k_base) {
	    cp = keyname(ind, last);
	    if (last < ele->k_base - 1) {
		strcpy(cp, " .. ");
		(VOID) keyname(cp + 4, ele->k_base - 1);
	    }
	    bindfound();
	}
	last = ele->k_num;
	for (i=ele->k_base; i <= last; i++) {
	    if (funct == ele->k_funcp[i - ele->k_base]) {
		if (funct == prefix) {
		    cp = map_name(ele->k_prefmap);
		    if (cp == NULL ||
			wstrncmpa(buf2, cp, strlen(cp)) != 0)
			continue;
		}
		(VOID) keyname(ind, i);
		bindfound();
	    }
	}
    }
    for (ele = &map->map_element[0];
	 ele < &map->map_element[map->map_num]; ele++) {
	if (ele->k_prefmap != NULL) {
	    for (i = ele->k_base;
		 ele->k_funcp[i - ele->k_base] != prefix; i++) {
		if (i >= ele->k_num)
		    return; /* damaged */
	    }
	    cp = keyname(ind, i);
	    *cp++ = ' ';
	    findbind(funct, cp, ele->k_prefmap);
	}
    }
}

static VOID
bindfound() {
    if (buf2p < &buf2[32]) {
	do {
	    *buf2p++ = NG_WSPACE;
	} while(buf2p < &buf2[32]);
    }
    else {
	*buf2p++ = NG_WCODE(',');
	*buf2p++ = NG_WSPACE;
    }
    wstrlcpya(buf2p, buf, NG_WCHARLEN(buf2));
    buf2p += strlen(buf);
}
#endif
