/* $Id: winmain.c,v 1.4 2000/09/01 19:41:21 amura Exp $ */
/*
 * NG : NG program main routine
 *
 * 1998/11/14:Eiichiroh Itoh
 *
 */

/*
 * $Log: winmain.c,v $
 * Revision 1.4  2000/09/01 19:41:21  amura
 * fix for suppress buffer overrun
 *
 * Revision 1.3  2000/07/22 20:46:33  amura
 * support "Drag&Drop"
 *
 * Revision 1.2  2000/07/18 12:42:34  amura
 * support IME convertion on the spot
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#include	<windows.h>
#include	<windowsx.h>
#include <commdlg.h>
#include <commctrl.h>
#ifdef KANJI
#include <imm.h>
#endif
#include "config.h"
#include "def.h"
#include	"ttyctrl.h"
#include	"resource.h"
#include	"winmain.h"
#include	"tools.h"

#define		EXCEPTION_QUIT			(1)
#define		IDM_START				(1001)
#define		IDM_EXIT				(1002)
#define		IDC_TTY					(2001)

#ifdef KANJI
#define MGTITLE TEXT("Ng")
#define MGCLASS TEXT("NG")
#else
#define MGTITLE TEXT("MG")
#define MGCLASS TEXT("MG")
#endif

HINSTANCE	g_hInst ;
HWND		g_hwndMain ;
HWND		g_hwndTty ;
HANDLE		g_hevtGetChar = 0 ;
HANDLE		g_hThread = 0 ;
BOOL		g_bExit = FALSE ;
TCHAR		g_szTitleName[ MAX_PATH ] = MGTITLE;
TCHAR		g_szClassName[] = MGCLASS;
DWORD		g_dwAppVersion = 12 ;
TCHAR		MessageBuf[ MAX_PATH ] = TEXT("") ;
CHAR		g_szArgBuf[ 256 ] ;
int			g_dwArgc ;
LPSTR		g_szArgv[ 128 ] ;
DWORD g_ctrlmap, g_beepsound;
TCHAR g_beepfile[128];
#ifndef _WIN32_WCE
BOOL g_maximized;
RECT g_rect;
#endif

static	BOOL	init_application( void ) ;
static	BOOL	init_instance( int nCmdShow ) ;
LRESULT CALLBACK	MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) ;
static	void	cmdline2args( LPSTR cmdline, int *argc, char **argv ) ;
static	void	ThreadMain( void ) ;

#ifdef CTRLMAP
#define VK_EISU 0xf0
#define VK_SOMETHING 0xe5

static BOOL
controlkey_swap(MSG *msg)
{
  if (msg->message == WM_KEYDOWN || msg->message == WM_KEYUP) {
    if ((int)msg->wParam == VK_EISU) {
      if (msg->message == WM_KEYDOWN && !(msg->lParam & (1 << 30))) {
	/* key was up before this message */
	keybd_event(VK_CONTROL, 0, 0, 0);
      }
      else if (msg->message == WM_KEYUP && (msg->lParam & (1 << 30))) {
	/* key was down before this message */
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
      }
      return TRUE;
    }
    if ((int)msg->wParam == VK_SOMETHING) { /* What's this? */
      /* Although I don't know the reason and details, some specific
	 virtual code is generated according to a key press of
	 Japanese mode-related keys such as EISU, HIRAGANA, HANKAKU
	 and so on.  Curiously, the virtual code is only generated for
	 depressing while Japanese input mode is open.  The following
	 program detects the virtual code and treats as control key in
	 case EISU is depressed. */

      SHORT eisu_down;

      eisu_down = GetAsyncKeyState(VK_EISU);
      if (eisu_down) {
	if (msg->message == WM_KEYDOWN && !(msg->lParam & (1 << 30))) {
	  /* key was up before this message */
	  keybd_event(VK_CONTROL, 0, 0, 0);
	}
	return TRUE;
      }
    }
  }
  return FALSE;
}
#endif /* CTRLMAP */

int WINAPI
#ifdef	_WIN32_WCE
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPTSTR lpszArgs, int nWinMode )
#else	/* _WIN32_WCE */
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPSTR lpszArgs, int nWinMode )
#endif	/* _WIN32_WCE */
{
	MSG			msg ;

	g_hInst = hThisInst ;
	/* define a TTY Window */
	if ( !TtyViewRegisterClass( g_hInst ) ) {
		goto ExitMain ;
	}
	/* create an event for keyboard input notification */
	g_hevtGetChar = CreateEvent( NULL, FALSE, FALSE, NULL ) ;
	if ( g_hevtGetChar == NULL ) {
		goto ExitMain ;
	}
	/* define a window class */
	if ( !init_application() ) {
		goto ExitMain ;
	}
	/* create a window */
	if ( !init_instance( nWinMode ) ) {
		goto ExitMain ;
	}
	InitCommonControls();

	{
	  DWORD foo;
	/* read configuration for Ng for Win32 */
	  g_ctrlmap = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, 
				    NGCTRLKEYMAPVAL) ? TRUE : FALSE;
	  g_beepsound = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY,
				      NGBEEPSOUNDVAL);
	  foo = sizeof(g_beepfile);
	  RegQueryString(HKEY_CURRENT_USER, NGREGKEY, NGBEEPFILEVAL,
			 g_beepfile, &foo);
	}

	/* create a command line processing routine */
#ifdef	_WIN32_WCE
	unicode2sjis( lpszArgs, g_szArgBuf, sizeof g_szArgBuf ) ;
