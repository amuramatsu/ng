/* $Id: buffer.h,v 1.1.2.6 2005/12/30 17:37:28 amura Exp $ */
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
BUFFER *bfind _PRO((const char *, int));
WINDOW *popbuf _PRO((BUFFER *));
int popbuftop _PRO((BUFFER *));
int killbuffer _PRO((int, int));
int set_tabwidth _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_H__ */
