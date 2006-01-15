/* $Id: autosave.c,v 1.5.2.3 2006/01/15 15:57:46 amura Exp $ */
/*
 Auto save support code programed by M.Suzuki
 Ver	1.0.0	1997/01/07	Create
*/

#include "config.h"

#ifdef AUTOSAVE
#include "def.h"
#include <time.h>

#include "autosave.h"
#include "i_buffer.h"
#include "echo.h"
#include "file.h"
#include "tty.h"
#include "ttyio.h"

static time_t autosave_interval = 10*60;/* Auto save interval time.(sec) */
static int autosave_flag = FALSE;
static int autosaved = FALSE;
#ifndef	ITIMER
static time_t check_time;
#endif

VOID
autosave_check(flag)
int flag;
{
    if (flag && autosave_interval!=0) {
	autosave_flag = TRUE;
	autosaved = FALSE;
#ifdef	ITIMER
	itimer(autosave_handler, autosave_interval);
#else
	check_time = time((time_t*)NULL);
#endif
    }
    else {
	autosave_flag = FALSE;
#ifdef	ITIMER
	itimer((void (*)())NULL, 0);
#endif
    }
}

VOID
autosave_handler()
{
    BUFFER *bp;
    int currow;
    int curcol;
#ifdef	ITIMER
    itimer((void (*)())NULL, 0);
#else	/* not ITIMER */
    time_t now_time;
    if (!autosave_flag || autosaved)
	return;
    now_time = time((time_t*)NULL);
    if ((now_time - check_time) < autosave_interval)
	return;
#endif	/* ITIMER */

    autosaved = TRUE;
    currow = ttrow;		/* save cursor position	*/
    curcol = ttcol;
    bp = bheadp;				/* For all buffers	*/
    while (bp != NULL) {
	if (bp->b_bname[0] != '*') {		/* Not internal buffer 	*/
	    if (bp->b_flag & BFACHG) {		/* modified ?	*/
		char fname[NFILEN];
		autosave_name(fname, bp->b_fname, NFILEN);
		writeout(bp, fname);
		bp->b_flag &= ~BFACHG;
	    }
	}
	bp = bp->b_bufp;
    }	/* end of while	*/
    ewprintf("autosave...done");
    ttmove(currow,curcol);
    ttflush();
}

VOID
del_autosave_file(name)
char *name;
{
    char fname[NFILEN];
    
    if (name!=NULL && name[0] != '*' ){	/* Not internal buffer ? */
	autosave_name(fname, name, NFILEN);
	unlink(fname);
	/* ewprintf("Unlink ... [%s]",fname); */
    }
}

VOID
clean_autosave_file()
{
    BUFFER *bp;
    for (bp=bheadp; bp!=NULL; bp=bp->b_bufp)
	del_autosave_file(bp->b_fname);
}

/*
 * COMMAND: set-auto-save-interval
 */
/*ARGSUSED*/
int
as_set_interval(f, n)
int f, n;
{
    if ((f & FFARG) == 0) {
    	if (getnum("auto-save-interval", &n) == FALSE)
	    return (FALSE);
    }
    autosave_interval = n;
    if (n == 0)
	autosave_flag = FALSE;
    return (TRUE);
}

#endif	/* AUTO_SAVE */