#else	/* _WIN32_WCE */
	strncpy(g_szArgBuf, lpszArgs, sizeof g_szArgBuf);
	g_szArgBuf[(sizeof g_szArgBuf)-1] = '\0';
#endif	/* _WIN32_WCE */
	cmdline2args( g_szArgBuf, &g_dwArgc, g_szArgv ) ;
	/* event loop */
	while ( GetMessage( &msg, NULL, 0, 0 ) ) {
#ifdef CTRLMAP
	  if (!g_ctrlmap || !controlkey_swap(&msg)) {
	    /* call controlkey_swap() if g_ctrlmap is TRUE */
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	  }
#else
	  TranslateMessage(&msg);
	  DispatchMessage(&msg);
#endif
	}

ExitMain:
	if ( g_hevtGetChar ) {
		/* close the event for keyboard input notification */
		CloseHandle( g_hevtGetChar ) ;
	}
	return TRUE ;
}

/*
 * register the main window class
 */
static	BOOL
init_application( void )
{
	WNDCLASS	wcl ;

	wcl.style         = 0 ;
	wcl.lpfnWndProc   = MainWndProc ;
	wcl.cbClsExtra    = 0 ;
	wcl.cbWndExtra    = 0 ;
	wcl.hInstance     = g_hInst ;
#ifdef _WIN32_WCE
	wcl.hIcon         = NULL ;
	wcl.hCursor       = NULL ;
#else
	wcl.hIcon         = LoadIcon (g_hInst,
				      (LPCTSTR)MAKEINTRESOURCE(IDI_APPICON));
	wcl.hCursor       = LoadCursor(NULL, IDC_ARROW);
#endif
	wcl.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
	wcl.lpszMenuName  = 0 ;
	wcl.lpszClassName = g_szClassName ;
	if ( !RegisterClass( &wcl ) ) {
		return FALSE ;
	}
	return TRUE ;
}

#define RegQueryBinary(r, k, v, d, s) RegQueryString(r, k, v, (LPTSTR)d, s)

/*
 * create a main window
 */
static	BOOL
init_instance( int nCmdShow )
{
	RECT	rect ;

#ifndef _WIN32_WCE
	DWORD foo;

        /* get window position from registry */
	foo = sizeof(g_rect);
        if (RegQueryBinary(HKEY_CURRENT_USER, NGREGKEY, NGPREVPOSVAL,
			   &g_rect, &foo) != ERROR_SUCCESS) {
	  g_rect.top = 0;
	  g_rect.bottom = 296;
	  g_rect.left = 0;
	  g_rect.right = 500;
	}
#endif

	/* create a main window */
	g_hwndMain = CreateWindowEx( 0, g_szClassName,
					g_szTitleName,
#ifdef	_WIN32_WCE
					WS_VISIBLE,
					0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#else	/* _WIN32_WCE */
                                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					g_rect.left, g_rect.top,
				        g_rect.right - g_rect.left,
				        g_rect.bottom - g_rect.top,
#endif	/* _WIN32_WCE */
					NULL, NULL, g_hInst, NULL ) ;
	if ( !g_hwndMain ) {
		return FALSE;
	}
	GetClientRect( g_hwndMain, &rect ) ;

	/* create a TTY window */
	g_hwndTty = CreateWindowEx( 0, CTRL_TTYVIEW,
					TEXT(""),
					WS_VISIBLE|WS_CHILD,
					0, 0, rect.right, rect.bottom,
					g_hwndMain, (HMENU) IDC_TTY, g_hInst, NULL ) ;
	if ( !g_hwndTty ) {
		return FALSE;
	}
	/* configure the event for keyboard input notification */
	SendMessage( g_hwndTty, TTYM_SETEVENT, (WPARAM) g_hevtGetChar, 0 ) ;

	/* show the main window */
	ShowWindow( g_hwndMain, nCmdShow ) ;
	UpdateWindow( g_hwndMain ) ;
	SetFocus( g_hwndTty ) ;

	/* send a message to commence a thread */
	PostMessage( g_hwndMain, WM_COMMAND, IDM_START, 0 ) ;
	return TRUE ;
}

void
MainWMCreate( HWND hWnd )
{
	HICON	hIcon ;

	hIcon = (HICON) LoadImage( g_hInst, MAKEINTRESOURCE(IDI_APPICON),
							  IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) ;
	if ( hIcon ) {
		SendMessage( hWnd, WM_SETICON, FALSE, (LPARAM)hIcon ) ;
	}
}

/*
 * WinProc for main window
 */
