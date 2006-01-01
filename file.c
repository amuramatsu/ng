/* $Id: file.c,v 1.15.2.4 2006/01/01 18:34:13 amura Exp $ */
/*
 *		File commands.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "file.h"

#include "i_buffer.h"
#include "i_window.h"
#include "fileio.h"
#include "undo.h"
#include "dir.h"
#include "dired.h"
#include "echo.h"
#include "line.h"
#include "buffer.h"
#include "modes.h"
#include "autosave.h"

static char *itos _PRO((char *, unsigned int));
static int fileopen_backend _PRO((int, int, int, int, char *));

/*
 * insert a file into the current buffer. Real easy - just call the
 * insertfile routine with the file name.
 */
/*ARGSUSED*/
int
fileinsert(f, n)
int f, n;
{
    register int s;
    NG_WCHAR_t fname[NFILEN];
    char *tmp;
    
#ifdef EXTD_DIR
    NG_WCHAR_t *wtmp;
    ensurecwd();
    LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
		       curbp->b_cwd, wtmp);
    edefset(wtmp);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if ((s=eread("Insert file: ", fname, NG_WCHARLEN(fname),
		 EFNEW|EFFILE|EFCR)) != TRUE)
#else /* NO_FILECOMP */
    if ((s=ereply("Insert file: ", fname, NG_WCHARLEN(fname))) != TRUE)
#endif /* NO_FILECOMP */
	return (s);
#ifdef READONLY	/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();			/* do only displaying warning.	*/
	return TRUE;
    }
#endif /* READONLY */
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
			fname, tmp);
    return insertfile(adjustname(tmp), (char *) NULL);
					/* don't set buffer name */
}

/*
 * fileopen is a combined routine of filevisit, filereadonly and
 * poptofile.
 */
static int
fileopen(f, n, readonly, popup, prompt)
int f, n, readonly, popup;
const char *prompt;
{
    int s;
    NG_WCHAR_t wfname[NFILEN];
    char *fname;
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    int saved_kexpect;
    extern int global_kexpect;	/* Defined at kanjic.	*/
#endif /* KANJI */
    
#ifdef EXTD_DIR
    NG_WCHAR_t *wtmp;
    ensurecwd();
    LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, curbp->b_cwd, wtmp);
    if (wtmp == NULL)
	return FALSE;
    edefset(wtmp);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if ((s=eread(prompt, wfname, NG_WCHARLEN(wfname), EFNEW|EFFILE|EFCR))
	!= TRUE)
#else /* NO_FILECOMP */
    if ((s=ereply(prompt, wfname, NG_WCHARLEN(wfname))) != TRUE)
#endif /* NO_FILECOMP */
	return s;
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    saved_kexpect = global_kexpect;
    if (f & FFARG) {
	if (n < 0) {
	    global_kexpect = NIL;
	}
	else if (n == NIL) {
	    global_kexpect = _T_;
	}
	else if (n >= 0 && n < NIL) {
	    global_kexpect = n;
	}
	else {
	    ewprintf("Invalid argument %d", n);
	    return FALSE;
	}
    }
#endif /* KANJI */
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, wfname, fname);
    return fileopen_backend(f, n, readonly, popup, fname);
}

static int
fileopen_backend(f, n, readonly, popup, fname)
int f, n, readonly, popup;
char *fname;
{
    int s;
    const char *adjf;
    register BUFFER *bp;
    register WINDOW *wp;

    adjf = adjustname(fname);
#ifndef NO_DIRED	/* 91.01.15  by K.Maeda *//* 91.01.16  by S.Yoshida */
    if (ffisdir(adjf)) {
	NG_WCHAR_t *wtmp;
	LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, adjf, wtmp);
	eargset(wtmp);
	return dired(f, n);
    }
