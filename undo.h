/*
 * Undo supports: Ng 1.4(upto beta4) support undo like emacs.
 * This undo is not support redo. and not perfect now.
 *
 * by MURAMATSU Atsushi
 */

/* $Id: undo.h,v 1.1 2000/06/01 05:21:24 amura Exp $ */

/* $Log: undo.h,v $
/* Revision 1.1  2000/06/01 05:21:24  amura
/* Initial revision
/*
*/

#define UDNONE		0
#define	UDDEL		1
#define UDBS		2
#define UDINS		3
#define	UDINSNL		4
#define UDOVER		5
#define UDTWIDDLE	6
#define	UDREPL		7

#define UDMASK		0x0F
#define	UDFIRST		0x10

typedef struct UNDO_DATA {
    int    u_type;
    int    u_dotlno;
    short  u_doto;
    short  u_size;
    char   u_code[2];
    struct UNDO_DATA* u_next;
    short  u_used;
    char   *u_buffer;
} UNDO_DATA;

extern UNDO_DATA** undoptr;
extern UNDO_DATA** undobefore;

/* undo support functions */

VOID ublock_open  pro((register BUFFER *));
VOID ublock_close pro((register BUFFER *));
VOID ublock_clear pro((register UNDO_DATA **));
VOID undo_clean   pro((BUFFER*));
VOID undo_balloc pro((register UNDO_DATA*, register int));
VOID undo_bgrow  pro((register UNDO_DATA*, register int));

/* undo support functions (implemented by macro for SPEED) */

#define isundo() (undoptr  != NULL)
#define undo_check(_bp)	(_bp->b_utop != _bp->b_ubottom)
#define undo_reset(_bp)	((void)(_bp->b_ubottom = _bp->b_utop = 0))
#define undo_setup(_u) \
{\
  /* if rewrite to function, this prototype is \
   * UNDO_DATA* undo_setup(UNDO_DATA*);        \
   */                                          \
    if (undoptr != NULL) {\
	if (*undoptr == NULL) {\
	    *undoptr = malloc(sizeof(UNDO_DATA));\
	    if (*undoptr == NULL)\
	    {\
		ewprintf("undo_setup: No memory");\
		ttwait();\
		undo_clean(curbp);\
		undoptr = NULL;\
		(_u) = NULL;\
	    }\
	    else\
		bzero(*undoptr, sizeof(UNDO_DATA));\
	}\
	if (undoptr != NULL)\
	    (_u) = *undoptr;\
    }\
}
#define undo_finish(_n) \
{\
    undobefore = undoptr;\
    undoptr = (_n);\
}
#define	undo_type(_u) ((_u)->u_type & UDMASK)
#define undo_bfree(_u) \
{\
    if ((_u)->u_size) {\
	free((_u)->u_buffer);\
	(_u)->u_size = 0;\
    }\
}

/* in line.c */
int get_lineno   pro((BUFFER*, LINE*));