/*
 * Name:	MG 2a
 *		Directory management functions
 * Created:	Ron Flax (ron@vsedev.vse.com)
 *		Modified for MG 2a by Mic Kaczmarczik 03-Aug-1987
 */
/* 90.01.29	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

/* $Id: dir.c,v 1.4 2000/06/01 05:22:25 amura Exp $ */

/* $Log: dir.c,v $
/* Revision 1.4  2000/06/01 05:22:25  amura
/* More robust
/*
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

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#ifndef NO_DIR

#ifndef	getwd			/* may be a #define */
#ifdef	MSDOS	/* 90.01.29  by S.Yoshida */
#include <dos.h>
char	*getcwd();
char	*fftolower();	/* 90.07.01  Add by S.Yoshida */
#else	/* NOT MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
#include <doslib.h>
char	*getcwd();
char	*tounixfn();
#else	/* NOT HUMAN68K */
#ifdef	HAVE_GETCWD
char	*getcwd();
#else
char	*getwd();
#endif
#endif	/* HUMAN68K */
#endif	/* MSDOS */
#endif

#ifdef	EXTD_DIR
extern VOID makepath pro((char *dname, char *fname, int len)); /* file.c */
#endif
#ifndef strncpy
extern char* strncpy();
#endif

char	*wdir;
static char cwd[NFILEN];
static char startdir[NFILEN];

/*
 * Initialize anything the directory management routines need
 */
VOID
dirinit()
{
#ifdef	MSDOS	/* 90.01.29  by S.Yoshida */
	/* 90.07.01  Add fftolower() by S.Yoshida */
	if (!(wdir = fftolower(getcwd(cwd, NFILEN - 1))))
#else	/* NOT MSDOS */
# ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
	if (!(wdir = tounixfn(getcwd(cwd, NFILEN - 1))))
# else	/* NOT HUMAN68K && NOT MSDOS*/
#  ifdef HAVE_GETCWD
	if (!(wdir = getcwd(cwd, NFILEN-1)))
#  else
	if (!(wdir = getwd(cwd)))
#  endif
# endif	/* HUMAN68K */
#endif	/* MSDOS */
		panic("Can't get current directory!");
	if (startdir[0] == '\0') {
		int i;
		strncpy(startdir, cwd, NFILEN-1);
#ifdef EXTD_DIR
		i = strlen(startdir) - 1;
#ifdef BDC2
		if (startdir[i] != BDC1 && startdir[i] != BDC2) {
			startdir[i+1] = BDC2;
			startdir[i+2] = '\0';
		}
#else
		if (startdir[i] != BDC1) {
			startdir[i+1] = BDC1;
			startdir[i+2] = '\0';
		}
#endif
#endif
	}
}

/*
 * dirend routine
 */
VOID
dirend()
{
#if defined(MSDOS)||defined(HUMAN68K)
# ifdef EXTD_DIR
	rchdir(startdir);
# else
	chdir(startdir);
# endif
#endif
}

#ifdef EXTD_DIR
/*
 * Store the current working directory with refering filename for the
 * specified buffer.
 *
 * By Tillanosoft, Mar 22, 1999.
 */

VOID
storecwd(bp)
BUFFER *bp;
{
  if (bp) {
    makepath(bp->b_cwd, bp->b_fname, NFILEN);
    if (bp->b_cwd[0] == '\0')
      strncpy(bp->b_cwd, startdir, NFILEN);
  }
}

/*
   rchdir() makes some effects to change directory.  It will affect
   system to set the actual current directory to the specified one.

   This routine is extracted from changedir(), which is currently do
   some virtual chdir but previously do the actual one.

   Both bufc and wdir should have enough space to store file path, that
   is, as long as NFILEN.

   *** This function has not been completed ***

   By Tillanosoft, Mar 22, 1999
 */
int
rchdir(newdir)
char *newdir;
{
#if defined(MSDOS)||defined(HUMAN68K)
    char dir[NFILEN];
    int i;

    strcpy(dir, newdir);
    i = strlen(dir) - 1;
#ifdef	BDC2
    if (dir[i] == BDC1 || dir[i] == BDC2)
#else
    if (dir[i] == BDC1)
#endif
	dir[i] = '\0';
    if (newdir[1] == ':' && newdir[0] != wdir[0]) {
	int	drive;
	int	ndrive;
	drive = newdir[0];
	/* 90.07.01  Change from TOUPPER() to TOLOWER() */
	/*                                 by S.Yoshida */
	if (ISUPPER(drive))
	    drive = TOLOWER(drive);
	/* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
	drive = drive - 'a' + 1;
#ifdef	HUMAN68K
	if (CHGDRV(drive) <= drive) {
	    drive = drive - 'a';
	    return(FALSE);
	}
#else
#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
	(VOID) setdisk(drive - 1);
#else	/* NOT __TURBOC__ */
	_dos_setdrive(drive, &ndrive);	/* Need MSC 5.1 */
#endif	/* __TURBOC__ */
#endif	/* HUMAN68K */
    }
    if (dir[1] == ':' && dir[2] == '\0') {
	dirinit();
    } else if (dir[1]==':' && chdir(dir+2) == -1) {
	return -1;
    } else if (chdir(dir) == -1) {
	return -1;
    } else {
	dirinit();
	return 0;
    }
#else	/* !(MSDOS||HUMAN68K) */
    /* Maybe this is just for AMIGA, UNIX and Windows */
    return chdir(newdir);
#endif	/* MSDOS||HUMAN68K */
}