#endif /* NO_DIRED */
    if ((bp = findbuffer(adjf)) == NULL) {
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
	global_kexpect = saved_kexpect;
#endif /* KANJI */
	return FALSE;
    }
    if (popup) {
	if ((wp = popbuf(bp)) == NULL) {
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
	    global_kexpect = saved_kexpect;
#endif /* KANJI */
	    return FALSE;
	}
	curbp = bp;
	curwp = wp;
    }
    else {
	curbp = bp;
	if (showbuffer(bp, curwp, WFHARD) != TRUE) {
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
	    global_kexpect = saved_kexpect;
#endif /* KANJI */
	    return FALSE;
	}
    }
    if (bp->b_fname == NULL) {
	s = readin(adjf);		/* Read it in.		*/
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
	global_kexpect = saved_kexpect;
#endif
#ifdef READONLY
	if (readonly) {
	    bp->b_flag |= BFRONLY;	/* Mark as read-only	*/
	}
	else if (bp->b_fname != NULL) {
	    if (fchkreadonly(bp->b_fname)) {/* If no write permission, */
		bp->b_flag |= BFRONLY;	  /* mark as read-only.      */
		ewprintf("File is write protected");
	    }
	}
#endif /* READONLY */
	return (s);
    }
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    global_kexpect = saved_kexpect;
#endif /* KANJI */
    
#ifdef READONLY
    if (readonly) {
	bp->b_flag |= BFRONLY; /* Mark as read-only	*/
	wp = wheadp;           /* Update mode lines.	*/
	while (wp != NULL) {
	    if (wp->w_bufp == bp) {
		wp->w_flag |= WFMODE;
	    }
	    wp = wp->w_wndp;
	}
    }
#endif
    return TRUE;
}

/*
 * Select a file for editing.
 * Look around to see if you can find the
 * file in another buffer; if you can find it
 * just switch to the buffer. If you cannot find
 * the file, create a new buffer, read in the
 * text, and switch to the new buffer.
 */
/*ARGSUSED*/
int
filevisit(f, n)
int f, n;
{
    return fileopen(f, n, FALSE, FALSE, "Find file: ");
}

/*
 * Select a file for editing.
 * Look around to see if you can find the
 * file in another buffer; if you can find it
 * just switch to the buffer. If you cannot find
 * the file, create a new buffer, read in the
 * text, and switch to the new buffer.
 */
/*ARGSUSED*/
int
filevisit_(fname, f, n)
const char *fname;
int f, n;
{
    char *tmp;
    tmp = (char *)alloca(strlen(fname) + 1);
    strcpy(tmp, fname);
    return fileopen_backend(f, n, FALSE, FALSE, tmp);
}

/*
 * Pop to a file in the other window. Same as last function, just
 * popbuf instead of showbuffer.
 */
/*ARGSUSED*/
int
poptofile(f, n)
int f, n;
{
    return fileopen(f, n, FALSE, TRUE, "Find file in other window: ");
}

#ifdef READONLY	/* 91.01.05  by S.Yoshida */
/*
 * Select a file for viewing.
 * Look around to see if you can find the
 * fine in another buffer; if you can find it
 * just switch to the buffer. If you cannot find
 * the file, create a new buffer, read in the
 * text, and switch to the new buffer.
 * This function is based on filevisit().
 */
/*ARGSUSED*/
int
filereadonly(f, n)
int f, n;
{
    return fileopen(f, n, TRUE, FALSE, "Find file read-only: ");
}
#endif /* READONLY */

int
filealternate(f, n)
int f, n;
{
    int	s;
    NG_WCHAR_t fname[NFILEN];
    NG_WCHAR_t *wtmp;
    const char *prompt = "Find alternate file: ";
    
#ifdef EXTD_DIR
    ensurecwd();
    LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
		       curbp->b_cwd, wtmp);
    edefset(wtmp);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    s = eread(prompt, fname, NFILEN, EFNEW | EFFILE | EFCR);
#else /* NO_FILECOMP */
    s = ereply(prompt, fname, NFILEN);
#endif /* NO_FILECOMP */
    if (s != TRUE) {
	return FALSE;
    }
    if (curbp) {
	LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
			   curbp->b_bname, wtmp);
	eargset(wtmp);
	if (killbuffer(0, 1)) {
	    eargset(fname);
	    return filevisit(f, n);
	}
    }
    return FALSE;
}

/*
 * given a file name, either find the buffer it uses, or create a new
 * empty buffer to put it in.
 */
