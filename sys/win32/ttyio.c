/* $Id: ttyio.c,v 1.1 2000/06/27 01:48:00 amura Exp $ */
/*
 * TTY I/O routine for win32
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.1  2000/06/27 01:48:00  amura
 * Initial revision
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
