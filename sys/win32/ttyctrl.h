/* $Id: ttyctrl.h,v 1.4 2000/11/16 14:21:32 amura Exp $ */
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
 * $Log: ttyctrl.h,v $
 * Revision 1.4  2000/11/16 14:21:32  amura
 * merge Ng for win32 0.5
 *
 * Revision 1.3  2000/10/23 16:52:51  amura
 * add GPL copyright to header
 *
 * Revision 1.2  2000/07/22 20:46:33  amura
 * support "Drag&Drop"
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#define		CTRL_LINEEDIT		TEXT("LINEEDIT")
#define		CTRL_TIMEEDIT		TEXT("TIMEEDIT")
#define		CTRL_BIGLIST		TEXT("BIGLIST")
#define		CTRL_LABEL			TEXT("KLABEL")
#define		CTRL_FILEVIEW		TEXT("FILEVIEW")
#define		CTRL_TTYVIEW		TEXT("TTYVIEW")

#define		RBM_GETSJIS			(WM_USER+0x100)
#define		RBM_SETSJIS			(WM_USER+0x101)
#define		RBM_GETMODIFY		(WM_USER+0x102)
#define		RBM_SETMODIFY		(WM_USER+0x103)
#define		RBN_ENTER			0x0100
#define		RBN_ESC				0x0101
#define		RBN_TAB				0x0102
#define		RBN_STAB			0x0103
#define		RBN_CHANGE			0x0104
#define		RBN_KEYDOWN			0x0105
#define		RBN_KILLFOCUS		0x0106

#define		TEM_GETTIMEVAL		(WM_USER+0x100)
#define		TEM_SETTIMEVAL		(WM_USER+0x101)
#define		TEM_GETMODIFY		(WM_USER+0x102)
#define		TEM_SETMODIFY		(WM_USER+0x103)
#define		TEN_ENTER			0x0100
#define		TEN_ESC				0x0101
#define		TEN_TAB				0x0102
#define		TEN_STAB			0x0103
#define		TEN_CHANGE			0x0104
#define		TEN_KEYDOWN			0x0105
#define		TEN_KILLFOCUS		0x0106

#define		BLM_SETPROC			(WM_USER+0x100)
#define		BLM_SETSPACEPROC	(WM_USER+0x101)
#define		BLM_GETCURSEL		(WM_USER+0x102)
#define		BLM_SETCURSEL		(WM_USER+0x103)
#define		BLM_SETKEYMASK		(WM_USER+0x104)
#define		BLM_FINDSTRING		(WM_USER+0x105)
#define		BLM_SETDBLCLK		(WM_USER+0x106)
#define		BLM_GETMAXLINE		(WM_USER+0x107)
#define		BLN_ENTER			0x0100
#define		BLN_ESC				0x0101
#define		BLN_TAB				0x0102
#define		BLN_STAB			0x0103
#define		BLN_CHANGE			0x0104
#define		BLN_CHAR			0x0105
#define		BLN_KEYDOWN			0x0200

#define		BL_MASK_RETURN		0x0001
#define		BL_MASK_ESCAPE		0x0002
#define		BL_MASK_TAB			0x0004
#define		BL_MASK_HOME		0x0008
#define		BL_MASK_END			0x0010
#define		BL_MASK_UP			0x0020
#define		BL_MASK_DOWN		0x0040
#define		BL_MASK_LEFT		0x0080
#define		BL_MASK_PRIOR		0x0100
#define		BL_MASK_RIGHT		0x0200
#define		BL_MASK_NEXT		0x0400

#define		LAM_SETINVERT		(WM_USER+0x100)
#define		LAM_SETCROSS		(WM_USER+0x101)
#define		LAN_CLICK			0x100

/* CTRL_LINEEDIT related messages */
#define		LEN_ENTER			0x0100
#define		LEN_TAB				0x0102
#define		LEN_STAB			0x0103
#define		LEN_CHANGE			0x0104
#define		LEN_KEYDOWN			0x0105
#define		LEN_KILLFOCUS		0x0106
#define		LEN_GOTFOCUS		0x0107

/* CTRL_FILEVIEW related message */
enum {
	FV_FUNC_NONE = 0,
	FV_FUNC_ENTER, FV_FUNC_ESC,
	FV_FUNC_UP, FV_FUNC_DOWN, FV_FUNC_PRIOR, FV_FUNC_NEXT,
	FV_FUNC_HOME, FV_FUNC_END,
	FV_FUNC_FIND,
} ;

