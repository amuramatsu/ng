/* $Id: tools.h,v 1.6 2003/02/22 08:09:47 amura Exp $ */
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

#ifndef __TOOLS_H__
#define __TOOLS_H__

#ifndef	APP_TOOLS1
#define	APP_TOOLS1

#ifdef	__cplusplus
extern "C" {
#endif

BOOL  Fopen(LPCTSTR, LPCTSTR);
void  Fclose(void);
int   Fgetc(void);
char* Fgets(char *, int);
int   Fputc(int);
int   Fputs(const char *);
void  Fwrite(const char *, int);
BOOL  Ferror(void);
void  slash2backslash(LPTSTR);
void  catdir(LPTSTR, LPCTSTR, LPTSTR);
void  rel2abs(LPTSTR, LPCTSTR, LPCTSTR);
int   SendClipboard(const char *);
int   ReceiveClipboard(char *, int);

int   unicode2sjis(const char *, unsigned char *, int);
int   sjis2unicode(const unsigned char *, char *, int);
TCHAR sjis2unicode_char(WORD);
WORD  unicode2sjis_char(TCHAR);
LONG  RegQueryString(HKEY, LPCTSTR, LPTSTR, LPTSTR, DWORD *);
LONG  RegSetString(HKEY, LPCTSTR, LPTSTR, LPTSTR);
LONG  RegSetBinary(HKEY, LPCTSTR, LPTSTR, LPBYTE, DWORD);
LONG  RegSetDWord(HKEY, LPCTSTR, LPTSTR, DWORD);
DWORD RegQueryDWord(HKEY, LPCTSTR, LPTSTR);
LONG  RegRemoveValue(HKEY, LPCTSTR, LPTSTR);
#ifdef	__cplusplus
};
#endif

#ifdef KANJI
#define NGREGKEY		TEXT("Software\\mg-developers\\Ng")
#define NGDEFAULTINIFILE	TEXT("\\ng.ini")
#else
#define NGREGKEY		TEXT("Software\\mg-developers\\MG")
#define NGDEFAULTINIFILE	TEXT("\\mg.ini")
#endif
#define NGSTARTUPFILEVAL	TEXT("Startup File")
#define NGCTRLKEYMAPVAL		TEXT("Control Key Mapping")
#define NGBEEPSOUNDVAL		TEXT("Beep Sound")
#define NGBEEPFILEVAL		TEXT("Beep Sound File")
#define NGFONTNAMEVAL		TEXT("Font Name")
#define NGFONTSIZEVAL		TEXT("Font Size")
#define NGLINESPACEVAL		TEXT("Line Space")
#define NGPREVPOSVAL		TEXT("Previous Position")
#define NGKEYBOARDLOCALEVAL	TEXT("Keyboard Locale")

#define NGKEYBOARD_US		1
#define NGKEYBOARD_JP		81

#define NG_WAVE_OFFSET		2

#endif	/* APP_TOOLS1 */

#endif /* __TOOLS_H__ */
