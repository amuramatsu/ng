/* $Id: ttyio.c,v 1.5 2001/05/25 15:59:08 amura Exp $ */
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
 * TTY I/O routine for win32
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.5  2001/05/25 15:59:08  amura
 * WIN32 version support AUTOSAVE feature
 *
 * Revision 1.4  2001/01/20 15:48:47  amura
 * very big terminal supported
 *
 * Revision 1.3  2000/11/16 14:21:32  amura
 * merge Ng for win32 0.5
 *
 * Revision 1.2  2000/10/23 16:52:51  amura
 * add GPL copyright to header
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#include	<windows.h>
#include	"config.h"
#include	"def.h"
#include	"winmain.h"
#include "tools.h"

int		nrow ;				/* Terminal size, rows.		*/
int		ncol ;				/* Terminal size, columns.	*/
BOOL	bLastChar = FALSE ;
CHAR	chLastChar = 0 ;

int
panic( char *s )
{
  TCHAR foo[256];

  sjis2unicode(s, foo, sizeof(foo));
  MessageBox( NULL, foo, TEXT(""), MB_ICONASTERISK|MB_OK ) ;
  return 0 ;
}

int
ttopen()
{
	GetWH( &ncol, &nrow ) ;
	if (NROW < nrow) {
		nrow = NROW;
	}
	if (NCOL < ncol) {
		ncol = NCOL;
	}
	return 0 ;
}

int
ttclose()
{
	return 0 ;
}

int
ttflush()
{
	Flush() ;
	return 0 ;
}

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
int
typeahead()
{
	return Kbhit() ;
}

/*
 * Write character to the display without ^C check.
 */
int
ttputc( int c )
{
	PutChar( (char) c ) ;
	return 0 ;
}

/*
 * Write character to the display without ^C check.
 */
int
ttputkc( int c1, int c2 )
{
  PutKChar((char)c1, (char)c2);
  return 0;
}

/*
 * Read character from terminal without ^C check.
 * All 8 bits are returned, so that you can use
 * a multi-national terminal.
 */
int
ttgetc()
{
	if ( bLastChar ) {
		bLastChar = FALSE ;
		return chLastChar ;
	}
#ifdef	AUTOSAVE
	while (!KbhitSleep(1))
		autosave_handler();
#endif
	return GetChar() ;
}

/*
 * Save pre-readed char to read again.
 */
int
ttungetc( int c )
{
	bLastChar = TRUE ;
	chLastChar = c ;
	return 0 ;
}

/*
 * A program to return TRUE if we wait for 1 seconds without anything
 * happening, else return FALSE.
 */
int
ttwait()
{
	return !KbhitSleep( 1 ) ;
}

/*
 * set the tty size. Functionized for Win32
 */
void
setttysize()
{
  GetWH(&ncol, &nrow);
}
