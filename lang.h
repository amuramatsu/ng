/* $Id: lang.h,v 1.1.2.1 2006/01/01 18:34:13 amura Exp $ */
/*
 * This file is the general header file for all parts
 * of the NG display editor.
 */

#ifndef __LANG_H__
#define __LANG_H__

#include "in_code.h"
#include "i_lang.h"

extern LANG_MODULE *default_lang;
extern LANG_MODULE *display_lang;

#ifdef __cplusplus
extern "C" {
#endif

VOID langinit _PRO((void));
int set_default_lang _PRO((int, int));
int set_buffer_lang _PRO((int, int));
int set_display_lang _PRO((int, int));
LANG_MODULE *get_default_lang _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __LANG_H__ */
