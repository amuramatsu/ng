/* $Id: lstring.c,v 1.1.2.2 2006/04/01 15:19:28 amura Exp $ */
/*
 * strlcpy and strlcat for old platform
 */

#include "config.h"
#include "def.h"

#include "lstring.h"

#ifndef HAVE_STRLCPY
size_t
strlcpy(dst, src, n)
register char *dst;
const char *src;
size_t n;
{
    size_t i;
    register const char *p = src;
    for (i=n-1; i>0 && *p!='\0'; i--)
	*dst++ = *p++;
    *dst = '\0';
    while (*p++ != '\0')
	/*NOP*/;
    return p - src;
}
#endif /* HAVE_STRLCPY */

#ifndef HAVE_STRLCAT
size_t
strlcat(dst, src, n)
char *dst;
const char *src;
size_t n;
{
    size_t i;
    register const char *psrc = src;
    register char *pdst = dst;
    while (*pdst != '\0')
	pdst++;
    for (i=n-(pdst-dst)-1; i>0 && *psrc != '\0'; i--)
	*pdst++ = *psrc++;
    *pdst = '\0';
    while (*psrc++ != '\0');
	/*NOP*/;
    return (pdst-dst) + (psrc-src);
}
#endif /* HAVE_STRLCAT */
