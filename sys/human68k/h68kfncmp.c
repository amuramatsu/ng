/* $Id: h68kfncmp.c,v 1.3.2.1 2006/04/01 17:15:15 amura Exp $ */
/*
 *		Human68k file name comparison
 */
/* 90.11.14	Created for Ng 1.2.1 Human68k by Sawayanagi Yosirou */

#include    "def.h"

int
h68kfncmp(s1, s2)
const register char *s1;
const register char *s2;
{
    register int c1;
    register int c2;

    while (*s1 != '\0' && *s2 != '\0') {
        c1 = *s1++;
	c2 = *s2++;
	if (ISUPPER (c1))
	    c1 = TOLOWER (c1);
	else if (c1 == '\\')
	    c1 = '/';
	if (ISUPPER (c2))
	    c2 = TOLOWER (c2);
	else if (c2 == '\\')
	    c2 = '/';
	if (c1 != c2)
	    return (c1 - c2);
    }
    if (*s1 == '\0' && *s2 == '\0')
        return (0);
    return (*s1 - *s2);
}
