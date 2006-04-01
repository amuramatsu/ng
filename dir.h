/* $Id: dir.h,v 1.1.2.3 2006/04/01 17:15:14 amura Exp $ */
/*
 * This file is the header file for dir parts
 * of the NG display editor.
 */
#ifndef __DIR_H__
#define __DIR_H__

#include "i_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID storecwd _PRO((BUFFER *bp));
VOID ensurecwd _PRO((void));
VOID dirinit _PRO((void)); /* in fileio.c */
VOID dirend _PRO((void)); /* in fileio.c */

#ifdef __cplusplus
}
#endif

#endif /* __DIR_H__ */
