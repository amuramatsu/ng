/* dired module for mg 2a	*/
/* by Robert A. Larson		*/

/* $Id: dired.c,v 1.1 1999/05/21 02:45:06 amura Exp $ */

/* $Log: dired.c,v $
/* Revision 1.1  1999/05/21 02:45:06  amura
/* Initial revision
/*
*/

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#ifndef NO_DIRED

BUFFER *dired_();

/*ARGSUSED*/
dired(f, n)
int f, n;
{
    char dirname[NFILEN];
    BUFFER *bp;

    dirname[0] = '\0';
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if(eread("Dired: ", dirname, NFILEN, EFNEW | EFCR | EFFILE) == ABORT)
#else	/* NO_FILECOMP */
    if(eread("Dired: ", dirname, NFILEN, EFNEW | EFCR) == ABORT)
#endif	/* NO_FILECOMP */
	return ABORT;
    if((bp = dired_(dirname)) == NULL) return FALSE;
    curbp = bp;
#ifdef	READONLY	/* 91.01.15  by K.Maeda */
    curbp->b_flag |= BFRONLY;
#endif	/* READONLY */
    return showbuffer(bp, curwp, WFHARD | WFMODE);
}

/*ARGSUSED*/
d_otherwindow(f, n)
int f, n;
{
    char dirname[NFILEN];
    BUFFER *bp;
    WINDOW *wp;

    dirname[0] = '\0';
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if(eread("Dired other window: ", dirname, NFILEN, EFNEW | EFCR | EFFILE) == ABORT)
#else	/* NO_FILECOMP */
    if(eread("Dired other window: ", dirname, NFILEN, EFNEW | EFCR) == ABORT)
#endif	/* NO_FILECOMP */
	return ABORT;
    if((bp = dired_(dirname)) == NULL) return FALSE;
#ifdef	READONLY	/* 91.01.15  by K.Maeda */
    bp->b_flag |= BFRONLY;
#endif	/* READONLY */
    if((wp = popbuf(bp)) == NULL) return FALSE;
    curbp = bp;
    curwp = wp;
    return TRUE;
}

/*ARGSUSED*/
d_del(f, n)
int f, n;
{
    if(n < 0) return FALSE;
    while(n--) {
	if(llength(curwp->w_dotp) > 0)
	    lputc(curwp->w_dotp, 0, 'D');
	if(lforw(curwp->w_dotp) != curbp->b_linep)
	    curwp->w_dotp = lforw(curwp->w_dotp);
    }
    curwp->w_flag |= WFEDIT | WFMOVE;
    curwp->w_doto = 0;
    return TRUE;
}

/*ARGSUSED*/
d_undel(f, n)
int f, n;
{
    if(n < 0) return d_undelbak(f, -n);
    while(n--) {
	if(llength(curwp->w_dotp) > 0)
	    lputc(curwp->w_dotp, 0, ' ');
	if(lforw(curwp->w_dotp) != curbp->b_linep)
	    curwp->w_dotp = lforw(curwp->w_dotp);
    }
    curwp->w_flag |= WFEDIT | WFMOVE;
    curwp->w_doto = 0;
    return TRUE;
}

/*ARGSUSED*/
d_undelbak(f, n)
int f, n;
{
    if(n < 0) return d_undel(f, -n);
    while(n--) {
	if(llength(curwp->w_dotp) > 0)
	    lputc(curwp->w_dotp, 0, ' ');
	if(lback(curwp->w_dotp) != curbp->b_linep)
	    curwp->w_dotp = lback(curwp->w_dotp);
    }
    curwp->w_doto = 0;
    curwp->w_flag |= WFEDIT | WFMOVE;
    return TRUE;
}

