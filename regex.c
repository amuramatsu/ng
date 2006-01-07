/* $Id: regex.c,v 1.2.2.3 2006/01/07 17:19:09 amura Exp $ */
/* This source is select Regular Expression routine */

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef REGEX
# include	"def.h"		/* defines VOID etc. for mg */
# include	"in_code.h"	/* defines NG_WCHAR_t */
# include	"chrdef.h"

# define ng
# include	<ctype.h>	/* for isspace, isxdigit, ispunct */
# include	"trex.h"
static TRexBool
trex_matchcclass(int cclass, TRexChar c)
{
    if (ISMULTIBYTE(c))	/* cannot handle yet */
	return TRex_False;
    switch (cclass) {
    case 'a': return (ISUPPER(c)||ISLOWER(c))?TRex_True:TRex_False;
    case 'A': return !(ISUPPER(c)||ISLOWER(c))?TRex_True:TRex_False;
    case 'w': return ISWORD(c)?TRex_True:TRex_False;
    case 'W': return !ISWORD(c)?TRex_True:TRex_False;
    case 's': return isspace(c)?TRex_True:TRex_False;
    case 'S': return !isspace(c)?TRex_True:TRex_False;
    case 'd': return ISDIGIT(c) ?TRex_True:TRex_False;
    case 'D': return !ISDIGIT(c)?TRex_True:TRex_False;
    case 'x': return isxdigit(c)?TRex_True:TRex_False;
    case 'X': return !isxdigit(c)?TRex_True:TRex_False;
    case 'c': return ISCTRL(c)  ? TRex_True : TRex_False;
    case 'C': return !ISCTRL(c) ? TRex_True : TRex_False;
    case 'p': return ispunct(c)?TRex_True:TRex_False;
    case 'P': return !ispunct(c)?TRex_True:TRex_False;
    case 'l': return ISLOWER(c)?TRex_True:TRex_False;
    case 'u': return ISUPPER(c)?TRex_True:TRex_False;
    }
    return TRex_False; /*cannot happen*/
}
# include	"trex.c"
#endif /* REGEX */
