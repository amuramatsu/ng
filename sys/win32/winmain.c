/* $Id: winmain.c,v 1.8 2001/02/14 09:19:09 amura Exp $ */
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
 * NG : NG program main routine
 *
 * 1998/11/14:Eiichiroh Itoh
 *
 */

/*
 * $Log: winmain.c,v $
 * Revision 1.8  2001/02/14 09:19:09  amura
 * code cleanup around putline()
 *
 * Revision 1.7  2001/01/20 15:47:23  amura
 * putline() support Hojo Kanji now
 *
 * Revision 1.6  2000/11/16 14:21:32  amura
 * merge Ng for win32 0.5
 *
 * Revision 1.5  2000/10/23 16:52:52  amura
 * add GPL copyright to header
 *
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
#include	<commdlg.h>
#include	<commctrl.h>
#include "config.h"
#include "def.h"
#include	"ttyctrl.h"
#include	"resource.h"
#include	"winmain.h"
#include	"tools.h"

#ifdef KANJI
#ifdef USE_KCTRL
#include "kctrl.h"
#include "cefep.h"
#define VALID_KCTRL_VERSION 13
#else /* not USE_KCTRL */
#include	<imm.h>
#endif /* USE_KCTRL */
#endif /* KANJI */

#if defined(WIN32_PLATFORM_PSPC) && 300 <= _WIN32_WCE
#define USE_SHMENU /* Define a macro to indicate using PocketPC menu */
#define POCKETPC_MENU_HEIGHT 26 /* Height of Menubar */
#else
#define POCKETPC_MENU_HEIGHT 0
#endif

/* #define MG_FOR_PPC
   This should be defined in the Makefile or dsp file */

#if defined(MG_FOR_PPC) || defined(__PsPC__) || 300 <= _WIN32_WCE
#include <aygshell.h>
#endif

#ifdef COMMANDBANDS
#if 200 <= _WIN32_WCE
#define USE_COMMANDBANDS
/* #define USE_BEGINTHREAD */
/* instead of CreateThread.  Though I do not care... */
#endif
#endif

#if !defined(MG_FOR_PPC) && !defined(_WIN32_WCE_EMULATION)
#ifndef USE_KCTRL
#define MG_IME_CONTROL
#endif /* USE_KCTRL */
#endif /* !defined(MG_FOR_PPC) && !defined(_WIN32_WCE_EMULATION) */

#define		EXCEPTION_QUIT			(1)
#define		IDM_START				(1001)
#define		IDM_EXIT				(1002)
#define		IDC_TTY					(2001)

#ifdef KANJI
#define MGTITLE TEXT("Ng")
#define MGCLASS TEXT("NG")
#else
#define MGTITLE TEXT("Mg")
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
int		g_dwArgc ;
LPSTR		g_szArgv[ 128 ] ;
#if defined(COMMANDBANDS) && defined(USE_COMMANDBANDS)
HIMAGELIST	g_himlrebar; /* Image list for rebar bands */
#endif
#ifdef CTRLMAP
DWORD g_ctrlmap;
#endif
DWORD g_beepsound, g_keyboardlocale;
TCHAR g_beepfile[128];
#if defined(KANJI) && defined(USE_KCTRL)
DWORD		g_dwDllVersion = 0 ;
#endif

#if (defined(COMMANDBANDS) && !defined(USE_SHMENU)) || !defined(_WIN32_WCE)
#define USE_REGINFO
#endif

#ifdef USE_REGINFO
/* Settings stored in registry */
struct reginfo {
#if defined(COMMANDBANDS) && !defined(USE_SHMENU)
  BOOL showbands;
#endif /* COMMANDBANDS */
#ifndef _WIN32_WCE
  BOOL maximized;
  RECT rect; /* rect for the window */
#else /* if _WIN32_WCE */
#if defined(COMMANDBANDS) && defined(USE_COMMANDBANDS) \
    && !defined(USE_SHMENU)
  COMMANDBANDSRESTOREINFO cbinfo[2]; /* Info for Bands */
#endif
#endif /* _WIN32_WCE */
  BOOL valid; /* tell if stored info is valid */
};

