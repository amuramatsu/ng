/* $Id: undo.c,v 1.8 2000/11/05 01:59:20 amura Exp $ */
/*
 *		Undo support routine.
 * The functions in this file
 * are a general set of undo support utilities.
 */

/*
 * $Log: undo.c,v $
 * Revision 1.8  2000/11/05 01:59:20  amura
 * ploblem with big undo is fixed
 *
 * Revision 1.7  2000/11/04 13:44:58  amura
 * undo memory exception is more safety
 *
 * Revision 1.6  2000/09/01 19:43:10  amura
 * change undo buffer memory allocation strategy for speed
 *
 * Revision 1.5  2000/07/22 20:49:38  amura
 * more secure run insert
 *
 * Revision 1.4  2000/07/18 18:59:57  amura
 * fixed never end loop on do_undo with arguments
 *
 * Revision 1.3  2000/07/16 15:47:06  amura
 * undo bug on autofill fixed
 *
 * Revision 1.2  2000/06/27 01:49:45  amura
 * import to CVS
 *
 * Revision 1.1  2000/06/01  05:36:42  amura
 * Initial revision
 *
 */

#include	"config.h"
#include	"def.h"
#ifdef	UNDO
#include 	"undo.h"

#ifndef	UBLOCK
#define	UBLOCK	32
#endif

UNDO_DATA **undoptr;
UNDO_DATA **undostart;
UNDO_DATA **undobefore;
static int undofirst;

VOID
ublock_open(bp)
register BUFFER *bp;
{
    if (undoptr != NULL)
	panic("bug: ublock openning error");
    undoptr = undostart = &bp->b_ustack[bp->b_utop];
    undobefore = bp->b_ulast;
    if ((curbp->b_flag&BFCHG) == 0)	/* First change	*/
	undofirst = TRUE;
    else
	undofirst = FALSE;
}

VOID
ublock_close(bp)
register BUFFER *bp;
{
    if (undoptr == NULL)
	return; 
    if (undoptr != undostart)
    {
        bp->b_ulast = undobefore;
	ublock_clear(undoptr);
	if (undofirst && *undostart!=NULL)
	    (*undostart)->u_type |= UDFIRST;
	bp->b_utop++;
	if (bp->b_utop > UNDOSIZE)
	    bp->b_utop = 0;
	if (bp->b_ubottom == bp->b_utop) {
	    bp->b_ubottom++;
	    if (bp->b_ubottom > UNDOSIZE)
		bp->b_ubottom = 0;
	}
    }
    undoptr = NULL;
}

VOID
undo_clean(bp)
BUFFER *bp;
{
    int i;
    UNDO_DATA **undo = bp->b_ustack;
    for (i=0; i<UNDOSIZE+1; i++)
	ublock_clear(undo++);
    undo_reset(bp);
}

VOID
ublock_clear(upp)
register UNDO_DATA **upp;
{
    register UNDO_DATA *up,*upold;
    
    if (upp == NULL)
	return; 
    up = *upp;
    while (up != NULL)
    {
	if (up->u_size)
	    free(up->u_buffer);
	upold = up;
	up = up->u_next;
	free(upold);
    }
    *upp = NULL;
}

int
undo_balloc(undo, size)
register UNDO_DATA *undo;
register RSIZE size;
{
    char *newbuffer;

    /* For memory save, too big undo buffer is turncate */
    if (undo->u_size<size && (size*4)<undo->u_size)
	undo_bfree(undo);

    if (undo->u_size < size)
    {
        size = (size + UBLOCK - 1) / UBLOCK * UBLOCK;
#ifdef	MALLOCROUND
	MALLOCROUND(size);
#endif
	if (size < undo->u_size)
	{
	    ewprintf("Undo buffer too BIG!");
	    ttwait();
	    undo_clean(curbp);
	    undoptr = NULL;
	    return FALSE;
	}
	
	if (undo->u_size) {
	    free(undo->u_buffer);
	    undo->u_size = 0;
	}
	newbuffer = malloc(size);
	if (newbuffer == NULL)
	{
	    ewprintf("Can't get %ld Bytes / Undo buffer clear", size);
	    ttwait();
	    undo_clean(curbp);
	    undoptr = NULL;
	    return FALSE;
	}
	undo->u_buffer = newbuffer;
	undo->u_size = size;
    }
    return TRUE;
}

int
undo_bgrow(undo, size)
register UNDO_DATA *undo;
RSIZE size;
{
    char *newbuffer;
    RSIZE newsize = (undo->u_used + size);

    if (newsize < size)
    {
	ewprintf("Undo buffer too BIG!");
	ttwait();
	undo_clean(curbp);
	undoptr = NULL;
	return FALSE;
    }
    if (newsize > undo->u_size)
    {
	size = (newsize + UBLOCK - 1) / UBLOCK * UBLOCK;
#ifdef	MALLOCROUND
	MALLOCROUND(size);
#endif
	if (size < newsize)
	    size = newsize;
	newbuffer = malloc(size);
	if (newbuffer == NULL)
	{
	    ewprintf("Can't get %ld Bytes / Undo buffer clear");
	    ttwait();
	    undo_clean(curbp);
	    undoptr = NULL;
	    return FALSE;
	}
	if (undo->u_size) {
	    bcopy(undo->u_buffer, newbuffer, undo->u_used);
	    free(undo->u_buffer);
	}
	undo->u_buffer = newbuffer;
	undo->u_size = size;
    }
    return TRUE;
}

