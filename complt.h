/* $Id: complt.h,v 1.3.2.1 2005/02/20 03:25:59 amura Exp $ */
#ifndef __COMPLT_H__
#define __COMPLT_H___

#define COMPLT_NO_MATCH		0
#define COMPLT_SOLE		1
#define COMPLT_AMBIGUOUS	2
#define COMPLT_NOT_UNIQUE	3

extern int complete _PRO((char *,int));
extern char *complete_message _PRO((int));
extern int complete_del_list _PRO((void));
extern int complete_scroll_down _PRO((void));
extern int complete_scroll_up _PRO((void));
extern int complete_list_names _PRO((char *, int));
#endif /* __COMPLT_H__ */
