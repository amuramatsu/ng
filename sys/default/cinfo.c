/* $Id: cinfo.c,v 1.4 2003/02/22 08:09:47 amura Exp $ */
/*
 *		Character class tables.
 * Do it yourself character classification
 * macros, that understand the multinational character set,
 * and let me ask some questions the standard macros (in
 * ctype.h) don't let you ask.
 */
/* 90.01.29	Modified for Ng 1.0  by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
/*
 * This table, indexed by a character drawn
 * from the 256 member character set, is used by my
 * own character type macros to answer questions about the
 * type of a character. It handles the full multinational
 * character set, and lets me ask some questions that the
 * standard "ctype" macros cannot ask.
 */
char	cinfo[256] = {
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,		/* 0x0X */
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,		/* 0x1X */
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,
_NGC_C,		_NGC_C,		_NGC_C,		_NGC_C,
0,		_NGC_P,		0,		0,		/* 0x2X */
_NGC_W,		_NGC_W,		0,		_NGC_W,
0,		0,		0,		0,
0,		0,		_NGC_P,		0,
_NGC_D|_NGC_W,	_NGC_D|_NGC_W,	_NGC_D|_NGC_W,	_NGC_D|_NGC_W,	/* 0x3X */
_NGC_D|_NGC_W,	_NGC_D|_NGC_W,	_NGC_D|_NGC_W,	_NGC_D|_NGC_W,
_NGC_D|_NGC_W,	_NGC_D|_NGC_W,	0,		0,
0,		0,		0,		_NGC_P,
0,		_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	/* 0x4X */
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	/* 0x5X */
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	0,
0,		0,		0,		0,
0,		_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	/* 0x6X */
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	/* 0x7X */
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	0,
0,		0,		0,		_NGC_C,
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
0,		0,		0,		0,		/* 0x8X */
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
				_NGC_K,
#else
				0,
#endif
#ifdef  HOJO_KANJI
						_NGC_K,
#else
						0,
#endif
0,		0,		0,		0,		/* 0x9X */
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,
0,		_NGC_K,		_NGC_K,		_NGC_K,		/* 0xAX */
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,		/* 0xBX */
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,		/* 0xCX */
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,		/* 0xDX */
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,		/* 0xEX */
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,		/* 0xFX */
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		_NGC_K,
_NGC_K,		_NGC_K,		_NGC_K,		0
#else	/* NOT KANJI (ORIGINAL) */
0,		0,		0,		0,		/* 0x8X */
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,		/* 0x9X */
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,		/* 0xAX */
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,		/* 0xBX */
0,		0,		0,		0,
0,		0,		0,		0,
0,		0,		0,		0,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	/* 0xCX */
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
0,		_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	/* 0xDX */
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	_NGC_U|_NGC_W,
_NGC_U|_NGC_W,	_NGC_U|_NGC_W,	0,		_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	/* 0xEX */
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
0,		_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	/* 0xFX */
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	_NGC_L|_NGC_W,
_NGC_L|_NGC_W,	_NGC_L|_NGC_W,	0,		0
#endif	/* KANJI */
};

/*
 * Find the name of a keystroke.  Needs to be changed to handle 8-bit printing
 * characters and function keys better.	 Returns a pointer to the terminating
 * '\0'.
 */

char *
keyname(cp, k)
register char *cp;
register int k;
{
    register char *np;
#ifdef	FKEYS
    extern char *keystrings[];
#endif

    if (k < 0)
	k = CHARMASK(k);			/* sign extended char */
    switch (k) {
    case CCHR('@'): np = "NUL"; break;
    case CCHR('I'): np = "TAB"; break;
    case CCHR('J'): np = "LFD"; break; /* yuck, but that's what GNU calls it */
    case CCHR('M'): np = "RET"; break;
    case CCHR('['): np = "ESC"; break;
    case ' ':	np = "SPC"; break; /* yuck again */
    case CCHR('?'): np = "DEL"; break;
    default:
#ifdef	FKEYS
	if (k >= KFIRST && k <= KLAST &&
	    (np = keystrings[k - KFIRST]) != NULL)
	    break;
#endif
	if (k > CCHR('?')) {
	    *cp++ = '0';
	    *cp++ = ((k>>6)&7) + '0';
	    *cp++ = ((k>>3)&7) + '0';
	    *cp++ = (k&7) + '0';
	    *cp = '\0';
	    return cp;
	}
	if (k < ' ') {
	    *cp++ = 'C';
	    *cp++ = '-';
	    k = CCHR(k);
	    if(ISUPPER(k)) k = TOLOWER(k);
	}
	*cp++ = k;
	*cp = '\0';
	return cp;
    }
    (VOID) strcpy(cp, np);
    return cp + strlen(cp);
}