static struct reginfo g_reginfo;
#endif /* USE_REGINFO */

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
#if defined(KANJI) && defined(USE_KCTRL)
	g_dwDllVersion = GetKVersion() ;
	if ( g_dwDllVersion < VALID_KCTRL_VERSION ) {
		wsprintf( MessageBuf,
				  TEXT("Obsolete KCTRL.DLL used\r\nPlease use Ver%d.%02d or later"),
				  VALID_KCTRL_VERSION / 100, VALID_KCTRL_VERSION % 100 ) ;
		MessageBox( NULL, MessageBuf, g_szTitleName, MB_OK|MB_ICONASTERISK ) ;
		return FALSE ;
	}
	if ( !InitFep() ) {
		MessageBox( NULL, TEXT("Can't initialize FEP."), g_szTitleName, MB_OK|MB_ICONASTERISK ) ;
		return FALSE ;
	}
	if ( !InitKanjiControls() ) {
		wsprintf( MessageBuf,
				  TEXT("KCTRL.DLL Initialize error\r\nError=%x"),
				  GetLastError() ) ;
		MessageBox( NULL, MessageBuf, g_szTitleName, MB_OK|MB_ICONASTERISK ) ;
		return FALSE ;
	}
#endif
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

	{
	  DWORD foo;
	/* read configuration for Ng for Win32 */
#ifdef CTRLMAP
	  g_ctrlmap = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, 
				    NGCTRLKEYMAPVAL) ? TRUE : FALSE;
#endif
	  g_beepsound = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY,
				      NGBEEPSOUNDVAL);
	  g_keyboardlocale = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, 
					   NGKEYBOARDLOCALEVAL);
	  if (g_keyboardlocale == 0) {
#ifndef JAPANESE_KEYBOARD
	    g_keyboardlocale = NGKEYBOARD_US;
#else
	    g_keyboardlocale = NGKEYBOARD_JP;
#endif
	  }
	  SendMessage(g_hwndTty, TTYM_SETKEYBOARDLOCALE,
		      (WPARAM)g_keyboardlocale, 0);

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
#if defined(KANJI) && defined(USE_KCTRL)
	ReleaseKanjiControls() ;
#endif
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

	/* initialize common controls */
#if 200 <= _WIN32_WCE
	{
	  INITCOMMONCONTROLSEX icex;

	  icex.dwSize = sizeof(icex);
	  icex.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES;
	  InitCommonControlsEx(&icex);
	}
#else
	InitCommonControls();
#endif
	return TRUE ;
}

#define RegQueryBinary(r, k, v, d, s) RegQueryString(r, k, v, (LPTSTR)d, s)

#ifdef COMMANDBANDS
static HWND g_hwndCB;
#endif

/*
 * create a main window
 */
static	BOOL
init_instance( int nCmdShow )
{
	RECT	rect ;
        HMENU hMenu = NULL;
	int x, y, cx, cy;
#ifdef USE_REGINFO
	DWORD foo;
#endif

#ifndef _WIN32_WCE
	hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MAINMENU));
#endif /* _WIN32_WCE */

#ifdef USE_REGINFO /* get window position from registry */
	foo = sizeof(g_reginfo);
        if (RegQueryBinary(HKEY_CURRENT_USER, NGREGKEY, NGPREVPOSVAL,
			   &g_reginfo, &foo) == ERROR_SUCCESS) {
#ifndef _WIN32_WCE
#define MG_MIN_HEIGHT 120
#define MG_MIN_WIDTH 160
	  if (g_reginfo.rect.bottom - g_reginfo.rect.top < MG_MIN_HEIGHT) {
	    g_reginfo.rect.bottom = g_reginfo.rect.top + MG_MIN_HEIGHT;
	  }
	  if (g_reginfo.rect.right - g_reginfo.rect.left < MG_MIN_WIDTH) {
	    g_reginfo.rect.right = g_reginfo.rect.left + MG_MIN_WIDTH;
	  }
#endif /* _WIN32_WCE */
	}
	else {
	  g_reginfo.valid = FALSE;
#ifndef _WIN32_WCE
	  g_reginfo.rect.top = 0;
	  g_reginfo.rect.bottom = 296;
	  g_reginfo.rect.left = 0;
	  g_reginfo.rect.right = 500;
#endif /* _WIN32_WCE */
	}

#if defined(COMMANDBANDS) && !defined(USE_SHMENU)
	if (!g_reginfo.valid) {
	  g_reginfo.showbands = TRUE;
	}
#endif /* defined(COMMANDBANDS) && !defined(USE_SHMENU) */
#endif /* USE_REGINFO */

	x = y = cx = cy = CW_USEDEFAULT;