LRESULT CALLBACK
MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	DWORD	threadID ;

	switch ( message ) {
	case WM_CREATE:
		MainWMCreate( hWnd ) ;
		break ;
	case WM_CLOSE:
		DestroyWindow( hWnd ) ;
		break ;
	case WM_DESTROY:
#ifndef _WIN32_WCE
		if (!g_maximized) {
		  GetWindowRect(g_hwndMain, &g_rect);
		  RegSetBinary(HKEY_CURRENT_USER, NGREGKEY, NGPREVPOSVAL,
			       (LPBYTE)&g_rect, sizeof(g_rect));
		}
#endif

#ifdef CTRLMAP
		{
		  if (g_ctrlmap) {
		    /* send ctrl key `up' event in order to prevent
		       ctrl key to be kept in a state of `depressed' */
		    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
		  }
		}
#endif
		g_bExit = TRUE ;
		Sleep( 1000 ) ;
		PostQuitMessage( 0 ) ;
		break ;
	case WM_SETFOCUS:
		SetFocus( g_hwndTty ) ;
		break ;

#if !defined(_WIN32_WCE) || 200 <= _WIN32_WCE
	case WM_SIZE:
		switch (wParam) {
		  RECT rect;

		case SIZE_MAXIMIZED:
		case SIZE_RESTORED:
		  GetClientRect(g_hwndMain, &rect);
		  MoveWindow(g_hwndTty, rect.left, rect.top,
			     rect.right, rect.bottom, TRUE);
#ifndef _WIN32_WCE
		  /* the following processing is intended to save the
                     previous window position */
		  g_maximized = (wParam == SIZE_MAXIMIZED);
#endif
		  break;

		default:
		  break;
		}
		return 0;
#endif

	case WM_COMMAND:
		switch ( GET_WM_COMMAND_ID( wParam, lParam ) ) {
		case IDM_START:
			g_hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) ThreadMain, 0, 0, &threadID ) ;
			if ( g_hThread != NULL ) {
				CloseHandle( g_hThread ) ;
				break ;
			}
			//
		case IDM_EXIT:
			SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			break ;
		}
		break ;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam ) ;
	}
	return 0 ;
}

int
Kbhit( void )
{
	return SendMessage( g_hwndTty, TTYM_KBHIT, 0, 0 ) ? 1 : 0 ;
}

int
KbhitSleep( DWORD sec )
{
	WaitForSingleObject( g_hevtGetChar, sec * 1000 ) ;
	if ( g_bExit ) {
		ExitThread( 0 ) ;
		return FALSE ;
	}
	return Kbhit() ;
}

int
GetChar( void )
{
	int		c ;

	while ( 1 ) {
	        while (1) {
		  extern VOID MouseEvent pro((int, int, int));
		  c = SendMessage(g_hwndTty, TTYM_GETWINDOWEVENT, 0, 0);
		  if (c < 0) {
		    break;
		  }
		  else {
		    switch (c & TTY_WM_MASK) {
		    case TTY_WM_MOUSE:
		      MouseEvent(0, (c >> 4) & 0x3fff, (c >> 18) & 0x3fff);
		      break;

#if !defined(_WIN32_WCE) || 200 <= _WIN32_WCE
		    case TTY_WM_RESIZE:
		      {
			extern int refresh pro((int, int));
			refresh(FFRAND, 0); /* Very easy way... */
		      }
		      break;
#endif

#ifdef	DROPFILES	/* 00.07.07  by sahf */
		    case TTY_WM_DROPFILES:
		      {
			HLOCAL hMemory;
			extern VOID DropEvent pro((const char *, int));
			/* hMemory got filename buffer address... */
			c = SendMessage( g_hwndTty, TTYM_DROPFILES, 0, (LPARAM)&hMemory );
			if ( c == -1 ) {
			  break ;
			}
			DropEvent((const char *)hMemory, c);
			LocalFree(hMemory);	/* allocated in ttyctrl.cpp */
		      }
		    break;
#endif	/* DROPFILES */
		    }
		  }
		}
		c = SendMessage( g_hwndTty, TTYM_GETCHAR, 0, 0 ) ;
		if ( c != -1 ) {
			break ;
		}
		WaitForSingleObject( g_hevtGetChar, INFINITE ) ;
		if ( g_bExit ) {
			ExitThread( 0 ) ;
			break ;
		}
	}
	ResetEvent( g_hevtGetChar ) ;
	return c ;
}

void
GotoXY( int x, int y )
{
	SendMessage( g_hwndTty, TTYM_GOTOXY, MAKEWPARAM( y, x ), 0 ) ;
}

void
EraseEOL( void )
{
	SendMessage( g_hwndTty, TTYM_ERASEEOL, 0, 0 ) ;
}

void
EraseEOP( void )
{
	SendMessage( g_hwndTty, TTYM_ERASEEOP, 0, 0 ) ;
}

void
PutChar( char c )
{
	SendMessage( g_hwndTty, TTYM_PUTCHAR, (WPARAM) c, 0 ) ;
}

void
PutKChar( char c1, char c2 )
{
  SendMessage(g_hwndTty, TTYM_PUTKCHAR, MAKEWPARAM(c2, c1), 0);
}

void
PutLine( int y, unsigned char *sjis, short color )
{
	SendMessage( g_hwndTty, TTYM_PUTLINE, MAKEWPARAM( color, y ), (LPARAM) sjis ) ;
}

void
Flush( void )
{
	SendMessage( g_hwndTty, TTYM_FLUSH, 0, 0 ) ;
}

void
GetWH( int *w, int *h )
{
	DWORD	ret ;

	ret = SendMessage( g_hwndTty, TTYM_GETWH, 0, 0 ) ;
	*w = HIWORD( ret ) ;
	*h = LOWORD( ret ) ;
}

void
Exit( int code )
{
	RaiseException( EXCEPTION_QUIT, 0, 0, 0 ) ;
}

void
MessageOut( LPCSTR buf )
{
	sjis2unicode( buf, MessageBuf, sizeof MessageBuf ) ;
	MessageBox( g_hwndMain, MessageBuf, MGTITLE, MB_OK ) ;
}

/*
 * Command line related procedure
 */
