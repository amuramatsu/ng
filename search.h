/* $Id: search.h,v 1.1.2.1 2005/04/09 06:26:39 amura Exp $ */

#ifndef __SEARCH_H__
#define __SEARCH_H__

#include "in_code.h"
extern NG_WCHAR_t pat[];

#ifdef __cplusplus
extern "C" {
#endif

int forwsrch _PRO((void));
int backsrch _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __SEARCH_H__ */