BUFFER *
findbuffer(fname)
char *fname;
{
    register BUFFER *bp;
    char bname[NBUFN], *cp;
    unsigned count = 1;
    
    for (bp=bheadp; bp!=NULL; bp=bp->b_bufp) {
	if (bp->b_fname!=NULL && (fncmp(bp->b_fname, fname)==0))
	    return bp;
    }
    makename(bname, fname);			/* New buffer name.	*/
    cp = bname + strlen(bname);
    while (bfind(bname, FALSE) != NULL) {
	*cp = '<';		/* add "<count>" to then name	*/
	(VOID) strcpy(itos(cp, ++count)+1, ">");
    }
    return bfind(bname, TRUE);
}

/*
 * Put the decimal representation of num into a buffer.  Hacked to be
 * faster, smaller, and less general.
 */
static char *
itos(bufp, num)
char *bufp;
unsigned int num;
{
    if (num >= 10) {
	bufp = itos(bufp, num/10);
	num %= 10;
    }
    *++bufp = '0' + num;
    return bufp;
}

/*
 * Read the file "fname" into the current buffer.
 * Make all of the text in the buffer go away, after checking
 * for unsaved changes. This is called by the "read" command, the
 * "visit" command, and the mainline (for "uemacs file").
 */
int
readin(fname)
const char *fname;
{
    register int status;
    register WINDOW *wp;
#ifdef C_MODE
    extern int flag_use_c_mode;
#endif
    
    if (bclear(curbp) != TRUE)		/* Might be old.	*/
	return TRUE;
    status = insertfile(fname, fname) ;
#ifdef AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
    curbp->b_flag &= ~(BFCHG|BFACHG);	/* No change.		*/
#else
    curbp->b_flag &= ~BFCHG;		/* No change.		*/
#endif /* AUTOSAVE	*/
    for (wp=wheadp; wp!=NULL; wp=wp->w_wndp) {
	if (wp->w_bufp == curbp) {
	    wp->w_dotp  = wp->w_linep = lforw(curbp->b_linep);
	    wp->w_lines = 0;
	    wp->w_doto  = 0;
	}
    }
#ifdef C_MODE	/* 91.01.13  by S.Yoshida */
    if (flag_use_c_mode) {
	const char *ptr;
	ptr = fname + strlen(fname);
	while (ptr > fname) {
	    if (*(--ptr) == '.')
		break;
	}
	
	if (ptr > fname) {
	    ptr++;
	    if (
(ptr[1]=='\0' && (ptr[0]=='c' || ptr[0] =='h' || ptr[0]=='C' || ptr[0]=='m'))
||(ptr[2]=='\0' && ptr[0]=='c' && ptr[1]=='c')
||(ptr[3]=='\0' && (ptr[0]=='c'||ptr[0]=='h') &&
   ptr[1]==ptr[2] && (ptr[1]=='p'||ptr[1]=='x'))
	       ) {
		cmode(0, 1);	/* Change into C-mode. */
	    }
	}
    }
#endif /* C_MODE */
    return status;
}
/*
 * insert a file in the current buffer, after dot. Set mark
 * at the end of the text inserted, point at the beginning.
 * Return a standard status. Print a summary (lines read,
 * error message) out as well. If the
 * BACKUP conditional is set, then this routine also does the read
 * end of backup processing. The BFBAK flag, if set in a buffer,
 * says that a backup should be taken. It is set when a file is
 * read in, but not on a new file (you don't need to make a backup
 * copy of nothing).
 */
