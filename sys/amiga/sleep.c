/* $Id: sleep.c,v 1.2 2001/11/23 11:56:45 amura Exp $ */
/*
 * Name:	MicroEmacs
 *		AmigaDOS sleep function
 * Version:	31
 * Last Edit:	18-Apr-86
 * Created:	18-Apr-86 ...!ihnp4!seismo!ut-sally!ut-ngp!mic
 */

/*
 * $Log: sleep.c,v $
 * Revision 1.2  2001/11/23 11:56:45  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 */

#include "config.h"	/* Dec. 15, 1992 by H.Ohkubo */
#include "def.h"
#ifdef INLINE_PRAGMAS
#include <pragmas/dos_pragmas.h>
#else
#include <clib/dos_protos.h>
#endif

/* There are really 60 ticks/second, but I don't want to wait that 	*/
/* long when matching parentheses... */
#define	TICKS	45

VOID
sleep(n)
int n;
{
    if (n > 0)
	Delay((long) n * TICKS);
}
