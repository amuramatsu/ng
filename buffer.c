/* $Id: buffer.c,v 1.16 2001/05/25 15:36:52 amura Exp $ */
/*
 *		Buffer handling.
 */

/*
 * $Log: buffer.c,v $
 * Revision 1.16  2001/05/25 15:36:52  amura
 * now buffers have only one mark (before windows have one mark)
 *
 * Revision 1.15  2001/05/08 17:52:43  amura
 * display buffer size in bufferlist
 *
 * Revision 1.14  2001/03/02 08:49:04  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.13  2001/02/28 17:06:05  amura
 * buffer size to use eread() is more secure
 *
 * Revision 1.12  2001/02/18 19:26:41  amura
 * remove malloc() prototype
 *
 * Revision 1.11  2001/02/18 17:07:23  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.10  2001/01/05 14:06:59  amura
 * first implementation of Hojo Kanji support
 *
 * -- snip --
 *
 * Revision 1.1  1999/05/19  03:47:59  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	"kbd.h"			/* needed for modes */
#ifdef	UNDO
#include	"undo.h"
#endif

#ifdef  VARIABLE_TAB
int defb_tab = 8;
int cmode_tab = 0;
#endif  /* VARIABLE_TAB */
#define	GETNUMLEN	6
static BUFFER *makelist pro((void));
static long buffersize pro((BUFFER*));

#ifdef	MOVE_BUFFER		/* 95.08.29 by M.Suzuki	*/
/* Move to the next buffer	*/
nextbuffer(f,n)
{
	register BUFFER *bp;

	/* Get buffer to use from user */
	if ((curbp->b_altb == NULL)
	    && ((curbp->b_altb = bfind("*scratch*", TRUE)) == NULL))
		 return FALSE;

	bp = bheadp;
	while (bp != NULL) {
		if (fncmp(curbp->b_bname, bp->b_bname) == 0){
			bp = bp->b_bufp;
			break;
		}
		bp = bp->b_bufp;
	}
	if ( bp == NULL){
		bp = bheadp;
	}

	/* and put it in current window */
	curbp = bp;
	return showbuffer(bp, curwp, WFFORCE|WFHARD);
}

/* Move to the previous buffer	*/
prevbuffer(f,n)
{
	register BUFFER *bp,*bp1;

	if ((curbp->b_altb == NULL)
	    && ((curbp->b_altb = bfind("*scratch*", TRUE)) == NULL))
		return FALSE;

	bp1 = bp = bheadp;
	while (bp != NULL) {
		if (fncmp(curbp->b_bname, bp->b_bname) == 0){
			if( bp == bp1 ){		/* abnomal found */
				while( bp != NULL ){	/* last search */
					bp1 = bp;
					bp = bp->b_bufp;
				}
			}
			break;
		}
		bp1 = bp;
		bp = bp->b_bufp;
	}

	/* and put it in current window */
	curbp = bp1;
	return showbuffer(bp1, curwp, WFFORCE|WFHARD);
}
#endif	/* MOVE_BUFFER */

/*
 * Attach a buffer to a window. The values of dot and mark come
 * from the buffer if the use count is 0. Otherwise, they come
 * from some other window.  *scratch* is the default alternate
 * buffer.
 */
/*ARGSUSED*/
usebuffer(f, n)
{
	register BUFFER *bp;
	register int	s;
	char		bufn[NBUFN];

	/* Get buffer to use from user */
	if ((curbp->b_altb == NULL)
	    && ((curbp->b_altb = bfind("*scratch*", TRUE)) == NULL))
		s=eread("Switch to buffer: ", bufn, sizeof(bufn), EFNEW|EFBUF);
	else
		s=eread("Switch to buffer: (default %s) ", bufn, sizeof(bufn),
			 EFNEW|EFBUF, curbp->b_altb->b_bname);

	if (s == ABORT) return s;
	if (s == FALSE && curbp->b_altb != NULL) bp = curbp->b_altb ;
	else if ((bp=bfind(bufn, TRUE)) == NULL) return FALSE;

	/* and put it in current window */
	curbp = bp;
	return showbuffer(bp, curwp, WFFORCE|WFHARD);
}