int
insertfile(fname, newname)
const char *fname, *newname;
{
    register LINE *lp1 = (LINE *)NULL;
    register LINE *lp2;
    register WINDOW *wp;
    int nbytes;
    LINE *olp;			/* Line we started at */
    int opos;			/* and offset into it */
    int s, nline;
    BUFFER *bp;
    char line[NLINE];
#if defined(SS_SUPPORT)||defined(USE_UNICODE)
    int leng;
#endif  /* SS_SUPPORT || USE_UNICODE */
    
    bp = curbp;				/* Cheap.		*/
    if (newname != (char *) NULL) {
	if (bp->b_fname != NULL)
	    free(bp->b_fname);
	if ((bp->b_fname=malloc(strlen(newname)+1)) == NULL) {
	    ewprintf("Could not allocate %d bytes",
		     strlen(newname) + 1);
	    return FALSE;
	}
	(VOID) strcpy(bp->b_fname, newname);
#ifdef EXTD_DIR
	if (bp->b_cwd != NULL)
	    free(bp->b_cwd);
	bp->b_cwd = NULL;
#endif 	
    }
    if ((s=ffropen(fname)) == FIOERR)	/* Hard file open.	*/
	goto out;
    if (s == FIOFNF) {			/* File not found.	*/
	if (newname != NULL)
	    ewprintf("(New file)");
	else
	    ewprintf("(File not found)");
	goto out;
    }
    opos = curwp->w_doto;
    /* Open a new line, at point, and start inserting after it */
    (VOID) lnewline();
    olp = lback(curwp->w_dotp);
    if (olp == curbp->b_linep) {
	/* if at end of buffer, create a line to insert before */
	(VOID) lnewline();
	curwp->w_dotp = lback(curwp->w_dotp);
    }
    nline = 0;			/* Don't count fake line at end */
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    ksetfincode(bp);
#endif /* KANJI */
    while ((s=ffgetline(line, NLINE, &nbytes)) != FIOERR) {
	switch(s) {
	case FIOSUC:
	    ++nline;
	    /*FALLTHRU*/
	case FIOEOF:	/* the last line of the file		*/
#if defined(SS_SUPPORT)||defined(USE_UNICODE)
	    leng = kcodecount(line, nbytes);
	    if ((lp1=lalloc(leng > nbytes ? leng : nbytes)) == NULL) {
		s = FIOERR;		/* Keep message on the	*/
		goto endoffile;		/* display.		*/
	    }
#else  /* Not SS_SUPPORT ||USE_UNICODE */
	    if ((lp1=lalloc(nbytes)) == NULL) {
		s = FIOERR;		/* Keep message on the	*/
		goto endoffile;		/* display.		*/
	    }
#endif  /* SS_SUPPORT || USE_UNICODE */
	    bcopy(line, &ltext(lp1)[0], nbytes);
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
	    if ((lp1->l_used = kcodeconv(ltext(lp1), nbytes, bp, leng)) < 0) {
		s = FIOERR;
		goto endoffile;
	    }
#endif /* KANJI */
    lineread:
	    lp2 = lback(curwp->w_dotp);
	    lp2->l_fp = lp1;
	    lp1->l_fp = curwp->w_dotp;
	    lp1->l_bp = lp2;
	    curwp->w_dotp->l_bp = lp1;
	    if(s == FIOEOF)
		goto endoffile;
	    break;
	case FIOLONG: {	/* a line to long to fit in our buffer	*/
	    char *cp;
	    char *cp2 = (char *)NULL;
	    int	 i;
	    
	    nbytes = 0;
	    for (;;) {
		if ((cp = malloc((unsigned)(nbytes + NLINE))) == NULL) {
		    ewprintf("Could not allocate %d bytes",
			     nbytes + NLINE);
		    s = FIOERR;
		    if (nbytes) {
			free(cp2);
			cp2 = (char *)NULL;
		    }
		    goto endoffile;
		}
		if (nbytes) {
		    bcopy(cp2, cp, nbytes);
		    free(cp2);
		    cp2 = (char *)NULL;
		}
		bcopy(line, cp+nbytes, NLINE);
		nbytes += NLINE;
		switch (s = ffgetline(line, NLINE, &i)) {
		case FIOERR:
		    free(cp);
		    goto endoffile;
		case FIOLONG:
		    cp2 = cp;
		    break;
		case FIOEOF:
		case FIOSUC:
		    cp2 = cp;
		    if ((cp = malloc((unsigned)( nbytes + i ))) 
			== NULL) {
			ewprintf("Could not allocate %d bytes",
				 nbytes + i);
			s = FIOERR;
			free(cp2);
			cp2 = (char *)NULL;
			goto endoffile;
		    }
		    bcopy(cp2, cp, nbytes);
		    bcopy(line, cp+nbytes, i);
		    free(cp2);
		    cp2 = (char *)NULL;
#if defined(SS_SUPPORT)||defined(USE_UNICODE)
		    leng = kcodecount(cp, nbytes+i);
		    if ((lp1=lalloc(leng > nbytes+i ? leng : nbytes+i))
			== NULL) {
			s = FIOERR;
			free(cp);
			goto endoffile;
		    }
		    bcopy(cp, &ltext(lp1)[0], nbytes + i);
		    
#else  /* not SS_SUPPORT || USE_UNICODE */
		    if ((lp1=lalloc(nbytes+i)) == NULL) {
			s = FIOERR;
			free(cp);
			goto endoffile;
		    }
		    bcopy(cp, &ltext(lp1)[0], llength(lp1));
#endif  /* SS_SUPPORT || USE_UNICODE */
		    free(cp);
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
		    if ((lp1->l_used = kcodeconv(ltext(lp1),
						 nbytes + i, bp, leng)) <0 ) {
			s = FIOERR;
			goto endoffile;
		    }
#endif /* KANJI */
		    goto lineread;
		}
	    }
	}
	default:
	    ewprintf("Unknown code %d reading file", s);
	    s = FIOERR;
	    break;
	}
    }
endoffile:
    (VOID) ffclose();			/* Ignore errors.	*/
    if (s == FIOEOF) {			/* Don't zap an error.	*/
	if (nline == 1)
	    ewprintf("(Read 1 line)");
	else
	    ewprintf("(Read %d lines)", nline);
    }
    /* Set mark at the end of the text */
    curwp->w_dotp = bp->b_markp = lback(curwp->w_dotp);
    bp->b_marko = llength(bp->b_markp);
    (VOID) ldelnewline();
    curwp->w_dotp = olp;
    curwp->w_doto = opos;
    if (olp == curbp->b_linep)
	curwp->w_dotp = lforw(olp);
#ifdef AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
#ifndef NO_BACKUP
    if (newname != NULL)
	bp->b_flag |= (BFCHG|BFBAK|BFACHG);/* Need a backup.	*/
    else
	bp->b_flag |= (BFCHG|BFACHG);
#else
    bp->b_flag |= (BFCHG|BFACHG);
#endif
#else /* not AUTOSAVE */
#ifndef NO_BACKUP
    if (newname != NULL)
	bp->b_flag |= BFCHG | BFBAK;	/* Need a backup.	*/
    else
#endif
	bp->b_flag |= BFCHG;
#endif /* AUTOSAVE */
    /* if the insert was at the end of buffer, set lp1 to the end of
     * buffer line, and lp2 to the beginning of the newly inserted
     * text.  (Otherwise lp2 is set to NULL.)  This is
     * used below to set pointers in other windows correctly if they
     * are also at the end of buffer.
     */
    lp1 = bp->b_linep;
    if (bp->b_markp == lp1) {
	lp2 = curwp->w_dotp;
    }
    else {
	(VOID) ldelnewline();		/* delete extranious newline */
out:
	lp2 = NULL;
    }
    for (wp=wheadp; wp!=NULL; wp=wp->w_wndp) {
	if (wp->w_bufp == curbp) {
	    wp->w_flag |= WFMODE|WFEDIT;
	    if (wp != curwp && lp2 != NULL) {
		if (wp->w_dotp == lp1)
		    wp->w_dotp  = lp2;
		if (bp->b_markp == lp1)
		    bp->b_markp = lp2;
		if (wp->w_linep == lp1) {
		    wp->w_linep = lp2;
		    wp->w_lines = 0;
		}
	    }
	}
    }
#ifdef UNDO
    undo_reset(bp);
#endif
    return s != FIOERR;			/* False if error.	*/
}

