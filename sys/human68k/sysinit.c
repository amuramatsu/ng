/* $Id: sysinit.c,v 1.3 2003/02/22 08:09:47 amura Exp $ */
/*
 *		Human68k systems initialization
 */
/* 90.11.09	Created for Ng 1.2.1 Human68k by Sawayanagi Yosirou */

#include    <stdlib.h>
int
sysinit()
{
#ifndef FLEX_MALLOC	/* 91.01.20  by K.Maeda */
    size_t max_size;
    
    if ((max_size = chkml()) == 0)
	return (-1);
    return (sbrk (max_size / 2) == (char *)-1);
#endif
}
