/* $Id: i_undo.h,v 1.1.2.1 2003/02/26 00:08:57 amura Exp $ */
/*
 * Undo supports: Ng 1.4(upto beta4) support undo like emacs.
 * This undo is not support redo. and not perfect now.
 *
 * by MURAMATSU Atsushi
 */

#ifndef __I_UNDO_H__
#define __I_UNDO_H__

#include "i_line.h"

typedef struct UNDO_DATA {
    char u_type;
    LINE_NO_t u_dotlno;
    LINE_OFF_t u_doto;
    RSIZE u_size;
    NG_WCHAR_t u_code[2];
    struct UNDO_DATA* u_next;
    RSIZE u_used;
    NG_WCHAR_t *u_buffer;
} UNDO_DATA;

#define UDNONE		0
#define	UDDEL		1
#define UDBS		2
#define UDINS		3
#define	UDINSNL		3	/* this is backward compatibirity */
#define UDOVER		5
#define UDTWIDDLE	6
#define	UDREPL		7

#define UDMASK		0x0F
#define	UDFIRST		0x10

extern UNDO_DATA** undoptr;
extern UNDO_DATA** undostart;
extern UNDO_DATA** undobefore;

#endif /* __I_UNDO_H__ */
