/* $Id: winmain.h,v 1.1 2000/06/27 01:48:00 amura Exp $ */
/*
 * $Log: winmain.h,v $
 * Revision 1.1  2000/06/27 01:48:00  amura
 * Initial revision
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
