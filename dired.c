/* $Id: dired.c,v 1.10.2.5 2005/09/17 05:17:18 amura Exp $ */
/* dired module for mg 2a	*/
/* by Robert A. Larson		*/

#include "config.h"	/* 90.12.20  by S.Yoshida */

#ifndef NO_DIRED
#include "def.h"
#include "dired.h"

#include <string.h>
#include "i_buffer.h"
#include "dir.h"
#include "line.h"
#include "echo.h"
#include "buffer.h"
#include "window.h"
#include "file.h"
#include "fileio.h"

#ifndef	max
#define	max(a,b)	(((a)<(b))?(b):(a))
#endif

static int d_copy_and_rename _PRO((int));

#if 0
static int
SearchDir(dirname)
char *dirname;
{
    LINE *llp;

    for (llp=curbp->b_linep; lforw(llp)!=curbp->b_linep; llp=lforw(llp)){
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
#endif

/*ARGSUSED*/
int
dired(f, n)
int f,n;
{
    NG_WCHAR_t dirname[NFILEN];
    BUFFER *bp;
    char *fname;
#ifdef	EXTD_DIR
    int i;
    NG_WCHAR_t *tmp;

    ensurecwd();
    LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
		       curbp->b_cwd, tmp);
    if (tmp == NULL)
	return FALSE;
    edefset(tmp);
#endif

    dirname[0] = NG_EOS;
    if (eread("Dired: ", dirname, NFILEN, EFNEW | EFCR | EFFILE) == ABORT)
	return ABORT;
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
			dirname, fname);
    if (fname == NULL)
	return FALSE;
    if ((fname = adjustname(fname)) == NULL) {
	ewprintf("Bad directory name");
	return FALSE;
    }
    if (!ffisdir(fname))
	return filevisit_(fname, f, n);
    if ((bp = dired_(fname)) == NULL)
	return FALSE;
    curbp = bp;
#ifdef	EXTD_DIR
    i = strlen(fname);
    if (curbp->b_cwd != NULL)
	free(curbp->b_cwd);
    if ((curbp->b_cwd=malloc(i+2)) == NULL)
	return FALSE;
    strcpy(curbp->b_cwd, fname);
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
int
d_otherwindow(f, n)
int f,n;
{
    NG_WCHAR_t tmp2[NFILEN];
    char *dirname;
    BUFFER *bp;
    WINDOW *wp;
#ifdef	EXTD_DIR
    NG_WCHAR_t *tmp;

    ensurecwd();
    LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
		       curbp->b_cwd, tmp);
    if (tmp == NULL)
	return FALSE;
    edefset(tmp);
#endif

    tmp2[0] = '\0';
    if (eread("Dired other window: ", tmp2,
	      NFILEN, EFNEW | EFCR | EFFILE) == ABORT)
	return ABORT;
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, tmp2, dirname);
    if (dirname == NULL)
	return FALSE;
    if ((bp = dired_(dirname)) == NULL)
	return FALSE;
#ifdef	READONLY	/* 91.01.15  by K.Maeda */
    bp->b_flag |= BFRONLY;
#endif	/* READONLY */
    if ((wp = popbuf(bp)) == NULL)
	return FALSE;
    curbp = bp;
    curwp = wp;
    return TRUE;
}

/*ARGSUSED*/
int
d_del(f, n)
int f,n;
{
    if (n < 0)
	return FALSE;
    while (n--) {
	if (llength(curwp->w_dotp) > 0) {
	    if (lgetc(curwp->w_dotp,0) != '/' &&
		wstrncmpa(curwp->w_dotp->l_text, "  total", 7) )
	    lputc(curwp->w_dotp, 0, 'D');
	}
	if (lforw(curwp->w_dotp) != curbp->b_linep)
	    curwp->w_dotp = lforw(curwp->w_dotp);
    }
    curwp->w_flag |= WFEDIT | WFMOVE;
    curwp->w_doto = 0;
    return TRUE;
}

/*ARGSUSED*/
int
d_undel(f, n)
int f, n;
{
    if (n < 0)
	return d_undelbak(f, -n);
    while (n--) {
	if (llength(curwp->w_dotp) > 0) {
	    if (lgetc(curwp->w_dotp,0) != '/' &&
		wstrncmpa(curwp->w_dotp->l_text,"  total",7))
	    lputc(curwp->w_dotp, 0, ' ');
	}
	if (lforw(curwp->w_dotp) != curbp->b_linep)
	    curwp->w_dotp = lforw(curwp->w_dotp);
    }
    curwp->w_flag |= WFEDIT | WFMOVE;
    curwp->w_doto = 0;
    return TRUE;
}

