/* $Id: i_buffer.h,v 1.1.2.4 2006/01/04 17:00:39 amura Exp $ */
/*
 * This file is the internal buffer definition of the NG
 * display editor.
 */

#ifndef __I_BUFFER_H__
#define __I_BUFFER_H__

#include "i_line.h"
#define UNDOSIZE	10
typedef unsigned char UNDO_IDX_t;

/*
 * Text is kept in buffers. A buffer header, described
 * below, exists for every buffer in the system. The buffers are
 * kept in a big list, so that commands that search for a buffer by
 * name can find the buffer header. There is a safe store for the
 * dot and mark in the header, but this is only valid if the buffer
 * is not being displayed (that is, if "b_nwnd" is 0). The text for
 * the buffer is kept in a circularly linked list of lines, with
 * a pointer to the header line in "b_linep".
 */
typedef struct BUFFER {
    LIST b_list;			/* buffer list pointer		*/
    struct BUFFER *b_altb;		/* Link to alternate buffer	*/
    struct LINE *b_dotp;		/* Link to "." LINE structure	*/
    struct LINE *b_markp;		/* ditto for mark		*/
    struct LINE *b_linep;		/* Link to the header LINE	*/
    struct MAPS_S *b_modes[PBMODES];	/* buffer modes		*/
    LINE_OFF_t b_doto;			/* Offset of "." in above LINE	*/
    LINE_OFF_t b_marko;			/* ditto for the "mark"		*/
    LINE_OFF_t b_nmodes;		/* number of non-fundamental modes */
    LINE_OFF_t b_nwnd;			/* Count of windows on buffer	*/
    char b_flag;			/* Flags			*/
    char *b_fname;			/* File name			*/
#ifdef	EXTD_DIR
    char *b_cwd;			/* Current working directory for
					   this buffer.  By Tillanosoft */
#endif
    struct LANG_MODULE *b_lang;		/* Language of buffer		*/
    short b_fio;			/* buffer file coding		*/
    char b_lfio;			/* Line delimiter		*/
#ifdef  VARIABLE_TAB
    char b_tabwidth;			/* Local TAB width		*/
#endif  /* VARIABLE_TAB */
#ifdef	UNDO
    struct UNDO_DATA *b_ustack[UNDOSIZE+1];
					/* Undo stack data		*/
    UNDO_IDX_t b_utop;			/* Undo stack top		*/
    UNDO_IDX_t b_ubottom;		/* Undo stack bottom		*/
    struct UNDO_DATA **b_ulast;		/* Last edited undo data	*/
#endif	/* UNDO */
} BUFFER;
#define b_bufp	b_list.l_p.x_bp
#define b_bname b_list.l_name

#define BFCHG		0x01		/* Changed.			*/
#define BFBAK		0x02		/* Need to make a backup.	*/
#define BFNOTAB		0x04		/* no tab mode			*/
#define BFOVERWRITE	0x08		/* overwrite mode		*/
#define	BFAUTOFILL	0x10		/* autofill mode with KANJI.	*/
#ifdef	READONLY
#define	BFRONLY		0x20		/* Read only mode.		*/
#endif	/* READONLY */
#ifdef CANNA
#define BFCANNA		0x40
#endif /* CANNA */
#ifdef	AUTOSAVE
#define	BFACHG		0x80		/* Auto save after changed.	*/
#endif	/* AUTOSAVE */

extern BUFFER *curbp;
extern BUFFER *bheadp;

#endif /* __I_BUFFER_H__ */
