/* $Id: tools.c,v 1.9 2000/12/14 18:12:14 amura Exp $ */
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
 * NG : C library compatible routine for Ng
 *
 * 1997/11/06:Eiichiroh Itoh
 *  (1) Start development
 * History of modifications
 * 1998/04/03:Eiichiroh Itoh
 *  (1) Convert from IrCopy for POBox
 *
 */

/*
 * $Log: tools.c,v $
 * Revision 1.9  2000/12/14 18:12:14  amura
 * use alloca() and more memory secure
 *
 * Revision 1.8  2000/11/16 14:21:31  amura
 * merge Ng for win32 0.5
 *
 * Revision 1.7  2000/10/23 16:52:51  amura
 * add GPL copyright to header
 *
 * Revision 1.6  2000/10/02 16:13:07  amura
 * ignore mouse event in minibuffer editing
 *
 * Revision 1.5  2000/09/01 19:39:12  amura
 * fix choped data bug on receiving from clipboard
 *
 * Revision 1.4  2000/07/25 15:04:21  amura
 * fix filevisit() args
 *
 * Revision 1.3  2000/07/22 20:46:32  amura
 * support "Drag&Drop"
 *
 * Revision 1.2  2000/07/18 12:40:35  amura
 * for Win32, enable to handle japanese directory
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#include	<windows.h>
#include	<tchar.h>
#include	"config.h"
#include	"tools.h"
#include "def.h"
#define	etos(c1, c2)	\
{\
	c1 &= 0x7f;\
	c2 &= 0x7f;\
	if(c1 >= 0x5f)\
		c1 += 0x80;\
	if((c1 % 2) == 0) {\
		c1 = (c1 - 0x30) / 2 + 0x88;\
		c2 += 0x7e;\
	} else {\
		if(c2 >= 0x60)\
			c2 += 0x01;\
		c1 = (c1 - 0x31) / 2 + 0x89;\
		c2 += 0x1f;\
	}\
	c1 &= 0xff;\
	c2 &= 0xff;\
}

HANDLE		g_hfile1 = INVALID_HANDLE_VALUE ;
BOOL		g_bEof1 = FALSE ;
BOOL		g_bBinary1 = FALSE ;
BOOL		g_bError1 = FALSE ;
DWORD		g_dwRin1 = 0 ;
DWORD		g_dwRout1 = 0 ;
DWORD		g_dwWin1 = 0 ;
BYTE		g_rBuf1[ 256 ] ;
BYTE		g_sBuf1[ 256 ] ;

BOOL
Fopen( LPCTSTR path, LPCTSTR modestr )
{
	BOOL	f_trunc = FALSE, f_last = FALSE ;
	DWORD	access = GENERIC_READ ;
	DWORD	mode = FILE_SHARE_READ ;
	DWORD	dist = OPEN_EXISTING ;

	g_bBinary1 = FALSE ;
	g_bError1 = FALSE ;
	if ( *modestr == TEXT('r') ) {
		access = GENERIC_READ ;
		dist = OPEN_EXISTING ;
		if ( modestr[1] == TEXT('+') ) {
			access = GENERIC_READ|GENERIC_WRITE ;
			dist = OPEN_ALWAYS ;
			if ( modestr[2] == TEXT('b') ) {
				g_bBinary1 = TRUE ;
			}
		} else if ( modestr[1] == TEXT('b') ) {
			g_bBinary1 = TRUE ;
		}
	} else if ( *modestr == TEXT('w') ) {
		access = GENERIC_WRITE ;
		dist = CREATE_ALWAYS ;
		f_trunc = TRUE ;
		if ( modestr[1] == TEXT('+') ) {
			access = GENERIC_READ|GENERIC_WRITE ;
			if ( modestr[2] == TEXT('b') ) {
				g_bBinary1 = TRUE ;
			}
		} else if ( modestr[1] == TEXT('b') ) {
			g_bBinary1 = TRUE ;
		}
	} else if ( *modestr == TEXT('a') ) {
		access = GENERIC_WRITE ;
		dist = CREATE_ALWAYS ;
		f_last = TRUE ;
		if ( modestr[1] == TEXT('+') ) {
			access = GENERIC_READ|GENERIC_WRITE ;
			if ( modestr[2] == TEXT('b') ) {
				g_bBinary1 = TRUE ;
			}
		} else if ( modestr[1] == TEXT('b') ) {
			g_bBinary1 = TRUE ;
		}
	} else {
		return FALSE ;
	}
	g_hfile1 = CreateFile( path, access, mode, NULL,
						 dist, FILE_ATTRIBUTE_NORMAL, 0 ) ;
	if ( g_hfile1 == INVALID_HANDLE_VALUE ) {
		return FALSE ;
	}
	if ( f_trunc ) {
		SetEndOfFile( g_hfile1 ) ;
	} else if ( f_last ) {
		SetFilePointer( g_hfile1, 0, NULL, FILE_END ) ;
	}
	g_bEof1 = FALSE ;
	g_dwRin1 = g_dwRout1 = g_dwWin1 = 0 ;
	return TRUE ;
}

void
Fclose( void )
{
	DWORD	nWrite ;

	if ( g_hfile1 != INVALID_HANDLE_VALUE ) {
		if ( g_dwWin1 ) {
			WriteFile( g_hfile1, g_sBuf1, g_dwWin1, &nWrite, NULL ) ;
		}
		CloseHandle( g_hfile1 ) ;
		g_hfile1 = INVALID_HANDLE_VALUE ;
	}
}

int
Fgetc( void )
{
	int		c ;
	BOOL	ret ;
	DWORD	nRead ;

	do {
		if ( g_bEof1 ) {
			return -1 ;
		}
		if ( g_dwRin1 == g_dwRout1 ) {
			g_dwRin1 = g_dwRout1 = 0 ;
			ret = ReadFile( g_hfile1, g_rBuf1, sizeof g_rBuf1, &nRead, NULL ) ;
			if ( !ret || !nRead ) {
				g_bEof1 = TRUE ;
				return -1 ;
			}
			g_dwRin1 = nRead ;
			g_dwRin1 %= sizeof g_rBuf1 ;
		}
		c = (int) g_rBuf1[ g_dwRout1 ] ;
		g_dwRout1 ++ ;
		g_dwRout1 %= sizeof g_rBuf1 ;
	} while ( !g_bBinary1 && c == '\r' ) ;
	return c ;
}

char *
Fgets( char *buf, int size )
{
	int		c ;
	LPBYTE	ptr = buf ;

	size -- ;
	while ( (c = Fgetc()) != -1 ) {
		if ( !size ) {
			break ;
		}
		*ptr++ = c ;
		if ( c == '\n' ) {
			break ;
		}
	}
	*ptr = 0 ;
	return ptr == buf ? NULL : buf ;
}

static	int
Fputc_sub( int c )
{
	DWORD	tmp, nWrite ;

	g_sBuf1[ g_dwWin1 ] = c ;
	g_dwWin1 ++ ;
	g_dwWin1 %= sizeof g_sBuf1 ;
	if ( !g_dwWin1 || c == '\n' ) {
		tmp = g_dwWin1 ? g_dwWin1 : sizeof g_sBuf1 ;
		g_dwWin1 = 0 ;
		if ( !WriteFile( g_hfile1, g_sBuf1, tmp, &nWrite, NULL ) ) {
			g_bError1 = TRUE ;
			return FALSE ;
		} else if ( nWrite == 0 ) {
			g_bError1 = TRUE ;
			return FALSE ;
		}
	}
	return TRUE ;
}

int
Fputc( int c )
{
	if ( !g_bBinary1 && c == '\n' ) {
		if ( !Fputc_sub( '\r' ) ) {
			return -1 ;
		}
	}
	return Fputc_sub( c ) ? c : -1 ;
}

int
Fputs( const char *buf )
{
	BYTE	c ;
	int		last = -1 ;

	while ( c = *buf++ ) {
		last = (int) c ;
		if ( Fputc( last ) == -1 ) {
			return -1 ;
		}
	}
	return last ;
}

void
Fwrite( const char *buf, int size )
{
	if ( !size ) {
		return ;
	}
	do {
		Fputc( *buf++ ) ;
	} while ( -- size ) ;
}

BOOL
Ferror( void )
{
	return g_bError1 ;
}

void
slash2backslash( LPTSTR str )
{
	LPTSTR	ptr ;

	/* convert '/' to '\' */
	while ( ptr = _tcschr( str, TEXT('/') ) ) {
		*ptr = TEXT('\\') ;
	}
}

