/* $Id: extend.h,v 1.1.2.3 2006/01/04 17:00:39 amura Exp $ */

#ifndef __EXTEND_H__
#define __EXTEND_H__

#include "in_code.h"

#ifdef __cplusplus
extern "C" {
#endif

int insert _PRO((int, int));
int excline _PRO((NG_WCHAR_t *));
int load _PRO((const char *));

#ifdef __cplusplus
}
#endif

#endif /* __EXTEND_H__ */

