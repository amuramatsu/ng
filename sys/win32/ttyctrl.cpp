/* $Id: ttyctrl.cpp,v 1.5 2000/09/21 17:19:30 amura Exp $ */

/*
 * $Log: ttyctrl.cpp,v $
 * Revision 1.5  2000/09/21 17:19:30  amura
 * TtyView::PutLine is replaced by old code for speed reason
 *
 * Revision 1.4  2000/07/24 15:34:55  amura
 * rewrite PutLine()
 *
 * Revision 1.3  2000/07/22 20:46:33  amura
 * support "Drag&Drop"
 *
 * Revision 1.2  2000/07/18 12:42:33  amura
 * support IME convertion on the spot
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#include "config.h"
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include "def.h"
#include	"ttyctrl.h"
#include	"tools.h"

#define		is_kanji(x)		(((x)>=0x81 && (x)<=0x9F)||((x)>=0xE0 && (x)<=0xFC))
#define		MAX_KEYBUF		256
#define MAX_WINEVENTBUF 32
#ifdef	DROPFILES	/* 00.07.07  by sahf */
#define		MAX_DROPBUF		32
#endif	/* DROPFILES */

static LOGFONT lfont = {
#ifdef KANJI
#if 0
  14, 7, 0, 0, 0x190, 0x00, 0x00, 0x00, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
  CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FF_MODERN | FIXED_PITCH,
//  {0xFF2D, 0xFF33, 0x0020, 0x30B4, 0x30B7, 0x30C3, 0x30AF, 0x0000}
  TEXT("")
#else
  12, 6, 0, 0, 0x0, 0x00, 0x00, 0x00, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
  CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FF_MODERN | FIXED_PITCH,
//  {0xFF2D, 0xFF33, 0x0020, 0x30B4, 0x30B7, 0x30C3, 0x30AF, 0x0000}
  TEXT("")
#endif
#else
  12, 6, 0, 0, 0x0, 0x00, 0x00, 0x00, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
  CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FF_MODERN | FIXED_PITCH,
#ifdef _WIN32_WCE
  TEXT("Courier New")
#else
  TEXT("")
#endif
#endif
};

#define GetFontH() (lfont.lfHeight + 1)
#define GetFontHW() (lfont.lfWidth)
#define KDrawText(hdc, str, len, rect, f) do { HFONT hOldFont;				\
  hOldFont = (HFONT)SelectObject(hdc, m_hFont);								\
  DrawText(hdc, str, len, rect, (DT_NOPREFIX | DT_LEFT | DT_EXPANDTABS));	\
  SelectObject(hdc, hOldFont); } while (/*CONSTCOND*/0)

class TtyView {
protected:
	HWND	m_hwnd ;			/* コントロールのウィンドウハンドル */
	HWND	m_hwndParent ;		/* コントロールの親ウィンドウ */
	WORD	m_idCtrl ;			/* コントロールの識別番号 */
	DWORD	m_dwCols ;			/* 表示できる半角文字数 */
	DWORD	m_dwLines ;			/* 表示できる行数 */
	DWORD	m_dwHMargin ;		/* 縦方向のマージン */
	DWORD	m_dwFontW ;			/* 1文字の幅 */
	DWORD	m_dwFontH ;			/* 1行の高さ */
	DWORD	m_dwTabSize ;		/* TABのサイズ */

	HBITMAP	m_bmpScreen ;		/* 画面イメージ */
	RECT	m_rcInvalidate ;	/* PutCharで描画されたRECT */
	DWORD	m_dwCurCol ;		/* カーソル桁位置 */
	DWORD	m_dwCurLine ;		/* カーソル行位置 */
	DWORD	m_dwCurH ;			/* カーソルの高さ */
	BOOL	m_bShowCursor ;		/* カーソル表示 */

	HANDLE	m_hevtKey ;			/* キー入力通知用イベント */
	WORD	m_szKeyBuf[ MAX_KEYBUF ] ;	/* キー入力バッファ */
	DWORD	m_dwKeyIn ;			/* キー受け付け位置 */
	DWORD	m_dwKeyOut ;		/* キー取り出し位置 */
	int m_szWinEventBuf[MAX_WINEVENTBUF]; /* Window Event buffer */
	DWORD m_dwWinEventOut; /* Window Event output pointer */
	DWORD m_dwWinEventIn;  /* Window Event input pointer */
#ifdef	DROPFILES	/* 00.07.07  by sahf */
	HLOCAL	m_szDropBuf[ MAX_DROPBUF ] ;	/* Drag And Drop 情報バッファ */
	int m_iDropLine[ MAX_DROPBUF ] ;	/* Drag and Drop point saver */
	DWORD	m_dwDropIn ;
	DWORD	m_dwDropOut ;
#endif	/* DROPFILES */

	HFONT	m_hFont; /* handle for font */
#ifdef	DROPFILES	/* 00.07.07  by sahf */
	int		GetDropFiles( HLOCAL *lphMemory ) ;
#endif	/* DROPFILES */

	void	SetupEvent( HANDLE hEvent ) ;
	void	ClearScreen() ;
	void	PutChar( BYTE c ) ;
	void	PutKChar( BYTE c1, BYTE c2 ) ;
	void	GotoXY( WORD x, WORD y ) ;
	void	EraseEOL() ;
	void	EraseEOP() ;
	void	PutLine( WORD y, WORD color, LPCSTR sjis ) ;
	void	Flush() ;
	BOOL	Kbhit() const ;
	int 	GetChar() ;
	DWORD	GetWH() const ;
	void	AddMetaChar( TCHAR c ) ;
	void	AddChar( TCHAR c ) ;

