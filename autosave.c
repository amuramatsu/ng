/* $Id */
/*
 Auto save support code programed by M.Suzuki
 Ver	1.0.0	1997/01/07	Create
*/
/*
 * $Log: autosave.c,v $
 * Revision 1.1  2001/02/18 17:07:23  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 */ 

#include "config.h"
#include "def.h"

#ifdef	AUTOSAVE
time_t autosave_interval = 10*60;/* Auto save interval time.(sec)	*/
int autosave_flag = FALSE;
int autosaved = FALSE;

#if 0
/* this is port dependent */
VOID
autosave_name(buff, name, buflen)
char* buff;
char* name;
{
    strcpy(buff, name);
    if (strlen(name)) {
	char *fn = index(name, BDC1);
#ifdef	BDC2
	if (fn == NULL) {
	    fn = index(name, BDC2);
	} else
#endif
	if (fn == NULL){
	    fn = buff;
	} else {
	    fn++;
	}
	strcpy(&buff[strlen(buff)-strlen(fn)], "#");
	strcat(buff, fn);
	strcat(buff, "#");
    }
}
#endif

static time_t check_time;
VOID
autosave_check(flag)
{
    autosave_flag = flag;
    if (flag) {
	autosaved = FALSE;
	check_time = time(NULL);
    }
}

VOID
autosave_handler()
{
    BUFFER *bp;
    int currow;
    int curcol;
    time_t now_time;

    if (!autosave_flag || autosaved)
	return;
    now_time = time(NULL);
    if ((now_time - check_time) < autosave_interval)
	return;

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
		/*
		ttmove(nrow-1,0);
		tteeol();
		ttputs("Auto save");
		*/
		bp->b_flag &= ~BFACHG;
	    }
	}
	bp = bp->b_bufp;
    }	/* end of while	*/
    ttmove(currow,curcol);
    ttflush();
}

VOID
del_autosave_file(name)
char* name;
{
    char fname[NFILEN];
    
    if (strlen(name) && name[0] != '*' ){	/* Not internal buffer ? */
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

#endif	/* AUTO_SAVE	*/
