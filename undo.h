/* $Id: undo.h,v 1.9.2.3 2005/04/07 17:15:20 amura Exp $ */
/*
 * Undo supports: Ng 1.4(upto beta4) support undo like emacs.
 * This undo is not support redo. and not perfect now.
 *
 * by MURAMATSU Atsushi
 */

#ifndef __UNDO_H__
#define __UNDO_H__

#ifdef UNDO

#include "i_buffer.h"
#include "i_undo.h"

/* undo support functions */

VOID ublock_open  _PRO((register BUFFER *));
VOID ublock_close _PRO((register BUFFER *));
VOID ublock_clear _PRO((register UNDO_DATA **));
VOID undo_clean   _PRO((BUFFER*));
int  undo_balloc  _PRO((register UNDO_DATA*, register RSIZE));
int  undo_bgrow   _PRO((register UNDO_DATA*, register RSIZE));

/* undo support functions (implemented by macro for SPEED) */

#define isundo() (undoptr  != NULL)
#define undo_check(_bp)	((_bp)->b_utop != (_bp)->b_ubottom)
#define undo_reset(_bp)	((void)((_bp)->b_ubottom = (_bp)->b_utop = 0, \
				(_bp)->b_ulast = NULL))
#define undo_init(_bp) do {					\
    bzero((_bp)->b_ustack, sizeof(UNDO_DATA *)*(UNDOSIZE+1));	\
    undo_reset((_bp));						\
} while (/*CONSTCOND*/0)
#define undo_setup(_u) do {				\
    if (undoptr != NULL) {				\
	if (*undoptr == NULL) {				\
	    *undoptr = malloc(sizeof(UNDO_DATA));	\
	    if (*undoptr == NULL) {			\
		ewprintf("undo_setup: No memory");	\
		ttwait();				\
		undo_clean(curbp);			\
		undoptr = NULL;				\
		(_u) = NULL;				\
	    } else					\
		bzero(*undoptr, sizeof(UNDO_DATA));	\
	}						\
	if (undoptr != NULL)				\
	    (_u) = *undoptr;				\
    }							\
} while (/*CONSTCOND*/0)
#define undo_finish(_n) do {				\
    undobefore = undoptr;				\
    undoptr = (_n);					\
} while (/*CONSTCOND*/0)
#define	undo_type(_u) ((_u)->u_type & UDMASK)
#define undo_bfree(_u) do {				\
    if ((_u)->u_size) {					\
	free((_u)->u_buffer);				\
	(_u)->u_size = 0;				\
    }							\
} while (/*CONSTCOND*/0)

#else /* not UNDO */

#define ublock_open(b)
#define undo_clean(b)		/*void*/
#define undo_balloc(u,b)
#define undo_bgrow(u,i)

#define isundo() 		0
#define undo_check(_bp)		0
#define undo_init(_bp)		/*void*/
#define undo_reset(bp)		/*void*/
#define undo_setup(_u)		/*void*/

#endif /* UNDO */
#endif /* __UNDO_H__ */