#if defined(WIN32_PLATFORM_PSPC) && 300 <= _WIN32_WCE
	{
	  SIPINFO si;

	  memset(&si, 0, sizeof(SIPINFO));
	  si.cbSize = sizeof(si);
	  if (SHSipInfo(SPI_GETSIPINFO, 0, &si, 0)) {
	    if (si.fdwFlags & SIPF_ON) {
	      x = si.rcVisibleDesktop.left;
	      y = si.rcVisibleDesktop.top;
	      cx = si.rcVisibleDesktop.right - si.rcVisibleDesktop.left;
	      cy = si.rcVisibleDesktop.bottom - si.rcVisibleDesktop.top
		+ POCKETPC_MENU_HEIGHT;
	    }
	  } 
  	}
#endif

	/* create a main window */
	g_hwndMain = CreateWindowEx( 0, g_szClassName,
					g_szTitleName,
#ifdef	_WIN32_WCE
				        x, y, cx, cy,
#else	/* _WIN32_WCE */
                                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				        g_reginfo.rect.left, 
				        g_reginfo.rect.top,
				        g_reginfo.rect.right -
				          g_reginfo.rect.left,
				        g_reginfo.rect.bottom -
				          g_reginfo.rect.top,
#endif	/* _WIN32_WCE */
					NULL, hMenu, g_hInst, NULL ) ;
	if ( !g_hwndMain ) {
		return FALSE;
	}
	GetClientRect( g_hwndMain, &rect ) ;

#ifdef COMMANDBANDS
	{
	  int nCommandHeight = 0;

#ifdef _WIN32_WCE
#ifndef USE_SHMENU
#ifdef USE_COMMANDBANDS
	  CommandBands_Show(g_hwndCB, g_reginfo.showbands);
	  if (g_reginfo.showbands) {
	    nCommandHeight = CommandBands_Height(g_hwndCB);
	  }
#else
	  CommandBar_Show(g_hwndCB, g_reginfo.showbands); 
	  if (g_reginfo.showbands) {
	    nCommandHeight = CommandBar_Height(g_hwndCB);
	  }
#endif /* USE_COMMANDBANDS */
#else /* if USE_SHMENU */
	  nCommandHeight = 0;
#endif /* USE_SHMENU */
#endif /* _WIN32_WCE */
	  rect.top += nCommandHeight;
	}
#endif /* COMMANDBANDS */

	/* create a TTY window */
	g_hwndTty = CreateWindowEx( 0, CTRL_TTYVIEW,
					TEXT(""),
					WS_VISIBLE|WS_CHILD,
					rect.left, rect.top,
				        rect.right - rect.left,
				        rect.bottom - rect.top,
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

#ifdef COMMANDBANDS

#if defined(USE_COMMANDBANDS) && !defined(USE_SHMENU)
#define RB_ICON_CX 11
#define RB_ICON_CY 15

/* Initialize the images for rebar bands */
static HIMAGELIST
InitRebarImageLists(HWND hwnd)
{
  HBITMAP hbmp;
  HIMAGELIST res;

  /* Create the image list for the item pictures */
  res = ImageList_Create(RB_ICON_CX, RB_ICON_CY, ILC_COLOR | ILC_MASK, 2, 0);
    /* Create two.  No more will be added.  */

  if (res) {
    /* Add the bitmaps to the list */
    hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_REBAR));
    if (hbmp) {
      if (ImageList_AddMasked(res, hbmp, RGB(255, 0, 0)) != -1) {
	/* Fail if not all the images were added */
	if (3 <= ImageList_GetImageCount(res)) {
	  /* All is fine.  But this specific condition does not check
	     anything. */
	}
      }
      /* Clean up the GDI objects */
      DeleteObject(hbmp);
    }
    else {
      ImageList_Destroy(res);
      res = (HIMAGELIST)NULL;
    }
  }
  return res;
}
#endif /* defined(USE_COMMANDBANDS) && !defined(USE_SHMENU) */

#ifndef USE_SHMENU
/* It is very strange for me to define this by myself... */
#define NUM_STD_BITMAPS   15

#define TBSTATE_DISABLED 0

#define BTNID(x) ((x) - IDBN_MARK + NUM_STD_BITMAPS)

/* Tool buttons */
static TBBUTTON MGButton[] = {
#ifndef USE_COMMANDBANDS
  /* For a space */
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0,  0},
#endif
  {BTNID(IDBN_MARK), IDC_MARK, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, -1},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0,  0},
  {STD_CUT, IDC_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, -1},
  {STD_COPY, IDC_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, -1},
  {STD_PASTE, IDC_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, -1},
  {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0,  0},
  {BTNID(IDBN_PRIOR), IDC_PRIOR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, -1},
  {BTNID(IDBN_NEXT), IDC_NEXT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, -1},
};