int
do_undo(f, n)
{
    register char *p;
    register int  i;
    register LINE* lp;
    UNDO_DATA *undo,*undoend;
    int firstcheck = FALSE;
    extern int twiddle();

    ewprintf("Undo!");

    if (n < 0)
	return FALSE;

    undoptr = NULL;
    while (n--)
    {
	if (curbp->b_utop == curbp->b_ubottom) {
	    ewprintf("No more undo data");
	    ttbeep();
	    return TRUE;
	}
	curbp->b_utop--;
	if (curbp->b_utop < 0)
	    curbp->b_utop = UNDOSIZE;
	undoend = NULL;
	while (1)
	{
	    undo = curbp->b_ustack[curbp->b_utop];
	    if (undo == undoend)
		break;
	    while (undo->u_next != undoend)
		undo = undo->u_next;
	    undoend = undo;

	    lp = lforw(curbp->b_linep);
	    for (i=undo->u_dotlno; i>0; i--)
	    {
		if (lp == curbp->b_linep)
		    break;
		lp = lforw(lp);
	    }
	    if (i != 0) {
		ewprintf("undo data error : line missing %d:%d",
			 undo->u_dotlno, undo->u_doto);
		undo_reset(curbp);
		return FALSE;
	    }
	    if (llength(lp) < undo->u_doto) {
		ewprintf("undo data error : offset missing %d:%d",
			 undo->u_dotlno, undo->u_doto);
		undo_reset(curbp);
		return FALSE;
	    }	    
	    curwp->w_dotp = lp;
	    curwp->w_doto = undo->u_doto;
	    
	    if (undo->u_type & UDFIRST)
		firstcheck = TRUE;
	    switch (undo->u_type & UDMASK)
	    {
	      case UDDEL:
	      case UDBS:
		if (undo->u_size)
		{
		    p = undo->u_buffer;
		    for (i=0; i<undo->u_used; i++,p++)
		    {
			if (*p == '\n')
			    lnewline();
			else
			    linsert(1, *p);
		    }
		}
		else
		{
		    p = undo->u_code;
		    if (*p == '\n')
			lnewline();
		    else
		    {
			linsert(1, *p++);
			if (*p)
			    linsert(1, *p);
		    }
		}
		if ((undo->u_type&UDMASK) == UDDEL)
		{
		    lp = lforw(curbp->b_linep);
		    for (i=undo->u_dotlno; i>0; i--)
		    {
			if (lp == curbp->b_linep)
			    break;
			lp = lforw(lp);
		    }
		    curwp->w_dotp = lp;
		    curwp->w_doto = undo->u_doto;
		}
		break;
#if	UDINSNL != UDINS		
	      case UDINSNL:
#endif
	      case UDINS:
		ldelete(undo->u_used, KNONE);
		break;

	      case UDTWIDDLE:
		twiddle(FFRAND, 1);
		curwp->w_dotp = lp;
		curwp->w_doto = undo->u_doto;
		break;
		
	      case UDOVER:
		i = 0;
		while (curwp->w_doto < llength(lp) && i < undo->u_used)
		    lputc(lp, curwp->w_doto++, undo->u_buffer[i++]);
		if (i != undo->u_used)
		{
		    ewprintf("do_undo: overwrite data error");
		    return FALSE;
		}
		if (undo->u_code[0]) {
		    if (curwp->w_doto >= llength(lp))
		    {
			ewprintf("do_undo: overwrite data error");
			undo_reset(curbp);
			return FALSE;
		    }
		    lputc(lp, curwp->w_doto, undo->u_code[0]);
		}
		curwp->w_doto = undo->u_doto;
		lchange(WFEDIT);
		break;
		
	      case UDREPL:
		curwp->w_doto += undo->u_used;
		if (llength(lp) < curwp->w_doto)
		{
		    ewprintf("do_undo: replace data error");
		    undo_reset(curbp);
		    return FALSE;
		}
		lreplace(undo->u_used, undo->u_buffer, TRUE);
		break;

	      case UDNONE:
	      default:
		panic("bug: do_undo");
	    }
	}
    }
    if (firstcheck) {
	WINDOW *wp;
	curbp->b_flag &= ~BFCHG;
	for (wp = wheadp; wp != NULL; wp = wp->w_wndp) {
	    if (wp->w_bufp == curbp) {
		wp->w_flag |= WFMODE;
		if(wp != curwp) wp->w_flag |= WFHARD;
	    }
	}
    }

    return TRUE;
}
#endif
