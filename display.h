/* $Id: display.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the header file for display parts
 * of the NG display editor.
 */
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "i_line.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID putline _PRO((int, int, NG_WCHAR_t *, short));
VOID PutLine _PRO((int, NG_WCHAR_t *, short));
int vtputs _PRO((NG_WCHAR_t *));
VOID vtputc _PRO((NG_WCHAR_t));
int ttwait _PRO((void));
VOID vtsetsize _PRO((int, int));
int countlines _PRO((LINE *));
int colrow _PRO((LINE *, LINE_OFF_t, int *, int *));
LINE_NO_t skipline _PRO((LINE *, int));
VOID update _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */
