/* $Id: file.c,v 1.8 2001/05/25 15:36:52 amura Exp $ */
/*
 *		File commands.
 */

/*
 * $Log: file.c,v $
 * Revision 1.8  2001/05/25 15:36:52  amura
 * now buffers have only one mark (before windows have one mark)
 *
 * Revision 1.7  2001/02/18 19:29:30  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.6  2001/02/18 17:07:25  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.5  2000/12/22 19:54:35  amura
 * fix some bug in filename handling
 *
 * Revision 1.4  2000/12/14 18:06:24  amura
 * filename length become flexible
 *
 * Revision 1.3  2000/06/27 01:49:43  amura
 * import to CVS
 *
 * Revision 1.2  2000/06/01  05:26:51  amura
 * Undo support
 *
 * Revision 1.1  1999/05/19  03:56:19  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#ifdef	UNDO
#include	"undo.h"
#endif

BUFFER	*findbuffer();
VOID	makename();
VOID	upmodes();
static	char *itos();

/*
 * insert a file into the current buffer. Real easy - just call the
 * insertfile routine with the file name.
 */
/*ARGSUSED*/
fileinsert(f, n)
{
	register int	s;
	char		fname[NFILEN];

#ifdef	EXTD_DIR
	ensurecwd();
	edefset(curbp->b_cwd);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
	if ((s=eread("Insert file: ", fname, NFILEN, EFNEW|EFFILE|EFCR)) != TRUE)
#else	/* NO_FILECOMP */
	if ((s=ereply("Insert file: ", fname, NFILEN)) != TRUE)
#endif	/* NO_FILECOMP */
		return (s);
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	} else
#endif	/* READONLY */
	return insertfile(adjustname(fname), (char *) NULL);
						/* don't set buffer name */
}

/*
 * fileopen is a combined routine of filevisit, filereadonly and
 * poptofile.
 */
static
fileopen(f, n, readonly, popup, prompt)
int f, n, readonly, popup;
char *prompt;
{
	register BUFFER *bp;
	register WINDOW *wp;
	int	s;
	char	fname[NFILEN];
	char	*adjf;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	int	saved_kexpect;
	extern	int	global_kexpect;	/* Defined at kanjic.	*/
#endif	/* KANJI */

#ifdef	EXTD_DIR
	ensurecwd();
	edefset(curbp->b_cwd);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
	if ((s=eread(prompt, fname, NFILEN, EFNEW|EFFILE|EFCR)) != TRUE)
#else	/* NO_FILECOMP */
	if ((s=ereply(prompt, fname, NFILEN)) != TRUE)
#endif	/* NO_FILECOMP */
		return s;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	saved_kexpect = global_kexpect;
	if (f & FFARG) {
		if (n < 0) {
			global_kexpect = NIL;
		} else if (n == 4) {
			global_kexpect = T;
		} else if (n >= 0 && n <= 3) {
			global_kexpect = n;
		} else {
			ewprintf("Invalid argument %d", n);
			return FALSE;
		}
	}
#endif	/* KANJI */
	adjf = adjustname(fname);
#ifndef NO_DIRED	/* 91.01.15  by K.Maeda */
			/* 91.01.16  by S.Yoshida */
	if (ffisdir(adjf)) {
		eargset(adjf);
		return dired(f, n);
	}
#endif	/* NO_DIRED */
	if ((bp = findbuffer(adjf)) == NULL) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		global_kexpect = saved_kexpect;
#endif	/* KANJI */
		return FALSE;
	}
	if (popup) {
	  if ((wp = popbuf(bp)) == NULL) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	    global_kexpect = saved_kexpect;
#endif	/* KANJI */
	    return FALSE;
	  }
	  curbp = bp;
	  curwp = wp;
	}
	else {
	  curbp = bp;
	  if (showbuffer(bp, curwp, WFHARD) != TRUE) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		global_kexpect = saved_kexpect;
#endif	/* KANJI */
		return FALSE;
	  }
	}
	if (bp->b_fname == NULL) {
		s = readin(adjf);		/* Read it in.		*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
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
#endif	/* READONLY */
  		return (s);
  	}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	global_kexpect = saved_kexpect;