	void	ShowCursor(void) ;
	void	HideCursor(void) ;
	void	ResetContent() ;
	int  GetWindowEvent(void);
	void AddWindowEvent(int);
	void SetFontValues(LOGFONT *, HDC);
	void AdjustScreen(HWND, HDC);

public:
	TtyView() ;
	~TtyView() ;

	BOOL	Create() { return FALSE ; }
#ifdef	DROPFILES	/* 00.07.07  by sahf */
	void	WMDropFiles( HDROP hDrop ) ;
#endif	/* DROPFILES */

	void	WMCreate( HWND hWnd, LPCREATESTRUCT lpcs ) ;
	void	WMPaint() ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;
	BOOL	WMCopydata( PCOPYDATASTRUCT cds ) ;
	BOOL	WMSysChar( TCHAR c, LONG keydata ) ;
	void	WMChar( TCHAR c, LONG keydata ) ;
	BOOL	WMKeyDown( int nVirtKey, LONG lKeyData ) ;
	void	WMMouseDown( WPARAM wParam, LPARAM lParam ) ;
	void	WMSize( DWORD fwSize, WORD nWidth, WORD nHeight ) ;
	BOOL	WMCopy() ;
	void	WMLButtonDown(int x, int y);

	void	SetTab( DWORD wParam, BOOL bUpdate ) ;
	void	SetHMargin( DWORD wParam, BOOL bUpdate ) ;
#ifndef _WIN32_WCE
	void	FontChanged(void);
#endif

	friend	LRESULT CALLBACK	TtyViewWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam ) ;
} ;
typedef	TtyView	*PTtyView ;

TtyView::TtyView()
{
	m_dwCols = 0 ;
	m_dwLines = 0 ;
	m_dwHMargin = 2;
	m_dwFontW = GetFontHW() ;
	m_dwFontH = GetFontH() + m_dwHMargin ;
	m_dwTabSize = 8 ;

	m_bmpScreen = 0 ;
	SetRectEmpty( &m_rcInvalidate ) ;
	m_dwCurCol = 0 ;
	m_dwCurLine = 0 ;
	m_dwCurH = GetFontH() ;
	m_bShowCursor = FALSE;

	m_hevtKey = 0 ;
	m_dwKeyIn = 0 ;
	m_dwKeyOut = 0 ;
	m_dwWinEventIn = m_dwWinEventOut = 0;
#ifdef	DROPFILES	/* 00.07.07  by sahf */
	m_dwDropIn = m_dwDropOut = 0;
#endif	/* DROPFILES */

	m_hFont = (HFONT)0;
}

TtyView::~TtyView()
{
	if ( m_bmpScreen ) {
		DeleteObject( m_bmpScreen ) ;
		m_bmpScreen = 0 ;
	}
	if (m_hFont) {
	  DeleteObject(m_hFont);
	  m_hFont = (HFONT)0;
	}
}

/*
 * 画面バッファの内容をクリアする
 */