/*
 * pop to buffer asked for by the user.
 */
/*ARGSUSED*/
poptobuffer(f, n)
{
	register BUFFER *bp;
	register WINDOW *wp;
	register int	s;
	char		bufn[NBUFN];
	WINDOW	*popbuf();

	/* Get buffer to use from user */
	if ((curbp->b_altb == NULL)
	    && ((curbp->b_altb = bfind("*scratch*", TRUE)) == NULL))
		s=eread("Switch to buffer in other window: ",
			bufn, sizeof(bufn), EFNEW|EFBUF);
	else
		s=eread("Switch to buffer in other window: (default %s) ",
			 bufn, sizeof(bufn), EFNEW|EFBUF, curbp->b_altb->b_bname);
	if (s == ABORT) return s;
	if (s == FALSE && curbp->b_altb != NULL) bp = curbp->b_altb ;
	else if ((bp=bfind(bufn, TRUE)) == NULL) return FALSE;

	/* and put it in a new window */
	if ((wp = popbuf(bp)) == NULL) return FALSE;
	curbp = bp;
	curwp = wp;
	return TRUE;
}

/*
 * Dispose of a buffer, by name.
 * Ask for the name. Look it up (don't get too
 * upset if it isn't there at all!). Clear the buffer (ask
 * if the buffer has been changed). Then free the header
 * line and the buffer header. Bound to "C-X K".
 */
