/* $Id: lstring.h,v 1.1.2.2 2007/01/11 14:42:59 amura Exp $ */
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
#ifndef HAVE_STRCASECMP
# if defined(HAVE_STRICMP)
#  define strcasecmp(s1, s2)	stricmp(s1, s2)
#  define HAVE_STRCASECMP 1
# elif defined(HAVE_STRCMPI)
#  define strcasecmp(s1, s2)	strcmpi(s1, s2)
#  define HAVE_STRCASECMP 1
# else
int strcasecmp _PRO((const char *, const char *));
# endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LSTRING_H__ */
