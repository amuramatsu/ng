/* $Id: fileio.c,v 1.14 2001/04/15 16:35:45 amura Exp $ */
/*  OS dependent code used by Ng for WinCE.
 *    Copyright (C) 1998 Eiichiro Ito
 *  Modified for Ng for Win32
 *    Copyright (C) 1999,2000 Toru Imai
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  ree Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * $Log: fileio.c,v $
 * Revision 1.14  2001/04/15 16:35:45  amura
 * patch for VC 6.0 from Katsuyoshi Ohara
 *
 * Revision 1.13  2001/03/02 08:48:32  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.12  2001/02/18 19:29:04  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.11  2001/01/17 18:34:53  amura
 * now compile successfull on VC++ and BC++
 *
 * Revision 1.10  2001/01/05 14:07:08  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.9  2001/01/05 13:55:28  amura
 * filename completion fixed
 *
 * Revision 1.8  2000/12/28 07:27:16  amura
 * homedirctory support with filename complition
 *
 * Revision 1.7  2000/12/27 16:55:44  amura
 * change d_makename() params for conservative reason, and bugfix in dires_()
 *
 * Revision 1.6  2000/12/14 18:10:48  amura
 * filename length become flexible
 *
 * Revision 1.5  2000/11/16 14:21:28  amura
 * merge Ng for win32 0.5
 *
 * Revision 1.4  2000/10/23 16:52:51  amura
 * add GPL copyright to header
 *
 * Revision 1.3  2000/10/23 13:18:45  amura
 * support Windoze line backup file
 *
 * Revision 1.2  2000/09/01 19:37:52  amura
 * support KANJI filename on WIN32
 *
 * Revision 1.1.1.1  2000/06/27 01:47:59  amura
 * import to CVS
 *
 */

#include	<windows.h>
#include	<tchar.h>
#include	"config.h"
#include	"def.h"
#include	"tools.h"
#include "resource.h"

#ifdef KANJI
void	bufetos( char *p, int len ) ;
int		bufstoe_c( char *p, int len ) ;
int		bufstoe( char *p, int len ) ;
#endif
void	kputc( char c, FILE* fp, int kfio ) ;
#if 0
char*	fftolower( char *name ) ;
#endif
int		bclear( BUFFER *bp ) ;
int		addline( BUFFER *bp, char *text ) ;

#ifdef _WIN32_WCE
extern strnicmp(char *, char *, size_t);
#endif

#ifndef HAVE_RINDEX
#define rindex(s,c)	strrchr(s,c)
#endif

TCHAR	g_szPath[ MAX_PATH ] = TEXT("") ;
TCHAR	g_szCurDir[ MAX_PATH ] = TEXT("\\") ;

#define	MALLOC_STEP	256