/*ARGSUSED*/
killbuffer(f, n)
{
	register BUFFER *bp;
	register BUFFER *bp1;
	register BUFFER *bp2;
	WINDOW		*wp;
	register int	s;
	char		bufn[NBUFN];

	if ((s=eread("Kill buffer: (default %s) ", bufn, sizeof(bufn),
		     EFNEW|EFBUF, curbp->b_bname)) == ABORT) return (s);
	else if (s == FALSE) bp = curbp;
	else if ((bp=bfind(bufn, FALSE)) == NULL) return FALSE;

	/* find some other buffer to display. try the alternate buffer,
	 * then the first different buffer in the buffer list.	if
	 * there's only one buffer, create buffer *scratch* and make
	 * it the alternate buffer.  return if *scratch* is only buffer
	 */
#ifdef	BUGFIX	/* 90.02.22  by S.Yoshida */
	if ((bp1 = bp->b_altb) == NULL || bp1 == bp) {
#else	/* ORIGINAL */
	if ((bp1 = bp->b_altb) == NULL) {
#endif	/* BUGFIX */
		bp1 = (bp == bheadp) ? bp->b_bufp : bheadp;
		if (bp1 == NULL) {
			/* only one buffer. see if it's *scratch* */
			if (bp == bfind("*scratch*",FALSE))
				return FALSE;
			/* create *scratch* for alternate buffer */
			if ((bp1 = bfind("*scratch*",TRUE)) == NULL)
				return FALSE;
		}
	}
	if (bp->b_fname == NULL) {
	  /* Do not ask for saving
	     if the buffer is not associated with a file.
	     by Tillanosoft */
	  bp->b_flag  &= ~BFCHG;
	}
	if (bclear(bp) != TRUE) return FALSE;
	for (wp = wheadp; bp->b_nwnd > 0; wp = wp->w_wndp) {
	    if (wp->w_bufp == bp) {
		bp2 = bp1->b_altb;		/* save alternate buffer */
		if(showbuffer(bp1, wp, WFMODE|WFFORCE|WFHARD) != FALSE)
			bp1->b_altb = bp2;
		else	bp1 = bp2;
	    }
	}
	if (bp == curbp) curbp = bp1;
	free((char *) bp->b_linep);		/* Release header line. */
	bp2 = NULL;				/* Find the header.	*/
	bp1 = bheadp;
	while (bp1 != bp) {
		if (bp1->b_altb == bp)
			bp1->b_altb = (bp->b_altb == bp1) ? NULL : bp->b_altb;
		bp2 = bp1;
		bp1 = bp1->b_bufp;
	}
	bp1 = bp1->b_bufp;			/* Next one in chain.	*/
	if (bp2 == NULL)			/* Unlink it.		*/
		bheadp = bp1;
	else
		bp2->b_bufp = bp1;
	while (bp1 != NULL) {			/* Finish with altb's	*/
		if (bp1->b_altb == bp)
			bp1->b_altb = (bp->b_altb == bp1) ? NULL : bp->b_altb;
		bp1 = bp1->b_bufp;
	}
	if (bp->b_fname != NULL)		/* Release filename block */
		free(bp->b_fname);
#ifdef	EXTD_DIR
	if (bp->b_cwd != NULL)			/* Release pathname block */
		free(bp->b_cwd);
#endif
	free(bp->b_bname);			/* Release name block	*/
#ifdef	UNDO
	undo_clean(bp);				/* Release undo data	*/
#endif
	free((char *) bp);			/* Release buffer block */
	return TRUE;
}

/*
 * Save some buffers - just call anycb with the arg flag.
 */
/*ARGSUSED*/
savebuffers(f, n)
{
	if (anycb(f) == ABORT) return ABORT;
	return TRUE;
}

/*
 * Display the buffer list. This is done
 * in two parts. The "makelist" routine figures out
 * the text, and puts it in a buffer. "popbuf"
 * then pops the data onto the screen. Bound to
 * "C-X C-B".
 */
/*ARGSUSED*/
listbuffers(f, n)
{
	register BUFFER *bp;
	register WINDOW *wp;
	BUFFER		*makelist();
	WINDOW		*popbuf();

	if ((bp=makelist()) == NULL || (wp=popbuf(bp)) == NULL)
		return FALSE;
	wp->w_dotp = bp->b_dotp;	/* fix up if window already on screen */
	wp->w_doto = bp->b_doto;
#ifdef BUFFER_MODE
	bp->b_modes[0] = name_mode("Buffer Menu");
	if (bp->b_modes[0] == NULL) {
	  bp->b_modes[0] = &map_table[0];
	  ewprintf("Could not find \"Buffer Menu\" mode");
	}
	else {
	  bp->b_nmodes = 0;
	}
#endif
	return TRUE;
}

/*
 * This routine rebuilds the text for the
 * list buffers command. Return TRUE if
 * everything works. Return FALSE if there
 * is an error (if there is no memory).
 */
BUFFER *
makelist() {
	register char	*cp1;
	register char	*cp2;
	register int	c;
	register BUFFER *bp;
	LINE		*lp;
	BUFFER		*blp;
	char		b[6+1];
	char		line[4+NBUFN+7+NFILEN+4];
#ifdef HANKANA
	int nhankana;
#endif

	if ((blp = bfind("*Buffer List*", TRUE)) == NULL) return NULL;
	if (bclear(blp) != TRUE) return NULL;
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
	blp->b_flag &= ~(BFCHG|BFACHG);		/* Blow away old.	*/
#else
  	blp->b_flag &= ~BFCHG;			/* Blow away old.	*/
#endif	/* AUTOSAVE	*/

	(VOID) strcpy(line, " MR Buffer");
	cp1 = line + 10;
	while(cp1 < line + 4 + NBUFN + 1) *cp1++ = ' ';
	(VOID) strcpy(cp1, "Size   File");
	if (addline(blp, line) == FALSE) return NULL;
	(VOID) strcpy(line, " -- ------");
	cp1 = line + 10;
	while(cp1 < line + 4 + NBUFN + 1) *cp1++ = ' ';
	(VOID) strcpy(cp1, "----   ----");
	if (addline(blp, line) == FALSE) return NULL;
	bp = bheadp;				/* For all buffers	*/
	while (bp != NULL) {
		sprintf(line,"%c%c%c %-30.30s %7ld %s",
				(bp == curbp) ? '.' : ' ',
				(bp->b_flag & BFCHG) ? '*' : ' ',
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
				(bp->b_flag & BFRONLY) ? '%' : ' ',
#else
				' ',
#endif
				bp->b_bname, buffersize(bp),
				(bp->b_fname != NULL) ? bp->b_fname : "" );
		if (addline(blp, line) == FALSE)
			return NULL;
		bp = bp->b_bufp;
	}
	blp->b_dotp = lforw(blp->b_linep);	/* put dot at beginning of buffer */
	blp->b_doto = 0;
#ifdef	READONLY	/* 91.02.06  by N.Kamei */
	blp->b_flag |= BFRONLY;
#endif	/* READONLY */
	return blp;				/* All done		*/
}

/*
 * Calculate buffer size
 */
static long
buffersize(bp)
    BUFFER *bp;
{
    long size = 0;
    LINE *line;
    line = bp->b_linep;
    line = lforw(line);
    while (line != bp->b_linep) {
	size += llength(line);
	size++;
	line = lforw(line);
    }
    return size;
}

/*
 * The argument "text" points to
 * a string. Append this line to the
 * buffer. Handcraft the EOL
 * on the end. Return TRUE if it worked and
 * FALSE if you ran out of room.
 */
addline(bp, text) register BUFFER *bp; char *text; {
	register LINE	*lp;
	register int	i;
	register int	ntext;

	ntext = strlen(text);
	if ((lp=lalloc(ntext)) == NULL)
		return FALSE;
	for (i=0; i<ntext; ++i)
		lputc(lp, i, text[i]);
	bp->b_linep->l_bp->l_fp = lp;		/* Hook onto the end	*/
	lp->l_bp = bp->b_linep->l_bp;
	bp->b_linep->l_bp = lp;
	lp->l_fp = bp->b_linep;
#ifdef CANTHAPPEN
	if (bp->b_dotp == bp->b_linep)		/* If "." is at the end	*/
		bp->b_dotp = lp;		/* move it to new line	*/
	if (bp->b_markp == bp->b_linep)		/* ditto for mark	*/
		bp->b_markp = lp;
#endif
	return TRUE;
}

/*
 * Look through the list of buffers, giving the user
 * a chance to save them.  Return TRUE if there are
 * any changed buffers afterwards. Buffers that don't
 * have an associated file don't count. Return FALSE
 * if there are no changed buffers.
 */
anycb(f) {
	register BUFFER *bp;
	register int	s = FALSE, save = FALSE;
	char		*prompt;
	VOID		upmodes();

	for (bp = bheadp; bp != NULL; bp = bp->b_bufp) {
		if (bp->b_fname != NULL
		&&  (bp->b_flag&BFCHG) != 0) {
		    	prompt = alloca(10 + strlen(bp->b_fname) + 1);
			if (prompt != NULL) {
				(VOID) strcpy(prompt, "Save file ");
				(VOID) strcpy(prompt + 10, bp->b_fname);
			}
			else {
				prompt = "Save file <Somethings>";
			}
			if ((f == TRUE || (save = eyorn(prompt)) == TRUE)
			&&  buffsave(bp) == TRUE) {
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
				bp->b_flag &= ~(BFCHG|BFACHG);
#else
				bp->b_flag &= ~BFCHG;
#endif	/* AUTOSAVE	*/
				upmodes(bp);
			} else s = TRUE;
			if (save == ABORT) return (save);
			save = TRUE;
		}
	}
	if (save == FALSE /* && kbdmop == NULL */ )	/* experimental */
		ewprintf("(No files need saving)");
	return s;
}

/*
 * Search for a buffer, by name.
 * If not found, and the "cflag" is TRUE,
 * create a buffer and put it in the list of
 * all buffers. Return pointer to the BUFFER
 * block for the buffer.
 */
BUFFER	*
bfind(bname, cflag) register char *bname; {
	register BUFFER *bp;
	register LINE	*lp;
	int i;
	extern int defb_nmodes;
	extern MAPS *defb_modes[PBMODES];
	extern int defb_flag;
	extern int defb_tab;

	bp = bheadp;
	while (bp != NULL) {
		if (fncmp(bname, bp->b_bname) == 0)
			return bp;
		bp = bp->b_bufp;
	}
	if (cflag!=TRUE) return NULL;
	/*NOSTRICT*/
	if ((bp=(BUFFER *)malloc(sizeof(BUFFER))) == NULL) {
		ewprintf("Can't get %d bytes", sizeof(BUFFER));
		return NULL;
	}
	if ((bp->b_bname=malloc((unsigned)(strlen(bname)+1))) == NULL) {
		ewprintf("Can't get %d bytes", strlen(bname)+1);
		free((char *) bp);
		return NULL;
	}
	if ((lp = lalloc(0)) == NULL) {
		free(bp->b_bname);
		free((char *) bp);
		return NULL;
	}
#ifdef	UNDO
	bzero(bp->b_ustack, sizeof(UNDO_DATA *)*(UNDOSIZE+1));
	undo_reset(bp);
#endif
	bp->b_altb = bp->b_bufp	 = NULL;
	bp->b_dotp  = lp;
	bp->b_doto  = 0;
	bp->b_markp = NULL;
	bp->b_marko = 0;
	bp->b_flag  = defb_flag;
	bp->b_nwnd  = 0;
	bp->b_linep = lp;
	bp->b_nmodes = defb_nmodes;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	bp->b_kfio = NIL;	/* changed by amura */
				/* set in fileio.c  */
#endif	/* KANJI */
#ifdef  VARIABLE_TAB
	bp->b_tabwidth = defb_tab;
#endif  /* VARIABLE_TAB */
	i = 0;
	do {
	    bp->b_modes[i] = defb_modes[i];
	} while(i++ < defb_nmodes);
	bp->b_fname = NULL;
#ifdef	EXTD_DIR
	bp->b_cwd = NULL;
	if (curbp) {
	  if (curbp->b_cwd == NULL) {
	    extern void storecwd pro((BUFFER *bp));
	    storecwd(curbp);
	  }
	  if (curbp->b_cwd != NULL &&
	      (bp->b_cwd=malloc(strlen(curbp->b_cwd)+1)) != NULL)
	    strcpy(bp->b_cwd, curbp->b_cwd);
	}
#endif
	(VOID) strcpy(bp->b_bname, bname);
	lp->l_fp = lp;
	lp->l_bp = lp;
	bp->b_bufp = bheadp;
	bheadp = bp;
	return bp;
}

/*
 * This routine blows away all of the text
 * in a buffer. If the buffer is marked as changed
 * then we ask if it is ok to blow it away; this is
 * to save the user the grief of losing text. The
 * window chain is nearly always wrong if this gets
 * called; the caller must arrange for the updates
 * that are required. Return TRUE if everything
 * looks good.
 */
bclear(bp) register BUFFER *bp; {
	register LINE	*lp;
	register int	s;
	VOID		lfree();

	if ((bp->b_flag&BFCHG) != 0		/* Changed.		*/
	&& (s=eyesno("Buffer modified; kill anyway")) != TRUE)
		return (s);
#ifdef	AUTOSAVE	/* 96.12.25 by M.Suzuki	*/
	if (bp->b_bname && bp->b_bname[0] != '*' ){/* file buffer only	*/
		del_autosave_file(bp->b_fname);
	}
	bp->b_flag  &= ~(BFCHG|BFACHG);		/* Not changed		*/
#else
	bp->b_flag  &= ~BFCHG;			/* Not changed		*/
#endif	/* AUTOSAVE	*/
	while ((lp=lforw(bp->b_linep)) != bp->b_linep)
		lfree(lp);
	bp->b_dotp  = bp->b_linep;		/* Fix "."		*/
	bp->b_doto  = 0;
	bp->b_markp = NULL;			/* Invalidate "mark"	*/
	bp->b_marko = 0;
	return TRUE;
}

/*
 * Display the given buffer in the given window. Flags indicated
 * action on redisplay.
 */
showbuffer(bp, wp, flags) register BUFFER *bp; register WINDOW *wp; {
	register BUFFER *obp;
	WINDOW		*owp;

	if (wp->w_bufp == bp) {			/* Easy case!	*/
		wp->w_flag |= flags;
		return TRUE;
	}

	/* First, dettach the old buffer from the window */
	if ((bp->b_altb = obp = wp->w_bufp) != NULL) {
		if (--obp->b_nwnd == 0) {
			obp->b_dotp  = wp->w_dotp;
			obp->b_doto  = wp->w_doto;
}
	}

	/* Now, attach the new buffer to the window */
	wp->w_bufp = bp;

	if (bp->b_nwnd++ == 0) {		/* First use.		*/
		wp->w_dotp  = bp->b_dotp;
		wp->w_doto  = bp->b_doto;
	} else
	/* already on screen, steal values from other window */
#ifdef	BUGFIX	/* ? 90.12.08    Sawayanagi Yosirou */
		for (owp = wheadp; owp != NULL; owp = owp->w_wndp)
			if (owp->w_bufp == bp && owp != wp) {
#else	/* ORIGINAL */
		for (owp = wheadp; owp != NULL; owp = wp->w_wndp)
			if (wp->w_bufp == bp && owp != wp) {
#endif	/* BUGFIX */
				wp->w_dotp  = owp->w_dotp;
				wp->w_doto  = owp->w_doto;
				break;
			}
	wp->w_flag |= WFMODE|flags;
	return TRUE;
}

/*
 * Pop the buffer we got passed onto the screen.
 * Returns a status.
 */
WINDOW *
popbuf(bp) register BUFFER *bp; {
	register WINDOW *wp;

	if (bp->b_nwnd == 0) {		/* Not on screen yet.	*/
		if ((wp=wpopup()) == NULL) return NULL;
	} else
		for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
			if (wp->w_bufp == bp) {
				wp->w_flag |= WFHARD|WFFORCE;
				return wp ;
			}
	if (showbuffer(bp, wp, WFHARD) != TRUE) return NULL;
	return wp;
}

/*
 * Insert another buffer at dot.  Very useful.
 */
/*ARGSUSED*/
bufferinsert(f, n)
{
	register BUFFER *bp;
	register LINE	*clp;
	register int	clo;
	register int	nline;
	int		s;
	char		bufn[NBUFN];

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
		warnreadonly();		/* do only displaying warning.	*/
		return TRUE;
	}
#endif	/* READONLY */

	/* Get buffer to use from user */
	if (curbp->b_altb != NULL)
		s=eread("Insert buffer: (default %s) ", bufn, sizeof(bufn),
#ifdef	BUGFIX	/* 91.01.04  by S.Yoshida */
			 EFNEW|EFBUF, curbp->b_altb->b_bname);
	else
		s=eread("Insert buffer: ", bufn, sizeof(bufn), EFNEW|EFBUF);
#else	/* ORIGINAL */
			 EFNEW|EFBUF, &(curbp->b_altb->b_bname),
			 (char *) NULL) ;
	else
		s=eread("Insert buffer: ", bufn, sizeof(bufn), EFNEW|EFBUF,
			 (char *) NULL) ;
#endif	/* BUGFIX */
	if (s == ABORT) return (s);
	if (s == FALSE && curbp->b_altb != NULL) bp = curbp->b_altb;
	else if ((bp=bfind(bufn, FALSE)) == NULL) return FALSE;

	if (bp==curbp) {
		ewprintf("Cannot insert buffer into self");
		return FALSE;
	}

	/* insert the buffer */
	nline = 0;
	clp = lforw(bp->b_linep);
	for(;;) {
		for (clo = 0; clo < llength(clp); clo++)
			if (linsert(1, lgetc(clp, clo)) == FALSE)
				return FALSE;
		if((clp = lforw(clp)) == bp->b_linep) break;
		if (newline(FFRAND, 1) == FALSE) /* fake newline */
			return FALSE;
		nline++;
	}
	if (nline == 1) ewprintf("[Inserted 1 line]");
	else	ewprintf("[Inserted %d lines]", nline);

	clp = curwp->w_linep;			/* cosmetic adjustment */
	if (curwp->w_dotp == clp) {		/* for offscreen insert */
		while (nline > 0 && lback(clp)!=curbp->b_linep) {
			clp = lback(clp);
			nline --;
		}
		curwp->w_linep = clp;		/* adjust framing.	*/
		curwp->w_lines = 0;
		curwp->w_flag |= WFHARD;
	}
	return (TRUE);
}

