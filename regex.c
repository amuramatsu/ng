/* $Id: regex.c,v 1.1 2000/06/27 01:47:56 amura Exp $ */
/* This source is select Regular Expression routine */

/*
 * $Log: regex.c,v $
 * Revision 1.1  2000/06/27 01:47:56  amura
 * Initial revision
 *
 */

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
