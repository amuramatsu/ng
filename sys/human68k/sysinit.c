/* $Id: sysinit.c,v 1.1 2000/06/27 01:47:57 amura Exp $ */
/*
 *		Human68k systems initialization
 */

/*
 * $Log: sysinit.c,v $
 * Revision 1.1  2000/06/27 01:47:57  amura
 * Initial revision
 *
 */
/* 90.11.09	Created for Ng 1.2.1 Human68k by Sawayanagi Yosirou */

#include    <stdlib.h>

sysinit()
{
#ifndef FLEX_MALLOC	/* 91.01.20  by K.Maeda */
	size_t    max_size;

	if ((max_size = chkml ()) == 0)
		return (-1);
	return (sbrk (max_size / 2) == (char *)-1);
#endif
}
