/* $Id: extern.h,v 1.4.2.1 2005/02/20 03:25:59 amura Exp $ */
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

#endif /* __NG_EXTERN_H__ */
