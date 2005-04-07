/* $Id: display.h,v 1.1.2.3 2005/04/07 17:15:19 amura Exp $ */
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

VOID putline _PRO((int, int, const NG_WCHAR_t *, short));
VOID PutLine _PRO((int, const NG_WCHAR_t *, short));
VOID vtputc _PRO((NG_WCHAR_t));
VOID vtsetsize _PRO((int, int));
int countlines _PRO((const LINE *));
int colrow _PRO((const LINE *, LINE_OFF_t, int *, int *));
LINE_NO_t skipline _PRO((const LINE *, int));

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */
