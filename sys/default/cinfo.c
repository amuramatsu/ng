/* $Id: cinfo.c,v 1.4.2.2 2006/01/14 19:03:23 amura Exp $ */
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
#include "in_code.h"

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
};

#ifndef PF_KEYSTRINGS
static const char *pf_keystrings[] = {
    "F1",		"F2",		"F3",		"F4",
    "F5",		"F6",		"F7",		"F8",
    "F9",		"F10",		"F11",		"F12",
    "F13",		"F14",		"F15",		"F16",
    "F17",		"F18",		"F19",		"F20",
    NULL
};
#else
extern const char *pf_keystrings[];
#endif /* PF_KEYSTRINGS */

#ifndef SP_KEYSTRINGS
static const char *sp_keystrings[] = {
    "Up",		"Down",		"Left",		"Right",
    "Home",		"End",		"PageUp",	"PageDown",
    "Help",		"Clear",	"Pause",	"",
    "",			"",		"",		"",
    "Shift-Up",		"Shift-Down",	"Shift-Left",	"Shift-Right",
    "Shift-Home",	"Shift-End",	"Shift-PageUp",	"Shift-PageDown",
    "Shift-Help",	"Shift-Clear",	"Shift-Pause",	NULL,
};
#else
extern const char *sp_keystrings[];
#endif /* SP_KEYSTRINGS */

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
    register const char *np;
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
	if (k >= NG_W_PF01 && k <= NG_W_PF20 &&
	    (np = pf_keystrings[k - NG_W_PF01]) != NULL)
	    break;
	if (k >= NG_W_UP && k <= NG_W_LAST &&
	    (np = sp_keystrings[k - NG_W_UP]) != NULL)
	    break;
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

static int
keyname_match(name, list)
const char *name;
const char *list[];
{
    const char *p1, *p2;
    char c1, c2;
    int i;
    for (i=0; list[i] != NULL; i++) {
	p1 = name;
	p2 = list[i];
	while (*p1) {
	    c1 = ISUPPER(*p1) ? TOLOWER(*p1) : *p1;
	    c2 = ISUPPER(*p2) ? TOLOWER(*p2) : *p2;
	    if (c1 != c2)
		break;
	}
	if (*p1 == '\0' && *p2 == '\0')
	    return i;
    }
    return -1;
}

NG_WCHAR_t
keyname_encode(name)
const char *name;
{
    int s;
    s = keyname_match(name, pf_keystrings);
    if (s > 0)
	return NG_W_PF01 + s;
    s = keyname_match(name, sp_keystrings);
    if (s > 0)
	return NG_W_UP + s;
    return NG_EOS;
}