static void
cmdline2args( LPSTR cmdline, int *argc, char **argv )
{
	CHAR	c ;
	BOOL	f_skip = TRUE, f_quote = FALSE ;

	argv[ 0 ] = "" ;
	*argc = 1 ;
	while ( c = *cmdline ) {
		if ( f_skip ) {
			if ( !(c == ' ' || c == '\t') ) {
				if ( c == '"' ) {
					f_quote = TRUE ;
					cmdline ++ ;
				}
				argv[ (*argc) ++ ] = cmdline ;
				f_skip = FALSE ;
			}
		} else {
			if ( f_quote ) {
				if ( c == '"' ) {
					f_quote = FALSE ;
					*cmdline = 0 ;
					f_skip = TRUE ;
				}
			} else if ( (c == ' ' || c == '\t') ) {
				*cmdline = 0 ;
				f_skip = TRUE ;
			}
		}
		cmdline ++ ;
	}
}

/*
 * main routine for a thread
 */
static void
ThreadMain( void )
{
	__try {
		Main( g_dwArgc, g_szArgv ) ;
	} __except ( GetExceptionCode() == EXCEPTION_QUIT ) {
		// do nothing
	}
	SendMessage( g_hwndMain, WM_COMMAND, IDM_EXIT, 0 ) ;
	ExitThread( 0 ) ;
}

#if 0
BOOL
Fep_WM_CHAR( HWND hWnd, TCHAR chCharCode, LONG lKeyData )
{
  return FALSE;
}

BOOL
Fep_WM_SYSCHAR( HWND hWnd, TCHAR chCharCode, LONG lKeyData )
{
  return FALSE;
}
#endif

#ifdef FEPCTRL
#define fep_init() /* nothing to do */
#define fep_term() /* nothing to do */

static int fepctrl = FALSE;		/* FEP control enable flag	*/
static int fepmode = TRUE;		/* now FEP mode			*/

static BOOL g_ime_prevopened;

fepmode_on()
{
  if (fepctrl && !fepmode) {
    HIMC hIMC = ImmGetContext(g_hwndTty);
    if (hIMC) {
      ImmSetOpenStatus(hIMC, g_ime_prevopened);
      ImmReleaseContext(g_hwndTty, hIMC);
    }
    fepmode = TRUE;
  }
  return 0;
}

/* The following fepmode_off() provides a doubled fepmode_off()
   feature.  That is, the second call for the fepmode_off() is
   expected to change the previous fep mode as off, without doing any
   actual fep control since the fep mode has been already turned off
   by the first call of the function.  So, the succeeding fepmode_on()
   will not turn on the fep mode even if the previous fep mode was on
   before the first call of fepmode_off().
   
   By Tillanosoft, Mar 21, 1999 */

fepmode_off()
{
  if (fepctrl) {
    if (fepmode) {
      HIMC hIMC = ImmGetContext(g_hwndTty);
      if (hIMC) {
	g_ime_prevopened = ImmGetOpenStatus(hIMC);
	if (g_ime_prevopened) {
	  ImmSetOpenStatus(hIMC, FALSE);
	}
	ImmReleaseContext(g_hwndTty, hIMC);
      }
      fepmode = FALSE;
    }
    else {
      g_ime_prevopened = FALSE;
    }
  }
  return 0;
}

fepmode_set(f, n)
{
  register int s;
  char buf[NFILEN];
  /* extern int ereply(); *//* declared in def.h */

  if (f & FFARG) {
    n = (n > 0);
  }
  else {
    if ((s = ereply("FEP Control: ", buf, NFILEN)) != TRUE)
      return (s);
    if (ISDIGIT(buf[0]) || buf[0] == '-')
      n = (atoi(buf) > 0);
    else if (buf[0] == 't' || buf[0] == 'T')
      n = TRUE;
    else /* if (buf[0] == 'n' || buf[0] == 'N') */
      n = FALSE;
  }
  
  if (!fepctrl && n) {
    fepmode = TRUE;
  }
  else if (fepctrl && !n) {
    /* nothing to do */
  }
  fepctrl = n;
  
  return TRUE;
}

fepmode_chg(f, n)
{
  fepctrl = !fepctrl;
  if (fepctrl) {
    fepmode = TRUE;
  }
  return TRUE;
}

int
fepmode_toggle( int f, int n )
{
  HIMC hIMC = ImmGetContext(g_hwndTty);
  if (hIMC) {
    BOOL ime_open = ImmGetOpenStatus(hIMC);
    g_ime_prevopened = !ime_open;
    ImmSetOpenStatus(hIMC, !ime_open);
    ImmReleaseContext(g_hwndTty, hIMC);
  }
  return TRUE;
}
#endif /* FEPCTRL */

#ifdef KANJI
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
#endif

void
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
putline( int row, int column, unsigned char *s, unsigned char *t, short color )
#else
putline(int row, int column, unsigned char *s, short color)
#endif
{
#ifdef KANJI
        int c1 = 0, c2;
#ifdef HANKANA
	unsigned char *ccp1;
#endif
#endif
	int c;
	unsigned char	sjis[ 256 ], *dst, *cp1, *cp2;

	dst = sjis ;
	cp1 = &s[0] ;
	cp2 = &s[ncol] ;
#ifdef HANKANA
	ccp1 = &t[0] ;
#endif
	while ( cp1 != cp2 ) {
		c = *cp1 ++ ;
#ifdef KANJI
#ifdef HANKANA
		c2 = *ccp1 ++ ;
#endif
		if ( c1 ) {
			etos( c1, c ) ;
			*dst++ = c1 ;
			*dst++ = c ;
			c1 = 0 ;
#ifdef HANKANA
		} else if ( (c & 0xFF) == 0x8E && c2 != 0 ) {
			*dst++ = c2 ;
#endif
		} else if ( ISKANJI( c ) ) {
			c1 = c ;
		} else
#endif
			*dst++ = c ;
	}
	*dst = 0 ;
	PutLine(row, sjis, (short)(color == CMODE ? 1 : 0));
}

