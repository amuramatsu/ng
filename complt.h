/* $Id: complt.h,v 1.2 2001/11/23 11:56:35 amura Exp $ */

/*
 * $Log: complt.h,v $
 * Revision 1.2  2001/11/23 11:56:35  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:55  amura
 * import to CVS
 *
 */

#define COMPLT_NO_MATCH		0
#define COMPLT_SOLE		1
#define COMPLT_AMBIGUOUS	2
#define COMPLT_NOT_UNIQUE	3

extern int complete _PRO((char *,int));
extern char *complete_message _PRO((int));
