/* $Id: sleep.c,v 1.1 2000/06/27 01:48:01 amura Exp $ */
/*
 * Name:	MicroEmacs
 *		AmigaDOS sleep function
 * Version:	31
 * Last Edit:	18-Apr-86
 * Created:	18-Apr-86 ...!ihnp4!seismo!ut-sally!ut-ngp!mic
 */

/*
 * $Log: sleep.c,v $
 * Revision 1.1  2000/06/27 01:48:01  amura
 * Initial revision
 *
 */

#include	"config.h"	/* Dec. 15, 1992 by H.Ohkubo */

/* There are really 60 ticks/second, but I don't want to wait that 	*/
/* long when matching parentheses... */
#define	TICKS	45
extern	long Delay();

#ifdef	SUPPORT_ANSI
void
#endif
sleep(n)
int n;
{
	if (n > 0)
		Delay((long) n * TICKS);
}
