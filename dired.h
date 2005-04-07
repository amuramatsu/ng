/* $Id: dired.h,v 1.1.2.2 2005/04/07 14:27:28 amura Exp $ */
/*
 * This file is the header file for dired parts
 * of the NG display editor.
 */
#ifndef __DIRED_H__
#define __DIRED_H__

#include "i_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

BUFFER *dired_ _PRO((char*));
int dired _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __DIRED_H__ */
