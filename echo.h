/* $Id: echo.h,v 1.1.2.2 2005/04/07 14:27:28 amura Exp $ */
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
int message(char *fp ,...);
int ereply(char *, char *, int , ... );
int eread(char *, char *, int, int, ...);
VOID ewprintf(char *, ... );
#else
int message();
int ereply();
int eread();
VOID ewprintf();
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