/*
 * Turn off the dirty bit on this buffer.
 */
/*ARGSUSED*/
notmodified(f, n)
{
	register WINDOW *wp;

#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
	curbp->b_flag &= ~(BFCHG|BFACHG);
#else
	curbp->b_flag &= ~BFCHG;
#endif	/* AUTOSAVE	*/
	wp = wheadp;				/* Update mode lines.	*/
	while (wp != NULL) {
		if (wp->w_bufp == curbp)
			wp->w_flag |= WFMODE;
		wp = wp->w_wndp;
	}
	ewprintf("Modification-flag cleared");
	return TRUE;
}

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
/*
 * Toggle the read-only bit on this buffer.
 */
/*ARGSUSED*/
togglereadonly(f, n)
{
	register WINDOW *wp;

	curbp->b_flag ^= BFRONLY;		/* Toggle read-only bit. */
	wp = wheadp;				/* Update mode lines.	*/
	while (wp != NULL) {
		if (wp->w_bufp == curbp)
			wp->w_flag |= WFMODE;
		wp = wp->w_wndp;
	}
	return TRUE;
}

/*
 * Display warning message.
 */
VOID
warnreadonly()
{
	ewprintf("Buffer is read-only: #<buffer %s>", curbp->b_bname);
	ttbeep();	/* 91.02.06  Add beep. by S.Yoshida */
}
#endif	/* READONLY */

