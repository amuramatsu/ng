/* $Id: dir.c,v 1.11 2000/12/14 18:06:23 amura Exp $ */
/*
 * Name:	MG 2a
 *		Directory management functions
 * Created:	Ron Flax (ron@vsedev.vse.com)
 *		Modified for MG 2a by Mic Kaczmarczik 03-Aug-1987
 */

/*
 * $Log: dir.c,v $
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
#ifdef	WIN32
#include <tchar.h>
#include <direct.h>
int	sjis2unicode(), unicode2sjis();
#ifndef	__BORLANDC__
#define	chdir	_tchdir
#define	getcwd	_tgetcwd
#endif	/* __BORLANDC__ */
#define	HAVE_GETCWD 1
#else	/* NOT WIN32 */
#ifdef	HAVE_GETCWD
char	*getcwd();
#else
char	*getwd();
#endif
#endif	/* WIN32 */
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
static char *startdir = NULL;

/*
 * Initialize anything the directory management routines need
 */
VOID
dirinit()
{
#ifdef	WIN32
	char wdir2[NFILEN];
#endif
#ifdef	MSDOS	/* 90.01.29  by S.Yoshida */
	/* 90.07.01  Add fftolower() by S.Yoshida */
	if (!(wdir = fftolower(getcwd(cwd, NFILEN - 1))))
#else	/* NOT MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
	if (!(wdir = tounixfn(getcwd(cwd, NFILEN - 1))))
#else	/* NOT HUMAN68K && NOT MSDOS*/
#ifdef HAVE_GETCWD
	if (!(wdir = getcwd(cwd, NFILEN-1)))
#else
	if (!(wdir = getwd(cwd)))
#endif
#endif	/* HUMAN68K */
#endif	/* MSDOS */
		panic("Can't get current directory!");
#ifdef	WIN32
	unicode2sjis(wdir, wdir2, sizeof(wdir2));
	strcpy(wdir, wdir2);
#ifdef	KANJI
	bufstoe(wdir, strlen(wdir)+1);
#endif
#ifndef	_WIN32_WCE
	if (wdir[1]==':' && ISUPPER(wdir[0]))
		wdir[0] = TOLOWER(wdir[0]);
#endif	/* _WIN32_WCE */
#else	/* WIN32 */
#if	defined(MSDOS)||defined(HUMAN68K)
#ifdef	KANJI
	bufstoe(wdir, strlen(wdir)+1);
#endif
#endif	/* MSDOS||HUMAN68K */
#endif	/* WIN32 */
	if (startdir == NULL) {
		int len = strlen(cwd);
		startdir = malloc(len + 2);
		if (startdir == NULL) {
		    ewprintf("Cannot alloc %d bytes", len + 2);
		    return;
		}
		strcpy(startdir, cwd);
#ifdef EXTD_DIR
		if (len > 1) {
#ifdef BDC2
		    if (startdir[len-1] != BDC1 && startdir[len-1] != BDC2) {
			startdir[len] = BDC2;
			startdir[len+1] = '\0';
		    }
#else
		    if (startdir[len-1] != BDC1) {
			startdir[len] = BDC1;
			startdir[len+1] = '\0';
		    }
#endif
		}
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
 * Modified by amura, Dec 1, 2000.
 */
VOID
storecwd(bp)
BUFFER *bp;
{
    char tmp[NFILEN];
    char *path;

    if (bp) {
	if (bp->b_fname) {
	    makepath(tmp, bp->b_fname, NFILEN);
	    if (tmp[0] == '\0')
		path = startdir;
	    else
		path = tmp;
	}
	else
	    path = startdir;
	if (bp->b_cwd)
	    free(bp->b_cwd);
	if ((bp->b_cwd=malloc(strlen(path)+1)) == NULL)
	    return;
	strcpy(bp->b_cwd, path);
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
#if defined(MSDOS)||defined(HUMAN68K)||(defined(WIN32)&&!defined(_WIN32_WCE))
    char dir[NFILEN];
#ifdef WIN32
    char dir2[NFILEN];
#endif
    int i;

    strcpy(dir, newdir);
#ifdef	KANJI
    bufetos(dir, strlen(dir)+1);
#endif
#ifdef	WIN32
    sjis2unicode(dir, dir2, sizeof(dir2));
    strcpy(dir, dir2);
#endif
    i = strlen(dir) - 1;
#ifdef	BDC2
    if (dir[i] == BDC1 || dir[i] == BDC2)
#else
    if (dir[i] == BDC1)
#endif
	dir[i] = '\0';
    if (dir[1] == ':' && dir[0] != wdir[0]) {
	int	drive;
	drive = newdir[0];
	/* 90.07.01  Change from TOUPPER() to TOLOWER() */
	/*                                 by S.Yoshida */
	if (ISUPPER(drive))
	    drive = TOLOWER(drive);
	/* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
	drive = drive - 'a' + 1;
#ifdef	WIN32
	_chdrive(drive);
#else
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
#endif	/* WIN32 */
    }
    if (dir[1] == ':' && dir[2] == '\0') {
	dirinit();
	return 0;
    } else if (dir[1]==':' && chdir(dir+2) == -1) {
	return -1;
    } else if (chdir(dir) == -1) {
	return -1;
    } else {
	dirinit();
	return 0;
    }
#else	/* NOT (MSDOS||HUMAN68K||WIN32) */
#ifdef	_WIN32_WCE
    /* WinCE has no drive */
    char dir[NFILEN], dir2[NFILEN];
    strcpy(dir, newdir);
#ifdef	KANJI
    bufetos(dir, strlen(dir)+1);
#endif
    sjis2unicode(dir, dir2, sizeof(dir2));
    return chdir(dir2);
#else	/* not _WIN32_WCE&&KANJI */
    /* Maybe this is just for AMIGA, UNIX */
    return chdir(newdir);
#endif	/* _WIN32_WCE&&KANJI */
#endif	/* MSDOS||HUMAN68K||WIN32 */
}

VOID
vchdir(newdir)
char *newdir;
{
    if (curbp) {
	if (curbp->b_cwd)
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
#ifdef	WIN32
	char bufc1[NPAT],bufc2[NPAT];
	char *bufc = bufc1;
#else
	char bufc[NPAT];
#endif

#ifndef	NO_FILECOMP	/* 90.04.04  by K.Maeda */
	if ((s=eread("Change default directory: ", bufc, NPAT, EFNEW|EFFILE|EFCR)) != TRUE)
#else	/* NO_FILECOMP */
	if ((s=ereply("Change default directory: ", bufc, NPAT)) != TRUE)
#endif	/* NO_FILECOMP */
		return(s);
	if (bufc[0] == '\0')
		(VOID) strcpy(bufc, wdir);
#ifdef	WIN32	/* 90.02.11  by S.Yoshida */
#ifdef	_WIN32_WCE
#ifdef	KANJI
	bufetos(bufc1, strlen(bufc1)+1);
#endif	/* KANJI */
	sjis2unicode(bufc1, bufc2, sizeof(bufc2));
	bufc = bufc2;
#else	/* not _WIN32_WCE */
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
		_chdrive(drive);
	}
#ifdef	KANJI
	bufetos(bufc1, strlen(bufc1)+1);
#endif
	sjis2unicode(bufc1, bufc2, sizeof(bufc2));
	bufc = bufc2;
	if (bufc1[1] == ':' && bufc1[2] == '\0') {
		/* 90.07.01  Add fftolower() by S.Yoshida */
		if (!(wdir = getcwd(cwd, NFILEN - 1)))
			panic("Can't get current directory!");
		ewprintf("Current directory is now %s", wdir);
	} else
#endif	/* _WIN32_WCE */
#endif	/* WIN32 */
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
#ifdef	KANJI
	bufetos(bufc, strlen(bufc)+1);
#endif
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
#ifdef	KANJI
	bufetos(bufc, strlen(bufc)+1);
#endif
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
	  if (curbp->b_cwd == NULL) {
	    storecwd(curbp);
	  }
	  if (curbp->b_cwd == NULL)
	      dirname[0] = '\0';
	  else {
	      (VOID)strcpy(dirname, curbp->b_cwd);
	      len = strlen(dirname) - 1;
	  }
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
#ifdef	KANJI
	bufstoe(wdir, strlen(wdir)+1);
#endif
#endif	/* MSDOS */
#if defined(HUMAN68K)||defined(WIN32)	/* Sawayanagi / WIN32 added by amura */
	dirinit ();
#endif	/* HUMAN68K || WIN32 */
	ewprintf("Current directory: %s", wdir);
#endif	/* EXTD_DIR */
	return(TRUE);
}
#endif	/* NO_DIR */
