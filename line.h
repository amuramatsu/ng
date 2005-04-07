/* $Id: line.h,v 1.1.2.2 2005/04/07 14:27:28 amura Exp $ */
/*
 * This file is the header file for line parts
 * of the NG display editor.
 */
#ifndef __LINE_H__
#define __LINE_H__

#include "i_line.h"

#ifdef __cplusplus
extern "C" {
#endif

LINE *lalloc _PRO((int));
LINE *lallocx _PRO((int));
int kinsert _PRO((int, int));
int kremove _PRO((int));
VOID kdelete _PRO((void));
int linsert _PRO((int, int));
int ldelete _PRO((RSIZE, int));
int lnewline _PRO((void));
int ldelnewline _PRO((void));

#ifdef __cplusplus
}
#endif

#endif /* __LINE_H__ */