#ifndef NO_STARTUP
/* Dialog Proc to change the startup file path */

#define MAXFILENAMESIZE 160
#define INIFILEFILTER TEXT("INI file (*.ini)\0*.ini\0All files (*.*)\0*.*\0")

static OPENFILENAME templateofn = {
    sizeof(OPENFILENAME), NULL, 0, INIFILEFILTER, NULL, 0, 0,
    NULL, 0, NULL, 0, NULL, TEXT("INI File"),
    OFN_HIDEREADONLY, 0, 0, NULL, 0, NULL, NULL};

static BOOL CALLBACK
StartFileProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
  TCHAR	unicode[MAX_PATH];
  DWORD foo = sizeof(unicode);

  switch (message) {
  case WM_INITDIALOG:
    if (RegQueryString(HKEY_CURRENT_USER, NGREGKEY, NGSTARTUPFILEVAL,
		       unicode, &foo) == ERROR_SUCCESS) {
      SetDlgItemText(hDlg, IDC_STARTUPFILE, unicode);
    }
    return 1;

  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_FILEOPENDLG:
      {
	OPENFILENAME ofn;
	TCHAR filename[MAXFILENAMESIZE];
	int stat;

	ofn = templateofn;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAXFILENAMESIZE;
	ofn.lpstrFile[0] = (TCHAR)0;
	stat = GetOpenFileName(&ofn);
	if (stat) {
	  SetDlgItemText(hDlg, IDC_STARTUPFILE, ofn.lpstrFile);
	}
      }
      break;

#ifndef _WIN32_WCE
    case IDC_STARTUPFILE:
      switch (HIWORD(wParam)) {
      case EN_CHANGE:
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
	break;
      }
      break;
#endif
    }
    return 0;

  case WM_NOTIFY:
    switch (((LPNMHDR)lParam)->code) {
    case PSN_SETACTIVE: /* Visit this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, 0);
      return TRUE;

    case PSN_APPLY:
      GetDlgItemText(hDlg, IDC_STARTUPFILE, unicode, foo / sizeof(TCHAR));
      RegSetString(HKEY_CURRENT_USER, NGREGKEY, NGSTARTUPFILEVAL, unicode);
      SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
      return TRUE;

    case PSN_KILLACTIVE: /* Go away from this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
      return(TRUE);

    case PSN_RESET: /* CANCEL Button */
      return TRUE;
    }

  default:
    return 0;
  }
}
#endif

#ifdef CTRLMAP
static BOOL CALLBACK
KeyMapProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
  DWORD val;

  switch (message) {
  case WM_INITDIALOG:
    val = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, 
			NGCTRLKEYMAPVAL) ? TRUE : FALSE;
    Button_SetCheck(GetDlgItem(hDlg, IDC_CONTROLMAP), val);
    return 1;

  case WM_COMMAND:
#ifndef _WIN32_WCE
    switch (LOWORD(wParam)) {
    case IDC_CONTROLMAP:
      SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
      break;
    }
#endif
    return 0;

  case WM_NOTIFY:
    switch (((LPNMHDR)lParam)->code) {
    case PSN_SETACTIVE: /* Visit this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, 0);
      return TRUE;

    case PSN_APPLY:
      val = Button_GetCheck(GetDlgItem(hDlg, IDC_CONTROLMAP));
      RegSetDWord(HKEY_CURRENT_USER, NGREGKEY, NGCTRLKEYMAPVAL, val);
      g_ctrlmap = val;
      SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
      return TRUE;

    case PSN_KILLACTIVE: /* Go away from this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
      return(TRUE);

    case PSN_RESET: /* CANCEL Button */
      return TRUE;
    }

  default:
    return 0;
  }
}
#endif

struct _beepsounds {
  UINT title;
  UINT val;
} beepsounds[] = {
  {IDS_OK, MB_OK},
  {IDS_ICONASTERISK, MB_ICONASTERISK},
  {IDS_ICONEXCLAMATION, MB_ICONEXCLAMATION},
  {IDS_ICONHAND, MB_ICONHAND},
  {IDS_ICONQUESTION, MB_ICONQUESTION}
};

#define NUMBEEPSOUNDS (sizeof(beepsounds) / sizeof(struct _beepsounds))

#define WAVFILEFILTER TEXT("Wave file (*.wav)\0*.wav\0All files (*.*)\0*.*\0")

static OPENFILENAME templatewaveofn = {
    sizeof(OPENFILENAME), NULL, 0, WAVFILEFILTER, NULL, 0, 0,
    NULL, 0, NULL, 0, TEXT("\\Windows"), TEXT("Wave File"),
    OFN_HIDEREADONLY, 0, 0, NULL, 0, NULL, NULL};

static void
SetBeepRadio(HWND hDlg, BOOL messagebeep)
{
  EnableWindow(GetDlgItem(hDlg, IDC_MESSAGECOMBO), messagebeep);
  EnableWindow(GetDlgItem(hDlg, IDC_SOUNDFILE), !messagebeep);
  EnableWindow(GetDlgItem(hDlg, IDC_SOUNDBUTTON), !messagebeep);
}

