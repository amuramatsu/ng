/* $Id: i_window.h,v 1.1.2.2 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the window structure definition of the NG
 * display editor.
 */

#ifndef __I_WINDOW_H__
#define __I_WINDOW_H__

#include "i_line.h"
#include "i_buffer.h"

/*
 * There is a window structure allocated for
 * every active display window. The windows are kept in a
 * big list, in top to bottom screen order, with the listhead at
 * "wheadp". Each window contains its own value of dot.
 * The flag field contains some bits that are set by commands
 * to guide redisplay; although this is a bit of a compromise in
 * terms of decoupling, the full blown redisplay is just too
 * expensive to run for every input character.
 */
typedef struct	WINDOW {
    LIST w_list;			/* List header		       */
    struct BUFFER *w_bufp;		/* Buffer displayed in window	*/
    struct LINE *w_linep;		/* Top line in the window	*/
    struct LINE *w_dotp;		/* Line containing "."		*/
    LINE_NO_t w_lines;			/* Top line displayed line number */
    LINE_OFF_t w_doto;			/* Byte offset for "."		*/
    DISP_ROW_t w_toprow;		/* Origin 0 top row of window	*/
    DISP_ROW_t w_ntrows;		/* # of rows of text in window	*/
    DISP_ROW_t w_force;			/* If NZ, forcing row.		*/
    char	w_flag;			/* Flags.			*/
    LINE_NO_t w_dotlines;		/* line containing "." on tty lines */
} WINDOW;
#define w_wndp	w_list.l_p.l_wp
#define w_name	w_list.l_name

/*
 * Window flags are set by command processors to
 * tell the display system what has happened to the buffer
 * mapped by the window. Setting "WFHARD" is always a safe thing
 * to do, but it may do more work than is necessary. Always try
 * to set the simplest action that achieves the required update.
 * Because commands set bits in the "w_flag", update will see
 * all change flags, and do the most general one.
 */
#define WFFORCE 0x01			/* Force reframe.		*/
#define WFMOVE	0x02			/* Movement from line to line.	*/
#define WFEDIT	0x04			/* Editing within a line.	*/
#define WFHARD	0x08			/* Better to a full display.	*/
#define WFMODE	0x10			/* Update mode line.		*/

extern WINDOW *curwp;
extern WINDOW *wheadp;

#endif /* __I_WINDOW_H__ */
