/* $Id: regex.c,v 1.2.2.5 2006/01/14 13:10:05 amura Exp $ */
/* This source is select Regular Expression routine */

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef REGEX
# include	"def.h"		/* defines VOID etc. for mg */
# include	"in_code.h"	/* defines NG_WCHAR_t */
# include	"chrdef.h"

# define ng
# include	"trex.h"

#define ISSPACE(c)	((c)==NG_WTAB||(c)==NG_WSPACE||(c)==NG_WCODE('\v')\
			 ||(c)==NG_WCODE('\f')||(c)==NG_WCODE('\r'))
static TRexBool
trex_matchcclass(int cclass, TRexChar c)
{
    switch (cclass) {
    case 'w': return ISWORD2(curbp->b_lang,c)?TRex_True:TRex_False;
    case 'W': return !ISWORD2(curbp->b_lang,c)?TRex_True:TRex_False;
    case 'l': return ISLOWER2(curbp->b_lang,c)?TRex_True:TRex_False;
    case 'u': return ISUPPER2(curbp->b_lang,c)?TRex_True:TRex_False;
    case 'a': return (ISUPPER2(curbp->b_lang,c)||ISLOWER2(curbp->b_lang,c))
	?TRex_True:TRex_False;
    case 'A': return !(ISUPPER2(curbp->b_lang,c)||ISLOWER2(curbp->b_lang,c))
	?TRex_True:TRex_False;
    default:
	if (!ISASCII(c))
	    return TRex_False;
	switch (cclass) {
	case 's': return ISSPACE(c)?TRex_True:TRex_False;
	case 'S': return !ISSPACE(c)?TRex_True:TRex_False;
	case 'd': return ISDIGIT(c) ?TRex_True:TRex_False;
	case 'D': return !ISDIGIT(c)?TRex_True:TRex_False;
	case 'x': return (ISDIGIT(c)||(c>='a'&&c<='f')||(c>='A'&&c<='F'))
	    ?TRex_True:TRex_False;
	case 'X': return !(ISDIGIT(c)||(c>='a'&&c<='f')||(c>='A'&&c<='F'))
	    ?TRex_True:TRex_False;
	case 'c': return ISCTRL(c)  ? TRex_True : TRex_False;
	case 'C': return !ISCTRL(c) ? TRex_True : TRex_False;
	case 'p': return !(c==NG_WSPACE||ISUPPER(c)||ISLOWER(c)||ISDIGIT(c))
	    ?TRex_True:TRex_False;
	case 'P': return (c==NG_WSPACE||ISUPPER(c)||ISLOWER(c)||ISDIGIT(c))
	    ?TRex_True:TRex_False;
	}
    }
    return TRex_False; /*cannot happen*/
}
# include	"trex.c"
#endif /* REGEX */
