/* $Id: fileio.h,v 1.1.2.2 2005/04/07 14:27:28 amura Exp $ */
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

int rchdir _PRO((const char *));
int d_undelbak _PRO((int, int));
int d_makename _PRO((LINE *, const char *, int));
#ifdef WIN32
int unlink _PRO((const char *));
int rmdir _PRO((const char *));
int rename _PRO((const char *, const char *));
#endif
int ffropen _PRO((const char *));
int ffgetline _PRO((const char *, int, int *));
int ffclose _PRO((void));
int ffisdir _PRO((const char *));
int fgetfilemode _PRO((const char *));
int fbackupfile _PRO((const char *));
int ffwopen _PRO((const char *));
int ffputbuf _PRO((BUFFER *));
VOID autosave_name _PRO((char *buff, const char *name, int buflen));
char *adjustname _PRO((const char *));
int copy _PRO((const char *, const char *));
int fchkreadonly _PRO((const char *fn));

#ifdef __cplusplus
}
#endif

#endif /* __FILEIO_H__ */