void
catdir( LPTSTR body, LPCTSTR adddir, LPTSTR pPrevDir )
{
	DWORD	len ;
	LPTSTR	ptr ;

	slash2backslash( body ) ;
	if ( pPrevDir ) {
		lstrcpy( pPrevDir, TEXT("..") ) ;
	}
	while ( *adddir ) {
		if ( _tcschr( TEXT("/\\"), *adddir ) ) {
			/* skip header if it is "/" or "\" */
			adddir ++ ;
			continue ;
		}
		if ( adddir[0] == TEXT('.') && adddir[1] == TEXT('.') ) {
			ptr = _tcsrchr( body, TEXT('\\') ) ;
			if ( ptr && pPrevDir ) {
				lstrcpy( pPrevDir, ptr + 1 ) ;
			}
			if ( ptr && ptr != body ) {
				*ptr = 0 ;
			} else {
				lstrcpy( body, TEXT("\\") ) ;
			}
			adddir += 2 ;
		} else if ( adddir[0] == TEXT('.') ) {
			adddir ++ ;
		} else {
			break ;
		}
	}
	if ( !*adddir ) {
		return ;
	}
	len = lstrlen( body ) ;
	if ( !len || body[ len - 1 ] != TEXT('\\') ) {
		body[ len ++ ] = TEXT('\\') ;
	}
	lstrcpy( &body[ len ], adddir ) ;
	slash2backslash( body ) ;
}

