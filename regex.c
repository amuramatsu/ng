/* $Id: regex.c,v 1.2.2.1 2005/04/09 06:26:14 amura Exp $ */
/* This source is select Regular Expression routine */

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef REGEX
#include	"def.h"		/* defines VOID etc. for mg */
# define ng
# ifdef REGEX_JAPANESE
#  include	"kanji_.c"
#  include	"regex_j.c"
# else
#  include	"regex_e.c"
# endif
#endif /* REGEX */
