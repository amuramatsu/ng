/* $Id: echo.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the header file for echo parts
 * of the NG display editor.
 */
#ifndef __ECHO_H__
#define __ECHO_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SUPPORT_ANSI
int ereply _PRO((char *, char *, int , ... ));
int eread _PRO((char *, char *, int, int, ...));
VOID ewprintf(char *, ... );
#else
int ereply _PRO((va_alist));
int eread _PRO((va_alist));
VOID ewprintf _PRO((va_alist));
#endif
int eyorn _PRO((char *));
int eyesno _PRO((char *));
VOID eerase _PRO((void));
VOID eargset _PRO((char *));
VOID edefset _PRO((char *));

#if defined(CMODE)||defined(VARIABLE_TAB)||defined(AUTOSAVE)
#define USING_GETNUM	1
int getnum _PRO((char *, int *));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ECHO_H__ */