void
rel2abs( LPTSTR newPath, LPCTSTR oldPath, LPCTSTR adddir )
{
#ifndef _WIN32_WCE
        if (adddir[1] == TEXT(':')) {
	  *newPath++ = *adddir++;
	  *newPath++ = *adddir++;
	}
#endif
	if ( _tcschr( TEXT("/\\"), *adddir ) ) {
		lstrcpy( newPath, adddir ) ;
		slash2backslash( newPath ) ;
	} else {
		lstrcpy( newPath, oldPath ) ;
		catdir( newPath, adddir, NULL ) ;
	}
}

int
sleep( int sec )
{
	Sleep( sec * 1000 ) ;
	return 0 ;
}

#ifndef	NO_STDIO
#undef		islower
#define		islower(c)		((c)>='a'&&(c)<='z')
#define		toupper(c)		(islower(c)?(c)-'a'+'A':(c))

int
stricmp( const char *src, const char *dst )
{
	char	c = 0, c1 ;

	while ( (c = *src++) ) {
		c = toupper( c ) ;
		c1 = *dst++ ;
		c1 = toupper( c1 ) ;
		if ( c != c1 ) {
			return c - c1 ;
		}
	}
	c = toupper( c ) ;
	c1 = *dst++ ;
	c1 = toupper( c1 ) ;
	return c - c1 ;
}

int
strnicmp(const char *src, const char *dst, size_t len)
{
	char	c = 0, c1 ;

	while ( len && (c = *src++) ) {
		c = toupper( c ) ;
		c1 = *dst++ ;
		c1 = toupper( c1 ) ;
		if ( c != c1 ) {
			return c - c1 ;
		}
		len -- ;
	}
	c = toupper( c ) ;
	c1 = *dst++ ;
	c1 = toupper( c1 ) ;
	return len ? c - c1 : 0 ;
}