static void
SetBeepButtons(HWND hDlg, DWORD dobeep)
{
  EnableWindow(GetDlgItem(hDlg, IDC_PLAY), dobeep);
  EnableWindow(GetDlgItem(hDlg, IDC_MESSAGEBEEP), dobeep);
  EnableWindow(GetDlgItem(hDlg, IDC_MESSAGECOMBO), dobeep);

#ifdef TARGET_WCEVER_IS_100
  EnableWindow(GetDlgItem(hDlg, IDC_PLAYSOUND), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDC_SOUNDFILE), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDC_SOUNDBUTTON), FALSE);
#else
  EnableWindow(GetDlgItem(hDlg, IDC_PLAYSOUND), dobeep);
  EnableWindow(GetDlgItem(hDlg, IDC_SOUNDFILE), dobeep);
  EnableWindow(GetDlgItem(hDlg, IDC_SOUNDBUTTON), dobeep);
#endif
  if (dobeep) {
    /* dobeep == 1 means to use sndPlaySound(), not to use MessageBeep() */
    Button_SetCheck(GetDlgItem(hDlg, IDC_MESSAGEBEEP), dobeep != 1);
    Button_SetCheck(GetDlgItem(hDlg, IDC_PLAYSOUND), dobeep == 1);
    SetBeepRadio(hDlg, dobeep != 1);
  }
}

static void
BeepGetButtons(HWND hDlg, DWORD *beep, LPTSTR beepfile, int len)
{
  DWORD val;

  val = Button_GetCheck(GetDlgItem(hDlg, IDC_BEEP));
  if (val) {
    if (Button_GetCheck(GetDlgItem(hDlg, IDC_PLAYSOUND))) {
      *beep = 1;
    }
    else {
      DWORD sel;
      sel = SendMessage(GetDlgItem(hDlg, IDC_MESSAGECOMBO),
			CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
      if (sel != CB_ERR) {
	*beep = beepsounds[sel].val + NG_WAVE_OFFSET;
      }
    }
  }
  else {
    *beep = 0;
  }
  GetDlgItemText(hDlg, IDC_SOUNDFILE, beepfile, len);
}

static BOOL CALLBACK
BeepProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
  DWORD val;
  HWND hcb;
  struct _beepsounds *pb, *epb;
  TCHAR buf[128];
  UINT cursound, nth = 0;

  switch (message) {
  case WM_INITDIALOG:
    /* initialize the combo box */
    cursound = g_beepsound - NG_WAVE_OFFSET;
    hcb = GetDlgItem(hDlg, IDC_MESSAGECOMBO);
    SendMessage(hcb, CB_RESETCONTENT, 0, 0);
    for (pb = beepsounds, epb = pb + NUMBEEPSOUNDS ; pb < epb ; pb++) {
      if (cursound == pb->val) {
	nth = pb - beepsounds;
      }
      LoadString(g_hInst, pb->title, buf, sizeof(buf) / sizeof(TCHAR));
      SendMessage(hcb, CB_ADDSTRING, 0, (LONG)buf);
    }
    SendMessage(hcb, CB_SETCURSEL, (WPARAM)nth, (LPARAM)0);
    
    /* initialize the edit box */
    if (g_beepfile[0]) {
      SetDlgItemText(hDlg, IDC_SOUNDFILE, g_beepfile);
    }
    
    /* set the current configuration for beep sounds */
    Button_SetCheck(GetDlgItem(hDlg, IDC_BEEP), g_beepsound);
    SetBeepButtons(hDlg, g_beepsound);
    return 1;

  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_MESSAGEBEEP:
    case IDC_PLAYSOUND:
      SetBeepRadio(hDlg, LOWORD(wParam) == IDC_MESSAGEBEEP);
#ifndef _WIN32_WCE
      SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
#endif
      break;

    case IDC_BEEP:
      val = Button_GetCheck(GetDlgItem(hDlg, IDC_BEEP));
      if (val) {
	val = Button_GetCheck(GetDlgItem(hDlg, IDC_PLAYSOUND)) ? 1 : 2;
      }
      SetBeepButtons(hDlg, val);
#ifndef _WIN32_WCE
      SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
#endif
      break;

    case IDC_SOUNDBUTTON:
      {
	OPENFILENAME ofn;
	TCHAR filename[MAXFILENAMESIZE];
	int stat;

	ofn = templatewaveofn;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAXFILENAMESIZE;
	ofn.lpstrFile[0] = (TCHAR)0;
#ifndef _WIN32_WCE
#define MEDIA_FOLDER TEXT("\\Media")
#define MEDIA_FOLDER_LEN (sizeof(MEDIA_FOLDER) / sizeof(TCHAR))
	{
	  TCHAR initialdir[MAX_PATH];
	  int len = GetWindowsDirectory(initialdir, MAX_PATH);
	  if (0 < len) {
	    if (len < MAX_PATH - MEDIA_FOLDER_LEN) {
	      lstrcat(initialdir, MEDIA_FOLDER);
	    }
	    ofn.lpstrInitialDir = initialdir;
	  }
	}
#endif
	stat = GetOpenFileName(&ofn);
	if (stat) {
	  SetDlgItemText(hDlg, IDC_SOUNDFILE, ofn.lpstrFile);
#ifndef _WIN32_WCE
	  SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
#endif
	}
      }
      break;

    case IDC_PLAY:
      val = g_beepsound;
      lstrcpy(buf, g_beepfile);
      BeepGetButtons(hDlg, &g_beepsound,
		     g_beepfile, sizeof(g_beepfile) / sizeof(TCHAR));
      ttbeep();
      g_beepsound = val;
      lstrcpy(g_beepfile, buf);
      break;

#ifndef _WIN32_WCE
    case IDC_MESSAGECOMBO:
      SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
      break;

    case IDC_SOUNDFILE:
      switch (HIWORD(wParam)) {
      case EN_CHANGE:
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
	break;
      }
      break;
#endif
    }
    return 0;

  case WM_NOTIFY:
    switch (((LPNMHDR)lParam)->code) {
    case PSN_SETACTIVE: /* Visit this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, 0);
      return TRUE;

    case PSN_APPLY:
      BeepGetButtons(hDlg, &g_beepsound,
		     g_beepfile, sizeof(g_beepfile) / sizeof(TCHAR));
      RegSetDWord(HKEY_CURRENT_USER, NGREGKEY, NGBEEPSOUNDVAL, g_beepsound);
      RegSetString(HKEY_CURRENT_USER, NGREGKEY, NGBEEPFILEVAL, g_beepfile);
      SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
      return TRUE;

    case PSN_KILLACTIVE: /* Go away from this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
      return(TRUE);

    case PSN_RESET: /* CANCEL Button */
      return TRUE;
    }

  default:
    return 0;
  }
}

