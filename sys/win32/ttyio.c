/* $Id: ttyio.c,v 1.7 2003/02/22 08:09:47 amura Exp $ */
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

#include <windows.h>
#include "config.h"
#include "def.h"
#include n"winmain.h"
#include "tools.h"

int nrow;				/* Terminal size, rows.		*/
int ncol;				/* Terminal size, columns.	*/
BOOL bLastChar = FALSE;
CHAR chLastChar = 0;

void
panic(char *s)
{
    TCHAR foo[256];
    
    sjis2unicode(s, foo, sizeof(foo));
    MessageBox( NULL, foo, TEXT(""), MB_ICONASTERISK|MB_OK ) ;
}

void
ttopen()
{
    GetWH(&ncol, &nrow);
    if (NROW < nrow)
	nrow = NROW;
    if (NCOL < ncol)
	ncol = NCOL;
}

void
ttclose()
{
}

void
ttflush()
{
    Flush();
}

/*
 * typeahead returns TRUE if there are characters available to be read
 * in.
 */
int
typeahead()
{
    return Kbhit();
}

/*
 * Write character to the display without ^C check.
 */
void
ttputc(int c)
{
    PutChar((char)c);
}

/*
 * Write character to the display without ^C check.
 */
void
ttputkc(int c1, int c2)
{
    PutKChar((char)c1, (char)c2);
}

/*
 * Read character from terminal without ^C check.
 * All 8 bits are returned, so that you can use
 * a multi-national terminal.
 */
int
ttgetc()
{
    if (bLastChar) {
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
void
ttungetc(int c)
{
    bLastChar = TRUE ;
    chLastChar = c ;
}

/*
 * A program to return TRUE if we wait for 1 seconds without anything
 * happening, else return FALSE.
 */
int
ttwait()
{
    return !KbhitSleep(1);
}

/*
 * set the tty size. Functionized for Win32
 */
void
setttysize()
{
    GetWH(&ncol, &nrow);
}