#if 0
int
bcmp( char *s1, char *s2, int len )
{
	return memcmp( s1, s2, len ) ;
}

int
bzero( char *s, int len )
{
	memset( s, 0, len ) ;
	return 0 ;
}
#endif

void
strcat_num( char *str, int num )
{
	CHAR	digits[ 10 ], *ptr = digits ;

	str += strlen( str ) ;
	if ( num < 0 ) {
		*str++ = '-' ;
		num = - num ;
	}
	if ( num ) {
		while ( num ) {
			*ptr++ = '0' + (CHAR) (num % 10) ;
			num /= 10 ;
		}
	} else {
		*ptr++ = '0' ;
	}
	while ( ptr -- != digits ) {
		*str++ = *ptr ;
	}
	*str = 0 ;
}

void
strcat_char( char *str, int c )
{
	str += strlen( str ) ;
	*str++ = (char) (c & 0xFF) ;
	*str = 0 ;
}
#endif	/* NO_STDIO */

#ifndef	NO_SPRINTF
static void
SprintfNum( char **buf, int i, int r )
{
	int		q ;

	if ( i < 0 ) {
		*(*buf) ++ = '-' ;
		i = -i ;
	}
	q = i / r ;
	if ( q ) {
		SprintfNum( buf, q, r ) ;
	}
	*(*buf) ++ = '0' + i % r ;
}

static int
SprintfSub( char *buf, const char *fmt, va_list *ap )
{
	int		c ;
	char	*orgbuf = buf ;

	while ( ( c = *fmt++ ) != '\0' ) {
		if ( c != '%') {
			*buf++ = c ;
		} else {
			c = *fmt++ ;
			switch ( c ) {
			case 'c':
				*buf++ = (char) (0xFF & va_arg( *ap, int )) ;
				break ;
			case 'l':
				c = *fmt++ ;
			case 'd':
				SprintfNum( &buf, va_arg( *ap, int ), 10 ) ;
				break ;
			case 'o':
				SprintfNum( &buf, va_arg( *ap, int ), 8 ) ;
				break ;
			case 's':
				strcpy( buf, va_arg( *ap, char * ) ) ;
				buf += strlen( buf ) ;
				break ;
			default:
				*buf++ = c ;
			}
		}
	}
	*buf = 0 ;
	return strlen( orgbuf ) ;
}

int
Sprintf( char *buf, const char *fmt, ... )
{
	int			ret ;
	va_list		pvar ;

	va_start( pvar, fmt ) ;
	ret = SprintfSub( buf, fmt, &pvar ) ;
	va_end( pvar ) ;
	return ret ;
}
#endif	/* NO_SPRINTF */

#define CETOOLS_MAX_TEMP_BUF 4
#define CETOOLS_MAX_FILE_NAME 160 /* c.f. NFILEN at mg/def.h is 80 */

#ifdef KANJI
#ifndef USE_KCTRL
int
unicode2sjis(const char *src, unsigned char *dst, int max)
{
#ifdef UNICODE
  return (unsigned long)WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)src, -1,
				    (LPWSTR)dst, max, NULL, NULL);
#else
  int size = lstrlen(src) + 1;
  if (dst) {
    bcopy(src, dst, size);
  }
  return size;
#endif
}

int
sjis2unicode(const unsigned char *src, char *dst, int max)
{
#ifdef UNICODE
  return MultiByteToWideChar(CP_ACP, 0, (LPCWSTR)src, -1,
				    (LPSTR)dst, max / sizeof(TCHAR)) * sizeof(TCHAR);
#else
  int size = strlen(src) + 1;
  if (dst) {
    bcopy(src, dst, size);
  }
  return size;
#endif
}

