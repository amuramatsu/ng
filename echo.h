/* $Id: echo.h,v 1.1.2.3 2005/04/26 15:48:44 amura Exp $ */
/*
 * This file is the header file for echo parts
 * of the NG display editor.
 */
#ifndef __ECHO_H__
#define __ECHO_H__

#include "in_code.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SUPPORT_ANSI
int message(char *fp ,...);
int ereply(const char *, NG_WCHAR_t *, int , ... );
int eread(const char *, NG_WCHAR_t *, int, int, ...);
VOID ewprintf(const char *, ... );
#else
int message();
int ereply();
int eread();
VOID ewprintf();
#endif
int eyorn _PRO((const char *));
int eyesno _PRO((const char *));
VOID eerase _PRO((void));
VOID eargset _PRO((NG_WCHAR_t *));
VOID edefset _PRO((NG_WCHAR_t *));

#if defined(CMODE)||defined(VARIABLE_TAB)||defined(AUTOSAVE)
#define USING_GETNUM	1
int getnum _PRO((char *, int *));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ECHO_H__ */



