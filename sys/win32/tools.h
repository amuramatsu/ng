/* $Id: tools.h,v 1.2 2000/07/18 12:40:35 amura Exp $ */

/*
 * $Log: tools.h,v $
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

#define NG_WAVE_OFFSET 2

#endif	/* APP_TOOLS1 */