/* Tool tips */
static int MGTips[] = {
  IDS_MARK,
  IDS_CUT,
  IDS_COPY,
  IDS_PASTE,
  IDS_PRIOR,
  IDS_NEXT,
};

#define NUM_TIPS (sizeof(MGTips) / sizeof(int))

#define MAX_TIP_STRING 32 /* Max string length for tool tips */

static LPTSTR *g_tooltips;

/* Create resource from Tool tip strings */
static LPTSTR *
LoadToolTips(int *tips)
{
  int *p, *ep;
  LPTSTR *res, *ps;
  TCHAR buf[MAX_TIP_STRING];

  res = LocalAlloc(LPTR, sizeof(LPTSTR) * NUM_TIPS);
  if (res) {
    for (p = MGTips, ep = p + NUM_TIPS, ps = res ; p < ep ; p++, ps++) {
      LoadString(g_hInst, *p, buf, MAX_TIP_STRING);
      *ps = LocalAlloc(LPTR, sizeof(TCHAR) * (lstrlen(buf) + 1));
      if (*ps) {
	lstrcpy(*ps, buf);
      }
      else {
	LPTSTR *q;

	for (q = res ; q < ps ; q++) {
	  LocalFree(*q);
	}
	LocalFree(res);
	return NULL;
      }
    }
  }
  g_tooltips = res;
  return res;
}

/* Destroy Tool tip strings  */
static void
FreeToolTips(void)
{
  if (g_tooltips) {
    LPTSTR *p, *ep;

    for (p = g_tooltips, ep = p + NUM_TIPS ; p < ep ; p++) {
      LocalFree(*p);
    }
    LocalFree(g_tooltips);
    g_tooltips = NULL;
  }
}

/* Create tool buttons */
static void
CreateToolButtons(HWND hwnd)
{
#ifdef _WIN32_WCE
  /* Are there any macro for "16"? */
  CommandBar_AddBitmap(hwnd, HINST_COMMCTRL, IDB_STD_SMALL_COLOR,
		       NUM_STD_BITMAPS, 16, 16);
  CommandBar_AddBitmap(hwnd, g_hInst, IDB_BUTTONS, 3, 16, 16);
  CommandBar_AddButtons(hwnd, sizeof(MGButton)/sizeof(TBBUTTON), MGButton);

  if (LoadToolTips(MGTips)) {
    CommandBar_AddToolTips(hwnd, NUM_TIPS, g_tooltips);
  }
#endif /* _WIN32_WCE */
}

#endif /* !USE_SHMENU */

#define IDD_REBAR    901 /* ID used at message processing */
#define SF_RBMENU    902
#define SF_RBBUTTON  903

#endif /* COMMANDBANDS */

