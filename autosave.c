/* $Id: autosave.c,v 1.3 2001/03/02 08:49:04 amura Exp $ */
/*
 Auto save support code programed by M.Suzuki
 Ver	1.0.0	1997/01/07	Create
*/
/*
 * $Log: autosave.c,v $
 * Revision 1.3  2001/03/02 08:49:04  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.2  2001/02/18 19:27:35  amura
 * fix del_autosave_file()
 *
 * Revision 1.1  2001/02/18 17:07:23  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 */ 

#include "config.h"

#ifdef	AUTOSAVE

#include "def.h"
#include <time.h>

time_t autosave_interval = 10*60;/* Auto save interval time.(sec) */
int autosave_flag = FALSE;
int autosaved = FALSE;

extern int getnum pro((char *prompt, int *num));
/* this is port dependent */
extern VOID autosave_name pro((char *buff, char *name, int buflen));
#ifdef	ITIMER
extern VOID itimer pro((VOID (*func)(void), time_t sec));
#else
static time_t check_time;
#endif

VOID
autosave_check(flag)
{
    if (flag && autosave_interval!=0) {
	autosave_flag = TRUE;
	autosaved = FALSE;
#ifdef	ITIMER
	itimer(autosave_handler, autosave_interval);
#else
	check_time = time(NULL);
#endif
    }
    else {
	autosave_flag = FALSE;
#ifdef	ITIMER
	itimer(NULL, 0);
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
    itimer(NULL, 0);
#else	/* not ITIMER */
    time_t now_time;
    if (!autosave_flag || autosaved)
	return;
    now_time = time(NULL);
    if ((now_time - check_time) < autosave_interval)
	return;
#endif	/* ITIMER */

    autosaved = TRUE;
    currow = ttrow;		/* save cursor position	*/
    curcol = ttcol;
    bp = bheadp;				/* For all buffers	*/
    while (bp != NULL) {
	if( bp->b_bname[0] != '*' ){		/* Not internal buffer 	*/
	    if( bp->b_flag & BFACHG ){		/* modified ?	*/
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
char* name;
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
as_set_interval(f, n)
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