#ifndef NO_HELP
/*
 * Popbuf and set all windows to top of buffer.	 Currently only used by
 * help functions.
 */

popbuftop(bp)
register BUFFER *bp;
{
    register WINDOW *wp;

    bp->b_dotp = lforw(bp->b_linep);
    bp->b_doto = 0;
    if(bp->b_nwnd != 0) {
	for(wp = wheadp; wp!=NULL; wp = wp->w_wndp)
	    if(wp->w_bufp == bp) {
		wp->w_dotp = bp->b_dotp;
		wp->w_doto = 0;
		wp->w_flag |= WFHARD;
	    }
    }
    return popbuf(bp) != NULL;
}
#endif

#if defined(CMODE)||defined(VARIABLE_TAB)||defined(AUTOSAVE)
#define		USING_GETNUM	1
#endif

#ifdef USING_GETNUM
getnum(prompt, num)
char *prompt;
int *num;
{
	char numstr[GETNUMLEN];

	if (ereply("%s : ", numstr, GETNUMLEN, prompt) == FALSE)
		return (FALSE);
	*num = atoi(numstr);
	return (TRUE);
}
#undef USING_GETNUM
#endif  /* USING_GETNUM */

#ifdef  VARIABLE_TAB
set_default_tabwidth(f, n)
int f, n;
{
    if ((f & FFARG) == 0)
	{
	if (getnum("default-tab-width", &n) == FALSE)
	    return (FALSE);
	}
    if (n>64 || n<=2)
    	return (FALSE);
    defb_tab = n;
    return (TRUE);
}