#ifndef _WIN32_WCE /* font configuration is not made functional for CE */

#ifdef KANJI
#define REQUIRED_CHARSET SHIFTJIS_CHARSET
#ifndef _WIN32_WCE
#define REQUIRED_PITCH FIXED_PITCH
#endif
#else /* !KANJI */
#define REQUIRED_PITCH FIXED_PITCH
#endif /* !KANJI */

static int CALLBACK
SetFontNames(CONST LOGFONT *lpelf, CONST TEXTMETRIC *lpntm, 
	     DWORD FontType, LPARAM lParam)
{
  HWND hDlg = (HWND)lParam, hcb = GetDlgItem(hDlg, IDC_FONTNAME);

  if ((lpelf->lfPitchAndFamily & 
       (FF_ROMAN | FF_SWISS | FF_MODERN | FF_SCRIPT | FF_DECORATIVE)) ==
      FF_MODERN
#ifdef REQUIRED_CHARSET
      && lpelf->lfCharSet == REQUIRED_CHARSET
#endif
#ifdef REQUIRED_PITCH
      && (lpelf->lfPitchAndFamily & REQUIRED_PITCH)
#endif
      ) {
    SendMessage(hcb, CB_ADDSTRING, 0, (LONG)lpelf->lfFaceName);
  }
  return 1;
}

static LPTSTR fontpoints[] = {
  TEXT("8"),
  TEXT("9"),
  TEXT("10"),
  TEXT("11"),
  TEXT("12"),
  TEXT("14"),
  TEXT("16"),
  TEXT("18"),
  TEXT("20"),
  TEXT("22"),
  TEXT("24"),
  TEXT("28"),
  TEXT("32")
};

#define NUMPOINTS (sizeof(fontpoints) / sizeof(int))

#define MAX_LINESPACE 32

static void
SetFontEntries(HWND hDlg)
{
  HDC hdc;
  HWND hSpin;
  LPTSTR *p, *ep;
  TCHAR fontname[LF_FACESIZE];
  DWORD val;

  hdc = GetDC(g_hwndTty);
  SendMessage(GetDlgItem(hDlg, IDC_FONTNAME), CB_RESETCONTENT, 0, 0);
  EnumFontFamilies(hdc, NULL, SetFontNames, (LPARAM)hDlg);
  ReleaseDC(g_hwndTty, hdc);

  SendMessage(GetDlgItem(hDlg, IDC_POINT), CB_RESETCONTENT, 0, 0);
  for (p = fontpoints, ep = p + NUMPOINTS ; p < ep ; p++) {
    SendMessage(GetDlgItem(hDlg, IDC_POINT), CB_ADDSTRING, 0, (LONG)*p);
  }

  /* Do something for spin (up-down) control */
  hSpin = GetDlgItem(hDlg, IDC_SPIN);
  SendMessage(hSpin, UDM_SETRANGE, (WPARAM)0,
	      (LPARAM)MAKELONG((short)MAX_LINESPACE, (short)0));
  SendMessage(hSpin, UDM_SETPOS, (WPARAM)0,
	      (LPARAM)MAKELONG((short)0, 0)); /* line space after (short) */

  /* set the current configurations */
  val = sizeof(fontname);
  if (RegQueryString(HKEY_CURRENT_USER, NGREGKEY, NGFONTNAMEVAL,
		     fontname, &val) == ERROR_SUCCESS) {
    SendMessage(GetDlgItem(hDlg, IDC_FONTNAME), CB_SELECTSTRING,
		(WPARAM)-1, (LPARAM)fontname);
    val = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, NGFONTSIZEVAL);
    if (val > 0) {
      SetDlgItemInt(hDlg, IDC_POINT, val, FALSE);
      val = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, NGLINESPACEVAL);
      SetDlgItemInt(hDlg, IDC_LINESPACE, val, FALSE);
      Button_SetCheck(GetDlgItem(hDlg, IDC_SPECIFY), TRUE);
      return;
    }
  }
  Button_SetCheck(GetDlgItem(hDlg, IDC_NOSPECIFY), TRUE);
}