/*
 * Take a file name, and from it
 * fabricate a buffer name. This routine knows
 * about the syntax of file names on the target system.
 * BDC1		left scan delimiter.
 * BDC2		optional second left scan delimiter.
 * BDC3		optional right scan delimiter.
 */
VOID
makename(bname, fname)
char *bname, *fname;
{
    register char *cp1;
    register char *cp2;
    
    cp1 = &fname[0];
    while (*cp1 != 0)
	++cp1;
    --cp1;			/* insure at least 1 character ! */
    while (cp1!=&fname[0] && cp1[-1]!=BDC1
#ifdef BDC2
	   && cp1[-1]!=BDC2
#endif
	   )
	--cp1;
    cp2 = &bname[0];
    while (cp2!=&bname[NBUFN-1] && *cp1!=0
#ifdef BDC3
	   && *cp1!=BDC3
#endif
	   )
	*cp2++ = *cp1++;
    *cp2 = 0;
}

#ifdef EXTD_DIR
/*
 * Take a file name, and from it
 * fabricate a path name. This routine knows
 * about the syntax of file names on the target system.
 * BDC1		left scan delimiter.
 * BDC2		optional second left scan delimiter.
 * BDC3		optional right scan delimiter.
 *
 * Modified based on makename() and introduced by Tillanosoft, Mar 22, 1999
 */
