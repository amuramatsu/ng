/* $Id: dosutil.c,v 1.4 2003/02/22 08:09:47 amura Exp $ */
/*
 *		UNIX like functions for MS-DOS.
 *
 *		Coded by Shigeki Yoshida (shige@csk.CO.JP)
 */
/* 90.02.11	Created for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */

#include <dos.h>

#ifndef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
/* 90.03.24	Modified by S.Yoshida
 * In old version (Ng 1.1), I used difftime() function to check
 * sleeping time. But, it returns double floating point value type,
 * and execute file was too big. So I change this function to not
 * use difftime(). This new sleep() use syssec() that was in a
 * ttyio.c. And move syssec() from ttyio.c to this dosutil.c file.
 */
int
sleep(sec)
int sec;
{
    register int s, ss, se;
    
    se = (ss = syssec()) + sec * 100;
    do {
	s = syssec();
	if (se >= 6000 && s < ss)
	    se -= 6000;
    } while (s < se);
}
#endif	/* __TURBOC__ */

/*
 * Get system time, and return (second * 100) value.
 */
int
syssec()
{
    union REGS regs;
    
    regs.h.ah = 0x2c;
    intdos(&regs, &regs);
    return(regs.h.dh * 100 + regs.h.dl);
}
