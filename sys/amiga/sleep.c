/* $Id: sleep.c,v 1.4 2003/02/22 08:09:47 amura Exp $ */
/*
 * Name:	MicroEmacs
 *		AmigaDOS sleep function
 * Version:	31
 * Last Edit:	18-Apr-86
 * Created:	18-Apr-86 ...!ihnp4!seismo!ut-sally!ut-ngp!mic
 */

#include "config.h"	/* Dec. 15, 1992 by H.Ohkubo */
#include "def.h"
#ifdef INLINE_PRAGMAS
#include <pragmas/dos_pragmas.h>
#else
#include <clib/dos_protos.h>
#endif

extern struct DOSBase *DOSBase;

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
