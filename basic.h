/* $Id: basic.h,v 1.1.2.3 2005/04/07 17:15:19 amura Exp $ */
/*
 * This file is the header file for basic parts
 * of the NG display editor.
 */
#ifndef __BASIC_H__
#define __BASIC_H__

#include "i_line.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID isetmark _PRO((void));
int setmark _PRO((int, int));

int getgoal _PRO((LINE *));
VOID setgoal _PRO((void));

int forwchar _PRO((int, int));
int backchar _PRO((int, int));
int forwpage _PRO((int, int));
int backpage _PRO((int, int));
int forwline _PRO((int, int));
int backline _PRO((int, int));
int gotobob _PRO((int, int));
int gotoeob _PRO((int, int));
int gotoline _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __BASIC_H__ */