VOID
makepath(dname, fname, len)
char *dname, *fname;
int len;
{
    register char *cp1, *ecp1, *cp2, *ecp2;
    
    cp1 = fname;
    while (*cp1 != 0)
	++cp1;
    while (fname < cp1 && cp1[-1] != BDC1
#ifdef BDC2
	   && cp1[-1] != BDC2
#endif
	   ) {
	--cp1;
    }
    ecp1 = (fname < cp1) ? cp1 : fname;
    cp1 = fname;
    cp2 = dname;
    ecp2 = dname + len - 1; /* -1 is for the EOS at the end */
    while (cp2 < ecp2 && cp1 < ecp1)
	*cp2++ = *cp1++;
    *cp2 = 0;
}
#endif /* EXTD_DIR */

/*
 * Ask for a file name, and write the
 * contents of the current buffer to that file.
 * Update the remembered file name and clear the
 * buffer changed flag. This handling of file names
 * is different from the earlier versions, and
 * is more compatable with Gosling EMACS than
 * with ITS EMACS.
 */
/*ARGSUSED*/
int
filewrite(f, n)
int f, n;
{
    register int s;
    NG_WCHAR_t fname[NFILEN];
    char *adjfname;
    char *newname;

#ifdef EXTD_DIR
    NG_WCHAR_t *wtmp;
    ensurecwd();
    LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
		       curbp->b_cwd, wtmp);
    edefset(wtmp);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if ((s=eread("Write file: ", fname, NG_WCHARLEN(fname),
		 EFNEW|EFFILE|EFCR)) != TRUE)
#else /* NO_FILECOMP */
    if ((s=ereply("Write file: ", fname, NG_WCHARLEN(fname))) != TRUE)
#endif /* NO_FILECOMP */
	return (s);
#ifdef AUTOSAVE	/* 01.01.06 by M.Suzuki	*/
    {
	char aname[NFILEN];
	char *tmp;
	LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
			    fname, tmp);
	autosave_name(aname, tmp, sizeof(aname));
	unlink(aname);
    }
#endif /* AUTOSAVE	*/
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, fname, adjfname);
    adjfname = adjustname(adjfname);
    if ((s=writeout(curbp, adjfname)) == TRUE) {
	if ((newname=malloc(strlen(adjfname)+1)) == NULL) {
	    ewprintf("Could not allocate %d bytes",
		     strlen(adjfname) + 1);
	    return FALSE;
	}
	(VOID) strcpy(newname, adjfname);
	if (curbp->b_fname != NULL)
	    free(curbp->b_fname);
	curbp->b_fname = newname;
#ifdef EXTD_DIR
	if (curbp->b_cwd != NULL)
	    free(curbp->b_cwd);
	curbp->b_cwd = NULL;
#endif
#ifdef AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
#ifndef NO_BACKUP
	curbp->b_flag &= ~(BFBAK | BFCHG | BFACHG);
#else
	curbp->b_flag &= ~(BFCHG|BFACHG);
#endif
#else /* not AUTOSAVE */
#ifndef NO_BACKUP
	curbp->b_flag &= ~(BFBAK | BFCHG);
#else
	curbp->b_flag &= ~BFCHG;
#endif
#endif /* AUTOSAVE */
	{
	    BUFFER *bp;
	    char bname[NBUFN], *cp;
	    unsigned count = 1;
	    
	    makename(bname, adjfname);	/* New buffer name.	*/
	    cp = bname + strlen(bname);
	    while ((bp = bfind(bname, FALSE)) != NULL) {
		if (bp->b_fname!=NULL &&
		    (fncmp(bp->b_fname, adjfname)==0)) {
		    break;
		}
		*cp = '<';	/* add "<count>" to then name	*/
		(VOID) strcpy(itos(cp, ++count)+1, ">");
	    }
	    if ((cp = malloc((unsigned)(strlen(bname)+1))) != NULL) {
		(VOID) strcpy(cp, bname);
		free(curbp->b_bname);
		curbp->b_bname = cp;
	    }
	}
	upmodes(curbp);
    }
    return s;
}

/*
 * Save the contents of the current buffer back into
 * its associated file.
 */
