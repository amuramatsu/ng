/* $Id: display.h,v 1.1.2.6 2006/01/15 09:41:24 amura Exp $ */
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

VOID vtinit _PRO((void));
VOID vttidy _PRO((void));
VOID putline _PRO((int, const NG_WCHAR_t *, int));
VOID vtputc _PRO((NG_WCHAR_t));
VOID vtsetsize _PRO((int, int));
int countlines _PRO((const LINE *));
int colrow _PRO((const LINE *, LINE_OFF_ta, int *, int *));
LINE_NO_t skipline _PRO((const LINE *, int));

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */
