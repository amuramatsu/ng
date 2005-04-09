/* $Id: word.h,v 1.1.2.2 2005/04/09 06:26:14 amura Exp $ */

#ifndef __WORD_H__
#define __WORD_H__

#ifdef __cplusplus
extern "C" {
#endif

int inword _PRO((void));

int forwword _PRO((int, int));
int backword _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __WORD_H__ */