#endif	/* KANJI */

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
filevisit(f, n)
{
  return fileopen(f, n, FALSE, FALSE, "Find file: ");
}


/*
 * Pop to a file in the other window. Same as last function, just
 * popbuf instead of showbuffer.
 */
/*ARGSUSED*/
poptofile(f, n)
{
  return fileopen(f, n, FALSE, TRUE, "Find file in other window: ");
}

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
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
filereadonly(f, n)
{
  return fileopen(f, n, TRUE, FALSE, "Find file read-only: ");
}
#endif	/* READONLY */

int
filealternate(f, n)
int f, n;
{
  int	s;
  char fname[NFILEN], *prompt = "Find alternate file: ";

#ifdef	EXTD_DIR
  ensurecwd();
  edefset(curbp->b_cwd);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
  s = eread(prompt, fname, NFILEN, EFNEW | EFFILE | EFCR);
#else	/* NO_FILECOMP */
  s = ereply(prompt, fname, NFILEN);
#endif	/* NO_FILECOMP */
  if (s != TRUE) {
    return FALSE;
  }
  if (curbp) {
    eargset(curbp->b_bname);
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
	char	bname[NBUFN], *cp;
	unsigned count = 1;

	for (bp=bheadp; bp!=NULL; bp=bp->b_bufp) {
		if (bp->b_fname!=NULL && (fncmp(bp->b_fname, fname)==0))
			return bp;
	}
	makename(bname, fname);			/* New buffer name.	*/
	cp = bname + strlen(bname);
	while(bfind(bname, FALSE) != NULL) {
		*cp = '<';		/* add "<count>" to then name	*/
		(VOID) strcpy(itos(cp, ++count)+1, ">");
	}
	return bfind(bname, TRUE);
}

/*
 * Put the decimal representation of num into a buffer.  Hacked to be
 * faster, smaller, and less general.
 */
static char *itos(bufp, num)
char *bufp;
unsigned num;
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
readin(fname) char *fname; {
	register int		status;
	register WINDOW		*wp;
#ifdef C_MODE
	extern	int	flag_use_c_mode;
#endif

	if (bclear(curbp) != TRUE)		/* Might be old.	*/
		return TRUE;
	status = insertfile(fname, fname) ;
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
	curbp->b_flag &= ~(BFCHG|BFACHG);	/* No change.		*/
#else
	curbp->b_flag &= ~BFCHG;		/* No change.		*/
#endif	/* AUTOSAVE	*/
	for (wp=wheadp; wp!=NULL; wp=wp->w_wndp) {
		if (wp->w_bufp == curbp) {
			wp->w_dotp  = wp->w_linep = lforw(curbp->b_linep);
			wp->w_lines = 0;
			wp->w_doto  = 0;
		}
	}
#ifdef	C_MODE	/* 91.01.13  by S.Yoshida */
	if (flag_use_c_mode) {
	    char *ptr;
	    ptr = fname + strlen(fname);
	    while (ptr > fname)
	    {
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
		   )
	      {
		  cmode(0, 1);	/* Change into C-mode. */
	      }
	  }
	}
#endif	/* C_MODE */
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
insertfile(fname, newname) char fname[], newname[]; {
	register LINE	*lp1 = (LINE *)NULL;
	register LINE	*lp2;
	register WINDOW *wp;
	int		nbytes;
	LINE		*olp;			/* Line we started at */
	int		opos;			/* and offset into it */
	int		s, nline;
	BUFFER		*bp;
	char		line[NLINE];
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
	int	leng;
#endif  /* HANKANA */

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
#ifdef	EXTD_DIR
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
		else	ewprintf("(File not found)");
		goto out;
	}
	opos = curwp->w_doto;
	/* Open a new line, at point, and start inserting after it */
	(VOID) lnewline();
	olp = lback(curwp->w_dotp);
	if(olp == curbp->b_linep) {
		/* if at end of buffer, create a line to insert before */
		(VOID) lnewline();
		curwp->w_dotp = lback(curwp->w_dotp);
	}
	nline = 0;			/* Don't count fake line at end */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	ksetfincode(bp);
#endif	/* KANJI */
	while ((s=ffgetline(line, NLINE, &nbytes)) != FIOERR) {
	    switch(s) {
	    case FIOSUC:
		++nline;
		/* and continue */
	    case FIOEOF:	/* the last line of the file		*/
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
		leng = kcodecount(line, nbytes);
		if ((lp1=lalloc(leng > nbytes ? leng : nbytes)) == NULL) {
			s = FIOERR;		/* Keep message on the	*/
			goto endoffile;		/* display.		*/
		}
#else  /* Not HANKANA */
		if ((lp1=lalloc(nbytes)) == NULL) {
			s = FIOERR;		/* Keep message on the	*/
			goto endoffile;		/* display.		*/
		}
#endif  /* HANKANA */
		bcopy(line, &ltext(lp1)[0], nbytes);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */

		if ((lp1->l_used = kcodeconv(ltext(lp1), nbytes, bp)) < 0) {
		    s = FIOERR;
		    goto endoffile;
		}
#endif	/* KANJI */
lineread:
		lp2 = lback(curwp->w_dotp);
		lp2->l_fp = lp1;
		lp1->l_fp = curwp->w_dotp;
		lp1->l_bp = lp2;
		curwp->w_dotp->l_bp = lp1;
		if(s==FIOEOF) goto endoffile;
		break;
	    case FIOLONG: {	/* a line to long to fit in our buffer	*/
		    char *cp;
		    char *cp2 = (char *)NULL;
		    int	 i;

		    nbytes = 0;
		    for(;;) {
			if((cp = malloc((unsigned)(nbytes + NLINE))) == NULL) {
			    ewprintf("Could not allocate %d bytes",
				    nbytes + NLINE);
			    s = FIOERR;
			    if(nbytes) {
			      free(cp2);
			      cp2 = (char *)NULL;
			    }
			    goto endoffile;
			}
			if(nbytes) {
			    bcopy(cp2, cp, nbytes);
			    free(cp2);
			    cp2 = (char *)NULL;
			}
			bcopy(line, cp+nbytes, NLINE);
			nbytes += NLINE;
			switch(s = ffgetline(line, NLINE, &i)) {
			    case FIOERR:
				free(cp);
				goto endoffile;
			    case FIOLONG:
				cp2 = cp;
				break;
			    case FIOEOF:
			    case FIOSUC:
				cp2 = cp;
				if((cp = malloc((unsigned)( nbytes + i ))) 
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
#ifdef HANKANA /* 92.11.21  by S.Sasaki */
				leng = kcodecount(cp, nbytes+i);
				if((lp1=lalloc(
				    leng > nbytes+i ? leng : nbytes+i
							)) == NULL) {
				    s = FIOERR;
				    free(cp);
				    goto endoffile;
				}
				bcopy(cp, &ltext(lp1)[0], nbytes + i);

#else  /* not HANKANA */
				if((lp1=lalloc(nbytes+i)) == NULL) {
				    s = FIOERR;
				    free(cp);
				    goto endoffile;
				}
				bcopy(cp, &ltext(lp1)[0], llength(lp1));
#endif  /* HANKANA */
				free(cp);
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
				if ( (lp1->l_used = kcodeconv(ltext(lp1),
						nbytes + i, bp)) <0 ) {
				    s =FIOERR;
				    goto endoffile;
				}
#endif	/* KANJI */
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
	if (s==FIOEOF) {			/* Don't zap an error.	*/
		if (nline == 1) ewprintf("(Read 1 line)");
		else		ewprintf("(Read %d lines)", nline);
	}
	/* Set mark at the end of the text */
	curwp->w_dotp = bp->b_markp = lback(curwp->w_dotp);
	bp->b_marko = llength(bp->b_markp);
	(VOID) ldelnewline();
	curwp->w_dotp = olp;
	curwp->w_doto = opos;
	if(olp == curbp->b_linep) curwp->w_dotp = lforw(olp);
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
#ifndef NO_BACKUP
	if (newname != NULL)
		bp->b_flag |= (BFCHG|BFBAK|BFACHG);/* Need a backup.	*/
	else	bp->b_flag |= (BFCHG|BFACHG);
#else
	bp->b_flag |= (BFCHG|BFACHG);
#endif
#else	/* not AUTOSAVE */
#ifndef NO_BACKUP
	if (newname != NULL)
		bp->b_flag |= BFCHG | BFBAK;	/* Need a backup.	*/
	else	bp->b_flag |= BFCHG;
#else
	bp->b_flag |= BFCHG;
#endif
#endif	/* AUTOSAVE */
	/* if the insert was at the end of buffer, set lp1 to the end of
	 * buffer line, and lp2 to the beginning of the newly inserted
	 * text.  (Otherwise lp2 is set to NULL.)  This is
	 * used below to set pointers in other windows correctly if they
	 * are also at the end of buffer.
	 */
	lp1 = bp->b_linep;
	if (bp->b_markp == lp1) {
		lp2 = curwp->w_dotp;
	} else {
		(VOID) ldelnewline();		/* delete extranious newline */
out:		lp2 = NULL;
	}
	for (wp=wheadp; wp!=NULL; wp=wp->w_wndp) {
		if (wp->w_bufp == curbp) {
			wp->w_flag |= WFMODE|WFEDIT;
			if (wp != curwp && lp2 != NULL) {
				if (wp->w_dotp == lp1)	wp->w_dotp  = lp2;
				if (bp->b_markp == lp1) bp->b_markp = lp2;
				if (wp->w_linep == lp1) {
					wp->w_linep = lp2;
					wp->w_lines = 0;
				}
			}
		}
	}
#ifdef	UNDO
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
makename(bname, fname) char bname[]; char fname[]; {
	register char	*cp1;
	register char	*cp2;

	cp1 = &fname[0];
	while (*cp1 != 0)
		++cp1;
	--cp1;			/* insure at least 1 character ! */
	while (cp1!=&fname[0] && cp1[-1]!=BDC1
#ifdef	BDC2
	       && cp1[-1]!=BDC2
#endif
	       )
		--cp1;
	cp2 = &bname[0];
	while (cp2!=&bname[NBUFN-1] && *cp1!=0
#ifdef	BDC3
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
  register char	*cp1, *ecp1, *cp2, *ecp2;

  cp1 = fname;
  while (*cp1 != 0) {
    ++cp1;
  }
  while (fname < cp1 && cp1[-1] != BDC1
#ifdef	BDC2
	 && cp1[-1] != BDC2
#endif
	 ) {
    --cp1;
  }
  ecp1 = (fname < cp1) ? cp1 : fname;
  cp1 = fname;
  cp2 = dname;
  ecp2 = dname + len - 1; /* -1 is for the EOS at the end */
  while (cp2 < ecp2 && cp1 < ecp1) {
    *cp2++ = *cp1++;
  }
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
filewrite(f, n)
{
	register int	s;
	char		fname[NFILEN];
	char		*adjfname;
	char		*newname;

#ifdef	EXTD_DIR
	ensurecwd();
	edefset(curbp->b_cwd);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
	if ((s=eread("Write file: ", fname, NFILEN, EFNEW|EFFILE|EFCR)) != TRUE)
#else	/* NO_FILECOMP */
	if ((s=ereply("Write file: ", fname, NFILEN)) != TRUE)
#endif	/* NO_FILECOMP */
		return (s);
#ifdef	AUTOSAVE	/* 01.01.06 by M.Suzuki	*/
	{
		char aname[NFILEN];
		autosave_name(aname, fname, NFILEN);
		unlink(aname);
	}
#endif	/* AUTOSAVE	*/
	adjfname = adjustname(fname);
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
#ifdef	EXTD_DIR
		if (curbp->b_cwd != NULL)
			free(curbp->b_cwd);
		curbp->b_cwd = NULL;
#endif
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
#ifndef NO_BACKUP
		curbp->b_flag &= ~(BFBAK | BFCHG | BFACHG);
#else
		curbp->b_flag &= ~(BFCHG|BFACHG);
#endif
#else	/* not AUTOSAVE */
#ifndef NO_BACKUP
		curbp->b_flag &= ~(BFBAK | BFCHG);
#else
		curbp->b_flag &= ~BFCHG;
#endif
#endif	/* AUTOSAVE */
#ifdef	BUGFIX	/* 91.01.18  by S.Yoshida */
	    {
		BUFFER		*bp;
		char		bname[NBUFN], *cp;
		unsigned	count = 1;

		makename(bname, adjfname);	/* New buffer name.	*/
		cp = bname + strlen(bname);
		while((bp = bfind(bname, FALSE)) != NULL) {
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
#endif	/* BUGFIX */
		upmodes(curbp);
	}
	return s;
}

/*
 * Save the contents of the current buffer back into
 * its associated file.
 */
#ifndef NO_BACKUP
#ifndef	MAKEBACKUP
#define	MAKEBACKUP TRUE
#endif
static int	makebackup = MAKEBACKUP;
#endif

/*ARGSUSED*/
filesave(f, n)
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
buffsave(bp) BUFFER *bp; {
	register int	s;
#ifndef NO_BACKUP	/* 90.02.14  by S.Yoshida */
#ifdef	BUGFIX		/* 90.02.14  by S.Yoshida */
	register int	m = -1;
	VOID		fsetfilemode();
#endif	/* BUGFIX */
#endif	/* NO_BACKUP */

	if ((bp->b_flag&BFCHG) == 0)	{	/* Return, no changes.	*/
		ewprintf("(No changes need to be saved)");
		return TRUE;
	}
	if (bp->b_fname == NULL) {		/* Must have a name.	*/
		ewprintf("No file name");
		return (FALSE);
	}
#ifndef NO_BACKUP
	if (makebackup && (bp->b_flag&BFBAK)) {
#ifdef	BUGFIX	/* 90.02.14  by S.Yoshida */
		m = fgetfilemode(bp->b_fname);
#endif	/* BUGFIX */
		s = fbackupfile(bp->b_fname);
		if (s == ABORT)			/* Hard error.		*/
			return FALSE;
		if (s == FALSE			/* Softer error.	*/
		&& (s=eyesno("Backup error, save anyway")) != TRUE)
			return s;
	}
#endif
#ifdef	AUTOSAVE	/* 01.01.06 by M.Suzuki	*/
	{
		char aname[NFILEN];
		autosave_name(aname, bp->b_fname, NFILEN);
		unlink(aname);
	}
#endif	/* AUTOSAVE	*/
	if ((s=writeout(bp, bp->b_fname)) == TRUE) {
#ifndef NO_BACKUP
#ifdef	BUGFIX	/* 90.02.14  by S.Yoshida */
		if (m != -1) {
			fsetfilemode(bp->b_fname, m);
		}
#endif	/* BUGFIX */
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
		bp->b_flag &= ~(BFCHG | BFBAK | BFACHG);
#else
		bp->b_flag &= ~(BFCHG | BFBAK);
#endif
#else	/* not NO_BACKUP */
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
		bp->b_flag &= ~(BFCHG | BFACHG);
#else
		bp->b_flag &= ~BFCHG;
#endif
#endif	/* NO_BACKUP */
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
makebkfile(f, n)
{
	if(f & FFARG) makebackup = n > 0;
	else makebackup = !makebackup;
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
writeout(bp, fn) register BUFFER *bp; char *fn; {
	register int	s;

	if (fn == NULL)
		return (FALSE);
	if ((s=ffwopen(fn)) != FIOSUC)		/* Open writes message. */
		return (FALSE);
	s = ffputbuf(bp);
	if (s == FIOSUC) {			/* No write error.	*/
		s = ffclose();
		if (s==FIOSUC)
			ewprintf("Wrote %s", fn);
	} else					/* Ignore close error	*/
		(VOID) ffclose();		/* if a write error.	*/
	return s == FIOSUC;
}

/*
 * Tag all windows for bp (all windows if bp NULL) as needing their
 * mode line updated.
 */
VOID
upmodes(bp) register BUFFER *bp; {
	register WINDOW *wp;

	for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
#ifdef	BUGFIX	/* 91.01.15  by K.Maeda */
		if (bp == NULL || wp->w_bufp == bp) wp->w_flag |= WFMODE;
#else	/* ORIGINAL */
		if (bp == NULL || curwp->w_bufp == bp) wp->w_flag |= WFMODE;
#endif	/* BUGFIX */
}
