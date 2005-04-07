/* $Id: buffer.h,v 1.1.2.2 2005/04/07 14:27:28 amura Exp $ */
/*
 * This file is the header file for buffer parts
 * of the NG display editor.
 */
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "i_buffer.h"
#include "i_window.h"

#ifdef __cplusplus
extern "C" {
#endif

int showbuffer _PRO((BUFFER *, WINDOW *, int));
int bclear _PRO((BUFFER *));
int anycb _PRO((int));
int addline _PRO((BUFFER *, char *));
BUFFER *bfind _PRO((char *, int));
WINDOW *popbuf _PRO((BUFFER *));
int popbuftop _PRO((BUFFER *));
VOID warnreadonly _PRO((void));
int killbuffer _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_H__ */