set_tabwidth(f, n)
int f, n;
{
    extern int refresh();
    if ((f & FFARG) == 0)
	{
	if (getnum("tab-width", &n) == FALSE)
	    return (FALSE);
	}
	if (n == 0)
		n = defb_tab;
    else if (n>64 || n<=2)
    	return (FALSE);
    curbp->b_tabwidth = n;
    if (f >= 0)
	    refresh(0, 0);
    return (TRUE);
}

set_cmode_tabwidth(f, n)
int f, n;
{
    if ((f & FFARG) == 0)
	{
	if (getnum("c-tab-width", &n) == FALSE)
	    return (FALSE);
	}
    if (n>64 || n<=2)
    	return (FALSE);
    cmode_tab = n;
    return (TRUE);
}
#endif  /* VARIABLE_TAB */

#ifdef BUFFER_MODE

#define BUFNAME_START_COL 4

static int
b_makename(lp, buf, len)
LINE *lp;
char *buf;
int len;
{
  if (BUFNAME_START_COL < llength(lp)) {
    char *p = lp->l_text + BUFNAME_START_COL, *q = buf, *ep = p + NBUFN;

    while (*ep == ' ') {
      ep--;
    }
    ep++;
    while (p < ep) {
      *q++ = *p++;
    }
    *q = '\0';
    return TRUE;
  }
  return FALSE;
}
  