void
MainWMCreate( HWND hWnd )
{
	HICON	hIcon ;

	hIcon = (HICON) LoadImage( g_hInst, MAKEINTRESOURCE(IDI_APPICON),
							  IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) ;
	if ( hIcon ) {
		SendMessage( hWnd, WM_SETICON, FALSE, (LPARAM)hIcon ) ;
	}

#ifdef _WIN32_WCE
#ifdef COMMANDBANDS
#ifndef USE_SHMENU
#ifndef USE_COMMANDBANDS
  g_hwndCB = CommandBar_Create(g_hInst, hWnd, 1);
  CommandBar_InsertMenubar(g_hwndCB, g_hInst, IDR_MAINMENU, 0);
  CreateToolButtons(g_hwndCB);
#ifndef MG_FOR_PPC
    /* add [X] Button */
  CommandBar_AddAdornments(g_hwndCB, 0, 0);
#endif
#else /* if USE_COMMANDBANDS */
  {
    HWND hwnd2;
    REBARBANDINFO arbbi[2];

    /* Initialize images for Rebar bands */
    g_himlrebar = InitRebarImageLists(hWnd);

    g_hwndCB = CommandBands_Create(g_hInst, hWnd, IDD_REBAR,
				 RBS_VARHEIGHT | RBS_BANDBORDERS |
				 RBS_SMARTLABELS, g_himlrebar);
    
    arbbi[0].cbSize = sizeof(REBARBANDINFO);
    arbbi[0].fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_IMAGE | RBBIM_ID;
    arbbi[0].wID = SF_RBMENU;
    arbbi[0].iImage = 0;
    arbbi[0].fStyle = RBBS_NOGRIPPER;
    arbbi[0].cx = 62;
    
    arbbi[1].cbSize = sizeof(REBARBANDINFO);
    arbbi[1].fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_IMAGE | RBBIM_ID;
    arbbi[1].wID = SF_RBBUTTON;
    arbbi[1].iImage = 1;
    arbbi[1].fStyle = 0;
    arbbi[1].cx = 300;

    if (g_reginfo.valid) { /* if valid info stored in registry */
      arbbi[0].fStyle = g_reginfo.cbinfo[0].fStyle;
      arbbi[1].fStyle = g_reginfo.cbinfo[1].fStyle;
      arbbi[0].cx = g_reginfo.cbinfo[0].cxRestored;
      arbbi[1].cx = g_reginfo.cbinfo[1].cxRestored;
    }
    
    CommandBands_AddBands(g_hwndCB, g_hInst, 2, arbbi);

    /* Add Menu */
    hwnd2 = CommandBands_GetCommandBar(g_hwndCB, 0);
    CommandBar_InsertMenubar(hwnd2, g_hInst, IDR_MAINMENU, 0);

    /* Add Buttons */
    hwnd2 = CommandBands_GetCommandBar(g_hwndCB, 1);
    CreateToolButtons(hwnd2);

#ifndef MG_FOR_PPC
    /* add [X] Button */
    CommandBands_AddAdornments(g_hwndCB, g_hInst, 0, 0);
#endif

    if (g_reginfo.valid) { /* if valid info stored in registry */
      if (g_reginfo.cbinfo[0].fMaximized) {
	SendMessage(g_hwndCB, RB_MAXIMIZEBAND, (WPARAM)0, (LPARAM)0);
      }
      if (g_reginfo.cbinfo[1].fMaximized) {
	SendMessage(g_hwndCB, RB_MAXIMIZEBAND, (WPARAM)1, (LPARAM)0);
      }
    }
  }
#endif /* USE_COMMANDBANDS */
#else /* USE_SHMENU */
  {
    SHMENUBARINFO mbi;
    RECT rc;

    memset(&mbi, 0, sizeof(SHMENUBARINFO));
    mbi.cbSize = sizeof(SHMENUBARINFO);
    mbi.hwndParent = hWnd;
    mbi.dwFlags = 0;
    mbi.nToolBarId = IDR_PPCMENU;
    mbi.hInstRes = g_hInst;
    mbi.nBmpId = IDB_BUTTONS;
    mbi.cBmpImages = 3;

    if (SHCreateMenuBar(&mbi)) {
      g_hwndCB = mbi.hwndMB;
    }
    else {
      g_hwndCB = (HWND)NULL;
    }

    GetWindowRect(hWnd, &rc);
    rc.bottom -= POCKETPC_MENU_HEIGHT;
    if (g_hwndCB) {
      MoveWindow(hWnd, rc.left, rc.top,
		 rc.right - rc.left, rc.bottom - rc.top, FALSE);
    }
  }
#endif /* USE_SHMENU */
#endif /* COMMANDBANDS */
#endif /* _WIN32_WCE */
}

#ifdef SPI_GETSIPINFO
static void
AdjustAgainstSIP(HWND hWnd, UINT param)
{
  SIPINFO sipinf;
  BOOL sipres;

  sipinf.cbSize = sizeof(sipinf);
  sipinf.dwImDataSize = 0;
  sipinf.pvImData = NULL;

#if _WIN32_WCE >= 210
  sipres = SipGetInfo(&sipinf);
#else
  sipres = SHSipInfo(SPI_GETSIPINFO, param, &sipinf, 0);
#endif

  if (sipres) {
    RECT winrect;

    GetWindowRect(hWnd, &winrect);
    if (!EqualRect(&winrect, &sipinf.rcVisibleDesktop)) {
      int shmenuheight = 0;

#ifdef USE_SHMENU
      if (!(sipinf.fdwFlags & SIPF_ON)) {
	shmenuheight = POCKETPC_MENU_HEIGHT;
      }
#endif /* USE_SHMENU */

      MoveWindow(hWnd,
		 sipinf.rcVisibleDesktop.left,
		 sipinf.rcVisibleDesktop.top,
		 sipinf.rcVisibleDesktop.right - 
		 sipinf.rcVisibleDesktop.left,
		 sipinf.rcVisibleDesktop.bottom -
		 sipinf.rcVisibleDesktop.top - shmenuheight, FALSE);
    }
  }
}
#endif /* SPI_GETSIPINFO */

