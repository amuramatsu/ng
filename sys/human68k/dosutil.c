/* $Id: dosutil.c,v 1.3 2002/04/06 22:59:25 amura Exp $ */
/*
 *		UNIX like functions for Human68k.
 *
 *		Coded by Shigeki Yoshida (shige@csk.CO.JP)
 */

/*
 * $Log: dosutil.c,v $
 * Revision 1.3  2002/04/06 22:59:25  amura
 * now Human68k port is validated
 *
 * Revision 1.2  2001/11/23 11:56:47  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */
/* 90.11.9	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.11	Created for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */

#include <iocslib.h>
#include <time.h>

/* Sleep (busily) for n seconds */
/* But return on key input (kaoru) */
/* 91.01.15  by K.Maeda */
int
sleep(n)
int n;
{
    register start, laps, s;

    for (; n > 43200; n -= 43200) {
	if (sleep(43200))
	    return 1;
    }
    for (s = n*100, start = ONTIME(); n;) {
	if (kbhit())
	    return 1;
	if ((laps = ONTIME() - start) < 0)
	    laps += (100 * 60 * 60 * 24);
	if (laps >= s)
	    return 0;
    }
}
