/* $Id: tty.h,v 1.1.2.4 2006/01/14 19:59:59 amura Exp $ */
/*
 * This file is the header file for tty parts
 * of the NG display editor.
 */
#ifndef __TTY_H__
#define __TTY_H__

#ifdef __cplusplus
extern "C" {
#endif

VOID ttopen _PRO((void));
VOID ttinit _PRO((void));
VOID ttcolor _PRO((int));
VOID ttwindow _PRO((int, int));
VOID ttnowindow _PRO((void));
VOID tteeol _PRO((void));
VOID tttidy _PRO((void));
VOID ttflush _PRO((void));
VOID ttclose _PRO((void));
VOID tteeop _PRO((void));
VOID ttinsl _PRO((int, int, int));
VOID ttdell _PRO((int, int, int));
VOID ttputc _PRO((int));
VOID ttmove _PRO((int, int));
VOID ttresize _PRO((void));
VOID ttmove _PRO((int, int));

#ifdef ADDOPT
VOID ttykeymapinit _PRO((char *ngrcfile));
#else
VOID ttykeymapinit _PRO((void));
#endif
VOID ttykeypadstart _PRO(());
VOID ttykeymaptidy _PRO(());

#ifdef __cplusplus
}
#endif

#endif /* __TTY_H__ */