static void
AdjustPane(HWND hwnd)
{
  RECT rect;
#ifdef _WIN32_WCE
#ifdef COMMANDBANDS
  int nCommandHeight;
#endif /* COMMANDBANDS */
#endif /* _WIN32_WCE */

  GetClientRect(g_hwndMain, &rect);

#ifdef _WIN32_WCE
#ifdef COMMANDBANDS
#ifndef USE_SHMENU
#ifndef USE_COMMANDBANDS
  if (CommandBar_IsVisible(g_hwndCB)) {
    nCommandHeight = CommandBar_Height(g_hwndCB);
  }
#else /* if USE_COMMANDBANDS */
  if (CommandBands_IsVisible(g_hwndCB)) {
    nCommandHeight = CommandBands_Height(g_hwndCB);
  }
  else {
    nCommandHeight = 0;
  }
#endif /* USE_COMMANDBANDS */
#else /* if USE_SHMENU */
  nCommandHeight = 0;
#endif /* USE_SHMENU */
  rect.top += nCommandHeight;
#endif /* COMMANDBANDS */
#endif /* _WIN32_WCE */

  MoveWindow(g_hwndTty, rect.left, rect.top,
	     rect.right - rect.left,
	     rect.bottom - rect.top, TRUE);
}

int ConfigStartupFilePath(int, int);

/*
 * WinProc for main window
 */
LRESULT CALLBACK
MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#ifdef SPI_GETSIPINFO
	static BOOL SIPChanged = FALSE;
