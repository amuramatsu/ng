/* $Id: line.h,v 1.1.2.3 2005/04/07 17:15:19 amura Exp $ */
/*
 * This file is the header file for line parts
 * of the NG display editor.
 */
#ifndef __LINE_H__
#define __LINE_H__

#include "i_line.h"
#include "i_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

LINE *lalloc _PRO((int));
LINE *lallocx _PRO((int));
VOID lfree _PRO((LINE *));

int kinsert _PRO((int, int));
int kremove _PRO((int));
VOID kdelete _PRO((void));
int kgrow _PRO((int));
int linsert _PRO((int, int));
int ldelete _PRO((RSIZE, int));
int lnewline _PRO((void));
int ldelnewline _PRO((void));

int get_lineno _PRO((BUFFER*, LINE*));

#ifdef CLIPBOARD
int send_clipboard _PRO((void));
int receive_clipboard _PRO((void));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LINE_H__ */


