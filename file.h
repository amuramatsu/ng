/* $Id: file.h,v 1.1.2.5 2005/09/17 05:17:18 amura Exp $ */
/*
 * This file is the header file for file parts
 * of the NG display editor.
 */
#ifndef __FILE_H__
#define __FILE_H__

#include "i_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

int filevisit _PRO((int, int));
int filevisit_ _PRO((const char *, int, int));
int poptofile _PRO((int, int));

int buffsave _PRO((BUFFER *));
VOID ksetbufcode _PRO((BUFFER *));
int writeout _PRO((BUFFER *, char *));
VOID upmodes _PRO((BUFFER *));
VOID makepath _PRO((char *, char *, int));
int readin _PRO((const char *));
int insertfile _PRO((const char *, const char *));
BUFFER *findbuffer _PRO(());
VOID makename _PRO(());
VOID upmodes _PRO(());

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H__ */