#ifndef NO_BACKUP
#ifndef MAKEBACKUP
#define	MAKEBACKUP TRUE
#endif
static int	makebackup = MAKEBACKUP;
#endif

/*ARGSUSED*/
int
filesave(f, n)
int f, n;
{
    return buffsave(curbp);
}

/*
 * Save the contents of the buffer argument into its associated file.
 * Do nothing if there have been no changes
 * (is this a bug, or a feature). Error if there is no remembered
 * file name. If this is the first write since the read or visit,
 * then a backup copy of the file is made.
 * Allow user to select whether or not to make backup files
 * by looking at the value of makebackup.
 */
int
buffsave(bp)
BUFFER *bp;
{
    register int s;
#ifndef NO_BACKUP	/* 90.02.14  by S.Yoshida */
    register int m = -1;
#endif /* NO_BACKUP */
    
    if ((bp->b_flag&BFCHG) == 0) {	/* Return, no changes.	*/
	ewprintf("(No changes need to be saved)");
	return TRUE;
    }
    if (bp->b_fname == NULL) {		/* Must have a name.	*/
	ewprintf("No file name");
	return (FALSE);
    }
#ifndef NO_BACKUP
    if (makebackup && (bp->b_flag&BFBAK)) {
	m = fgetfilemode(bp->b_fname);
	s = fbackupfile(bp->b_fname);
	if (s == ABORT)			/* Hard error.		*/
	    return FALSE;
	if (s == FALSE			/* Softer error.	*/
	    && (s=eyesno("Backup error, save anyway")) != TRUE)
	    return s;
    }
#endif
#ifdef AUTOSAVE	/* 01.01.06 by M.Suzuki	*/
    {
	char aname[NFILEN];
	autosave_name(aname, bp->b_fname, NFILEN);
	unlink(aname);
    }
#endif /* AUTOSAVE	*/
    if ((s=writeout(bp, bp->b_fname)) == TRUE) {
#ifndef NO_BACKUP
	if (m != -1)
	    fsetfilemode(bp->b_fname, m);
#ifdef AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
	bp->b_flag &= ~(BFCHG | BFBAK | BFACHG);
#else
	bp->b_flag &= ~(BFCHG | BFBAK);
#endif
#else /* not NO_BACKUP */
#ifdef AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
	bp->b_flag &= ~(BFCHG | BFACHG);
#else
	bp->b_flag &= ~BFCHG;
#endif
#endif /* NO_BACKUP */
	upmodes(bp);
    }
    return s;
}

#ifndef NO_BACKUP
/* Since we don't have variables (we probably should)
 * this is a command processor for changing the value of
 * the make backup flag.  If no argument is given,
 * sets makebackup to true, so backups are made.  If
 * an argument is given, no backup files are made when
 * saving a new version of a file. Only used when BACKUP
 * is #defined.
 */
/*ARGSUSED*/
int
makebkfile(f, n)
int f, n;
{
    if (f & FFARG)
	makebackup = n > 0;
    else
	makebackup = !makebackup;
    ewprintf("Backup files %sabled", makebackup ? "en" : "dis");
    return TRUE;
}
#endif

/*
 * This function performs the details of file
 * writing; writing the file in buffer bp to
 * file fn. Uses the file management routines
 * in the "fileio.c" package. Most of the grief
 * is checking of some sort.
 */
int
writeout(bp, fn)
register BUFFER *bp;
char *fn;
{
    register int s;
    
    if (fn == NULL)
	return (FALSE);
    if ((s=ffwopen(fn)) != FIOSUC)	/* Open writes message. */
	return (FALSE);
    s = ffputbuf(bp);
    if (s == FIOSUC) {			/* No write error.	*/
	s = ffclose();
	if (s == FIOSUC)
	    ewprintf("Wrote %s", fn);
    }
    else				/* Ignore close error	*/
	(VOID) ffclose();		/* if a write error.	*/
    return s == FIOSUC;
}

/*
 * Tag all windows for bp (all windows if bp NULL) as needing their
 * mode line updated.
 */
VOID
upmodes(bp)
register BUFFER *bp;
{
    register WINDOW *wp;
    
    for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
	if (bp == NULL || wp->w_bufp == bp) wp->w_flag |= WFMODE;
}
