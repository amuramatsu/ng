/* $Id: ttyio.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the header file for ttyio parts
 * of the NG display editor.
 */
#ifndef __TTYIO_H__
#define __TTYIO_H__

extern int nrow;
extern int ncol;
extern int ttrow;
extern int ttcol;
extern int tceeol;
extern int tcinsl;
extern int tcdell;

#ifdef __cplusplus
extern "C" {
#endif

int typeahead _PRO((void));
VOID panic _PRO((char *));
int ttgetc _PRO((void));
VOID ttungetc _PRO((int));
#ifdef FEPCTRL
VOID fepmode_off _PRO((void));
VOID fepmode_on _PRO((void));
#endif
VOID itimer _PRO((VOID (*func)(void), time_t sec));

#ifdef __cplusplus
}
#endif

#endif /* __TTYIO_H__ */