#endif
	DWORD	threadID ;

	switch ( message ) {
	case WM_CREATE:
		MainWMCreate( hWnd ) ;
		break ;
	case WM_CLOSE:
		SendMessage(g_hwndTty, TTYM_COMMAND,
			    (WPARAM)IDC_CLOSE, (LPARAM)NULL);
		break ;
	case WM_DESTROY:
#ifdef CTRLMAP
		if (g_ctrlmap) {
		  /* send ctrl key `up' event in order to prevent
		     ctrl key to be kept in a state of `depressed' */
		  keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
		}
#endif
#if defined(COMMANDBANDS) && !defined(USE_SHMENU)
#ifdef _WIN32_WCE
#ifdef USE_COMMANDBANDS
		g_reginfo.cbinfo[0].cbSize = sizeof(COMMANDBANDSRESTOREINFO);
		CommandBands_GetRestoreInformation(g_hwndCB,
	     SendMessage(g_hwndCB, RB_IDTOINDEX, (WPARAM)SF_RBMENU, (LPARAM)0),
			g_reginfo.cbinfo);
		CommandBands_GetRestoreInformation(g_hwndCB,
	     SendMessage(g_hwndCB, RB_IDTOINDEX, (WPARAM)SF_RBBUTTON,
			(LPARAM)0), g_reginfo.cbinfo + 1);
		/* Is "Destroy" necessary?
		   No.  Because there are no CommandBands_Destroy()!? */
#else /* if !USE_COMMANDBANDS */
		   CommandBar_Destroy(g_hwndCB);
#endif /* !USE_COMMANDBANDS */
#endif /* _WIN32_WCE */
		/* Free Tool Tip strings */
		FreeToolTips();
#endif /* COMMANDBANDS && !USE_SHMENU */

#ifndef _WIN32_WCE
		if (!g_reginfo.maximized)
			GetWindowRect(g_hwndMain, &g_reginfo.rect);
#endif /* _WIN32_WCE */
#ifdef USE_REGINFO
		/* Store settings to registry db */
		g_reginfo.valid = TRUE;
		RegSetBinary(HKEY_CURRENT_USER, NGREGKEY, NGPREVPOSVAL,
			     (LPBYTE)&g_reginfo, sizeof(g_reginfo));
#endif

#ifdef _WIN32_WCE
#if defined(COMMANDBANDS) && defined(USE_COMMANDBANDS) \
		&& !defined(USE_SHMENU)
		if (g_himlrebar) { /* Destroy Image List */
			ImageList_Destroy(g_himlrebar);
			g_himlrebar = NULL;
		}
#endif
#else /* not _WIN32_WCE */
		/* Destroy Menu */
		if (GetMenu(hWnd)) {
			DestroyMenu(GetMenu(hWnd));
		}
#endif
		g_bExit = TRUE ;
		Sleep( 1000 ) ;
		PostQuitMessage( 0 ) ;
		break ;
	case WM_SETFOCUS:
#ifdef SPI_GETSIPINFO		
		AdjustAgainstSIP(hWnd, 0);
		SIPChanged = FALSE;
#endif
		SetFocus( g_hwndTty ) ;
		break ;

#if !defined(_WIN32_WCE) || 200 <= _WIN32_WCE
	case WM_SIZE:
		switch (wParam) {
		case SIZE_MAXIMIZED:
		case SIZE_RESTORED:
		  AdjustPane(g_hwndMain);
#ifndef _WIN32_WCE
		  /* the following processing is intended to save the
                     previous window position */
		  g_reginfo.maximized = (wParam == SIZE_MAXIMIZED);
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
			DestroyWindow(hWnd);
			break ;

		case IDC_OPTION:
			ConfigStartupFilePath(0, 0);
			break;

	        default:
			SendMessage(g_hwndTty, TTYM_COMMAND, wParam, lParam);
			break;
  		}
		break ;

#ifdef SPI_GETSIPINFO
	case WM_SETTINGCHANGE:
		switch (wParam) {
		case SPI_SETSIPINFO:
		case SPI_SETCURRENTIM:
			if (GetActiveWindow())
				AdjustAgainstSIP(hWnd, (UINT)lParam);
			else
				SIPChanged = TRUE;
	 		break;
#endif

	case WM_NOTIFY:
		switch(((LPNMHDR)lParam)->code) {
#ifdef RBN_HEIGHTCHANGE
		/* The height of Rebar has changed */
		case RBN_HEIGHTCHANGE:
			AdjustPane(hWnd);
			break;
#endif
		}
		break;
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

#ifdef FEPCTRL
#define fep_init() /* nothing to do */
#define fep_term() /* nothing to do */

static int fepctrl = FALSE;		/* FEP control enable flag	*/
static int fepmode = TRUE;		/* now FEP mode			*/

static BOOL g_ime_prevopened;

fepmode_on()
{
#ifndef USE_KCTRL
	if (fepctrl && !fepmode) {
		HIMC hIMC = ImmGetContext(g_hwndTty);
		if (hIMC) {
			ImmSetOpenStatus(hIMC, g_ime_prevopened);
			ImmReleaseContext(g_hwndTty, hIMC);
		}
		fepmode = TRUE;
	}
#endif
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
#ifndef USE_KCTRL
	if (fepctrl) {
		if (fepmode) {
			HIMC hIMC = ImmGetContext(g_hwndTty);
			if (hIMC) {
				g_ime_prevopened = ImmGetOpenStatus(hIMC);
				if (g_ime_prevopened)
					ImmSetOpenStatus(hIMC, FALSE);
				ImmReleaseContext(g_hwndTty, hIMC);
			}
			fepmode = FALSE;
		}
		else {
		    g_ime_prevopened = FALSE;
		}
	}
#endif
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
#ifdef USE_KCTRL
	Fep_Execute( g_hwndMain ) ;
	return TRUE ;
#else /* if !USE_KCTRL */
	HIMC hIMC = ImmGetContext(g_hwndTty);
	if (hIMC) {
		BOOL ime_open = ImmGetOpenStatus(hIMC);
		g_ime_prevopened = !ime_open;
		ImmSetOpenStatus(hIMC, !ime_open);
		ImmReleaseContext(g_hwndTty, hIMC);
	}
	return TRUE;
#endif
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
#ifdef SS_SUPPORT  /* 92.11.21  by S.Sasaki */
putline( int row, int column, unsigned char *s, unsigned char *t, short color )
#else
putline(int row, int column, unsigned char *s, short color)
#endif
{
#ifdef KANJI
        int c1 = 0, c2;
#ifdef SS_SUPPORT
	unsigned char *ccp1;
#endif
#endif
	int c;
	unsigned char	sjis[ 256 ], *dst, *cp1, *cp2;

	dst = sjis ;
	cp1 = &s[0] ;
	cp2 = &s[ncol] ;
#ifdef SS_SUPPORT
	ccp1 = &t[0] ;
#endif
	while ( cp1 != cp2 ) {
		c = *cp1 ++ ;
#ifdef KANJI
#ifdef SS_SUPPORT
		c2 = *ccp1 ++ ;
#endif
		if ( c1 ) {
			etos( c1, c ) ;
			*dst++ = c1 ;
			*dst++ = c ;
			c1 = 0 ;
#ifdef HANKANA
		} else if (ISHANKANA(c) && c2 != 0 ) {
			*dst++ = c2 ;
#endif
#ifdef HOJO_KANJI
		} else if (ISHOJO(c) && c2 != 0 ) {
			*dst++ = c2 ;
#endif
		} else if ( ISKANJI( c ) ) {
			c1 = c ;
		} else
#endif
			*dst++ = c ;
	}
	*dst = 0 ;
	PutLine(row-1, sjis, (short)(color == CMODE ? 1 : 0));
}

#if defined(COMMANDBANDS) && !defined(USE_SHMENU)
static BOOL CALLBACK
ViewProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
  switch (message) {
  case WM_INITDIALOG:
    Button_SetCheck(GetDlgItem(hDlg, IDC_SHOWMENUBAR), g_reginfo.showbands);
    return 1;

  case WM_COMMAND:
#if 0
    switch (LOWORD(wParam)) {
    case IDC_SHOWMENUBAR:
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
      {
	BOOL newvalue;
	newvalue = Button_GetCheck(GetDlgItem(hDlg, IDC_SHOWMENUBAR));
	if (g_reginfo.showbands != newvalue) {
	  g_reginfo.showbands = newvalue;
#ifdef _WIN32_WCE
#ifdef USE_COMMANDBANDS
	  CommandBands_Show(g_hwndCB, newvalue);
#else
	  CommandBar_Show(g_hwndCB, newvalue);
#endif
	  AdjustPane(g_hwndMain);
#endif
	}
      }
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
#endif /* defined(COMMANDBANDS) && !defined(USE_SHMENU) */

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

#if defined(CTRLMAP) || defined(JAPANESE_KEYBOARD)
static BOOL CALLBACK
KeyMapProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
  DWORD val;

  switch (message) {
  case WM_INITDIALOG:
#ifdef CTRLMAP
    val = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, 
			NGCTRLKEYMAPVAL) ? TRUE : FALSE;
    Button_SetCheck(GetDlgItem(hDlg, IDC_CONTROLMAP), val);
#else /* if !CTRLMAP */
    EnableWindow(GetDlgItem(hDlg, IDC_CONTROLMAP), FALSE);
#endif /* !CTRLMAP */

#ifdef JAPANESE_KEYBOARD
    val = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, NGKEYBOARDLOCALEVAL);
    switch (val) {
    case NGKEYBOARD_US:
      val = BST_UNCHECKED;
      break;
    case NGKEYBOARD_JP:
      val = BST_CHECKED;
      break;
    default:
    case 0:
      val = BST_CHECKED;
      break;
    }
    Button_SetCheck(GetDlgItem(hDlg, IDC_KEYBOARDLOCALE), val);
#else /* if !JAPANESE_KEYBOARD */
    EnableWindow(GetDlgItem(hDlg, IDC_KEYBOARDLOCALE), FALSE);
#endif /* !JAPANESE_KEYBOARD */
    return 1;

  case WM_COMMAND:
    switch (LOWORD(wParam)) {
#if 0 /* Did not work as I supposed. */
    case IDC_ALT:
      alt = !alt;
      Button_SetState(GetDlgItem(hDlg, IDC_ALT), alt);
      if (alt) {
	keybd_event(VK_MENU, 0, 0, 0);
      }
      else {
	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
      }
      break;
#endif
#if defined(CTRLMAP) && !defined(_WIN32_WCE)
    case IDC_CONTROLMAP:
      SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(HWND)hDlg, 0L);
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
#ifdef CTRLMAP
      val = Button_GetCheck(GetDlgItem(hDlg, IDC_CONTROLMAP));
      RegSetDWord(HKEY_CURRENT_USER, NGREGKEY, NGCTRLKEYMAPVAL, val);
      g_ctrlmap = val;
#endif
#ifdef JAPANESE_KEYBOARD
      val = Button_GetCheck(GetDlgItem(hDlg, IDC_KEYBOARDLOCALE)) ?
	NGKEYBOARD_JP : NGKEYBOARD_US;
      RegSetDWord(HKEY_CURRENT_USER, NGREGKEY, NGKEYBOARDLOCALEVAL, val);
      g_keyboardlocale = val;
      SendMessage(g_hwndTty, TTYM_SETKEYBOARDLOCALE, (WPARAM)val, 0);
#endif
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

#ifndef USE_KCTRL /* font configuration is not made functional for KCTRL */
#define CONFIG_FONT
#endif

#ifdef CONFIG_FONT

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

  /* To force the screen to be refreshed. */
  nrow = 4; /* This number (4) of rows may not be used... */
  
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

#endif /* CONFIG_FONT */

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

#if defined(_WIN32_WCE) && defined(COMMANDBANDS) && !defined(USE_SHMENU)
  AddPage(&psh, IDD_VIEW, ViewProc);
#endif
#ifndef NO_STARTUP  
  AddPage(&psh, IDD_STARTUPFILE, StartFileProc);
#endif
#if defined(CTRLMAP) || defined(JAPANESE_KEYBOARD)
  AddPage(&psh, IDD_KEY, KeyMapProc);
#endif
  AddPage(&psh, IDD_BEEP, BeepProc);
#ifdef CONFIG_FONT
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

