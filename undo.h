/* $Id: undo.h,v 1.9 2003/02/22 08:09:47 amura Exp $ */
/*
 * Undo supports: Ng 1.4(upto beta4) support undo like emacs.
 * This undo is not support redo. and not perfect now.
 *
 * by MURAMATSU Atsushi
 */

#ifndef __UNDO_H__
#define __UNDO_H__

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

typedef struct UNDO_DATA {
    int u_type;
    int u_dotlno;
    short u_doto;
    RSIZE u_size;
    char u_code[2];
    struct UNDO_DATA* u_next;
    RSIZE u_used;
    char *u_buffer;
} UNDO_DATA;

extern UNDO_DATA** undoptr;
extern UNDO_DATA** undostart;
extern UNDO_DATA** undobefore;

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

/* in line.c */
int get_lineno   _PRO((BUFFER*, LINE*));

#endif /* __UNDO_H__ */
