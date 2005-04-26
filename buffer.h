/* $Id: buffer.h,v 1.1.2.5 2005/04/26 15:48:44 amura Exp $ */
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
BUFFER *bfindw _PRO((const NG_WCHAR_t *, int, const LANG_MODULE *));
WINDOW *popbuf _PRO((BUFFER *));
int popbuftop _PRO((BUFFER *));
int killbuffer _PRO((int, int));
int set_tabwidth _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_H__ */
