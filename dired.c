/* $Id: dired.c,v 1.8 2001/02/18 19:29:29 amura Exp $ */
/* dired module for mg 2a	*/
/* by Robert A. Larson		*/

/*
 * $Log: dired.c,v $
 * Revision 1.8  2001/02/18 19:29:29  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.7  2001/02/18 17:07:24  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.6  2000/12/27 16:56:01  amura
 * change d_makename() params for conservative reason, and bugfix in dires_()
 *
 * Revision 1.5  2000/12/14 18:06:24  amura
 * filename length become flexible
 *
 * Revision 1.4  2000/09/21 17:28:29  amura
 * replace macro _WIN32 to WIN32 for Cygwin
 *
 * Revision 1.3  2000/07/22 17:54:09  amura
 * fix typo bug
 *
 * Revision 1.2  2000/06/27 01:49:42  amura
 * import to CVS
 *
 * Revision 1.1  1999/05/21  02:45:06  amura
 * Initial revision
 *
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#ifndef NO_DIRED

BUFFER *dired_();
#ifndef strncpy
extern char* strncpy();
#endif

#ifdef	CHGMISC		/* 99.8.11 by M.Suzuki	*/
static int SearchDir(dirname)
char* dirname;
{
    LINE *llp;

    for (llp=curbp->b_linep; lforw(llp)!=curbp->b_linep; llp=lforw(llp)){
#ifndef	max
#define	max(a,b)	(((a)<(b))?(b):(a))
#endif
	if (strncmp(dirname, llp->l_text, max(strlen(dirname),llength(llp)))
	   == 0 ){
	    curwp->w_dotp = llp;
	    curwp->w_flag |= WFEDIT | WFMOVE;
	    curwp->w_doto = llength(llp);
	    return TRUE;
	}
    }
    return FALSE;
}
#endif	/* CHGMISC	*/

/*ARGSUSED*/
dired(f, n)
int f, n;
{
    char dirname[NFILEN];
    BUFFER *bp;
#ifdef	CHGMISC		/* 1999.8.17 by M.Suzuki	*/
    char *fname;
#endif	/* CHGMISC	*/
#ifdef	EXTD_DIR
    int i;

    ensurecwd();
    edefset(curbp->b_cwd);
#endif

    dirname[0] = '\0';
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if(eread("Dired: ", dirname, NFILEN, EFNEW | EFCR | EFFILE) == ABORT)
#else	/* NO_FILECOMP */
    if(eread("Dired: ", dirname, NFILEN, EFNEW | EFCR) == ABORT)
#endif	/* NO_FILECOMP */
	return ABORT;
#ifdef	CHGMISC		/* 1999.8.17 by M.Suzuki	*/
    if((fname = adjustname(dirname)) == NULL) {
	ewprintf("Bad directory name");
	return FALSE;
    }
    if( !ffisdir(fname) ){
	return filevisit_(fname,f,n);
    }
    if((bp = dired_(fname)) == NULL) return FALSE;
#else
    if((bp = dired_(dirname)) == NULL) return FALSE;
#endif	/* CHGMISC	*/
    curbp = bp;
#ifdef	EXTD_DIR
    i = strlen(dirname);
    if (curbp->b_cwd != NULL)
	free(curbp->b_cwd);
    if ((curbp->b_cwd=malloc(i+2)) == NULL)
	return FALSE;
    strcpy(curbp->b_cwd, dirname);
    if (i >= 1) {
	if (curbp->b_cwd[i-1] != BDC1
#ifdef BDC2
	    && curbp->b_cwd[i-1] != BDC2
#endif
	    ) {
	    curbp->b_cwd[i] = BDC1;
	    curbp->b_cwd[i+1] = '\0';
	}
    }
#endif	/* EXTD_DIR */
#ifdef	READONLY	/* 91.01.15  by K.Maeda */
    curbp->b_flag |= BFRONLY;
#endif	/* READONLY */
#ifdef FEPCTRL
    /* The following fepmode_off() is only effective if the provided
       fepcontrol supports a doubled fepmode_off() feature.  That is,
       the second call for the fepmode_off() is expected to change the
       previous fep mode as off, without doing any actual fep control
       since the fep mode has been already turned off by the first
       call of the function.  So, the succeeding fepmode_on() will not
       turn on the fep mode even if the previous fep mode was on
       before the first call of fepmode_off().

       If this feature is not provided, entering dired does not affect
       the fep mode.

       By Tillanosoft, Mar 21, 1999 */
    fepmode_off();
#endif
    return showbuffer(bp, curwp, WFHARD | WFMODE);
}