/*ARGSUSED*/
d_findfile(f, n)
int f, n;
{
    char fname[NFILEN];
    register BUFFER *bp;
    register int s;
    BUFFER *findbuffer();

    if((s = d_makename(curwp->w_dotp, fname)) == ABORT) return FALSE;
    if ((bp = (s ? dired_(fname) : findbuffer(fname))) == NULL) return FALSE;
#ifdef	READONLY	/* 91.01.16  by S.Yoshida */
    if (s) {			/* If dired buffer,	*/
	bp->b_flag |= BFRONLY;	/* mark as read-only.	*/
    }
#endif	/* READONLY */
    curbp = bp;
    if (showbuffer(bp, curwp, WFHARD) != TRUE) return FALSE;
    if (bp->b_fname[0] != 0) return TRUE;
#ifdef	READONLY	/* 91.01.16  by S.Yoshida */
    s = readin(fname);
    if (fchkreadonly(bp->b_fname)) { /* If no write permission, */
	    bp->b_flag |= BFRONLY;	 /* mark as read-only.      */
	    ewprintf("File is write protected");
    }
    return s;
#else	/* NOT READONLY */
    return readin(fname);
#endif	/* READONLY */
}

/*ARGSUSED*/
d_ffotherwindow(f, n)
int f, n;
{
    char fname[NFILEN];
    register BUFFER *bp;
    register int s;
    register WINDOW *wp;
    BUFFER *findbuffer();

    if((s = d_makename(curwp->w_dotp, fname)) == ABORT) return FALSE;
    if ((bp = (s ? dired_(fname) : findbuffer(fname))) == NULL) return FALSE;
#ifdef	READONLY	/* 91.01.16  by S.Yoshida */
    if (s) {			/* If dired buffer,	*/
	bp->b_flag |= BFRONLY;	/* mark as read-only.	*/
    }
#endif	/* READONLY */
    if ((wp = popbuf(bp)) == NULL) return FALSE;
    curbp = bp;
    curwp = wp;
    if (bp->b_fname[0] != 0) return TRUE;  /* never true for dired buffers */
#ifdef	READONLY	/* 91.01.16  by S.Yoshida */
    s = readin(fname);
    if (fchkreadonly(bp->b_fname)) { /* If no write permission, */
	    bp->b_flag |= BFRONLY;	 /* mark as read-only.      */
	    ewprintf("File is write protected");
    }
    return s;
#else	/* NOT READONLY */
    return readin(fname);
#endif	/* READONLY */
}

/*ARGSUSED*/
d_expunge(f, n)
int f, n;
{
    register LINE *lp, *nlp;
    char fname[NFILEN];
    VOID lfree();

    for(lp = lforw(curbp->b_linep); lp != curbp->b_linep; lp = nlp) {
	nlp = lforw(lp);
	if(llength(lp) && lgetc(lp, 0) == 'D') {
	    switch(d_makename(lp, fname)) {
		case ABORT:
		    ewprintf("Bad line in dired buffer");
		    return FALSE;
		case FALSE:
		    if(unlink(fname) < 0) {
			ewprintf("Could not delete '%s'", fname);
			return FALSE;
		    }
		    break;
		case TRUE:
		    if(unlinkdir(fname) < 0) {
			ewprintf("Could not delete directory '%s'", fname);
			return FALSE;
		    }
		    break;
	    }
	    lfree(lp);
	    curwp->w_flag |= WFHARD;
	}
    }
    return TRUE;
}

/*ARGSUSED*/
d_copy(f, n)
int f, n;
{
    char frname[NFILEN], toname[NFILEN];
    int stat;

    if(d_makename(curwp->w_dotp, frname) != FALSE) {
	ewprintf("Not a file");
	return FALSE;
    }
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if((stat = eread("Copy %s to: ", toname, NFILEN, EFNEW | EFCR | EFFILE, frname))
#else	/* NO_FILECOMP */
    if((stat = eread("Copy %s to: ", toname, NFILEN, EFNEW | EFCR, frname))
#endif	/* NO_FILECOMP */
	!= TRUE) return stat;
    return copy(frname, toname) >= 0;
}

/*ARGSUSED*/
d_rename(f, n)
int f, n;
{
    char frname[NFILEN], toname[NFILEN];
    int stat;

    if(d_makename(curwp->w_dotp, frname) != FALSE) {
	ewprintf("Not a file");
	return FALSE;
    }
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if((stat = eread("Rename %s to: ", toname, NFILEN, EFNEW | EFCR | EFFILE, frname))
#else	/* NO_FILECOMP */
    if((stat = eread("Rename %s to: ", toname, NFILEN, EFNEW | EFCR, frname))
#endif	/* NO_FILECOMP */
	!= TRUE) return stat;
    return rename(frname, toname) >= 0;
}
#endif


