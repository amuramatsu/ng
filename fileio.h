/* $Id: fileio.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the header file for fileio parts
 * of the NG display editor.
 */
#ifndef __FILEIO_H__
#define __FILEIO_H__

#include "i_buffer.h"
#include "i_line.h"

#ifdef __cplusplus
extern "C" {
#endif

int rchdir _PRO((char *));
int d_undelbak _PRO((int, int));
int d_makename _PRO((LINE *, char *, int));
#ifdef WIN32
int unlink _PRO((const char *));
int rmdir _PRO((const char *));
int rename _PRO((const char *, const char *));
#endif
int ffropen _PRO((char *));
int ffgetline _PRO((char *, int, int *));
int ffclose _PRO((void));
int ffisdir _PRO((char *));
int fgetfilemode _PRO((char *));
int fbackupfile _PRO((char *));
int ffwopen _PRO((char *));
int ffputbuf _PRO((BUFFER *));
VOID autosave_name _PRO((char *buff, char *name, int buflen));
char *adjustname _PRO((char *));
int copy _PRO((char *, char *));
int fchkreadonly _PRO((char *fn));

#ifdef __cplusplus
}
#endif

#endif /* __FILEIO_H__ */
