/* $Id: dir.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
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

#ifdef __cplusplus
}
#endif

#endif /* __DIR_H__ */
