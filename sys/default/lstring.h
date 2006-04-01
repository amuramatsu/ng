/* $Id: lstring.h,v 1.1.2.1 2006/04/01 14:29:43 amura Exp $ */
/*
 * strlcpy and strlcat for old platform
 */

#ifndef __LSTRING_H__
#define __LSTRING_H__

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_STRLCPY
size_t strlcpy _PRO((char *, const char *, size_t));
#endif
#ifndef HAVE_STRLCAT
size_t strlcat _PRO((char *, const char *, size_t));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LSTRING_H__ */