typedef struct {
	WORD		keyCode ;
	BYTE		keyMode ;
	BYTE		functionID ;
} KeyMapInfo, *LPKeyMapInfo ;

#define		FVM_CLEAR			(WM_USER+0x100)
#define		FVM_LOAD			(WM_USER+0x101)
#define		FVM_RELOAD			(WM_USER+0x102)
#define		FVM_OPEN			(WM_USER+0x103)
#define		FVM_CLOSE			(WM_USER+0x104)
#define		FVM_GETTOPLINE		(WM_USER+0x105)
#define		FVM_SETTOPLINE		(WM_USER+0x106)
#define		FVM_GETTOPPOS		(WM_USER+0x107)
#define		FVM_SETTOPPOS		(WM_USER+0x108)
#define		FVM_GETMAXLINE		(WM_USER+0x109)
#define		FVM_SETTAB			(WM_USER+0x10A)
#define		FVM_SETHMARGIN		(WM_USER+0x10B)
#define		FVM_SETKEYMAP		(WM_USER+0x10C)
#define		FVM_FINDSTRING		(WM_USER+0x10D)
#define		FVN_ENTER			0x0100
#define		FVN_ESC				0x0101
#define		FVN_TAB				0x0102
#define		FVN_STAB			0x0103
#define		FVN_CHANGE			0x0104
#define		FVN_CHAR			0x0105
#define		FVN_KEYDOWN			0x0200

#define		FV_MASK_RETURN		0x0001
#define		FV_MASK_ESCAPE		0x0002
#define		FV_MASK_TAB			0x0004
#define		FV_MASK_HOME		0x0008
#define		FV_MASK_END			0x0010
#define		FV_MASK_UP			0x0020
#define		FV_MASK_DOWN		0x0040
#define		FV_MASK_LEFT		0x0080
#define		FV_MASK_PRIOR		0x0100
#define		FV_MASK_RIGHT		0x0200
#define		FV_MASK_NEXT		0x0400

#define		TTYM_SETTAB			(WM_USER+0x100)
#define		TTYM_SETHMARGIN		(WM_USER+0x101)
#define		TTYM_SETEVENT		(WM_USER+0x102)
#define		TTYM_GOTOXY			(WM_USER+0x103)
#define		TTYM_PUTCHAR		(WM_USER+0x104)
#define		TTYM_PUTKCHAR		(WM_USER+0x105)
#define		TTYM_PUTLINE		(WM_USER+0x106)
#define		TTYM_ERASEEOL		(WM_USER+0x107)
#define		TTYM_ERASEEOP		(WM_USER+0x108)
#define		TTYM_FLUSH			(WM_USER+0x109)
#define		TTYM_KBHIT			(WM_USER+0x10A)
#define		TTYM_GETCHAR		(WM_USER+0x10B)
#define		TTYM_GETWH			(WM_USER+0x10C)
#define		TTYM_GETWINDOWEVENT	(WM_USER+0x10D)
#define		TTYM_FONTCHANGED	(WM_USER+0x10E)
#ifdef	DROPFILES	/* 00.07.07  by sahf */
#define		TTYM_DROPFILES		(WM_USER+0x10F)
#endif	/* DROPFILES */
#define		TTYM_COMMAND		(WM_USER+0x110)
#define		TTYM_SETKEYBOARDLOCALE	(WM_USER+0x111)

#define		IDM_FEPOK			(41260)
#define		IDM_FEPACTIVE		(41261)
#define		IDM_FEPKEY			(41262)

/* The following are window events from TTYCTRL.  The maximum number
   of event id is 0x0f (decimal 15) */

#define TTY_WM_MASK 0x0f
#define TTY_WM_MOUSE  1
#define TTY_WM_RESIZE 2
#ifdef	DROPFILES	/* 00.07.07  by sahf */
#define TTY_WM_DROPFILES 3
#endif	/* DROPFILES */

#ifdef	__cplusplus
extern "C" {
#endif
	BOOL	LineEditRegisterClass( HINSTANCE hInst ) ;
	BOOL	BigListRegisterClass( HINSTANCE hInst ) ;
	BOOL	LabelRegisterClass( HINSTANCE hInst ) ;
	BOOL	TimeEditRegisterClass( HINSTANCE hInst ) ;
	BOOL	FileViewRegisterClass( HINSTANCE hInst ) ;
	BOOL	TtyViewRegisterClass( HINSTANCE hInst ) ;
	void	ShowSIP( BOOL bShow ) ;
#ifdef	__cplusplus
} ;
#endif
