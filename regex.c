/* $Id: regex.c,v 1.2 2003/02/22 08:09:47 amura Exp $ */
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
#endif