void
TtyView::ClearScreen()
{
	RECT	rect ;
	HGDIOBJ	hOldObj ;
	HDC		hDC, hdcBmp ;

	GetClientRect( m_hwnd, &rect ) ;
	hDC = GetDC( m_hwnd ) ;
	CreateCaret(m_hwnd, NULL, 2, m_dwFontH);
	HideCursor() ;
	hdcBmp = CreateCompatibleDC( hDC ) ;
	hOldObj = SelectObject( hdcBmp, m_bmpScreen ) ;
	FillRect( hdcBmp, &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
	FillRect( hDC,    &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
	SelectObject( hdcBmp, hOldObj ) ;
	DeleteDC( hdcBmp ) ;
	ShowCursor() ;
	ReleaseDC( m_hwnd, hDC ) ;
}

void
TtyView::SetupEvent( HANDLE hEvent )
{
	m_hevtKey = hEvent ;
}

void
TtyView::PutChar( BYTE c )
{
	RECT	rect ;
	HGDIOBJ	hOldObj ;
	HDC		hDC, hdcBmp ;
	TCHAR	unicode[ 2 ] ;

	if ( c == '\b' ) {
		if ( m_dwCurCol ) {
			GotoXY( (WORD) (m_dwCurCol - 1), (WORD) m_dwCurLine ) ;
		}
		return ;
	}
	rect.left = m_dwCurCol * m_dwFontW ;
	rect.top = m_dwCurLine * m_dwFontH ;
	rect.right = rect.left + m_dwFontW - 1 ;
	rect.bottom = rect.top + m_dwFontH - 1 ;
	unicode[0] = sjis2unicode_char( c ) ;
	unicode[1] = 0 ;

	hDC = GetDC( m_hwnd ) ;
	HideCursor() ;
	hdcBmp = CreateCompatibleDC( hDC ) ;
	hOldObj = SelectObject( hdcBmp, m_bmpScreen ) ;

	if ( unicode[0] == TEXT(' ') ) {
		rect.right ++ ;
		rect.bottom ++ ;
		FillRect( hdcBmp, &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
//		FillRect( hDC,    &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
		rect.right -- ;
		rect.bottom -- ;
	} else {
		KDrawText( hdcBmp, unicode, -1, &rect, 0 ) ;
//		BitBlt( hDC, rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1,
//				hdcBmp, rect.left, rect.top, SRCCOPY ) ;
	}
	UnionRect( &m_rcInvalidate, &m_rcInvalidate, &rect ) ;

	SelectObject( hdcBmp, hOldObj ) ;
	DeleteDC( hdcBmp ) ;

	m_dwCurCol ++ ;
	if ( m_dwCurCol >= m_dwCols ) {
		m_dwCurCol = 0 ;
		m_dwCurLine ++ ;
		if ( m_dwCurLine >= m_dwLines ) {
			m_dwCurLine = 0 ;
		}
	}

	ShowCursor() ;
	ReleaseDC( m_hwnd, hDC ) ;
}

void
TtyView::PutKChar(BYTE c1, BYTE c2)
{
  RECT rect;
  HGDIOBJ hOldObj;
  HDC hDC, hdcBmp;
  TCHAR	unicode[2];
  BYTE foo[3];
  
  rect.left = m_dwCurCol * m_dwFontW;
  rect.top = m_dwCurLine * m_dwFontH;
  rect.right = rect.left + (m_dwFontW * 2) - 1;
  rect.bottom = rect.top + m_dwFontH - 1;

  foo[0] = c1;
  foo[1] = c2;
  foo[2] = (BYTE)0;
  sjis2unicode(foo, unicode, sizeof(unicode));

  hDC = GetDC(m_hwnd);
  HideCursor();
  hdcBmp = CreateCompatibleDC(hDC);
  hOldObj = SelectObject(hdcBmp, m_bmpScreen);

  KDrawText(hdcBmp, unicode, -1, &rect, 0);

  UnionRect(&m_rcInvalidate, &m_rcInvalidate, &rect);

  SelectObject(hdcBmp, hOldObj);
  DeleteDC(hdcBmp);

  m_dwCurCol += 2;
  if (m_dwCurCol >= m_dwCols) {
    m_dwCurCol = 0;
    m_dwCurLine++;
    if (m_dwCurLine >= m_dwLines) {
      m_dwCurLine = 0;
    }
  }

  ShowCursor();
  ReleaseDC(m_hwnd, hDC);
}

void
TtyView::GotoXY( WORD x, WORD y )
{
	DWORD dwCurX, dwCurY;
	
	m_dwCurCol = x;
	m_dwCurLine = y;
	dwCurX = m_dwFontW * m_dwCurCol ;
	dwCurY = m_dwFontH * m_dwCurLine ;
	SetCaretPos(dwCurX, dwCurY);
#ifdef	FEPCTRL
	{
		HIMC hIMC = ImmGetContext(m_hwnd);
		if (hIMC) {
			COMPOSITIONFORM cf;
			cf.dwStyle = CFS_POINT;
			cf.ptCurrentPos.x = dwCurX;
			cf.ptCurrentPos.y = dwCurY;
			ImmSetCompositionWindow(hIMC, &cf);
			ImmReleaseContext(m_hwnd, hIMC);
		}
	}
#endif
	ShowCursor();
}

BOOL
TtyView::Kbhit() const
{
	return m_dwKeyIn != m_dwKeyOut ;
}

void
TtyView::EraseEOL()
{
	RECT	rect ;
	HGDIOBJ	hOldObj ;
	HDC		hDC, hdcBmp ;

	hDC = GetDC( m_hwnd ) ;
	HideCursor() ;
	hdcBmp = CreateCompatibleDC( hDC ) ;
	hOldObj = SelectObject( hdcBmp, m_bmpScreen ) ;

	rect.left   = m_dwCurCol * m_dwFontW ;
	rect.top    = m_dwCurLine * m_dwFontH ;
	rect.right  = m_dwCols * m_dwFontW ;
	rect.bottom = rect.top + m_dwFontH ;
	FillRect( hdcBmp, &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
//	FillRect( hDC,    &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
	UnionRect( &m_rcInvalidate, &m_rcInvalidate, &rect ) ;

	SelectObject( hdcBmp, hOldObj ) ;
	DeleteDC( hdcBmp ) ;
	ShowCursor() ;
	ReleaseDC( m_hwnd, hDC ) ;
}

void
TtyView::EraseEOP()
{
	RECT	rect ;
	HGDIOBJ	hOldObj ;
	HDC		hDC, hdcBmp ;

	hDC = GetDC( m_hwnd ) ;
	HideCursor() ;
	hdcBmp = CreateCompatibleDC( hDC ) ;
	hOldObj = SelectObject( hdcBmp, m_bmpScreen ) ;

	rect.left   = m_dwCurCol * m_dwFontW ;
	rect.top    = m_dwCurLine * m_dwFontH ;
	rect.right  = m_dwCols * m_dwFontW ;
	rect.bottom = rect.top + m_dwFontH ;
	if ( !IsRectEmpty( &rect ) ) {
		FillRect( hdcBmp, &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
		FillRect( hDC,    &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
	}
	rect.left = 0 ;
	rect.top = (m_dwCurLine + 1) * m_dwFontH ;
	rect.right = m_dwCols * m_dwFontW ;
	rect.bottom = m_dwLines * m_dwFontH ;
	if ( !IsRectEmpty( &rect ) ) {
		FillRect( hdcBmp, &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
		FillRect( hDC,    &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
	}

	SelectObject( hdcBmp, hOldObj ) ;
	DeleteDC( hdcBmp ) ;
	ShowCursor() ;
	ReleaseDC( m_hwnd, hDC ) ;
}

void
TtyView::PutLine( WORD y, WORD color, LPCSTR sjis )
{
#if 0
	WORD	x ;
	RECT	rect ;
	HGDIOBJ	hOldObj ;
	HDC	hDC, hdcBmp ;
	TCHAR	unicode[ 2 ] ;
	BYTE	foo[3] ;
	HFONT hOldFont ;		/* to speed hack */

	hDC = GetDC( m_hwnd ) ;
	HideCursor() ;
	hdcBmp = CreateCompatibleDC( hDC ) ;
	hOldObj = SelectObject( hdcBmp, m_bmpScreen ) ;
	/* This is speed hack, default this is on KDrawText() */
	hOldFont = (HFONT)SelectObject( hdcBmp, m_hFont );

	rect.left = 0;
	rect.top  = y * m_dwFontH ;
	rect.right = rect.left + m_dwFontW;
	rect.bottom = rect.top + m_dwFontH;

	for (x=0; x<m_dwCols; x++)
	{
		if (*sjis == 0) {
			unicode[0] = TEXT(' ');
			unicode[1] = 0;
		} else if (is_kanji((BYTE)*sjis)) {
			foo[0] = *sjis++;
			foo[1] = *sjis++;
			foo[2] = (BYTE)0;
			sjis2unicode(foo, unicode, sizeof unicode );
			rect.right += m_dwFontW;
			x++;
		} else {
			unicode[0] = sjis2unicode_char( *sjis++ ) ;
			unicode[1] = 0 ;
		}


		DrawText(hdcBmp, unicode, -1, &rect,
			 (DT_NOPREFIX | DT_LEFT | DT_EXPANDTABS));
		// KDrawText( hdcBmp, unicode, -1, &rect, 0 ) ;
		/* goto next position */
		rect.left  = rect.right;
		rect.right = rect.left + m_dwFontW;
	}
	if ( color ) {
	  PatBlt(hdcBmp, 0, y * m_dwFontH, m_dwCols * m_dwFontW - 1, m_dwCurH,
		 PATINVERT);
	}
	BitBlt( hDC, 0, y * m_dwFontH, m_dwCols * m_dwFontW, m_dwCurH,
			hdcBmp, 0, y * m_dwFontH, SRCCOPY ) ;
	SelectObject( hdcBmp, hOldFont ) ;	/* speed hack */
	SelectObject( hdcBmp, hOldObj ) ;
	DeleteDC( hdcBmp ) ;

	ShowCursor() ;
	ReleaseDC( m_hwnd, hDC ) ;
#else
	RECT	rect ;
	HGDIOBJ	hOldObj ;
	HDC		hDC, hdcBmp ;
	TCHAR	unicode[ 256 ] ;

	hDC = GetDC( m_hwnd ) ;
	HideCursor() ;
	hdcBmp = CreateCompatibleDC( hDC ) ;
	hOldObj = SelectObject( hdcBmp, m_bmpScreen ) ;

	rect.left = 0 ;
	rect.top = y * m_dwFontH ;
	rect.right = m_dwCols * m_dwFontW - 1 ;
	rect.bottom = rect.top + m_dwFontH - 1 ;
	sjis2unicode( (LPBYTE) sjis, unicode, sizeof unicode ) ;
	KDrawText( hdcBmp, unicode, -1, &rect, 0 ) ;
	if ( color ) {
	  PatBlt(hdcBmp,
		 rect.left, rect.top, rect.right - rect.left, m_dwCurH,
		 PATINVERT);
	}
	BitBlt( hDC, rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1,
			hdcBmp, rect.left, rect.top, SRCCOPY ) ;
	SelectObject( hdcBmp, hOldObj ) ;
	DeleteDC( hdcBmp ) ;
	ShowCursor() ;
	ReleaseDC( m_hwnd, hDC ) ;
#endif
}

void
TtyView::Flush()
{
	if ( !IsRectEmpty( &m_rcInvalidate ) ) {
		InvalidateRect( m_hwnd, &m_rcInvalidate, TRUE ) ;
//		UpdateWindow( m_hwnd ) ;
		SetRectEmpty( &m_rcInvalidate ) ;
	}
}

/*
 * 1文字取得する
 */
int
TtyView::GetChar()
{
  WORD c;

  if (m_dwKeyIn == m_dwKeyOut) {
    return -1;
  }
  c = m_szKeyBuf[m_dwKeyOut];
  m_dwKeyOut = (m_dwKeyOut + 1) % MAX_KEYBUF;
  return (int) c;
}

DWORD
TtyView::GetWH() const
{
	return MAKELONG( m_dwLines, m_dwCols ) ;
}

void
TtyView::AddMetaChar( TCHAR c )
{
	DWORD	next1 ;

	next1 = (m_dwKeyIn + 1) % MAX_KEYBUF ;
	if ( next1 == m_dwKeyOut ) {
		return ;
	}
	m_szKeyBuf[ m_dwKeyIn ] = 0x100 | (c & 0xFF) ;
	m_dwKeyIn = next1 ;
	if ( m_hevtKey ) {
		::SetEvent( m_hevtKey ) ;
	}
}

void
TtyView::AddChar( TCHAR c )
{
	WORD		sjisChar ;
	DWORD		next1, next2 ;

	sjisChar = c ? unicode2sjis_char( c ) : 0 ;
	next1 = (m_dwKeyIn + 1) % MAX_KEYBUF ;
	if ( next1 == m_dwKeyOut ) {
		return ;
	}
	if ( sjisChar / 0x100 ) {
		next2 = (m_dwKeyIn + 2) % MAX_KEYBUF ;
		if ( next2 == m_dwKeyOut ) {
			return ;
		}
		m_szKeyBuf[ m_dwKeyIn ] = sjisChar / 0x100 ;
		m_dwKeyIn = next1 ;
		next1 = next2 ;
	}
	m_szKeyBuf[ m_dwKeyIn ] = sjisChar & 0xFF;
	m_dwKeyIn = next1 ;
	if ( m_hevtKey ) {
		::SetEvent( m_hevtKey ) ;
	}
}

#ifdef	DROPFILES	/* 00.07.07  by sahf */
/*
 * Get Drag & Drop files (from main thread)
 */

int
TtyView::GetDropFiles( HLOCAL *lphMemory )
{
    int line;

    if ( lphMemory == NULL )
	return -1;
    
    if (m_dwDropIn == m_dwDropOut) {
	return -1;
    }
    *lphMemory = m_szDropBuf[m_dwDropOut];
    line = m_iDropLine[m_dwDropOut];
    m_dwDropOut = (m_dwDropOut + 1) % MAX_DROPBUF;
    
    return line;
}
#endif	/* DROPFILES */

/*
 * Put a window event onto a queue.
 */

int
TtyView::GetWindowEvent()
{
  int c;

  if (m_dwWinEventIn == m_dwWinEventOut) {
    return -1 ;
  }
  c = m_szWinEventBuf[m_dwWinEventOut];
  m_dwWinEventOut = (m_dwWinEventOut + 1) % MAX_WINEVENTBUF;
  return c;
}

/*
 * Get a window event from a queue.
 */

void
TtyView::AddWindowEvent(int ev)
{
  DWORD next1;

  next1 = (m_dwWinEventIn + 1) % MAX_WINEVENTBUF;
  if (next1 == m_dwWinEventOut) {
    return;
  }
  m_szWinEventBuf[m_dwWinEventIn] = ev;
  m_dwWinEventIn = next1;
  if (m_hevtKey) {
    ::SetEvent(m_hevtKey);
  }
}


/*---------------------------------------------------------------------*
 * カーソル表示関係
 *---------------------------------------------------------------------*/
/*
 * カーソルを表示する
 */
void
TtyView::ShowCursor(void)
{
	ShowCaret(m_hwnd);
	m_bShowCursor = TRUE;
}

/*
 * カーソルを消去する
 */
void
TtyView::HideCursor(void)
{
	if ( !m_bShowCursor ) {
		return ;
	}
	HideCaret(m_hwnd);
	m_bShowCursor = FALSE;
}

void
TtyView::ResetContent()
{
	ClearScreen() ;
}

void
TtyView::SetFontValues(LOGFONT *lf, HDC hdc)
{
  DWORD val, point, linespace;
  FLOAT cyDpi, cxDpi;
  POINT pt;
  TCHAR fontname[LF_FACESIZE];

  val = sizeof(fontname);
  if (RegQueryString(HKEY_CURRENT_USER, NGREGKEY, NGFONTNAMEVAL,
		     fontname, &val) == ERROR_SUCCESS) {
    point = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, NGFONTSIZEVAL);
    if (point > 0) {
      linespace = RegQueryDWord(HKEY_CURRENT_USER, NGREGKEY, NGLINESPACEVAL);

      cxDpi = (FLOAT)GetDeviceCaps(hdc, LOGPIXELSX);
      cyDpi = (FLOAT)GetDeviceCaps(hdc, LOGPIXELSY);
      pt.x = (int)(point * cxDpi / 72);
      pt.y = (int)(point * cyDpi / 72);
#ifndef _WIN32_WCE
      DPtoLP(hdc, &pt, 1);
      /* What should I do for Windows CE for above? */
#endif
      if (pt.y < 0) {
	pt.y = -pt.y; /* obtain an absolute value */
      }
      lf->lfHeight = -(int)pt.y; /* specify character height, not cell one */
      lf->lfWidth = 0;
      lstrcpy(lf->lfFaceName, fontname);
      m_dwHMargin = linespace;
      return;
    }
  }
  /* reset to default values */
  lf->lfHeight = 12;
  lf->lfWidth = 6;

#ifdef KANJI
  lf->lfFaceName[0] = TEXT('\0');
#else
#ifdef _WIN32_WCE
  lstrcpy(lf->lfFaceName, TEXT("Courier New"));
#else
  lf->lfFaceName[0] = TEXT('\0');
#endif
#endif

  m_dwHMargin = 2;
}

void
TtyView::AdjustScreen(HWND hWnd, HDC hDC)
{
  TEXTMETRIC tm;
  RECT rect;

  /* create an handle of required font */
  SetFontValues(&lfont, hDC);
  m_hFont = CreateFontIndirect(&lfont);

  /* re-calculate the font metrics */
  SelectObject(hDC, m_hFont);
  GetTextMetrics(hDC, &tm);
  m_dwFontW = tm.tmAveCharWidth;
  m_dwFontH = tm.tmHeight + m_dwHMargin;
  m_dwCurH = tm.tmHeight;

#ifdef	FEPCTRL
  /* set font to imm */
  {
    HIMC hIMC = ImmGetContext(m_hwnd);
    if (hIMC) {
      ImmSetCompositionFont(hIMC, &lfont);
      ImmReleaseContext(m_hwnd, hIMC);
    }
  }
#endif
  
  /* Calculate the line numbers and column numbers */
  GetClientRect(m_hwnd, &rect);
  m_dwLines = rect.bottom / m_dwFontH;
  m_dwCols = rect.right / m_dwFontW;

  /* Create a display buffer */
  if (m_bmpScreen) {
    DeleteObject(m_bmpScreen);
  }
  m_bmpScreen = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
}

void
TtyView::WMCreate( HWND hWnd, LPCREATESTRUCT lpcs )
{
	HDC		hDC ;

	m_hwnd = hWnd ;
	m_hwndParent = lpcs->hwndParent ;
	m_idCtrl = (WORD) lpcs->hMenu ;

	hDC = GetDC(m_hwnd);
	AdjustScreen(hWnd, hDC);
	ReleaseDC(m_hwnd, hDC);

	ClearScreen() ;
}

void
TtyView::WMPaint()
{
	PAINTSTRUCT	ps ;
	RECT		rect ;
	HGDIOBJ		hOldObj ;
	HDC			hDC, hdcBmp ;

	GetClientRect( m_hwnd, &rect ) ;
	hDC = ::BeginPaint( m_hwnd, &ps ) ;
	//HideCursor() ;
	hdcBmp = CreateCompatibleDC( hDC ) ;
	hOldObj = SelectObject( hdcBmp, m_bmpScreen ) ;

	BitBlt( hDC, 0, 0, rect.right, rect.bottom,
			hdcBmp, 0, 0, SRCCOPY ) ;

	SelectObject( hdcBmp, hOldObj ) ;
	DeleteDC( hdcBmp ) ;
	ShowCursor() ;
	::EndPaint( m_hwnd, &ps ) ;
}

void
TtyView::WMChar( TCHAR chCharCode, LONG lKeyData )
{
	BOOL	fControl = 0x80 & GetKeyState( VK_CONTROL ) ;

	if ( chCharCode == TEXT(' ') && fControl ) {
		chCharCode = 0 ;
	}
	AddChar( chCharCode ) ;
}

BOOL
TtyView::WMSysChar( TCHAR chCharCode, LONG lKeyData )
{
        if ( !(lKeyData & 0x20000000) ) {
		return FALSE ;
	}
	AddMetaChar( chCharCode ) ;
	return TRUE ;
}

BOOL
TtyView::WMKeyDown( int nVirtKey, LONG lKeyData )
{
	BOOL	fShift   = 0x80 & GetKeyState( VK_SHIFT ) ;
	BOOL	fControl = 0x80 & GetKeyState( VK_CONTROL ) ;
	BOOL	fRepeat = (lKeyData & 0x000F) > 1 ? TRUE : FALSE ;

	switch ( nVirtKey ) {
	case VK_UP:
		AddChar( TEXT('P') - TEXT('@') ) ;
		break ;
	case VK_DOWN:
		AddChar( TEXT('N') - TEXT('@') ) ;
		break ;
	case VK_LEFT:
		AddChar( TEXT('B') - TEXT('@') ) ;
		break ;
	case VK_RIGHT:
		AddChar( TEXT('F') - TEXT('@') ) ;
		break ;
	case VK_PRIOR:
		AddMetaChar( TEXT('V') ) ;
		break ;
	case VK_NEXT:
		AddChar( TEXT('V') - TEXT('@') ) ;
		break ;
	case VK_HOME:
		if (fControl) {
		  AddMetaChar(TEXT('<'));
		}
		else {
		  AddChar(TEXT('A') - TEXT('@'));
		}
		break ;
	case VK_END:
		if (fControl) {
		  AddMetaChar(TEXT('>'));
		}
		else {
		  AddChar(TEXT('E') - TEXT('@'));
		}
		break ;
	case VK_DELETE:
		AddChar(0x7f);
		break;
#ifdef JAPANESE_KEYBOARD
  /* The following mapping is achieved by observing the behavior of
     Windows CE Japanese version.  It looks as if the virtual key code
     is the same as English one, but mapping from virtual key to
     character differs. */

#ifdef VK_LBRACKET
	case VK_LBRACKET:
		if (fControl) {
		  AddChar(TEXT('@') - TEXT('@'));
		}
		break;
#endif

#ifdef VK_BACKQUOTE
	case VK_BACKQUOTE:
		if (fControl) {
		  AddChar(TEXT('_') - TEXT('@'));
		}
		break;
#endif

#ifdef VK_EQUAL
	case VK_EQUAL:
		if (fControl) {
		  AddChar(TEXT('^') - TEXT('@'));
		}
		break;
#endif
#endif /* JAPANESE_KEYBOARD */
	
	default:
		return FALSE;
	}
	return TRUE;
}

LRESULT
TtyView::WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
	int		nVirtKey ;

	switch ( LOWORD(wp) ) {
	case IDM_FEPOK:
		/* FEP受け入れ応答 */
		return IDM_FEPOK ;
	case IDM_FEPKEY:
		/* FEPからのキー入力があった場合 */
		nVirtKey = lp & 0xFF ;
		WMKeyDown( nVirtKey, 0 ) ;
		break ;
	}
	return 0 ;
}

BOOL
TtyView::WMCopydata( PCOPYDATASTRUCT cds )
{
	TCHAR		c ;
	LPCTSTR		ptr = (LPCTSTR) cds->lpData ;

	while ( c = *ptr++ ) {
		AddChar( c ) ;
	}
	return TRUE ;
}

void
TtyView::WMSize( DWORD fwSize, WORD nWidth, WORD nHeight )
{
	HDC		hDC ;
	RECT	rect ;
	DWORD prevlines, prevcols;

	prevlines = m_dwLines;
	prevcols = m_dwCols;

	/* 1画面に表示できる行数を求める */
	GetClientRect( m_hwnd, &rect ) ;
	m_dwLines = rect.bottom / m_dwFontH ;
	m_dwCols = rect.right / m_dwFontW ;
	/* 画面バッファを作成する */
	if ( m_bmpScreen ) {
		DeleteObject( m_bmpScreen ) ;
	}
	/* 画面バッファを作成する */
	hDC = GetDC( m_hwnd ) ;
	m_bmpScreen = CreateCompatibleBitmap( hDC, rect.right, rect.bottom ) ;
	ReleaseDC( m_hwnd, hDC ) ;
	ResetContent() ;
	if (prevlines != m_dwLines || prevcols != m_dwCols) {
		AddWindowEvent(TTY_WM_RESIZE);
	}
}

void
TtyView::WMLButtonDown(int x, int y)
{
  int xx, yy;
  xx = x / m_dwFontW;
  yy = y / m_dwFontH;
  AddWindowEvent((yy << 18) | (xx << 4) | TTY_WM_MOUSE);
}

#ifdef	DROPFILES	/* 00.07.07  by sahf */
void
TtyView::WMDropFiles( HDROP hDrop )
{
    UINT	iFile ;
    HLOCAL	hMemory ;
	POINT	sDropPoint ;

    /* ファイル名を格納するためのメモリを確保 */
    hMemory = LocalAlloc(LPTR, 16) ;

    /* ドロップされたファイル数を得る */
    iFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0) ;
    if( iFile && hMemory) {
	UINT	iMemSize ;	/* 確保されたメモリサイズ */
	UINT	iMemPos ;

	DragQueryPoint(hDrop, &sDropPoint);
	iMemSize = LocalSize( hMemory ) ;
	iMemPos = 0 ;
	while( iFile-- ) {
	    UINT bufsz ;	/* 個々のファイル名の長さ（単位：バイト） */
	    UINT newsize ;	/* 追加格納するために必要な容量（要求量） */
	    char *fname ;

	    /* 個々のファイル名の長さ(必要なバッファサイズ)を得る  */
	    bufsz = DragQueryFile(hDrop, iFile, NULL, 0) ;
	    ++bufsz ;
	    bufsz *= sizeof(TCHAR) ;	/* 文字数 -> バイト数 */

	    /* hMemory の空き容量をチェック！ */
	    newsize = iMemPos + bufsz + sizeof(TCHAR) ;
	    if ( iMemSize < newsize ) {
		HLOCAL	hNewMem ;
		/* 足りなければ追加確保 */
		hNewMem = LocalAlloc(LPTR, newsize) ;
		if ( hNewMem == NULL ) {
		    LocalFree( hMemory );  hMemory = NULL;
		    break ;
		}
		CopyMemory(hNewMem, hMemory, iMemSize);
		LocalFree(hMemory);
		hMemory = hNewMem;
		iMemSize = LocalSize( hMemory ) ;
	    }

	    /* 読み取りのバッファ(fname)は hMemory から iMemPos バイト目 */
	    fname = (char *) hMemory ;
	    fname += iMemPos ;
	    iMemPos += bufsz ;
	    fname[bufsz] = 0;

	    /* ファイル名を読み取る */
	    DragQueryFile(hDrop, iFile, (LPTSTR) fname, bufsz) ;
	}
    }

    DragFinish(hDrop);

    if ( hMemory ) {
	DWORD		next1 ;
	
	next1 = (m_dwDropIn + 1) % MAX_DROPBUF ;
	if ( next1 == m_dwDropOut ) {
	    return ;
	}
	m_szDropBuf[ m_dwDropIn ] = hMemory ;
	m_iDropLine[ m_dwDropIn ] = sDropPoint.y / m_dwFontH ;
	m_dwDropIn = next1 ;
    }
    
    AddWindowEvent(TTY_WM_DROPFILES);
}
#endif	/* DROPFILES */

#if 0
/*
 * TABのサイズを設定する
 */
void
TtyView::SetTab( DWORD wParam, BOOL bUpdate )
{
	if ( m_dwTabSize == wParam ) {
		return ;
	}
	m_dwTabSize = wParam ;
	if ( bUpdate ) {
		ResetContent() ;
	}
}
#endif

/*
 * 縦方向のマージンを設定する
 */
void
TtyView::SetHMargin( DWORD wParam, BOOL bUpdate )
{
	RECT	rect ;

	if ( m_dwHMargin == wParam ) {
		return ;
	}
	GetClientRect( m_hwnd, &rect ) ;
	m_dwHMargin = wParam ;
	m_dwFontH = GetFontH() + m_dwHMargin ;
	m_dwLines = rect.bottom / m_dwFontH ;
	if ( bUpdate ) {
		ResetContent() ;
	}
}

#ifndef _WIN32_WCE
/*
 * Notified if font was changed.
 */
void
TtyView::FontChanged(void)
{
  HDC		hDC ;

  hDC = GetDC(m_hwnd);

  AdjustScreen(m_hwnd, hDC);
  ReleaseDC(m_hwnd, hDC);
  ClearScreen() ;

  AddWindowEvent(TTY_WM_RESIZE);
}
#endif

BOOL
TtyViewRegisterClass( HINSTANCE hInst )
{
	WNDCLASS	wc ;

	wc.style         = 0 ;
	wc.lpfnWndProc   = (WNDPROC) TtyViewWndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 4 ;
	wc.hInstance     = hInst ;
	wc.hIcon         = NULL ;
#ifdef _WIN32_WCE
	wc.hCursor       = NULL ;
#else
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
#endif
	wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
	wc.lpszMenuName  = 0 ;
	wc.lpszClassName = CTRL_TTYVIEW ;
	return ::RegisterClass( &wc ) ;
}

LRESULT CALLBACK
TtyViewWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam )
{
	PTtyView	pWindow ;

	if ( msg == WM_CREATE ) {
		LPCREATESTRUCT	lpStruct = (LPCREATESTRUCT) lParam ;
		pWindow = new TtyView() ;
		SetWindowLong( hWnd, 0, (LONG) pWindow ) ;
		pWindow->WMCreate( hWnd, lpStruct ) ;
#ifdef	DROPFILES	/* 00.07.07  by sahf */
		DragAcceptFiles(hWnd, TRUE) ;
#endif	/* DROPFILES */
		return 0 ;
	}
	pWindow = (PTtyView) GetWindowLong( hWnd, 0 ) ;
	if ( !pWindow )
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	switch ( msg ) {
	case WM_DESTROY:
#ifdef	DROPFILES	/* 00.07.07  by sahf */
		DragAcceptFiles( hWnd, FALSE ) ;
#endif	/* DROPFILES */
		delete pWindow ;
		SetWindowLong( hWnd, 0, (LONG) 0 ) ;
		break ;
	case WM_SETFOCUS:
		CreateCaret(hWnd, NULL, 2, pWindow->m_dwFontH);
		if (pWindow->m_bShowCursor)
			pWindow->GotoXY((WORD)pWindow->m_dwCurCol,
					(WORD)pWindow->m_dwCurLine);
		break;
	case WM_KILLFOCUS:
		DestroyCaret();
		break;
	case WM_COMMAND:
		return pWindow->WMCommand( hWnd, msg, wParam, lParam ) ;
	case WM_COPYDATA:
		return pWindow->WMCopydata( (PCOPYDATASTRUCT) lParam ) ;
	case WM_PAINT:
		pWindow->WMPaint() ;
		break ;
	case WM_CHAR:
		pWindow->WMChar( (TCHAR) wParam, lParam ) ;
		break ;
	case WM_SYSCHAR:
		if ( !pWindow->WMSysChar( (TCHAR) wParam, lParam ) )
			return DefWindowProc( hWnd, msg, wParam, lParam ) ;
		break ;
	case WM_KEYDOWN:
		if ( !pWindow->WMKeyDown( (int) wParam, lParam ) )
			return DefWindowProc( hWnd, msg, wParam, lParam ) ;
		break ;
	case WM_SIZE:
		pWindow->WMSize( wParam, LOWORD(lParam), HIWORD(lParam) ) ;
		break ;
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS ;
	case WM_LBUTTONDOWN:
		pWindow->WMLButtonDown((int)(lParam & 0xffff),
				       (int)((lParam >> 16) & 0xffff));
		break;
#ifdef	DROPFILES	/* 00.07.07  by sahf */
	case WM_DROPFILES:
		pWindow->WMDropFiles( (HDROP) wParam ) ;
		break ;
#endif	/* DROPFILES */
#if 0
	case TTYM_SETTAB:		pWindow->SetTab( (DWORD) wParam, (BOOL) lParam ) ;		break ;
	case TTYM_SETHMARGIN:	pWindow->SetHMargin( (DWORD) wParam, (BOOL) lParam ) ;	break ;
#endif
	case TTYM_SETEVENT:		pWindow->SetupEvent( (HANDLE) wParam ) ;					break ;
#ifndef _WIN32_WCE
	case TTYM_FONTCHANGED:
		pWindow->FontChanged();
		break;
#endif
	case TTYM_GOTOXY:		pWindow->GotoXY( HIWORD(wParam), LOWORD(wParam) ) ;		break ;
	case TTYM_PUTCHAR:		pWindow->PutChar( (BYTE) wParam ) ;						break ;
	case TTYM_PUTKCHAR:		pWindow->PutKChar( (BYTE) HIWORD(wParam), (BYTE) LOWORD(wParam) ) ;	break ;
	case TTYM_PUTLINE:		pWindow->PutLine( HIWORD(wParam), LOWORD(wParam), (LPCSTR) lParam ) ;	break ;
	case TTYM_ERASEEOL:		pWindow->EraseEOL() ;									break ;
	case TTYM_ERASEEOP:		pWindow->EraseEOP() ;									break ;
	case TTYM_FLUSH:		pWindow->Flush() ;										break ;
	case TTYM_KBHIT:		return pWindow->Kbhit() ;
	case TTYM_GETCHAR:		return pWindow->GetChar() ;
	case TTYM_GETWINDOWEVENT:	return pWindow->GetWindowEvent() ;
	case TTYM_GETWH:		return pWindow->GetWH() ;
#ifdef	DROPFILES	/* 00.07.07  by sahf */
	case TTYM_DROPFILES:	return  pWindow->GetDropFiles( (LPHANDLE) lParam ) ;
#endif	/* DROPFILES */

	default:
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	return 0 ;
}
