/* $Id: cefep.c,v 1.1 2000/11/16 14:21:28 amura Exp $ */
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
 * NG : NG FEP controll routine
 *
 * 1998/11/14:Eiichiroh Itoh
 *
 */

/*
 * $Log: cefep.c,v $
 * Revision 1.1  2000/11/16 14:21:28  amura
 * merge Ng for win32 0.5
 *
 */

#ifndef STRICT
#define	STRICT
#endif
#include	<windows.h>
#include	<tchar.h>
#include "config.h"
#include "def.h"

#if defined(KANJI) && defined(USE_KCTRL)
#include	"cefep.h"

#define		NO_CTRLSPACE

#define		GawaroBaseKey			TEXT("Software\\Gawaro")
#define		VAR_FEPNAME				TEXT("FepName")
#define		VAR_FEPPROGRAM			TEXT("FepProgram")
#define		VAR_USECTRL				TEXT("UseCtrl")

TCHAR		g_szFepName[ MAX_PATH ] = TEXT("KANJIIN") ;
TCHAR		g_szFepProgram[ MAX_PATH ] = TEXT("KanjiIn.exe") ;
BOOL		g_fKanji = FALSE ;
BOOL		g_fUseCtrl = FALSE ;
BOOL		g_fFepOption = FALSE ;

/*
 * Initilize FEP
 */
BOOL
InitFep( void )
{
	HKEY	hk ;
	LONG	lret ;
	LPCTSTR	name ;
	LPBYTE	lpData ;
	TCHAR	sValue[ 256 ] ;
	DWORD	dwType, cbData, dwValue ;

	/* Open Registory */
	lret = RegOpenKeyEx( HKEY_CURRENT_USER, GawaroBaseKey, 0,
						 KEY_QUERY_VALUE, &hk ) ;
	if ( lret == ERROR_SUCCESS ) {
		/* FEP name */
		name = VAR_FEPNAME ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) sValue ;
		cbData = sizeof sValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			_tcscpy( g_szFepName, sValue ) ;
		}
		/* FEP Program Name */
		name = VAR_FEPPROGRAM ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) sValue ;
		cbData = sizeof sValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			_tcscpy( g_szFepProgram, sValue ) ;
		}
		/* CTRL/ALT swapping */
		name = VAR_USECTRL ;
		dwType = REG_DWORD ;
		lpData = (LPBYTE) &dwValue ;
		cbData = sizeof dwValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			g_fUseCtrl = dwValue ;
		}
		/* Close Registory */
		RegCloseKey( hk ) ;
	}
	return TRUE ;
}

/*
 * Open FEP
 */
void
Fep_Execute( HWND hWnd )
{
	HWND	hWndFep ;

	hWndFep = FindWindow( g_szFepName, NULL ) ;
	if ( hWndFep ) {
		/* If FEP process exist, change it state */
		g_fKanji = SendMessage( hWndFep, WM_COMMAND, IDM_FEPGETMODE, 0 ) ;
		SendMessage( hWndFep, WM_COMMAND, IDM_FEPSETMODE, g_fKanji ? 0 : 1 ) ;
#ifdef	_WIN32_WCE
	} else {
		/* FEP process is not exist */
		PROCESS_INFORMATION	pi ;
		TCHAR				param[ 20 ] ;
		wsprintf( param, TEXT("%d"), hWnd ) ;
		g_fKanji = CreateProcess( g_szFepProgram, param,
						NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi ) ;
#endif	/* _WIN32_WCE */
	}
}

/*
 * WM_CHAR
 */
BOOL
Fep_WM_CHAR( HWND hWnd, TCHAR chCharCode, LONG lKeyData )
{
	HWND	hWndFep ;

#ifndef	NO_CTRLSPACE
	if ( chCharCode == TEXT(' ') ) {
		if ( g_fUseCtrl && (GetKeyState( VK_CONTROL) & 0x80) ) {
			/* Pressed CTRL + SPACE */
			Fep_Execute( hWnd ) ;
			return TRUE ;
		}
		return FALSE ;
	}
#endif	/* NO_CTRLSPACE */
	if ( chCharCode < TEXT(' ') ) {
		return FALSE ;
	}
	hWndFep = FindWindow( g_szFepName, NULL ) ;
	if ( hWndFep ) {
		g_fKanji = SendMessage( hWndFep, WM_COMMAND, IDM_FEPGETMODE, 0 ) ? FALSE : TRUE ;
	} else {
		g_fKanji = FALSE ;
	}
	if ( g_fKanji && (chCharCode != TEXT(' ')) ) {
		SendMessage( hWndFep, WM_COMMAND, MAKEWPARAM( IDM_ACTIVATE_HWND, chCharCode ), (LPARAM) hWnd ) ;
		return TRUE ;
	}
	return FALSE ;
}

/*
 * WM_SYSCHAR
 */
BOOL
Fep_WM_SYSCHAR( HWND hWnd, TCHAR chCharCode, LONG lKeyData )
{
#ifndef	NO_CTRLSPACE
	if ( chCharCode == TEXT(' ') ) {
		if ( !g_fUseCtrl && (lKeyData & 0x20000000) ) {
			/* Pressed ALT + SPACE */
			Fep_Execute( hWnd ) ;
			return TRUE ;
		}
	}
#endif	/* NO_CTRLSPACE */
	return FALSE ;
}

BOOL
Fep_WM_ACTIVATE( HWND hWnd, WORD fActive, BOOL fMinimize, HWND hWndDeactive )
{
	if ( g_fFepOption ) {
		if ( fActive == WA_ACTIVE || fActive == WA_CLICKACTIVE ) {
			HWND	hWndFep = FindWindow( g_szFepName, NULL ) ;
			if ( hWndFep ) {
				g_fKanji = SendMessage( hWndFep, WM_COMMAND, IDM_FEPGETMODE, 0 ) ? FALSE : TRUE ;
				if ( g_fKanji ) {
					SendMessage( hWndFep, WM_COMMAND, IDM_FEPSETMODE, 1 ) ;
				}
			}
		}
	}
	return FALSE ;
}

#endif /* defined(KANJI) && defined(USE_KCTRL) */
