/* $Id: kanji_.c,v 1.3 2003/02/22 08:09:46 amura Exp $ */
/* mskanji.c: ms-kanji handling routines
 * -> kanji_.c: euc kanji handling routines
 *serow / amura
 */

#include "kanji_.h"

#define UCH(c)   ((unsigned char)(c))

/* kpart(pLim, pChr);
 * char * pLim;  Buffer Top or Limit for scanning
 * char * pChr;  Pointer to the char
 *		return whith 1 : *pChr is First byte of MS-Kanji
 *		return whith 2 : *pChr is Second byte of MS-Kanji
 *		return whith 0 : otherwize
 */
int
kpart(pLim, pChr)
char *pLim;
char *pChr;
{
    register char *p  = pChr - 1;
    register int ct = 0;
    
    while (ISKANJI(*p) && p >= pLim) {
	p--;
	ct++;
    }
    return (ct & 1) ? 2 : ISKANJI(*pChr);
}

/* jstrlen: return the number of charctors in string.
 */
int
jstrlen(s)
char *s;
{
    int len;
    
    for (len = 0; *s; s++, len++) {
	if (ISKANJI(*s) && s[1])
	    s++;
    }
    return len;
}

/* jnthchar: return with the pointer to n'th charactor in string.
 *           return (char *)0, when jstrlen(s) < n
 */
char *
jnthchar(s, n)
char *s;
int n;
{
    if (n) {
	while (--n) {
	    if (ISKANJI(*s))
		s++;
	    s++;
	}
    }
    return s;
}

/* jindex: return with the pointer to 'ch' in "string"
 *         return with (char *)0, when not found
 */
char *
jindex(s, c)
char *s;
int   c;
{
    while (*s) {
	if (UCH(*s) == UCH(c))
	    return s;
	
	if (ISKANJI(*s) && s[1]) {
	    s++;
	}
	s++;
    }
    return (char *)0;
}
/* jrindex: return with the pointer to Right end 'ch' in "string"
 *          return with (char *)0, when not found
 */
char *
jrindex(s, c)
char *s;
int   c;
{
    char * olds = (char *)0;
    
    while (*s) {
	if (UCH(*s) == UCH(c))
	    olds = s;
	
	if (ISKANJI(*s) && s[1]) {
	    s++;
	}
	s++;
    }
    return olds;
}

char *
jstrlower(s)
char *s;
{
    char *ws = s;

    while (*ws) {
	if (ISKANJI(*ws) && ws[1])
	    ws++;
	else
	    *ws = (('A' <= *ws) && (*ws <= 'Z')) ? *ws - 'A' + 'a': *ws;
	ws++;
    }
    return s;
}
