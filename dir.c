/* $Id: dir.c,v 1.13 2001/02/18 19:29:29 amura Exp $ */
/*
 * Name:	MG 2a
 *		Directory management functions
 * Created:	Ron Flax (ron@vsedev.vse.com)
 *		Modified for MG 2a by Mic Kaczmarczik 03-Aug-1987
 */

/*
 * $Log: dir.c,v $
 * Revision 1.13  2001/02/18 19:29:29  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.12  2000/12/22 19:54:35  amura
 * fix some bug in filename handling
 *
 * Revision 1.11  2000/12/14 18:06:23  amura
 * filename length become flexible
 *
 * Revision 1.10  2000/09/21 17:28:29  amura
 * replace macro _WIN32 to WIN32 for Cygwin
 *
 * Revision 1.9  2000/07/22 17:54:09  amura
 * fix typo bug
 *
 * Revision 1.8  2000/07/18 20:30:34  amura
 * rewrite some description (for Win32)
 *
 * Revision 1.7  2000/07/18 12:40:34  amura
 * for Win32, enable to handle japanese directory
 *
 * Revision 1.6  2000/06/27 01:49:42  amura
 * import to CVS
 *
 * Revision 1.5  2000/06/04  05:42:59  amura
 * win32 drive support
 *
 * Revision 1.4  2000/06/01  05:22:25  amura
 * More robust
 *
 * Revision 1.3  2000/03/28  02:37:46  amura
 * Added startdir.
 *
 * Revision 1.2  2000/03/10  21:26:50  amura
 * Merge Ng for win32 0.4
 *
 * Revision 1.1  1999/05/21  02:20:08  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#ifndef NO_DIR
#ifdef	EXTD_DIR
extern VOID makepath pro((char *dname, char *fname, int len)); /* file.c */
#endif

char	*startdir = NULL;
char	*wdir;

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
#ifdef	BDC2
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
ensurecwd pro((VOID))
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
changedir(f, n)
{
    register int s;
    int len;
    char bufc[NPAT];

    ensurecwd();
    edefset(curbp->b_cwd);

#ifndef	NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if ((s=eread("Change default directory: ", bufc, NPAT, EFNEW|EFFILE|EFCR))
	!= TRUE)
#else	/* NO_FILECOMP */
    if ((s=ereply("Change default directory: ", bufc, NPAT)) != TRUE)
#endif	/* NO_FILECOMP */
	return(s);
    strcpy(bufc, adjustname(bufc));
    if (rchdir(bufc) < 0) {
	ewprintf("Can't change dir to %s", bufc);
    }
    else {
	ewprintf("Current directory is now %s", bufc);
	len = strlen(bufc);
	if (len<NFILEN-1 && bufc[len-1]!=BDC1
#ifdef	BDC2
	    && bufc[len-1]!=BDC2
#endif
	    ) {
	    bufc[len] = BDC1;
	    bufc[len+1] = '\0';
#ifdef	AMIGA
	    for (s=len; s>=0; s--)
		if (bufc[s] == ':')
		    break;
	    if (bufc[s] != ':')
		bufc[len] = ':';
#endif
	}
	vchdir(bufc);
    }
    return TRUE;
}

#else	/* EXTD_DIR */

/*
 * Change current working directory
 */
/*ARGSUSED*/
changedir(f, n)
{
    register int s;
    char bufc[NFILEN];

#ifndef	NO_FILECOMP	/* 90.04.04  by K.Maeda */
    if ((s=eread("Change default directory: ", bufc, NFILEN, EFNEW|EFFILE|EFCR))
	!= TRUE)
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
showcwdir(f, n)
{
#ifdef	EXTD_DIR
    char dirname[NFILEN];
    int  len;
    
    if (curbp) {
	if (curbp->b_cwd == NULL) {
	    storecwd(curbp);
	}
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