/*ARGSUSED*/
d_otherwindow(f, n)
int f, n;
{
    char dirname[NFILEN];
    BUFFER *bp;
    WINDOW *wp;

#ifdef	EXTD_DIR
    ensurecwd();
    edefset(curbp->b_cwd);
#endif

    dirname[0] = '\0';
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if(eread("Dired other window: ", dirname, NFILEN, EFNEW | EFCR | EFFILE) == ABORT)
#else	/* NO_FILECOMP */
    if(eread("Dired other window: ", dirname, NFILEN, EFNEW | EFCR) == ABORT)
#endif	/* NO_FILECOMP */
	return ABORT;
#ifdef	CHGMISC		/* 99.6.18 by M.Suzuki	*/
    if((bp = dired_(dirname)) == NULL) return FALSE;
#else
    if((bp = dired_(dirname)) == NULL) return FALSE;
#endif	/* CHGMISC	*/
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
#ifdef	CHGMISC		/* 99.6.18 by M.Suzuki	*/
	    if(lgetc(curwp->w_dotp,0) != '/' &&
	       strncmp(curwp->w_dotp->l_text, "  total", 7) )
#endif	/* CHGMISC	*/
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
#ifdef	CHGMISC		/* 99.6.18 by M.Suzuki	*/
	    if( lgetc(curwp->w_dotp,0) != '/' &&
		strncmp(curwp->w_dotp->l_text,"  total",7) )
#endif	/* CHGMISC	*/
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
d_flag(f, n)
int f, n;
{
  struct LINE *lp;
  char flag = (f & FFARG) ? ' ' : 'D';
  int nflags = 0, len;

  lp = curbp->b_linep;
  do {
    len = llength(lp);
    if (len > 0 && lgetc(lp, len - 1) == '~') {
      lputc(lp, 0, flag);
      nflags++;
    }
    lp = lforw(lp);
  } while (lp != curbp->b_linep);
  curwp->w_flag |= WFEDIT | WFMOVE;
  ewprintf(flag == 'D' ? 
	   "%d backup file%s flagged." : "%d backup file%s unmarked.",
	   nflags, nflags == 1 ? "" : "s");
  return TRUE;
}

/*
 * unified routine for d_findfile and d_ffotherwindow
 */

static
d_fileopen(f, n, popup)
int f, n, popup;
{
    char fname[NFILEN];
    register BUFFER *bp;
    register int s;
    register WINDOW *wp;
    BUFFER *findbuffer();

    if ((s = d_makename(curwp->w_dotp, fname, sizeof(fname))) == ABORT)
	return FALSE;
    if ((bp = (s ? dired_(fname) : findbuffer(fname))) == NULL)
      return FALSE;
#ifdef	READONLY	/* 91.01.16  by S.Yoshida */
    if (s) {			/* If dired buffer,	*/
	bp->b_flag |= BFRONLY;	/* mark as read-only.	*/
    }
#endif	/* READONLY */
    if (popup) {
      if ((wp = popbuf(bp)) == NULL)
	return FALSE;
      curbp = bp;
      curwp = wp;
    }
    else {
      curbp = bp;
      if (showbuffer(bp, curwp, WFHARD) != TRUE)
	return FALSE;
    }
    if (bp->b_fname != NULL)
      return TRUE;
    s = readin(fname);
#ifdef	READONLY	/* 91.01.16  by S.Yoshida */
    if (fchkreadonly(bp->b_fname)) { /* If no write permission, */
	    bp->b_flag |= BFRONLY;	 /* mark as read-only.      */
	    ewprintf("File is write protected");
    }
#endif	/* READONLY */
    return s;
}

/*ARGSUSED*/
d_findfile(f, n)
int f, n;
{
  return d_fileopen(f, n, FALSE);
}

#ifdef READONLY
/*ARGSUSED*/
int
d_viewfile(f, n)
int f, n;
{
  int res;

  res = d_findfile(f, n);
  if (res) {
    curbp->b_flag |= BFRONLY; /* set read-only bit. */
  }
  return res;
}
#endif

/*ARGSUSED*/
d_ffotherwindow(f, n)
int f, n;
{
  return d_fileopen(f, n, TRUE);
}

/*ARGSUSED*/
d_expunge(f, n)
int f, n;
{
    register LINE *lp, *nlp;
    char fname[NFILEN];
    VOID lfree();

#ifdef	EXTD_DIR
    ensurecwd();
#endif

    for(lp = lforw(curbp->b_linep); lp != curbp->b_linep; lp = nlp) {
	nlp = lforw(lp);
	if(llength(lp) && lgetc(lp, 0) == 'D') {
	    switch(d_makename(lp, fname, sizeof(fname))) {
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

static char *
filename(path)
char *path;
{
  char *cp1;

  cp1 = path;
  while (*cp1 != 0) {
    ++cp1;
  }
  --cp1; /* insure at least 1 character ! */
  while (cp1!= path && cp1[-1] != BDC1
#ifdef	BDC2
	 && cp1[-1] != BDC2
#endif
	 ) {
    --cp1;
  }
  return cp1;
}

/*ARGSUSED*/
d_copy(f, n)
int f, n;
{
    char frname[NFILEN], toname[NFILEN], *fr;
    int stat;

#ifdef	EXTD_DIR
    ensurecwd();
#endif

    switch (d_makename(curwp->w_dotp, frname, sizeof(frname))) {
    case TRUE:
      ewprintf("Not a file");
      return FALSE;

    case ABORT:
      return FALSE;

    case FALSE:
      /* nothing to do */
      break;
    }
  
    fr = filename(frname);
#ifdef	EXTD_DIR
    edefset(curbp->b_cwd);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if((stat = eread("Copy %s to: ", toname, NFILEN, EFNEW | EFCR | EFFILE, fr))
#else	/* NO_FILECOMP */
    if((stat = eread("Copy %s to: ", toname, NFILEN, EFNEW | EFCR, fr))
#endif	/* NO_FILECOMP */
	!= TRUE) {
	return stat;
    }
    stat = (copy(frname, toname) >= 0);
    return stat;
}

/*ARGSUSED*/
d_rename(f, n)
int f, n;
{
    char frname[NFILEN], toname[NFILEN], *fr;
    int stat;

#ifdef	EXTD_DIR
    ensurecwd();
#endif

    switch (d_makename(curwp->w_dotp, frname, sizeof(frname))) {
    case TRUE:
      ewprintf("Not a file");
      return FALSE;

    case ABORT:
      return FALSE;

    case FALSE:
      /* nothing to do */
      break;
    }

    fr = filename(frname);
#ifdef	EXTD_DIR
    edefset(curbp->b_cwd);
#endif

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if((stat = eread("Rename %s to: ", toname, NFILEN, EFNEW | EFCR | EFFILE,
		     fr))
#else	/* NO_FILECOMP */
    if((stat = eread("Rename %s to: ", toname, NFILEN, EFNEW | EFCR, fr))
#endif	/* NO_FILECOMP */
	!= TRUE) {
      return stat;
    }
    stat = (rename(frname, toname) >= 0);
    return stat;
}

/*ARGSUSED*/
int
d_execute(f, n)
int f, n;
{
#ifdef WIN32
  char fname[NFILEN];
  register int s;
  extern void WinExecute(char *);

  s = d_makename(curwp->w_dotp, fname, sizeof(fname));
  if (s == ABORT) {
    return FALSE;
  }
  else if (s) { /* that is, fname points to a directory */
#if !defined(_WIN32_WCE) || 200 <= _WIN32_WCE
    goto noproblem;
#endif
    return FALSE;
  }
  else {
noproblem:
    WinExecute(fname);
    return TRUE;
  }
#else	/* not WIN32 */
  return TRUE;
#endif	/* WIN32 */
}

#endif	/* NO_DIRED */
