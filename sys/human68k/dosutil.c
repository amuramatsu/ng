/* $Id: dosutil.c,v 1.1 2000/06/27 01:47:58 amura Exp $ */
/*
 *		UNIX like functions for Human68k.
 *
 *		Coded by Shigeki Yoshida (shige@csk.CO.JP)
 */

/*
 * $Log: dosutil.c,v $
 * Revision 1.1  2000/06/27 01:47:58  amura
 * Initial revision
 *
 */
/* 90.11.9	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/* 90.02.11	Created for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	<time.h>

/* Sleep (busily) for n seconds */
/* But return on key input (kaoru) */
/* 91.01.15  by K.Maeda */
int
sleep (n)
    int n;
{
    register start, laps, s;

    for(; n > 43200; n -= 43200)
	if(sleep(43200))
	    return 1;
    for(s = n*100, start = ONTIME(); n;) {
	if ((laps = ONTIME() - start) < 0)
	    laps += 8640000;
	if (laps >= s)
	    return 0;
    }
}
