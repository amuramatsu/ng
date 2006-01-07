/* $Id: regex.c,v 1.2.2.2 2006/01/07 12:44:06 amura Exp $ */
/* This source is select Regular Expression routine */

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef REGEX
# include	"def.h"		/* defines VOID etc. for mg */
# include	"in_code.h"	/* defines NG_WCHAR_t */
# define ng
# include	"trex.c"
#endif /* REGEX */
