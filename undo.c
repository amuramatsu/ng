/* $Id: undo.c,v 1.2 2000/06/27 01:49:45 amura Exp $ */
/*
 *		Undo support routine.
 * The functions in this file
 * are a general set of undo support utilities.
 */

/*
 * $Log: undo.c,v $
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

UNDO_DATA **undoptr;
UNDO_DATA **undobefore;
static int undofirst;

VOID
ublock_open(bp)
register BUFFER *bp;
{
    if (undoptr != NULL)
	panic("bug: ublock openning error");
    undoptr = &bp->b_ustack[bp->b_utop];
    undobefore = NULL;
    if ((curbp->b_flag&BFCHG) == 0)	/* First change	*/
	undofirst = TRUE;
    else
	undofirst = FALSE;
}

VOID
ublock_close(bp)
register BUFFER *bp;
{
    if (undoptr == NULL) {
	undobefore = NULL;
	return; 
    }

    if (undobefore != NULL)
    {
	ublock_clear(undoptr);
	if (undofirst && *undobefore!=NULL)
	    (*undobefore)->u_type |= UDFIRST;
	bp->b_utop++;
	if (bp->b_utop > UNDOSIZE)
	    bp->b_utop = 0;
	if (bp->b_ubottom == bp->b_utop) {
	    bp->b_ubottom++;
	    if (bp->b_ubottom > UNDOSIZE)
		bp->b_ubottom = 0;
	}
    }
    undoptr = undobefore = NULL;
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

VOID
undo_balloc(undo, size)
register UNDO_DATA *undo;
register int size;
{
    char *newbuffer;
    if (undo->u_size < size)
    {
#ifdef	MALLOCROUND
	MALLOCROUND(size);
#endif
	newbuffer = malloc(size);
	if (newbuffer == NULL)
	{
	    ewprintf("undo_balloc: No Memory");
	    ttwait();
	    undo_clean(curbp);
	    undoptr = NULL;
	    return;
	}
	if (undo->u_size)
	    free(undo->u_buffer);
	undo->u_buffer = newbuffer;
	undo->u_size = size;
    }
}

VOID
undo_bgrow(undo, size)
register UNDO_DATA *undo;
{
    char *newbuffer;
    int newsize = undo->u_used + size;

    if (newsize > undo->u_size)
    {
#ifdef	MALLOCROUND
	MALLOCROUND(newsize);
#endif
	newbuffer = malloc(newsize);
	if (newbuffer == NULL)
	{
	    ewprintf("undo_bgrow: No Memory");
	    ttwait();
	    undo_clean(curbp);
	    undoptr = NULL;
	    return;
	}
	if (undo->u_size) {
	    bcopy(undo->u_buffer, newbuffer, undo->u_used);
	    free(undo->u_buffer);
	}
	undo->u_buffer = newbuffer;
	undo->u_size = newsize;
    }
}

int
do_undo(f, n)
{
    register char *p;
    register int  i;
    register LINE* lp;
    UNDO_DATA *undo,*undonext;
    int firstcheck = FALSE;

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
	while (1)
	{
	    undo = curbp->b_ustack[curbp->b_utop];
	    undonext = undo->u_next;
	    while (undonext!=NULL && undonext->u_type!=UDNONE)
	    {
		undo = undonext;
		undonext = undo->u_next;
	    }
	    if (undo->u_type == UDNONE)
		break;

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
		
	      case UDINSNL:
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
	    undo->u_type = UDNONE;
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