int
fffiles( char *name, char **buf )
{
	char	pathbuf[ NFILEN ], tmpnam[ NFILEN ] ;
	char	pathbufs[ NFILEN ], ff_namee[ NFILEN ] ;
	char	*cp, *dirpart, *nampart, *buffer ;
	int		n, len, size, dirpartlen, nampartlen, l ;
	HANDLE	hFind ;
	WIN32_FIND_DATA	find ;
	TCHAR	findPath[ MAX_PATH ], unicode[ MAX_PATH ] ;
#ifdef HOMEDIR
	char	*home;
	int	homelen;
#endif

#ifndef _WIN32_WCE
	if (name[1] == ':' && name[2] == '\0') {
	  return -1;
	}
#endif

#ifdef HOMEDIR
	if (name[0] == '~' && (name[1]=='/' || name[1]=='\\') &&
	    (home = getenv("HOME"))) {
		homelen = strlen(home)-1;
		strncpy(pathbuf, home, sizeof(pathbuf));
		pathbuf[NFILEN-1] = '\0';
		strncat(pathbuf, &name[1], sizeof(pathbuf)-strlen(pathbuf)-1);
	} else {
		home = NULL;
		homelen = 0;
		strncpy(pathbuf, name, sizeof(pathbuf));
		pathbuf[NFILEN-1] = '\0';
    	}
#else
	strncpy(pathbuf, name, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
#endif
	dirpart = NULL ;
	for ( cp = pathbuf ; *cp ; cp ++ ) {
		if ( *cp == '/' ) {
			*cp = '\\' ;
			dirpart = cp ;
		} else if ( *cp == '\\' ) {
			dirpart = cp ;
		} else if ( dirpart == NULL && *cp == ':' ) {
			dirpart = cp ;
		}
	}
	if ( dirpart ) {
		*++dirpart = '\0' ;
		dirpartlen = dirpart - pathbuf ;
	} else {
		strcpy( pathbuf, ".\\" ) ;	/* 90.05.30  by A.Shirahashi */
		dirpartlen = 0 ;
	}
#ifdef	HOMEDIR
	nampart = name + dirpartlen - homelen;
#else
	nampart = name + dirpartlen;
#endif
	nampartlen = strlen( nampart ) ;

	buffer = malloc( MALLOC_STEP ) ;
	if ( buffer == NULL ) {
		return -1 ;
	}
	size = MALLOC_STEP ;
	len = 0 ;
	n = 0 ;

	(VOID) strcat( pathbuf, "*.*" ) ;
	strcpy( pathbufs, pathbuf ) ;
#ifdef KANJI
	bufetos( pathbufs, strlen(pathbufs) + 1 ) ;
#endif
	/* Convert the search path to UNICODE and get absolute path */
	sjis2unicode( (LPBYTE) pathbufs, unicode, sizeof unicode ) ;
	rel2abs( findPath, g_szCurDir, unicode ) ;
	/* Find first file */
	hFind = FindFirstFile( findPath, &find ) ;
	if ( hFind == INVALID_HANDLE_VALUE ) {
		*buf = buffer ;
		buffer[0] = '\0' ;
		return 0 ;
	}
	do {
		if ( !lstrcmp( find.cFileName, TEXT(".") ) ) {
			continue ;
		} else if ( !lstrcmp( find.cFileName, TEXT("..") ) ) {
			continue ;
		}
		unicode2sjis( find.cFileName, (LPBYTE) ff_namee, sizeof ff_namee ) ;
#ifdef KANJI
		bufstoe( ff_namee, strlen(ff_namee) + 1 ) ;
#endif
		if ( strnicmp( nampart, ff_namee, nampartlen ) ) {
			continue ;		/* no-case-sensitive comparison */
		}
		strncpy( tmpnam, pathbuf, dirpartlen ) ;
		strcpy( tmpnam + dirpartlen, ff_namee ) ;
		if ( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			strcat( tmpnam, "\\" ) ;
		}
		l = strlen(tmpnam) + 1 ;
		/* 90.05.30  by A.Shirahashi */
		if ( l > 5 && (stricmp( &tmpnam[l-5], ".OBJ" ) == 0 ||
						stricmp( &tmpnam[l-5], ".EXE" ) == 0 ||
						stricmp( &tmpnam[l-5], ".COM" ) == 0 ) ) {
			/* 90.05.30  by A.Shirahashi */
			continue ;
		}
		if ( l + len >= size ) {
			/* make room for double null */
			if ( ( buffer = realloc( buffer, size += MALLOC_STEP ) ) == NULL ) {
			        FindClose(hFind);
				return -1 ;
			}
		}
		/* 90.06.08  by A.Shirahashi: to */
		/* 00.12.28  by amura.. contributed by sahf */
#ifdef HOMEDIR
		if(home) {
			strcpy(buffer+len, "~");
			strcat(buffer+len, tmpnam+homelen+1);
			l -= homelen;
		} else
#endif
		strcpy( buffer + len, tmpnam ) ;
		len += l ;
		n ++ ;
	} while ( FindNextFile( hFind, &find ) ) ;

	FindClose(hFind);
	*buf = buffer ;
	buffer[len] = '\0' ;
	return n ;
}

char *
file_name_part( char *s )
{
	int		i ;

	for (i = strlen (s); i > 0; i--) {
		if (s[i - 1] == '/' || s[i - 1] == '\\' || s[i - 1] == ':') {
			break ;
		}
	}
	return s + i ;
}

#ifndef	NO_DIR
#include <tchar.h>
#include <direct.h>
#ifndef	__BORLANDC__
#define	chdir	_tchdir
#define	getcwd	_tgetcwd
#endif	/* __BORLANDC__ */

extern char	*wdir;
extern char	*startdir;
static char	cwd[NFILEN];

/*
 * Initialize anything the directory management routines need
 */
VOID
dirinit()
{
    char wdir2[NFILEN];
    
    if (!(wdir = getcwd(cwd, NFILEN-1)))
	panic("Can't get current directory!");
    unicode2sjis(wdir, wdir2, sizeof(wdir2));
    strcpy(wdir, wdir2);
#ifdef	KANJI
    bufstoe(wdir, strlen(wdir)+1);
#endif
#ifndef	_WIN32_WCE
    if (wdir[1]==':' && ISUPPER(wdir[0]))
	wdir[0] = TOLOWER(wdir[0]);
#endif	/* _WIN32_WCE */
    if (startdir == NULL) {
	int len = strlen(cwd);
	startdir = malloc(len + 2);
	if (startdir == NULL) {
	    ewprintf("Cannot alloc %d bytes", len + 2);
	    return;
	}
	strcpy(startdir, cwd);
    }
}

/*
   rchdir() makes some effects to change directory.  It will affect
   system to set the actual current directory to the specified one.

   This routine is extracted from changedir(), which is currently do
   some virtual chdir but previously do the actual one.

   Both bufc and wdir should have enough space to store file path, that
   is, as long as NFILEN.

   By Tillanosoft, Mar 22, 1999
 */
int
rchdir(newdir)
char *newdir;
{
    char dir[NFILEN], dir2[NFILEN];
    int i;

    strcpy(dir2, newdir);
#ifdef	KANJI
    bufetos(dir2, strlen(dir2)+1);
#endif
    sjis2unicode(dir2, dir, sizeof(dir));
    i = strlen(dir) - 1;
    if (dir[i] == '\\')
	dir[i] = '\0';
#ifndef	_WIN32_WCE    /* WinCE has no drive */
    if (dir[1] == ':' && dir[0] != wdir[0]) {
	int drive = newdir[0];
	/* 90.07.01  Change from TOUPPER() to TOLOWER() */
	/*                                 by S.Yoshida */
	if (ISUPPER(drive))
	    drive = TOLOWER(drive);
	/* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
	drive = drive - 'a' + 1;
	_chdrive(drive);
    }
    if (dir[1] == ':') {
	if (dir[2]=='\0') {
	    dirinit();
	    return 0;
	} else if (chdir(dir+2) == -1)
	    return -1;
    } else
#endif
    if (chdir(dir) == -1)
	return -1;
    dirinit();
    return 0;
}
#endif

/*
 * The string "fn" is a file name.
 * Perform any required appending of directory name or case adjustments.
 * If NO_DIR is not defined, the same file should be refered to even if the
 * working directory changes.
 */

char *
adjustname( char *fn )
{
	HANDLE hFind;
	TCHAR findPath[MAX_PATH];
	WIN32_FIND_DATA find;
	char		*cp ;
	static char	fnb[ NFILEN ] ;

	cp = fnb ;
#ifndef _WIN32_WCE
	if (fn[1] == ':') {
	  *cp++ = *fn++;
	  *cp++ = *fn++;
	}
#endif
#ifdef HOMEDIR
#ifndef _WIN32_WCE
	else
#endif
	if (fn[0]=='~' && (fn[1]=='/' || fn[1]=='\\')) {
		strcpy(fnb, getenv("HOME"));
		while (*cp) {
    			if (*cp == '/')
    				*cp = '\\';
    			cp++;
    		}
		fn++;
	}
#endif
	switch( *fn ) {
	case '/':
		*fn = '\\' ;
	case '\\':
		*cp++ = *fn++ ;
		break ;
#ifndef _WIN32_WCE
        case '\0':
		break;
#endif
	default:
#ifndef	NO_DIR	/* 91.01.17  NODIR -> NO_DIR. by S.Yoshida */
#if 0 /* something incomprehensive thing is done here */
		extern	char	*wdir ;

		if ( cp == fnb || fnb[0] == wdir[0] ) {
			(VOID) strcpy( fnb, wdir ) ;
			cp = fnb + strlen( fnb ) ;
		} else
#endif
		{			/* Different drive. */
			unicode2sjis( g_szCurDir, (LPBYTE) fnb, NFILEN - 1 ) ;
#if 0 /* to prevent making lower case */
			/* 90.07.01  Add fftolower() by S.Yoshida */
			if ( !fftolower( fnb ) ) {
				cp = fnb ;
			} else {
				cp = fnb + strlen( fnb ) ;
			}
#else /* not 0 */
			cp = fnb ? fnb + strlen(fnb) : fnb;
#endif
		}
		break ;
#else
		return fn ;				/* punt */
#endif
	}
	if ( cp != fnb && cp[-1] != '\\' ) {
		*cp++ = '\\' ;
	}
	while ( *fn ) {
		switch ( *fn ) {
		case '.':
			switch ( fn[1] ) {
			case '\0':
				*--cp = '\0' ;
				return fnb ;
			case '/':
			case '\\':
				fn += 2 ;
				continue ;
			case '.':
				if ( fn[2]=='\\' || fn[2] == '/' || fn[2] == '\0' ) {
					--cp ;
					while ( cp > fnb && *--cp != '\\' ) {
					}
					++cp ;
					if ( fn[2] == '\0' ) {
						*cp = '\0' ;
						goto finalcheck;
					}
					fn += 3 ;
					continue ;
				}
				break ;
			default:
				break ;
			}
			break ;
		case '/':
		case '\\':
			fn ++ ;
			continue ;
		default:
			break ;
		}
		while ( *fn && (*cp++ = *fn++) != '\\' ) {
#if 0
			/* 90.06.05  by S.Yoshida */
			/* 90.06.08  by A.Shirahashi, convert to lower case */
			if ( ISUPPER( cp[-1] ) ) {
				cp[-1] = TOLOWER( cp[-1] ) ;
			}
#endif
			if ( cp[-1] == '/' ) {
				cp[-1] = '\\' ;
				break ;
			}
		}
	}
      finalcheck:
	if ( cp[-1] == '\\' ) {
#ifndef _WIN32_WCE
	        if (cp == &fnb[3] && fnb[1] == ':') {
		  /* It's OK */
		} else
#endif
		if ( cp != &fnb[1] ) {
			--cp ;
		}
	}
	*cp = '\0' ;

	/* Do final confirmation for the case of the file name.  This
	   is very essential because Win32 file system tells the case
	   of file names for writing but does not tell for reading.
	   The purpose here is to confirm the actual case of the
	   existing file name by reading the folder using
	   FindFirstFile(). */
	   
	sjis2unicode(fnb, findPath, MAX_PATH * sizeof(TCHAR));
	hFind = FindFirstFile(findPath, &find);
	if (hFind != INVALID_HANDLE_VALUE) {
	  for (cp = fnb + strlen(fnb) ; fnb < cp ;) {
	    if (*--cp == '\\') {
	      cp++;
	      break;
	    }
	  }
	  unicode2sjis(find.cFileName, cp, NFILEN - (cp - fnb));
	  FindClose(hFind);
	}
	return fnb ;
}

/*
 * Open a file for reading.
 */
int
ffropen( char *fn )
{
	CHAR	fns[ NFILEN ] ;
	TCHAR	unicode[ MAX_PATH ] ;

	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( fns, strlen(fns) + 1 ) ;
#endif
	unicode[0] = 0 ;
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( g_szPath, g_szCurDir, unicode ) ;
	if ( !Fopen( g_szPath, TEXT("r") ) ) {
		return FIOFNF ;
	}
	return FIOSUC ;
}

/*
 * Open a file for writing.
 * Return TRUE if all is well, and
 * FALSE on error (cannot create).
 */
int
ffwopen( char *fn )
{
	CHAR	fns[ NFILEN ] ;
	TCHAR	unicode[ MAX_PATH ] ;

	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( fns, strlen(fns) + 1 ) ;
#endif
	unicode[0] = 0 ;
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( g_szPath, g_szCurDir, unicode ) ;
	if ( !Fopen( g_szPath, TEXT("w") ) ) {
		ewprintf( "Cannot open file for writing" ) ;
		return FIOERR ;
	}
	return FIOSUC ;
}

/*
 * Close a file.
 * Should look at the status.
 */
int
ffclose()
{
	Fclose() ;
	return FIOSUC ;
}

/*
 * Write a buffer to the already
 * opened file. bp points to the
 * buffer. Return the status.
 * Check only at the newline and
 * end of buffer.
 */
int
ffputbuf( BUFFER *bp )
{
	char	*cp ;
	char	*cpend ;
	LINE	*lp ;
	LINE	*lpend ;
#ifdef KANJI
	int		kfio ;
#endif

	lpend = bp->b_linep ;
#ifdef KANJI
	kfio  = bp->b_kfio ;
#endif
	lp = lforw(lpend) ;
	do {
		cp = &ltext(lp)[0] ;		/* begining of line	*/
		cpend = &cp[llength(lp)] ;	/* end of line		*/
		while ( cp != cpend ) {
#ifdef KANJI
			kputc( *cp, NULL, kfio ) ;
#else
			putc(*cp, NULL);
#endif
			cp ++ ;		/* putc may evalualte arguments more than once */
		}
#ifdef KANJI
		if ( kfio == JIS ) {
			kfselectcode(NULL, FALSE) ;
		}
#endif
		lp = lforw( lp ) ;
		if ( lp == lpend ) {
			break ;		/* no implied newline on last line */
		}
		putc( '\n', NULL) ;
	} while( !Ferror() ) ;
	if ( Ferror() ) {
		ewprintf( "Write I/O error" ) ;
		return FIOERR ;
	}
	return FIOSUC ;
}

/*
 * Read a line from a file, and store the bytes
 * in the supplied buffer. Stop on end of file or end of
 * line.  When FIOEOF is returned, there is a valid line
 * of data without the normally implied \n.
 */
int
ffgetline( char *buf, int nbuf, int *nbytes )
{
	int		c ;
	int		i ;

	i = 0 ;
	while ( ( c = Fgetc() ) != -1 && c != '\n' ) {
		buf[ i++ ] = c ;
		if ( i >= nbuf ) {
			return FIOLONG ;
		}
	}
	if ( c == -1  && Ferror() != FALSE ) {
		ewprintf( "File read error" ) ;
		return FIOERR ;
	}
	*nbytes = i ;
	return c == -1 ? FIOEOF : FIOSUC ;
}

#ifdef	READONLY
/*
 * Check whether file is read-only of a file fn.
 */
int
fchkreadonly( char *fn )
{
	DWORD	attr ;
	char	fns[ NFILEN ] ;
	TCHAR	szPath[ MAX_PATH ], unicode[ MAX_PATH ] ;

	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( fns, strlen( fns ) + 1 ) ;
#endif
	/* Generate absolute path based on the current directory */
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( szPath, g_szCurDir, unicode ) ;
	attr = GetFileAttributes( szPath ) ;
	if ( attr == 0xFFFFFFFF ) {
		return FALSE ;
	}
	return attr & FILE_ATTRIBUTE_READONLY ;
}
#endif	/* READONLY */

#ifndef	NO_STARTUP

/*
 * Find a startup file for the user and return its name. As a service
 * to other pieces of code that may want to find a startup file (like
 * the terminal driver in particular), accepts a suffix to be appended
 * to the startup file name.
 */
/*ARGSUSED*/
char *
#ifdef	ADDOPT
startupfile(ngrcfile, suffix)
char* ngrcfile;
#else
startupfile(suffix)
#endif
char* suffix;
{
  static BYTE sjis[MAX_PATH];
  TCHAR	unicode[MAX_PATH];
  DWORD foo = sizeof(unicode);

#ifdef ADDOPT
  if (ngrcfile) {
    lstrcpy(unicode, ngrcfile);
    if (GetFileAttributes(unicode) != 0xFFFFFFFF) {
#ifdef	_WIN32_WCE
	  unicode2sjis(unicode, sjis, sizeof sjis);
#else
	  strncpy(sjis, unicode,  sizeof sjis);
	  sjis[(sizeof sjis)-1] = '\0';
#endif
#ifdef KANJI
	  bufstoe(sjis, strlen(sjis)+1);
#endif
      return (char*)sjis;
    }
  }
#endif
  if (RegQueryString(HKEY_CURRENT_USER, NGREGKEY, NGSTARTUPFILEVAL,
		     unicode, &foo) != ERROR_SUCCESS) {
    if (GetFileAttributes(unicode) == 0xFFFFFFFF) {
      lstrcpy(unicode, NGDEFAULTINIFILE);
      if (GetFileAttributes(unicode) == 0xFFFFFFFF) {
	lstrcpy(unicode, TEXT("\\Storage Card") NGDEFAULTINIFILE);
	if (GetFileAttributes(unicode) == 0xFFFFFFFF) {
	  return NULL;
	}
      }
    }
  }
  unicode2sjis(unicode, sjis, sizeof sjis);
#ifdef	KANJI
  bufstoe(sjis, strlen(sjis)+1);
#endif
  return (char*)sjis;
}

#endif	/* NO_STARTUP */

#ifndef NO_DIRED
#include "kbd.h"

int		mkfileline( LPSTR *_line, WIN32_FIND_DATA *info ) ;

int
unlink( const char *fn )
{
	char	fns[ NFILEN ] ;
	TCHAR	unicode[ MAX_PATH ], szPath[ MAX_PATH ] ;

	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( fns, strlen( fns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( szPath, g_szCurDir, unicode ) ;

	return DeleteFile( szPath ) ? 0 : -1 ;
}

int
rmdir( const char *fn )
{
	char	fns[ NFILEN ] ;
	TCHAR	unicode[ MAX_PATH ], szPath[ MAX_PATH ] ;

	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( fns, strlen( fns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( szPath, g_szCurDir, unicode ) ;
	return RemoveDirectory( szPath ) ? 0 : -1 ;
}

int
rename( const char *oldfn, const char *newfn )
{
	static	char	oldfns[ NFILEN ], newfns[ NFILEN ] ;
	static	TCHAR	unicode[ MAX_PATH ], szOldPath[ MAX_PATH ], szNewPath[ MAX_PATH ] ;

	strcpy( oldfns, oldfn ) ;
#ifdef KANJI
	bufetos( oldfns, strlen( oldfns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) oldfns, unicode, sizeof unicode ) ;
	rel2abs( szOldPath, g_szCurDir, unicode ) ;

	strcpy( newfns, newfn ) ;
#ifdef KANJI
	bufetos( newfns, strlen( newfns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) newfns, unicode, sizeof unicode ) ;
	rel2abs( szNewPath, g_szCurDir, unicode ) ;

	return MoveFile( szOldPath, szNewPath ) ? 0 : -1 ;
}

int
copy( char *oldfn, char *newfn )
{
	static	char	oldfns[ NFILEN ], newfns[ NFILEN ] ;
	static	TCHAR	unicode[ MAX_PATH ], szOldPath[ MAX_PATH ], szNewPath[ MAX_PATH ] ;

	strcpy( oldfns, oldfn ) ;
#ifdef KANJI
	bufetos( oldfns, strlen( oldfns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) oldfns, unicode, sizeof unicode ) ;
	rel2abs( szOldPath, g_szCurDir, unicode ) ;

	strcpy( newfns, newfn ) ;
#ifdef KANJI
	bufetos( newfns, strlen( newfns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) newfns, unicode, sizeof unicode ) ;
	rel2abs( szNewPath, g_szCurDir, unicode ) ;

	return CopyFile( szOldPath, szNewPath, FALSE ) ? 0 : -1 ;
}

BUFFER *
dired_( char *dirname )
{
	BUFFER	*bp ;
	BUFFER	*findbuffer() ;
	char	**files, **getfilelist() ;
	int		numfiles ;
	int		i ;

	if ( ( dirname = adjustname( dirname ) ) == NULL ) {
		ewprintf( "Bad directory name" ) ;
		return NULL ;
	}
	if ( dirname[ strlen( dirname ) - 1 ] != '\\' ) {
		(VOID) strcat( dirname, "\\" ) ;
	}
	if ( ( bp = findbuffer( dirname ) ) == NULL ) {
		ewprintf( "Could not create buffer" ) ;
		return NULL ;
	}
	if ( bclear( bp ) != TRUE ) {
		return FALSE ;
	}
	if ( ( files = getfilelist( &numfiles, dirname ) ) == NULL ) {
		ewprintf( "Could not get directory info" ) ;
		return NULL ;
	}
	for ( i = 0 ; i < numfiles ; i ++ ) {
		(VOID) addline( bp, files[i] ) ;
		free( files[i] ) ;
	}
	free( files ) ;
	bp->b_dotp = lforw( bp->b_linep ) ;		/* go to first line */
	if (bp->b_fname != NULL) free(bp->b_fname);
	if ((bp->b_fname=malloc(strlen(dirname)+1)) != NULL)
		(VOID) strcpy(bp->b_fname, dirname);
#ifdef EXTD_DIR
	if(bp->b_cwd != NULL) free(bp->b_cwd);
	bp->b_cwd = NULL;
#endif
	if ( ( bp->b_modes[0] = name_mode( "dired" ) ) == NULL ) {
		bp->b_modes[0] = &map_table[0] ;
		ewprintf( "Could not find mode dired" ) ;
		return NULL ;
	}
	bp->b_nmodes = 0 ;
	return bp ;
}

int
d_makename( LINE *lp, char *fn, int buflen)
{
	char	*cp ;
	int len;

	if ( llength( lp ) <= 41 ) {
		return ABORT ;
	}
	len = strlen(curbp->b_fname) + llength(lp) - 41;
	if (buflen <= len) return ABORT;
	cp = fn;
	strcpy(cp, curbp->b_fname);
	cp += strlen(cp);
	bcopy(lp->l_text + 41, cp, llength(lp) - 41);
	fn[len] = '\0';
	return lgetc(lp, 2) == 'd';
}

static int __cdecl
#ifdef __BORLANDC__
filelinecmp(const void *x, const void *y)
#else
filelinecmp(const char **x, const char **y)
#endif
{
	register unsigned char	*xx, *yy;

#ifdef __BORLANDC__
	xx = (unsigned char*)&(*(char **)x)[41];
	yy = (unsigned char*)&(*(char **)y)[41];
#else
	xx = (unsigned char*)&(*x)[41];
	yy = (unsigned char*)&(*y)[41];
#endif

	if (*xx != *yy) {		/* for "." ".." directories */
		if (*xx == '.') return -1;
		if (*yy == '.') return 1;
	  	return(*xx - *yy);
	}

	if (*xx) {
		for (xx++, yy++; *xx && *xx == *yy; xx++, yy++) {}
	}
	return(*xx - *yy);
}

char **
getfilelist( int *numfiles, char *dirname )
{
  char **files;
  int maxfiles, n;
  char filename[NFILEN];
  char filenames[NFILEN];
  HANDLE hFind = INVALID_HANDLE_VALUE;
  TCHAR	unicode[MAX_PATH], findPath[MAX_PATH];
  WIN32_FIND_DATA find;
  
  n = 0; /* store n to *numfiles later */
  if (strlen(dirname) + 4 > NFILEN) {
    *numfiles = n;
    return NULL;
  }
  (VOID)strcpy(filename, dirname);
  (VOID)strcat(filename, "*.*");
  
  maxfiles = 50; /* Magic number here.  Should be eliminated */
  files = (char **)malloc(maxfiles * sizeof(char *));
  if (files) {
    memset(files, 0, maxfiles * sizeof(char *));
  
    strcpy(filenames, filename);
#ifdef KANJI
    bufetos(filenames, strlen(filenames) + 1);
#endif
    /* Convert the search path to UNICODE and get absolute path */
    sjis2unicode((LPBYTE)filenames, unicode, sizeof(unicode));
    rel2abs(findPath, g_szCurDir, unicode);
    /* Add ".." except for root dir */
    if (*dirname && strcmp(dirname, "\\")) {
      if (!mkfileline(files + n, NULL)) {
	goto failed;
      }
      n++;
    }
    /* Find first file */
    hFind = FindFirstFile(findPath, &find);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
	if (!lstrcmp(find.cFileName, TEXT("."))) {
	  continue; /* ignore '.' */
	} else if (!lstrcmp(find.cFileName, TEXT(".."))) {
	  continue; /* ignore '..' */
	}
	if (n >= maxfiles) {
	  files = (char **)realloc(files, sizeof(char *) * (maxfiles + 20));
	  if (files == NULL) {
	    goto failed;
	  }
	  maxfiles += 20;
	}
	if (mkfileline(files + n, &find)) {
	  n++;
	}
      } while (FindNextFile(hFind, &find));

      FindClose(hFind);
      qsort(files, n, sizeof(char *), filelinecmp);
      *numfiles = n;
      return files;
    }
    else {
      DWORD err = GetLastError();

      if (err == ERROR_NO_MORE_FILES || err == ERROR_FILE_NOT_FOUND) {
	*numfiles = n; /* 'n' must be 1 */
	return files;
      }
    }
  }
 failed:
  if (hFind != INVALID_HANDLE_VALUE) {
    FindClose(hFind);
  }
  if (files) {
    char **pp, **epp;
    for (pp = files, epp = pp + n ; pp < epp ; pp++) {
      free(*pp);
    }
    free(files);
  }
  *numfiles = 0;
  return NULL;
}

int
mkfileline( LPSTR *lptr, WIN32_FIND_DATA *info )
{
	SYSTEMTIME	st ;
	FILETIME	lft ;
	int			len ;
	BYTE		sjis[ MAX_PATH ] ;
	TCHAR		unicode[ MAX_PATH ], *line = unicode ;

	line[0] = line[1] = TEXT(' ') ;
	line[2] = !info || (info->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? TEXT('d') : TEXT('-') ;
	line[3] = TEXT('r') ;
	line[4] = !info || (info->dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? TEXT('-') : TEXT('w') ;
	line[5] = TEXT('-') ;
	line[6] = info && (info->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? TEXT('a') : TEXT('-') ;
	wsprintf( &line[7], TEXT("%13d"), info ? info->nFileSizeLow : 0 ) ;
	if ( info
	  && FileTimeToLocalFileTime( &info->ftLastWriteTime, &lft )
	  && FileTimeToSystemTime( &lft, &st ) ) {
		wsprintf( &line[20], TEXT("  %4d-%02d-%02d  %02d:%02d  "),
				  st.wYear, st.wMonth, st.wDay,
				  st.wHour, st.wMinute ) ;
	} else {
		lstrcpy( &line[20], TEXT("                     ") ) ;
	}
	lstrcpy( &line[41], info ? info->cFileName : TEXT("..") ) ;
	unicode2sjis( unicode, sjis, sizeof sjis ) ;
#ifdef KANJI
	len = bufstoe_c( sjis, strlen( sjis ) + 1 ) ;
#else
	len = strlen(sjis) + 1;
#endif
	(*lptr) = (LPSTR) malloc( len * sizeof (CHAR) ) ;
	if ( !*lptr ) {
		return FALSE ;
	}
	strcpy( *lptr, sjis ) ;
#ifdef KANJI
	(VOID) bufstoe( &(*lptr)[41], strlen( &(*lptr)[41] ) + 1 ) ;
#endif
#if 0
	(VOID) fftolower( &(*lptr)[41] ) ;
#endif
	return TRUE ;
}

/*
 * Check whether file "dn" is directory.
 */
int
ffisdir( char *dn )
{
	DWORD	attr ;
	char	dns[ NFILEN ] ;
	TCHAR	szPath[ MAX_PATH ], szUnicode[ MAX_PATH ] ;

	strcpy( dns, dn ) ;
#ifdef KANJI
	bufetos( dns, strlen( dns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) dns, szUnicode, sizeof szUnicode ) ;
	/* Generate absolute path based on the current directory */
	rel2abs( szPath, g_szCurDir, szUnicode ) ;
	attr = GetFileAttributes( szPath ) ;
	if ( attr == 0xFFFFFFFF ) {
#if 0
		DWORD err = GetLastError();
#endif
		return FALSE ;
	}
	return attr & FILE_ATTRIBUTE_DIRECTORY ;
}
#endif

#if 0
/* 90.07.01  Add function to convert strings into lower case by S.Yoshida */
char *
fftolower( char *name )
{
	char	*p ;

	if ( (p = name) != NULL ) {
		while ( *p != '\0' ) {
			if ( ISUPPER( *p ) ) {
				*p = TOLOWER( *p ) ;
			}
			p ++ ;
		}
	}
	return name ;
}
#endif

int
chdir( const char *dir )
{
	DWORD	attr ;
	TCHAR	unicode[ MAX_PATH ], szPath[ MAX_PATH ] ;

	sjis2unicode( (LPBYTE) dir, unicode, sizeof unicode ) ;
	rel2abs( szPath, g_szCurDir, unicode ) ;
	attr = GetFileAttributes( szPath ) ;
	if ( attr == 0xFFFFFFFF || !(attr & FILE_ATTRIBUTE_DIRECTORY) ) {
		return -1 ;
	}
	lstrcpy( g_szCurDir, szPath ) ;
	return 0 ;
}

#ifndef	NO_BACKUP
/*
 * Get file mode of a file fn.
 */
int
fgetfilemode( char *fn )
{
	DWORD	attr ;
	char	fns[ NFILEN ] ;
	TCHAR	unicode[ MAX_PATH ], szPath[ MAX_PATH ] ;

	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( fns, strlen( fns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( szPath, g_szCurDir, unicode ) ;
	attr = GetFileAttributes( szPath ) ;
	if ( attr == 0xFFFFFFFF ) {
		return -1 ;
	}
	return (int) attr ;
}

/*
 * Set file mode of a file fn to the specified mode.
 */
VOID
fsetfilemode( char *fn, int mode )
{
	char	fns[ NFILEN ] ;
	TCHAR	unicode[ MAX_PATH ], szPath[ MAX_PATH ] ;

	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( fns, strlen( fns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( szPath, g_szCurDir, unicode ) ;
	SetFileAttributes( szPath, (DWORD) mode ) ;
}

/*
 * Rename the file "fname" into a backup
 * copy. On Unix the backup has the same name as the
 * original file, with a "~" on the end; this seems to
 * be newest of the new-speak. The error handling is
 * all in "file.c".
 */
int
fbackupfile( char *fn )
{
	int	len;
	char	*nname ;
	VOID	strmfe() ;	/* 90.07.26  Add by N.Kamei */
	char	fns[ NFILEN ] ;
	char	nnames[ NFILEN ] ;
	TCHAR unicode[NFILEN];
	TCHAR	szFns[ MAX_PATH ] ;
	TCHAR	szNNames[ MAX_PATH ] ;

#ifdef EMACS_BACKUP_STYLE
	len = strlen(fn) + 1 + 1;
#else
	len = strlen(fn) + 4 + 1;
#endif
	if ( ( nname = alloca(len) ) == NULL ) {
		ewprintf( "Can't get %d bytes", len) ;
		return ABORT ;
	}
	(VOID) strcpy(nname, fn);
#ifdef EMACS_BACKUP_STYLE
	(VOID) strcat(nname, "~");
#else
	(VOID) strcat(nname, ".bak");
#endif
	strcpy( nnames, nname ) ;
	strcpy( fns, fn ) ;
#ifdef KANJI
	bufetos( nnames, strlen( nnames ) + 1 ) ;
	bufetos( fns, strlen( fns ) + 1 ) ;
#endif
	sjis2unicode( (LPBYTE) nnames, unicode, sizeof unicode ) ;
	rel2abs( szNNames, g_szCurDir, unicode ) ;
	sjis2unicode( (LPBYTE) fns, unicode, sizeof unicode ) ;
	rel2abs( szFns, g_szCurDir, unicode ) ;
	DeleteFile( szNNames ) ;
	if ( MoveFile( szFns, szNNames ) < 0 )
		return FALSE ;
	return TRUE ;
}
#endif	/* NO_BACKUP */

#ifdef	AUTOSAVE
VOID
autosave_name(buff, name, buflen)
char* buff;
char* name;
{
    strcpy(buff, name);
    if (strlen(name)) {
	char *fn = rindex(name, '/');
	if (fn == NULL)
	    fn = rindex(name, '\\');
	if (fn == NULL)
	    fn = rindex(name, ':');
	if (fn == NULL){
	    fn = buff;
	} else {
	    fn++;
	}
	strcpy(&buff[strlen(buff)-strlen(fn)], "#");
	strcat(buff, fn);
	strcat(buff, "#");
    }
}
#endif	/* AUTOSAVE */
