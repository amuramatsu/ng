/* $Id: dir.c,v 1.16.2.5 2006/01/04 17:00:39 amura Exp $ */
/*
 * Name:	MG 2a
 *		Directory management functions
 * Created:	Ron Flax (ron@vsedev.vse.com)
 *		Modified for MG 2a by Mic Kaczmarczik 03-Aug-1987
 */
/* 90.01.29	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#include "dir.h"
#include "i_buffer.h"
#include "i_lang.h"
#include "echo.h"
#include "file.h"
#include "fileio.h"

#ifndef NO_DIR

char *startdir = NULL;
char *wdir;

#ifdef EXTD_DIR
/*
 * Store the current working directory with refering filename for the
 * specified buffer.
 *
 * By Tillanosoft, Mar 22, 1999.
 * Modified by amura, Dec 1, 2000.
 */
VOID
storecwd(bp)
BUFFER *bp;
{
    char path[NFILEN];
    int lastchar;

    if (bp) {
	if (bp->b_fname != NULL)
	    makepath(path, bp->b_fname, NFILEN);
	if (startdir != NULL &&
	    (bp->b_fname == NULL || path[0] == '\0')) {
	    strncpy(path, startdir, NFILEN);
	    path[NFILEN-1] = '\0';
	    lastchar = strlen(path)-1;
	    if (path[lastchar] != BDC1
#ifdef BDC2
		&& path[lastchar] != BDC2
#endif
		) {
		if (lastchar+2 >= NFILEN)
		    lastchar = NFILEN-2;
		path[lastchar+1] = BDC1;
		path[lastchar+2] = '\0';
	    }
	}
	if (bp->b_cwd != NULL)
	    free(bp->b_cwd);
	if ((bp->b_cwd=malloc(strlen(path)+1)) == NULL)
	    return;
	strcpy(bp->b_cwd, path);
    }
}

VOID
vchdir(newdir)
char *newdir;
{
    if (curbp) {
	if (curbp->b_cwd != NULL)
	    free(curbp->b_cwd);
	if ((curbp->b_cwd=malloc(strlen(newdir)+1)) == NULL)
	    return;
	strcpy(curbp->b_cwd, newdir);
    }
}

VOID
ensurecwd()
{
    if (curbp) {
	if (curbp->b_cwd == NULL) {
	    storecwd(curbp);
	}
	if (curbp->b_cwd != NULL) {
	    rchdir(curbp->b_cwd); /* ensure we are in the current dir */
	}
    }
}

/*
 * Change current working directory
 */
/*ARGSUSED*/
int
changedir(f, n)
int f, n;
{
    register int s;
    int len;
    NG_WCHAR_t bufc[NPAT];
    NG_WCHAR_t *tmp;
    char *bufca;

    ensurecwd();
    LM_IN_CONVERT_TMP2(curbp->b_lang, NG_CODE_FOR_FILENAME, curbp->b_cwd, tmp);
    if (tmp == NULL)
	return FALSE;
    edefset(tmp);

#ifndef	NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if ((s=eread("Change default directory: ", bufc, NG_WCHARLEN(bufc),
		 EFNEW|EFFILE|EFCR)) != TRUE)
#else	/* NO_FILECOMP */
    if ((s=ereply("Change default directory: ", bufc, NG_WCHARLEN(bufc)))
        != TRUE)
#endif	/* NO_FILECOMP */
	return(s);
    LM_OUT_CONVERT_TMP2(curbp->b_lang, NG_CODE_FOR_FILENAME, bufc, bufca);
    if (bufca == NULL)
	return FALSE;
    strlcpy(bufca, adjustname(bufca), NG_WCHARLEN(len+1));
    if (rchdir(bufca) < 0) {
	ewprintf("Can't change dir to %s", bufca);
    }
    else {
	ewprintf("Current directory is now %s", bufca);
	len = strlen(bufca);
	if (len<NFILEN-1 && bufca[len-1]!=BDC1
#ifdef	BDC2
	    && bufca[len-1]!=BDC2
#endif
	    ) {
	    bufca[len] = BDC1;
	    bufca[len+1] = '\0';
#ifdef	AMIGA
	    for (s=len; s>=0; s--)
		if (bufca[s] == NG_WCODE(':'))
		    break;
	    if (bufca[s] != ':')
		bufca[len] = ':';
#endif
	}
	vchdir(bufca);
    }
    return TRUE;
}

#else	/* EXTD_DIR */

/*
 * Change current working directory
 */
/*ARGSUSED*/
int
changedir(f, n)
int f, n;
{
    register int s;
    char bufc[NFILEN];

#ifndef	NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if ((s=eread("Change default directory: ", bufc,
		 NFILEN, EFNEW|EFFILE|EFCR)) != TRUE)
#else	/* NO_FILECOMP */
    if ((s=ereply("Change default directory: ", bufc, NFILEN)) != TRUE)
#endif	/* NO_FILECOMP */
	return(s);
    if (bufc[0] == '\0')
	(VOID) strcpy(bufc, wdir);
    
    if (rchdir(bufc) == -1) {
	ewprintf("Can't change dir to %s", bufc);
	return(FALSE);
    }
    dirinit();
    return(TRUE);
}
#endif	/* EXTD_DIR */

/*
 * Show current directory
 */
/*ARGSUSED*/
int
showcwdir(f, n)
int f, n;
{
#ifdef	EXTD_DIR
    char dirname[NFILEN];
    int  len;
    
    if (curbp) {
	if (curbp->b_cwd == NULL)
	    storecwd(curbp);
	if (curbp->b_cwd == NULL) 
	    dirname[0] = '\0';
	else {
	    (VOID)strcpy(dirname, curbp->b_cwd);
	    len = strlen(dirname) - 1;
	    if (len >= 0) {
#ifdef	BDC2
# ifdef	AMIGA
		if (dirname[len]==BDC2)
# else
		if (dirname[len]==BDC1 || dirname[len]==BDC2)
# endif
#else
	 	if (dirname[len] == BDC1)
#endif
		    dirname[len] = '\0';
	    }
	}
	ewprintf("Current directory: %s", dirname);
    }
#else	/* !EXTD_DIR */
    dirinit();
    ewprintf("Current directory: %s", wdir);
#endif	/* EXTD_DIR */
    return(TRUE);
}

#endif	/* NO_DIR */
