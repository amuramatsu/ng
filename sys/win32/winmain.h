/* $Id: winmain.h,v 1.4 2003/02/22 08:09:47 amura Exp $ */
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

#ifndef __WINMAIN_H__
#define __WINMAIN_H__

#ifdef	__cplusplus
extern "C" {
#endif
int  Main(int, char *[]);
int  Kbhit(void);
int  KbhitSleep(DWORD);
int  GetChar(void);
void GotoXY(int, int);
void EraseEOL(void);
void EraseEOP(void);
void PutChar(char);
void PutKChar(char, char);
void Flush(void );
void Exit(int);
void GetWH(int *, int *);
#ifdef	__cplusplus
};
#endif

#endif /* __WINMAIN_H__ */
