/* $Id: basic.h,v 1.1.2.1 2005/02/20 03:25:59 amura Exp $ */
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

int forwchar _PRO((int, int));
int setmark _PRO((int, int));
int backline _PRO((int, int));
int getgoal _PRO((LINE *));
int backpage _PRO((int, int));
int countlines _PRO((LINE *));
VOID setgoal _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __BASIC_H__ */
