/* $Id: window.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the header file for window parts
 * of the NG display editor.
 */
#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "i_window.h"

#ifdef __cplusplus
extern "C" {
#endif

int nextwind _PRO((int, int));
WINDOW *wpopup _PRO((void));
int splitwind _PRO((int, int));
int delwind _PRO((int, int));
int refresh _PRO((int, int));
int shrinkwind _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __WINDOW_H__ */
