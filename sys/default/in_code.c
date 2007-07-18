/* $Id: in_code.c,v 1.1.2.8 2007/07/18 16:40:37 amura Exp $ */
/*
 * Some special charactors of buffer internal code
 */

#include "config.h"
#include "def.h"

#include "in_code.h"

#ifdef SUPPORT_ANSI
# include <stdarg.h>
#else /* !SUPPPORT_ANSI */
# ifdef	LOCAL_VARARGS
#  include "varargs.h"
# else
# include <varargs.h>
# endif
#endif /* SUPPORT_ANSI */

int
wstrcmp(a, b)
register const NG_WCHAR_t *a;
register const NG_WCHAR_t *b;
{
    while (*a != NG_EOS) {
	int s = (long)*a++ - *b++;
	if (s)
	    return s;
    }
    return (int)*a - *b;
}

int
wstrncmp(a, b, n)
register const NG_WCHAR_t *a;
register const NG_WCHAR_t *b;
size_t n;
{
    while (*a != NG_EOS) {
	int s;
	if (n-- <= 0)
	    return 0;
	s = (long)*a++ - *b++;
	if (s)
	    return s;
    }
    return (long)*a - *b;
}

int
wstrncmpa(a, b, n)
register const NG_WCHAR_t *a;
const char *b;
size_t n;
{
    register const unsigned char *bp = (const unsigned char *)b;
    while (*a != NG_EOS) {
	int s;
	if (n-- <= 0)
	    return 0;
	s = (long)*a++ - (*bp++ & 0xff);
	if (s)
	    return s;
    }
    return (long)*a - (*bp & 0xff);
}

size_t
wstrlen(s)
const NG_WCHAR_t *s;
{
    register const NG_WCHAR_t *p = s;
    while (*p != NG_EOS)
	++p; 
    return p - s;
}

NG_WCHAR_t *
wstrcpy(dst, src)
NG_WCHAR_t *dst;
register const NG_WCHAR_t *src;
{
    register NG_WCHAR_t *p = dst;
    while (*src != NG_EOS)
	*p++ = *src++;
    *p = NG_EOS;
    return dst;
}

NG_WCHAR_t *
wstrcat(dst, src)
NG_WCHAR_t *dst;
register const NG_WCHAR_t *src;
{
    register NG_WCHAR_t *p = dst;
    while (*p != NG_EOS)
	p++;
    while (*src != NG_EOS)
	*p++ = *src++;
    *p = NG_EOS;
    return dst;
}

size_t
wstrlcpy(dst, src, n)
register NG_WCHAR_t *dst;
const NG_WCHAR_t *src;
size_t n;
{
    size_t i;
    register const NG_WCHAR_t *p = src;
    for (i=n-1; i>0 && *p!=NG_EOS; i--)
	*dst++ = *p++;
    *dst = NG_EOS;
    while (*p++ != NG_EOS)
	/*NOP*/;
    return p - src;
}

size_t
wstrlcat(dst, src, n)
NG_WCHAR_t *dst;
const NG_WCHAR_t *src;
size_t n;
{
    size_t i;
    register const NG_WCHAR_t *psrc = src;
    register NG_WCHAR_t *pdst = dst;
    while (*pdst != NG_EOS)
	pdst++;
    for (i=n-(pdst-dst)-1; i>0 && *psrc != NG_EOS; i--)
	*pdst++ = *psrc++;
    *pdst = NG_EOS;
    while (*psrc++ != NG_EOS)
	/*NOP*/;
    return (pdst-dst) + (psrc-src);
}

size_t
wstrlcpya(dst, src, n)
NG_WCHAR_t *dst;
const char *src;
size_t n;
{
    size_t i;
    register const char *p = src;
    for (i=n-1; i > 0 && *p != '\0'; i--)
	*dst++ = NG_WCODE(*p++ & 0xff);
    *dst = NG_EOS;
    while (*p++ != NG_EOS)
	/*NOP*/;
    return p - src;
}

size_t
wstrlcata(dst, src, n)
NG_WCHAR_t *dst;
const char *src;
size_t n;
{
    size_t i;
    register const char *psrc = src;
    register NG_WCHAR_t *pdst = dst;
    while (*pdst != NG_EOS)
	pdst++;
    for (i=n-(pdst-dst)-1; i > 0 && *psrc != '\0'; i--)
	*pdst++ = NG_WCODE(*psrc++ & 0xff);
    *pdst = NG_EOS;
    while (*psrc++ != '\0')
	/*NOP*/;
    return (pdst-dst) + (psrc-src);
}

size_t
strlcpyw(dst, src, n)
char *dst;
const NG_WCHAR_t *src;
size_t n;
{
    size_t i;
    register const NG_WCHAR_t *p = src;
    for (i=n-1; i>0 && *p!=NG_EOS; i--) {
	if ((*p | 0x7f) != 0x7f) {
	    *dst++ = '?';
	    ++p;
	}
	else
	    *dst++ = *p++ & 0x7f;
    }
    *dst = NG_EOS;
    while (*p++ != NG_EOS)
	/*NOP*/;
    return p - src;
}

/* XXX size_t strlcatw _PRO((char *, const NG_WCHAR_t *, size_t)); */

int
watoi(str)
const NG_WCHAR_t *str;
{
    size_t len;
    char *tmp;
    
    len = wstrlen(str) + 1;
    tmp = (char *)alloca(len);
    strlcpyw(tmp, str, len);
    return atoi(tmp);
}

#ifdef SUPPORT_ANSI
size_t
wsnprintf(NG_WCHAR_t *buf, size_t size, const char *format, ...)
{
    va_list va;
    char *tmp = (char *)alloca(size);
    va_start(va, format);
    vsnprintf(tmp, size, format, va);
    va_end(va);
    return wstrlcpya(buf, tmp, size);
}
#else /* not SUPPORT_ANSI */
size_t
wsnprintf(va_alist)
va_dcl
{
    va_list pvar;
    NG_WCHAR_t *buf;
    size_t size;
    const char *format;
    char *tmp;

    va_start(pvar);
    buf = va_arg(pvar, NG_WCHAR_t *);
    size = va_arg(pvar, size_t);
    format = va_arg(pvar, cont char *);
    tmp = (char *)alloca(size);
    vsnprintf(tmp, size, format, va);
    va_end(va);
    return wstrlcpya(buf, tmp, size);
}
#endif /* SUPPORT_ANSI */

NG_WCHAR_t *
_ng_wstr(s)
const char *s;
{
    static NG_WCHAR_t buf[256];
    int i;
    NG_WCHAR_t *p;
    
    for (i=0, p=buf; *s && i<NG_WCHARLEN(buf)-1; i++) {
	*p++ =  *s++;
    }
    *p = NG_EOS;
    return buf;
}