WORD
unicode2sjis_char(TCHAR c)
{
  TCHAR unic[CETOOLS_MAX_TEMP_BUF];
  BYTE sjis[CETOOLS_MAX_TEMP_BUF];
  WORD retval;

  unic[0] = c;
  unic[1] = 0;
#ifdef UNICODE
  WideCharToMultiByte(CP_ACP, 0, unic, -1, sjis, CETOOLS_MAX_TEMP_BUF,
		      NULL, NULL);
#else
  if (sjis) {
    bcopy(unic, sjis, lstrlen(unic) + 1);
  }
#endif

  retval = (WORD)sjis[0];
  if (sjis[1]) {
    retval = ((retval << 8) | (WORD)sjis[1]);
  }
  return retval;
}

TCHAR
sjis2unicode_char(WORD c)
{
  TCHAR unic[CETOOLS_MAX_TEMP_BUF];
  BYTE sjis[CETOOLS_MAX_TEMP_BUF], *p = sjis;

  if (c & 0x0ff00L) {
    *p++ = (c >> 8);
  }
  *p++ = (BYTE)c;
  *p = (BYTE)0;
#ifdef UNICODE
  MultiByteToWideChar(CP_ACP, 0, sjis, p - sjis, unic, CETOOLS_MAX_TEMP_BUF);
#else
  if (unic) {
    bcopy(sjis, unic, strlen(sjis) + 1);
  }
#endif
  return unic[0];
}
#endif  /* USE_KCTRL */
#else	/* !KANJI */
int
unicode2sjis(const char *src, unsigned char *dst, int max)
{
  BYTE *p = dst, *ep = p + max - 1;
  LPCTSTR sp = src;

  if (dst) {
    while (*sp && p < ep) {
      *p++ = (BYTE)*sp++;
    }
    *p++ = (BYTE)0;
    return p - dst;
  }
  else {
    return lstrlen(src) + 1;
  }
}

int
sjis2unicode(const unsigned char *src, char *dst, int max)
{
  LPTSTR p = dst, ep = p + max - 1;
  const BYTE *sp = src;

  if (dst) {
    while (*sp && p < ep) {
      *p++ = (TCHAR)*sp++;
    }
    *p++ = (TCHAR)0;
    return p - dst;
  }
  else {
    return sizeof(TCHAR) * (strlen(src) + 1);
  }
}

WORD
unicode2sjis_char(TCHAR c)
{
  TCHAR unic[CETOOLS_MAX_TEMP_BUF];
  BYTE sjis[CETOOLS_MAX_TEMP_BUF];
  WORD retval;

  unic[0] = c;
  unic[1] = 0;
  unicode2sjis(unic, sjis, CETOOLS_MAX_TEMP_BUF);
  retval = (WORD)sjis[0];
  if (sjis[1]) {
    retval = ((retval << 8) | (WORD)sjis[1]);
  }
  return retval;
}

TCHAR
sjis2unicode_char(WORD c)
{
  TCHAR unic[CETOOLS_MAX_TEMP_BUF];
  BYTE sjis[CETOOLS_MAX_TEMP_BUF], *p = sjis;

  if (c & 0x0ff00L) {
    *p++ = (c >> 8);
  }
  *p++ = (BYTE)c;
  *p = (BYTE)0;
  sjis2unicode(sjis, unic, CETOOLS_MAX_TEMP_BUF);
  return unic[0];
}
#endif	/* KANJI */


extern int allow_mouse_event;
/*
 * Callback function for mouse events.  This function will be called while
 * calling a function getkbd().
 */
VOID
MouseEvent(int fn, int x, int y)
{
  if (allow_mouse_event) {
    WINDOW *wp = wheadp;
    int startrow;
    int offset;
    LINE *lp;
    extern int rowcol2offset(LINE *lp, int row, int col);

    while (wp) {
      startrow = wp->w_toprow;
      if (startrow <= y && y < startrow + wp->w_ntrows + 1) {
	if (y < startrow + wp->w_ntrows) {
	  curwp = wp;
	  curbp = wp->w_bufp;
	  lp = wp->w_linep;
	  y = y - startrow + wp->w_lines;
	  offset = rowcol2offset(lp, y, x);
	  while (offset < 0) {
	    y = -offset - 1;
	    if (lforw(lp) != curbp->b_linep) {
	      lp = lforw(lp);
	      offset = rowcol2offset(lp, y, x);
	    }
	    else {
	      offset = llength(lp);
	      break;
	    }
	  }
	  wp->w_dotp = lp;
	  wp->w_doto = offset;
	}
	else {
	  /* In case on a mode line, do nothing. */
	}
	break;
      }
     wp = wp->w_wndp;
    }
    update();
  }
}

