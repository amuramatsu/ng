/*
 * Name:	MG 2a
 *		Directory management functions
 * Created:	Ron Flax (ron@vsedev.vse.com)
 *		Modified for MG 2a by Mic Kaczmarczik 03-Aug-1987
 */
/* 90.01.29	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

/* $Id: dir.c,v 1.1 1999/05/21 02:20:08 amura Exp $ */

/* $Log: dir.c,v $
/* Revision 1.1  1999/05/21 02:20:08  amura
/* Initial revision
/*
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
char	*getwd();
#endif	/* HUMAN68K */
#endif	/* MSDOS */
#endif
char	*wdir;
static char cwd[NFILEN];

/*
 * Initialize anything the directory management routines need
 */
dirinit()
{
#ifdef	MSDOS	/* 90.01.29  by S.Yoshida */
	/* 90.07.01  Add fftolower() by S.Yoshida */
	if (!(wdir = fftolower(getcwd(cwd, NFILEN - 1))))
#else	/* NOT MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
	if (!(wdir = tounixfn(getcwd(cwd, NFILEN - 1))))
#else	/* NOT HUMAN68K */
	if (!(wdir = getwd(cwd)))
#endif	/* HUMAN68K */
#endif	/* MSDOS */
		panic("Can't get current directory!");
}

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
		(void) setdisk(drive - 1);
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
		if (!(wdir = getwd(cwd)))
#endif	/* HUMAN68K */
#endif	/* MSDOS */
#ifndef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
			panic("Can't get current directory!");
#endif	/* HUMAN68K */
		ewprintf("Current directory is now %s", wdir);
		return(TRUE);
	}
}

/*
 * Show current directory
 */
/*ARGSUSED*/
showcwdir(f, n)
{
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
	return(TRUE);
}
#endif