static void
ApplyFontEntries(HWND hDlg)
{
  DWORD point, linespace;
  TCHAR fontname[LF_FACESIZE];

  if (Button_GetCheck(GetDlgItem(hDlg, IDC_SPECIFY))) {
    point = GetDlgItemInt(hDlg, IDC_POINT, NULL, FALSE);
    linespace = GetDlgItemInt(hDlg, IDC_LINESPACE, NULL, FALSE);
    GetDlgItemText(hDlg, IDC_FONTNAME, fontname, LF_FACESIZE);

    RegSetString(HKEY_CURRENT_USER, NGREGKEY, NGFONTNAMEVAL, fontname);
    RegSetDWord(HKEY_CURRENT_USER, NGREGKEY, NGFONTSIZEVAL, point);
    RegSetDWord(HKEY_CURRENT_USER, NGREGKEY, NGLINESPACEVAL, linespace);
  }
  else {
    RegRemoveValue(HKEY_CURRENT_USER, NGREGKEY, NGFONTNAMEVAL);
    RegRemoveValue(HKEY_CURRENT_USER, NGREGKEY, NGFONTSIZEVAL);
    RegRemoveValue(HKEY_CURRENT_USER, NGREGKEY, NGLINESPACEVAL);
  }

  /* Then, notify the change to TTYCTRL */
  SendMessage(g_hwndTty, TTYM_FONTCHANGED, (WPARAM)0, (LPARAM)0);
}

static BOOL CALLBACK
FontProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
  switch (message) {
  case WM_INITDIALOG:
    SetFontEntries(hDlg);
    return 1;

  case WM_COMMAND:
    switch (LOWORD(wParam)) {
#ifndef _WIN32_WCE
    case IDC_NOSPECIFY:
    case IDC_SPECIFY:
    case IDC_FONTNAME:
    case IDC_POINT:
    case IDC_LINESPACE:
      SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
      break;
#endif
    default:
      break;
    }
    return 0;

  case WM_NOTIFY:
    switch (((LPNMHDR)lParam)->code) {
    case PSN_SETACTIVE: /* Visit this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, 0);
      return TRUE;

    case PSN_APPLY:
      ApplyFontEntries(hDlg);
      SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
      return TRUE;

    case PSN_KILLACTIVE: /* Go away from this page */
      SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
      return(TRUE);

    case PSN_RESET: /* CANCEL Button */
      return TRUE;
    }

  default:
    return 0;
  }
}

#endif /* _WIN32_WCE */

#define MAX_PAGES 5

/*
   Add one property sheet page.
 */

static void
AddPage(LPPROPSHEETHEADER ppsh, UINT id, DLGPROC pfn)
{
  if (ppsh->nPages < MAX_PAGES) {
    PROPSHEETPAGE psp;

    psp.dwSize = sizeof(psp);
    psp.dwFlags = PSP_DEFAULT;
    psp.hInstance = ppsh->hInstance;
#ifdef __BORLANDC__
    psp.DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(id);
#else
    psp.pszTemplate = MAKEINTRESOURCE(id);
#endif
    psp.pfnDlgProc = pfn;
    psp.lParam = 0;

#ifdef __BORLANDC__
    ppsh->DUMMYUNIONNAME3.phpage[ppsh->nPages] = CreatePropertySheetPage(&psp);
    if (ppsh->DUMMYUNIONNAME3.phpage[ppsh->nPages]) {
#else
    ppsh->phpage[ppsh->nPages] = CreatePropertySheetPage(&psp);
    if (ppsh->phpage[ppsh->nPages]) {
#endif
      ppsh->nPages++;
    }
  }
}

/*
 CreatePropertySheet creates a property sheet.
 */

static void
CreatePropertySheet(HINSTANCE hInstance, HWND hwnd)
{
  PROPSHEETHEADER psh;
  HPROPSHEETPAGE rPages[MAX_PAGES];

  psh.hwndParent = hwnd;
  psh.dwSize = sizeof(psh);
  psh.dwFlags = 0;
  psh.hInstance = hInstance;
#ifdef __BORLANDC__
  psh.DUMMYUNIONNAME.hIcon = NULL;
#else
  psh.hIcon = NULL;
#endif
  psh.pszCaption = MAKEINTRESOURCE(IDS_CONFIGNAME);
  psh.nPages = 0;
#ifdef __BORLANDC__
  psh.DUMMYUNIONNAME2.nStartPage = 0;
  psh.DUMMYUNIONNAME3.phpage = rPages;
#else
  psh.nStartPage = 0;
  psh.phpage = rPages;
#endif
  psh.pfnCallback = NULL;
#ifndef NO_STARTUP  
  AddPage(&psh, IDD_STARTUPFILE, StartFileProc);
#endif
#ifdef CTRLMAP
  AddPage(&psh, IDD_KEY, KeyMapProc);
#endif
  AddPage(&psh, IDD_BEEP, BeepProc);
#ifndef _WIN32_WCE
  AddPage(&psh, IDD_FONT, FontProc);
#endif

  PropertySheet(&psh);
}


/* change the path to store the path into the registry database of Win32 */

int
ConfigStartupFilePath(int f, int n)
{
  CreatePropertySheet(g_hInst, g_hwndTty);
  return TRUE;
}

