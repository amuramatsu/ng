/* $Id: file.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
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

int buffsave _PRO((BUFFER *));
VOID ksetbufcode _PRO((BUFFER *));
int writeout _PRO((BUFFER *, char *));
VOID upmodes _PRO((BUFFER *));
VOID makepath _PRO((char *dname, char *fname, int len));
int readin _PRO((char *fname));
int insertfile _PRO((char *, char *));

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H__ */
