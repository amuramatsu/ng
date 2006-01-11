/* $Id: lang.h,v 1.1.2.2 2006/01/11 14:47:34 amura Exp $ */
/*
 * This file is the general header file for all parts
 * of the NG display editor.
 */

#ifndef __LANG_H__
#define __LANG_H__

#include "in_code.h"
#include "i_lang.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID langinit _PRO((void));
int set_default_lang _PRO((int, int));
int set_buffer_lang _PRO((int, int));
int set_terminal_lang _PRO((int, int));
LANG_MODULE *get_default_lang _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __LANG_H__ */
