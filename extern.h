/* $Id: extern.h,v 1.4.2.3 2005/04/07 17:15:19 amura Exp $ */
/*
 * This file is the general header file for all parts
 * of the MicroEMACS display editor. It contains all of the
 * general definitions and macros. It also contains some
 * conditional compilation flags. All of the per-system and
 * per-terminal definitions are in special header files.
 * The most common reason to edit this file would be to zap
 * the definition of CVMVAS or BACKUP.
 */
/*
 * Externals.
 */

#ifndef __NG_EXTERN_H__
#define __NG_EXTERN_H__

extern int thisflag;
extern int lastflag;
extern int curgoal;
extern int epresf;
extern int sgarbf;
extern int mode;
extern char pat[];

extern char cinfo[];
extern char *keystrings[];

#ifdef __cplusplus
extern "C" {
#endif

VOID panic _PRO((const char *));	/* tty.c */
VOID ttbeep _PRO((void));		/* tty.c */
VOID update _PRO((void));		/* display.c */
int ttwait _PRO((void));		/* display.c */
int selfinsert _PRO((int, int));	/* kbd.c */
VOID warnreadonly _PRO((void));		/* buffer.c */

#ifdef __cplusplus
}
#endif

#endif /* __NG_EXTERN_H__ */
