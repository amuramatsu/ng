/* $Id: tools.h,v 1.4 2000/11/16 14:21:31 amura Exp $ */
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
 * $Log: tools.h,v $
 * Revision 1.4  2000/11/16 14:21:31  amura
 * merge Ng for win32 0.5
 *
 * Revision 1.3  2000/10/23 16:52:51  amura
 * add GPL copyright to header
 *
 * Revision 1.2  2000/07/18 12:40:35  amura
 * for Win32, enable to handle japanese directory
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#ifndef	APP_TOOLS1
#define	APP_TOOLS1

#ifdef	__cplusplus
extern "C" {
#endif

BOOL	Fopen( LPCTSTR path, LPCTSTR modestr ) ;
void	Fclose( void ) ;
int		Fgetc( void ) ;
char*	Fgets( char *buf, int size ) ;
int		Fputc( int c ) ;
int		Fputs( const char *buf ) ;
void	Fwrite( const char *buf, int size ) ;
BOOL	Ferror( void ) ;
void	slash2backslash( LPTSTR str ) ;
void	catdir( LPTSTR body, LPCTSTR adddir, LPTSTR pPrevDir ) ;
void	rel2abs( LPTSTR newPath, LPCTSTR oldPath, LPCTSTR adddir ) ;
int		SendClipboard( const char *sjis ) ;
int ReceiveClipboard(char *sjis, int bufsize);

int unicode2sjis(const char *src, unsigned char *dst, int max);
int sjis2unicode(const unsigned char *src, char *dst, int max);
TCHAR sjis2unicode_char(WORD sjis);
WORD unicode2sjis_char(TCHAR unicode);
LONG RegQueryString(HKEY, LPCTSTR, LPTSTR, LPTSTR, DWORD*);
LONG RegSetString(HKEY, LPCTSTR, LPTSTR, LPTSTR);
LONG RegSetBinary(HKEY, LPCTSTR, LPTSTR, LPBYTE, DWORD);
LONG RegSetDWord(HKEY, LPCTSTR, LPTSTR, DWORD);
DWORD RegQueryDWord(HKEY, LPCTSTR, LPTSTR);
LONG RegRemoveValue(HKEY, LPCTSTR, LPTSTR);
#ifdef	__cplusplus
} ;
#endif

#ifdef KANJI
#define NGREGKEY TEXT("Software\\mg-developers\\Ng")
#define NGDEFAULTINIFILE TEXT("\\ng.ini")
#else
#define NGREGKEY TEXT("Software\\mg-developers\\MG")
#define NGDEFAULTINIFILE TEXT("\\mg.ini")
#endif
#define NGSTARTUPFILEVAL TEXT("Startup File")
#define NGCTRLKEYMAPVAL TEXT("Control Key Mapping")
#define NGBEEPSOUNDVAL TEXT("Beep Sound")
#define NGBEEPFILEVAL TEXT("Beep Sound File")
#define NGFONTNAMEVAL TEXT("Font Name")
#define NGFONTSIZEVAL TEXT("Font Size")
#define NGLINESPACEVAL TEXT("Line Space")
#define NGPREVPOSVAL TEXT("Previous Position")
#define NGKEYBOARDLOCALEVAL TEXT("Keyboard Locale")

#define NGKEYBOARD_US  1
#define NGKEYBOARD_JP 81

#define NG_WAVE_OFFSET 2

#endif	/* APP_TOOLS1 */