#ifdef	DROPFILES	/* 00.07.07  by sahf */
VOID
DropEvent(const char *files, int y)
{
  if (allow_mouse_event) {
    WINDOW *wp = wheadp;
    const char *mp;
    char filename[NFILEN];
    int  startrow;
    extern int filevisit pro((int,int));

    /* change current window to drag'n drop */
    while (wp) {
      startrow = wp->w_toprow;
      if (startrow <= y && y < startrow + wp->w_ntrows + 1) {
        curwp = wp;
        curbp = wp->w_bufp;
        break;
      }
      wp = wp->w_wndp;
    }
 
    /* 各ファイル名について find_file() 実行 */
    mp = files;
    do {
      unicode2sjis(mp, filename, NFILEN-1);
#ifdef	KANJI
      bufstoe(filename, strlen(filename)+1);
#endif
      eargset(filename);
      if (!filevisit(FFRAND, 0))
        break;
	  mp += lstrlen(mp)+1;
    } while ( *mp != 0 );

    update();
  }
}
#endif	/* DROPFILES */

#ifdef	CLIPBOARD
int
euc2sjis_crlf( char *dst, const char *euc, int len )
{
    int		c1, count ;
    const char	*endp = euc + len ;

    count = 0 ;
    while ( euc < endp ) {
	c1 = *euc++ & 0xff ;
#ifdef KANJI
	if ( ISKANJI( c1 ) ) {
#ifdef HANKANA
	    if ( c1 == SS2 ) {
		if ( dst ) {
		    *dst++ = *euc++ ;
		}
		count ++ ;
	    } else
#endif  /* HANKANA */
	    {
		int c2 = *euc++ & 0xff;
		etos( c1, c2 ) ;
		if ( dst ) {
		    *dst++ = c1 ;
		    *dst++ = c2 ;
		}
		count += 2 ;
	    }
	} else
#endif
	if ( c1 == '\n' ) {
	    if ( dst ) {
		*dst++ = '\r' ;
		*dst++ = '\n' ;
	    }
	    count += 2 ;
	} else {
	    if ( dst ) {
		*dst++ = c1 ;
	    }
	    count ++ ;
	}
    }
    return count ;
}

int
send_clipboard_(const char *euc, int len)
{
  char       *sjis;
  LPTSTR     ptr;
  DWORD	     size;
#ifdef	_WIN32_WCE
  HLOCAL     hGMem;
#else
  HGLOBAL    hGMem;
#endif

  size = euc2sjis_crlf(NULL, euc, len);
  sjis = alloca(size+1);
  if (sjis == NULL)
    return FALSE;
  (void)euc2sjis_crlf(sjis, euc, len);
  sjis[size] = '\0';
  size = sjis2unicode((LPBYTE) sjis, NULL, 0 ) ;
  if (size == 0)
    return TRUE ;

  /* Open clipboard */
  if ( !OpenClipboard( 0 ) ) {
    return FALSE;
  } else if ( !EmptyClipboard() ) {
    CloseClipboard() ;
    return FALSE;
  }
  /* allocate memory for some amount needed */
#ifdef	_WIN32_WCE
  hGMem = LocalAlloc( LMEM_FIXED, size ) ;
  ptr = (LPTSTR) hGMem ;
#else	/* _WIN32_WCE */
  hGMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, size ) ;
  ptr = (LPTSTR) GlobalLock( hGMem ) ;
#endif	/* _WIN32_WCE */
  /* Convert UNICODE to SJIS */
  sjis2unicode( (LPBYTE) sjis, ptr, size ) ;
  /* Store the result to clipboard */
