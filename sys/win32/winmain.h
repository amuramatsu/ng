/* $Id: winmain.h,v 1.2 2000/10/23 16:52:52 amura Exp $ */
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
 * $Log: winmain.h,v $
 * Revision 1.2  2000/10/23 16:52:52  amura
 * add GPL copyright to header
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#ifdef	__cplusplus
extern "C" {
#endif
	int		Main( int argc, char *argv[] ) ;
	int		Kbhit( void ) ;
	int		KbhitSleep( DWORD sec ) ;
	int		GetChar( void ) ;
	void	GotoXY( int x, int y ) ;
	void	EraseEOL( void ) ;
	void	EraseEOP( void ) ;
	void	PutChar( char c ) ;
	void	PutKChar( char c1, char c2 ) ;
	void	Flush( void ) ;
	void	Exit( int code ) ;
	void	GetWH( int *w, int *h ) ;
#ifdef	__cplusplus
} ;
#endif