VOID
vchdir(newdir)
char *newdir;
{
  if (curbp) {
    strcpy(curbp->b_cwd, newdir);
  }
}

VOID
ensurecwd pro((VOID))
{
  if (curbp) {
    if (curbp->b_cwd[0] == '\0') {
      storecwd(curbp);
    }
    if (curbp->b_cwd[0]) {
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
	if ((s=eread("Change default directory: ", bufc, NPAT, EFNEW|EFFILE|EFCR)) != TRUE)
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
#ifdef BDC2
	  if (len<NFILEN-1 && bufc[len-1]!=BDC1 && bufc[len-1]!=BDC2) {
	    bufc[len] = BDC2;
	    bufc[len+1] = '\0';
	    for (s=len; s>=0; s--)
	      if (bufc[s] == BDC1)
		break;
	    if (bufc[s] != BDC1)
	      bufc[len] = BDC1;
	  }
#else
	  if (len<NFILEN-1 && bufc[len-1] != BDC1) {
	    bufc[len] = BDC1;
	    bufc[len+1] = '\0';
	  }
#endif
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
	char bufc[NPAT];

#ifndef	NO_FILECOMP	/* 90.04.04  by K.Maeda */
	if ((s=eread("Change default directory: ", bufc, NPAT, EFNEW|EFFILE|EFCR)) != TRUE)
#else	/* NO_FILECOMP */
	if ((s=ereply("Change default directory: ", bufc, NPAT)) != TRUE)
#endif	/* NO_FILECOMP */
		return(s);
	if (bufc[0] == '\0')
		(VOID) strcpy(bufc, wdir);
#ifdef	MSDOS	/* 90.02.11  by S.Yoshida */
	else if (bufc[1] == ':' && bufc[0] != wdir[0]) {
		int	drive;
		int	ndrive;
		drive = bufc[0];
		/* 90.07.01  Change from TOUPPER() to TOLOWER() */
		/*                                 by S.Yoshida */
		if (ISUPPER(drive)) {
			drive = TOLOWER(drive);
		}
		/* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
		drive = drive - 'a' + 1;
#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
		(VOID) setdisk(drive - 1);
#else	/* NOT __TURBOC__ */
		_dos_setdrive(drive, &ndrive);	/* Need MSC 5.1 */
#endif	/* __TURBOC__ */
	}
	if (bufc[1] == ':' && bufc[2] == '\0') {
		/* 90.07.01  Add fftolower() by S.Yoshida */
		if (!(wdir = fftolower(getcwd(cwd, NFILEN - 1))))
			panic("Can't get current directory!");
		ewprintf("Current directory is now %s", wdir);
	} else
#endif	/* MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
	else if (bufc[1] == ':' && bufc[0] != wdir[0]) {
		int	drive;
		int	ndrive;
		drive = bufc[0];
		if (ISUPPER(drive)) {
			drive = TOLOWER(drive);
		}
		drive = drive - 'a';
		if (CHGDRV(drive) <= drive)
		{
		    ewprintf("Can't change dir to %s", bufc);
		    return(FALSE);
		}
	}
	if (bufc[1] == ':' && bufc[2] == '\0') {
		dirinit();
		ewprintf("Current directory is now %s", wdir);
	} else
#endif	/* HUMAN68K */
	if (chdir(bufc) == -1) {
		ewprintf("Can't change dir to %s", bufc);
		return(FALSE);
	} else {
#ifdef	MSDOS	/* 90.01.29  by S.Yoshida */
		/* 90.07.01  Add fftolower() by S.Yoshida */
		if (!(wdir = fftolower(getcwd(cwd, NFILEN - 1))))
#else	/* NOT MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
		dirinit ();
#else	/* NOT HUMAN68K */
#ifdef HAVE_GETCWD
		if (!(wdir = getcwd(cwd, NFILEN-1)))
#else
		if (!(wdir = getwd(cwd)))
#endif
#endif	/* HUMAN68K */
#endif	/* MSDOS */
#ifndef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
			panic("Can't get current directory!");
#endif	/* HUMAN68K */
		ewprintf("Current directory is now %s", wdir);
		return(TRUE);
	}
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
	  if (curbp->b_cwd[0] == '\0') {
	    storecwd(curbp);
	  }
	  (VOID)strcpy(dirname, curbp->b_cwd);
	  len = strlen(dirname) - 1;
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
	  ewprintf("Current directory: %s", dirname);
	}
#else	/* !EXTD_DIR */
#ifdef	MSDOS	/* 90.02.11  by S.Yoshida */
	/* This is for changing floppy disk */
	/* 90.07.01  Add fftolower() by S.Yoshida */
	if (!(wdir = fftolower(getcwd(cwd, NFILEN - 1))))
		panic("Can't get current directory!");
#endif	/* MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
	dirinit ();
#endif	/* HUMAN68K */
	ewprintf("Current directory: %s", wdir);
#endif	/* EXTD_DIR */
	return(TRUE);
}
#endif	/* NO_DIR */