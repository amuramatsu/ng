/* $Id: cefep.h,v 1.1 2000/11/16 14:21:28 amura Exp $ */
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
 * $Log: cefep.h,v $
 * Revision 1.1  2000/11/16 14:21:28  amura
 * merge Ng for win32 0.5
 *
 */

#define		CTRL_FEPBOX			TEXT("FEPBOX")

#define		IDM_PEXCEL			(41258)
#define		IDM_UNKNOWN			(41259)
#define		IDM_FEPOK			(41260)
#define		IDM_ACTIVATE		(41261)
#define		IDM_FEPKEY			(41262)
#define		IDM_FEPCHAR			(41263)
#define		IDM_ACTIVATE_HWND	(41264)
#define		IDM_FEPGETMODE		(41265)
#define		IDM_FEPSETMODE		(41266)
#define		IDM_WMCHAR			(41267)

#define		IDM_EXPLORE			(1)

#define		FEP_ISSENDING		(WM_USER+0x0100)

#ifdef	__cplusplus
extern "C" {
#endif
	BOOL	FepBoxRegisterClass( HINSTANCE hInst ) ;
	HWND	GetCenterWindow() ;
	void	FepSetParent( HWND hWnd, BOOL bForce ) ;
	BOOL	FepSetDict( LPCTSTR dict_path ) ;

	BOOL	InitFep( void ) ;
	void	Fep_Execute( HWND hWnd ) ;
	BOOL	Fep_WM_CHAR( HWND hWnd, TCHAR chCharCode, LONG lKeyData ) ;
	BOOL	Fep_WM_SYSCHAR( HWND hWnd, TCHAR chCharCode, LONG lKeyData ) ;
	BOOL	Fep_WM_ACTIVATE( HWND hWnd, WORD fActive, BOOL fMinimize, HWND hWndDeactive ) ;

	extern	TCHAR	g_szFepName[ MAX_PATH ] ;
	extern	TCHAR	g_szFepProgram[ MAX_PATH ] ;
	extern	BOOL	g_fKanji ;
	extern	BOOL	g_fUseCtrl ;
	extern	BOOL	g_fFepOption ;
#ifdef	__cplusplus
} ;
#endif
