/* $Id: complt.h,v 1.3 2003/02/22 08:09:46 amura Exp $ */
#ifndef __COMPLT_H__
#define __COMPLT_H___

#define COMPLT_NO_MATCH		0
#define COMPLT_SOLE		1
#define COMPLT_AMBIGUOUS	2
#define COMPLT_NOT_UNIQUE	3

extern int complete _PRO((char *,int));
extern char *complete_message _PRO((int));
#endif /* __COMPLT_H__ */