/*ARGSUSED*/
int
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
int
d_flag(f, n)
int f,n;
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

static int
d_fileopen(f, n, popup)
int f,n,popup;
{
    NG_WCHAR_t tmp[NFILEN];
    char *fname;
    register BUFFER *bp;
    register int s;
    register WINDOW *wp;

    if ((s = d_makename(curwp->w_dotp, tmp, NG_WCHARLEN(tmp))) == ABORT)
	return FALSE;
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, tmp, fname);
    if (tmp == NULL)
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
    if (fchkreadonly(bp->b_fname)) {	 /* If no write permission, */
	bp->b_flag |= BFRONLY;		 /* mark as read-only.      */
	ewprintf("File is write protected");
    }
#endif	/* READONLY */
    return s;
}

/*ARGSUSED*/
int
d_findfile(f, n)
int f,n;
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
int
d_ffotherwindow(f, n)
int f, n;
{
    return d_fileopen(f, n, TRUE);
}

/*ARGSUSED*/
int
d_expunge(f, n)
int f, n;
{
    register LINE *lp, *nlp;
    NG_WCHAR_t wfname[NFILEN];
    char *fname;
    int s;

#ifdef	EXTD_DIR
    ensurecwd();
#endif

    for (lp = lforw(curbp->b_linep); lp != curbp->b_linep; lp = nlp) {
	nlp = lforw(lp);
	if (llength(lp) && lgetc(lp, 0) == 'D') {
	    switch (s = d_makename(lp, wfname, NG_WCHARLEN(wfname))) {
	    case ABORT:
		ewprintf("Bad line in dired buffer");
		return FALSE;
	    case TRUE:
	    case FALSE:
		LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
				    wfname, fname);
		if (fname == NULL)
		    return FALSE;
		if (s == TRUE) {
		    if (unlink(fname) < 0) {
			ewprintf("Could not delete '%s'", fname);
			return FALSE;
		    }
		}
		else /* if (s == FALSE) */ {
		    if (unlinkdir(fname) < 0) {
			ewprintf("Could not delete directory '%s'", fname);
			return FALSE;
		    }
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

static int
d_copy_and_rename(copymode)
int copymode;
{
    NG_WCHAR_t frname_tmp[NFILEN], toname_tmp[NFILEN];
    char *frname, *toname, *fr;
    int stat;

#ifdef	EXTD_DIR
    ensurecwd();
#endif

    switch (d_makename(curwp->w_dotp, frname_tmp, NG_WCHARLEN(frname_tmp))) {
    case TRUE:
	ewprintf("Not a file");
	return FALSE;
	
    case ABORT:
	return FALSE;
	
    case FALSE:
	/* nothing to do */
	break;
    }
  
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, frname_tmp, frname);
    if (frname == NULL)
	return FALSE;
    fr = filename(frname);
#ifdef	EXTD_DIR
    {
	NG_WCHAR_t *tmp;
	LM_IN_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code,
			   curbp->b_cwd, tmp);
	if (tmp == NULL)
	    return FALSE;
	edefset(tmp);
    }
#endif

    if ((stat = eread("%s %s to: ", toname_tmp, NG_WCHARLEN(toname_tmp),
		      EFNEW | EFCR | EFFILE,
		      copymode ? "Copy" : "Rename", fr)) != TRUE)
	return stat;
    LM_OUT_CONVERT_TMP2(curbp->b_lang, lm_buffer_name_code, toname_tmp, toname);
    if (toname == NULL)
	return FALSE;
    if (copymode)
	stat = (copy(frname, toname) >= 0);
    else
	stat = (rename(frname, toname) >= 0);
    return stat;
}

/*ARGSUSED*/
int
d_copy(f, n)
int f, n;
{
    return d_copy_and_rename(TRUE);
}

/*ARGSUSED*/
int
d_rename(f, n)
int f, n;
{
    return d_copy_and_rename(FALSE);
}

/*ARGSUSED*/
int
d_execute(f, n)
int f, n;
{
#ifdef WIN32
    NG_WCHAR_t fname[NFILEN];
    register int s;

    s = d_makename(curwp->w_dotp, fname, NG_WCHARLEN(fname));
    if (s == ABORT)
	return FALSE;
    else if (s) { /* that is, fname points to a directory */
#if !defined(_WIN32_WCE) || 200 <= _WIN32_WCE
	return FALSE;
#endif
    }
    WinExecute(fname);
    return TRUE;
#else /* not WIN32 */
    return TRUE;
#endif /* WIN32 */
}

#endif /* NO_DIRED */
