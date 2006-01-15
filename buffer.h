/* $Id: buffer.h,v 1.1.2.7 2006/01/15 01:14:06 amura Exp $ */
/*
 * This file is the header file for buffer parts
 * of the NG display editor.
 */
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "i_buffer.h"
#include "i_window.h"
#include "i_lang.h"

#ifdef __cplusplus
extern "C" {
#endif

int showbuffer _PRO((BUFFER *, WINDOW *, int));
int bclear _PRO((BUFFER *));
int anycb _PRO((int));
int addline _PRO((BUFFER *, const NG_WCHAR_t *));
BUFFER *bfind _PRO((const NG_WCHAR_t *, int));
WINDOW *popbuf _PRO((BUFFER *));
int popbuftop _PRO((BUFFER *));
int killbuffer _PRO((int, int));
int set_tabwidth _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_H__ */