#ifdef UNICODE
  SetClipboardData( CF_UNICODETEXT, hGMem ) ;
#else
  SetClipboardData(CF_TEXT, hGMem);
#endif
#ifndef	_WIN32_WCE
  GlobalUnlock( hGMem ) ;
#endif
  /* Close clipboard */
  CloseClipboard() ;
  return TRUE;
}

int
size_clipboard_()
{
  LPCTSTR	cbptr ;
  char		*p;
  int		i, cbsize, size, ret = 0;
#ifdef  _WIN32_WCE
  HLOCAL	hGMem ;
#else   /* _WIN32_WCE */
  HGLOBAL	hGMem ;
#endif  /* _WIN32_WCE */

  if ( !OpenClipboard( 0 ) ) {
    return 0;
  }
#ifdef  _WIN32_WCE
  hGMem = GetClipboardData(CF_UNICODETEXT) ;
  cbsize = LocalSize(hGMem);
  cbptr = (LPCTSTR) hGMem;
#else   /* _WIN32_WCE */
#ifdef UNICODE
  hGMem = GetClipboardData(CF_UNICODETEXT);
#else
  hGMem = GetClipboardData(CF_TEXT);
#endif
  cbsize = GlobalSize( hGMem ) ;
  cbptr = (LPCTSTR) GlobalLock( hGMem ) ;
#endif  /* _WIN32_WCE */
  if ( !cbsize ) {
    goto exit ;
  }
  ret = unicode2sjis(cbptr, (LPBYTE)NULL, 0);
  size = 1;		/* ASCIZ NULL byte */
  for (i=0,p=(char *)cbptr; i<ret; i++,p++)
  {
    if (*p > 0xA0 && *p < 0xE0) /* *p is kana */
       size += 2;
    else if (*p != '\r')
       size++;
  }
  if (size > ret)
    ret = size;

exit:
#ifndef _WIN32_WCE
  GlobalUnlock( hGMem ) ;
#endif  /* _WIN32_WCE */
  CloseClipboard() ;
  return ret;
}

int
recieve_clipboard_(char *buffer, int *size)
{
  LPCTSTR	cbptr ;
  int		cbsize, ret = FALSE;
  char		*src,*dst;
  char		c;
#ifdef  _WIN32_WCE
  HLOCAL	hGMem ;
#else
  HGLOBAL	hGMem ;
#endif

  if ( !OpenClipboard( 0 ) ) {
    return FALSE;
  }
#ifdef  _WIN32_WCE
  hGMem = GetClipboardData(CF_UNICODETEXT) ;
  cbsize = LocalSize( hGMem );
  cbptr = (LPCTSTR) hGMem;
#else   /* _WIN32_WCE */
#ifdef UNICODE
  hGMem = GetClipboardData(CF_UNICODETEXT) ;
#else
  hGMem = GetClipboardData(CF_TEXT);
#endif
  cbsize = GlobalSize(hGMem) ;
  cbptr = (LPCTSTR) GlobalLock(hGMem) ;
#endif  /* _WIN32_WCE */
  if ( !cbsize ) {
    goto exit ;
  }
  ret = unicode2sjis(cbptr, (LPBYTE)buffer, cbsize);
  dst = src = buffer;
  while (c = *src++) {
	if (c != '\r') {
		*dst++ = c;
	}
  }
#ifdef KANJI
  *size = bufstoe(buffer, dst-buffer);
#else
  *size = dst - buffer;
#endif

exit:
#ifndef _WIN32_WCE
  GlobalUnlock( hGMem ) ;
#endif
  CloseClipboard() ;
  return ret;
}
#endif	/* NO_CLIPBOARD */

