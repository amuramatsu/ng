/* $Id: file.h,v 1.1.2.4 2005/04/28 16:53:36 amura Exp $ */
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
int poptofile _PRO((int, int));

int buffsave _PRO((BUFFER *));
VOID ksetbufcode _PRO((BUFFER *));
int writeout _PRO((BUFFER *, char *));
VOID upmodes _PRO((BUFFER *));
VOID makepath _PRO((char *dname, char *fname, int len));
int readin _PRO((const NG_WCHAR_t *fname));
int insertfile _PRO((char *, char *));
BUFFER *findbuffer _PRO(());
VOID makename _PRO(());
VOID upmodes _PRO(());

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H__ */