/*ARGSUSED*/
b_thiswin(f, n)
int f, n;
{
  char bufname[NBUFN];
  register BUFFER *bp;
  LINE *lp = curwp->w_dotp;
  int s;

  s = b_makename(lp, bufname, NBUFN);
  if (s) {
    bp = bfind(bufname, FALSE);
    if (bp != NULL) {
      /* put it in current window */
	curbp = bp;
	return showbuffer(bp, curwp, WFFORCE|WFHARD);
      }
    else {
      ewprintf("No buffer named \"%s\"", bufname);
    }
  }
  return FALSE;
}
  
/*ARGSUSED*/
static
b_delundel(ch)
int ch;
{
  LINE *lp = curwp->w_dotp;

  if (lback(lp) != curbp->b_linep &&
      lback(lback(lp)) != curbp->b_linep) {
    if (llength(lp) > 0) {
      lputc(lp, 0, ch);
    }
    if (lforw(lp) != curbp->b_linep) {
      curwp->w_dotp = lforw(lp);
    }
    curwp->w_flag |= WFEDIT | WFMOVE;
    curwp->w_doto = 0;
    return TRUE;
  }
  return FALSE;
}

/*ARGSUSED*/
b_del(f, n)
int f, n;
{
  return b_delundel((int)'D');
}

/*ARGSUSED*/
b_undel(f, n)
int f, n;
{
  return b_delundel((int)' ');
}

/*ARGSUSED*/
b_expunge(f, n)
int f, n;
{
  char bufname[NBUFN];
  register LINE *lp, *nlp;
  VOID lfree();

  for (lp = lforw(curbp->b_linep) ; lp != curbp->b_linep ; lp = nlp) {
    nlp = lforw(lp);
    if (0 < llength(lp) && lgetc(lp, 0) == 'D') {
      switch (b_makename(lp, bufname, NBUFN)) {
      case FALSE:
	break;

      case TRUE:
	eargset(bufname);
	if (killbuffer(f, n)) {
	  lfree(lp);
	  curwp->w_flag |= WFHARD;
	}
	break;
      }
    }
  }
  return TRUE;
}
#endif /* BUFFER_MODE */
