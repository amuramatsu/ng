/* $Id: random.h,v 1.1.2.2 2005/04/07 17:15:19 amura Exp $ */
/*
 * This file is the header file for random parts
 * of the NG display editor.
 */
#ifndef __RANDOM_H__
#define __RANDOM_H__

#ifdef __cplusplus
extern "C" {
#endif

int forwdel _PRO((int, int));
int backdel _PRO((int, int));
int getcolpos _PRO((void));
int newline _PRO((int, int));
int delwhite _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __RANDOM_H__ */