#ifndef NO_DIRED
void
WinExecute(char *fname)
{
  SHELLEXECUTEINFO sei;
  extern HWND g_hwndTty;
  int sz;
  char *sjis;
  BOOL res;
#ifdef KANJI
  extern void bufetos(char *, int);
#endif

  bzero((char *)&sei, sizeof(sei));
  sei.cbSize = sizeof(sei);
  sei.hwnd = g_hwndTty;
  sei.lpVerb = (LPCTSTR)0;
  sei.nShow = SW_SHOW;

  sz = strlen(fname) + 1;
  sei.lpFile =(LPTSTR)alloca(sz * (sizeof(TCHAR) + 1));
  if (sei.lpFile == NULL) {
    ewprintf("Memory exhausted!");
    return;
  }
  sjis = (char *)(sei.lpFile + sz);
  strcpy(sjis, fname);
#ifdef KANJI
  bufetos(sjis, sz);
#endif
  sjis2unicode(sjis, (LPTSTR)sei.lpFile, sizeof(TCHAR) * sz);
  res = ShellExecuteEx(&sei);
  if (!res) {
      DWORD err = GetLastError();
  }
}
#endif

LONG
RegSetSomething(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName,
		LPBYTE lpSomething, DWORD dwSize, DWORD dwType)
{
  LONG retval;
  HKEY newkey;
  DWORD disposition;

  retval = RegCreateKeyEx(hKey, lpSubKey, 0,
			  NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
			  &newkey, &disposition);

  if (retval == ERROR_SUCCESS) {
    retval = RegSetValueEx(newkey, lpValueName, 0,
			   dwType, lpSomething, dwSize);
    RegCloseKey(newkey);
  }
  return retval;
}

LONG
RegSetDWord(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName, DWORD data)
{
  return RegSetSomething(hKey, lpSubKey, lpValueName,
			 (LPBYTE)&data, sizeof(DWORD), REG_DWORD);
}

/* Caution! No distinction between 0 return value and fail  */

DWORD
RegQueryDWord(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName)
{
  LONG retval;
  HKEY newkey;
  DWORD val, valsize;

  retval = RegOpenKeyEx(hKey, lpSubKey, REG_OPTION_NON_VOLATILE,
			KEY_READ, &newkey);
  if (retval == ERROR_SUCCESS) {
    DWORD datatype;

    valsize = sizeof(DWORD);
    retval = RegQueryValueEx(newkey, lpValueName, (LPDWORD)NULL, &datatype,
			     (LPBYTE)&val, &valsize);
    RegCloseKey(newkey);
  }
  if (retval == ERROR_SUCCESS) {
    return val;
  }
  else {
    return (DWORD)0;
  }
}

LONG
RegQueryString(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName,
	       LPTSTR lpstr, DWORD *lpstrsize)
{
  LONG retval;
  HKEY newkey;

  retval = RegOpenKeyEx(hKey, lpSubKey, REG_OPTION_NON_VOLATILE,
			KEY_READ, &newkey);
  if (retval == ERROR_SUCCESS) {
    DWORD datatype;

    retval = RegQueryValueEx(newkey, lpValueName, (LPDWORD)NULL, &datatype,
			     (LPBYTE)lpstr, lpstrsize);
    RegCloseKey(newkey);
  }
  return retval;
}

LONG
RegSetString(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName, LPTSTR lpstr)
{
  return RegSetSomething(hKey, lpSubKey, lpValueName,
			 (LPBYTE)lpstr, sizeof(TCHAR) * (lstrlen(lpstr) + 1),
			 REG_SZ);
}

LONG
RegSetBinary(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName,
	     LPBYTE lpByte, DWORD dwSize)
{
  return RegSetSomething(hKey, lpSubKey, lpValueName,
			 (LPBYTE)lpByte, dwSize, REG_BINARY);
}

LONG
RegRemoveValue(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName)
{
  LONG retval;
  HKEY newkey;

  retval = RegOpenKeyEx(hKey, lpSubKey, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, &newkey);
  if (retval == ERROR_SUCCESS) {
    retval = RegDeleteValue(newkey, lpValueName);
    RegCloseKey(newkey);
  }
  return retval;
}
