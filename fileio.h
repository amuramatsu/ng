/* $Id: fileio.h,v 1.1.2.8 2006/01/15 00:06:30 amura Exp $ */
/*
 * This file is the header file for fileio parts
 * of the NG display editor.
 */
#ifndef __FILEIO_H__
#define __FILEIO_H__

#include "i_buffer.h"
#include "i_line.h"

#define	NG_EOL_NONE	0
#define	NG_EOL_LF	1
#define	NG_EOL_CRLF	2
#define	NG_EOL_CR	3
#define NG_EOL_MASK(n)	((n)&0x03)
#define	NG_EOL_2BE	0x10
#define	NG_EOL_2LE	0x20

#ifdef __cplusplus
extern "C" {
#endif

int fffiles _PRO((char *, char **));
#ifdef NEW_COMPLETE
char *file_name_part _PRO((char *));
#endif
#ifndef NO_STARTUP
char *startupfile _PRO((char *, char *));
#endif

int ffputline _PRO((const char *, int, int));
int ffgetline _PRO((char *, int, int *, int));

int rchdir _PRO((const char *));
int d_undelbak _PRO((int, int));
int d_makename _PRO((LINE *, NG_WCHAR_t *, int));
#ifdef WIN32
int unlink _PRO((const char *));
int rmdir _PRO((const char *));
int rename _PRO((const char *, const char *));
#endif
int ffwopen _PRO((const char *));
int ffropen _PRO((const char *));
int ffclose _PRO((void));
int ffisdir _PRO((const char *));
int fgetfilemode _PRO((const char *));
VOID fsetfilemode _PRO((const char *, int));
int fbackupfile _PRO((const char *));
int ffwopen _PRO((const char *));
int ffputbuf _PRO((BUFFER *));
char *adjustname _PRO((const char *));
int copy _PRO((const char *, const char *));
int fchkreadonly _PRO((const char *fn));

#ifdef __cplusplus
}
#endif

#endif /* __FILEIO_H__ */
