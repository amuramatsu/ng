/* $Id: lstring.c,v 1.1.2.3 2007/01/11 14:42:59 amura Exp $ */
/*
 * strlcpy, strlcat and strcasecmp for old platform
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

#ifndef HAVE_STRCASECMP
# include <ctype.h>
int
strcasecmp(s1, s2)
const char *s1;
const char *s2;
{
    while (*s1 && *s2) {
	int c1, c2;
	if ((*s1 & 0x80) != 0) {
	    c1 = tolower(*s1++);
	    c2 = tolower(*s2++);
	}
	else {
	    c1 = (*s1++) & 0xFF;
	    c2 = (*s2++) & 0xFF;
	}
	if (c1 > c2)
	    return 1;
	else if (c1 < c2)
	    return -1;
    }
    if (*s1 == '\0') {
	if (*s2 == '\0')
	    return 0;
	return -1;
    }
    return 1;
}
#endif /* HAVE_STRCASECMP */
