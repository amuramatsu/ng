/* $Id: tty.c,v 1.2 2000/10/23 16:52:51 amura Exp $ */
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
 * $Log: tty.c,v $
 * Revision 1.2  2000/10/23 16:52:51  amura
 * add GPL copyright to header
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#include	<windows.h>
#include	"config.h"
#include	"def.h"
#include	"winmain.h"
#include "tools.h"

extern	int	ttrow ;
extern	int	ttcol ;
extern	int	tttop ;
extern	int	ttbot ;
extern	int	tthue ;

int		SG = 0 ;
int		tceeol ;			/* Costs are set later */
int		tcinsl ;
int		tcdell ;

int
ttinit()
{
	tceeol = ttcol ;
	tcdell = NROW * NCOL ;
	tcinsl = NROW * NCOL ;
	return 0 ;
}

int
tttidy()
{
	return 0 ;
}

int
ttmove( int row, int col )
{
	if ( ttrow != row || ttcol != col ) {
		GotoXY( col, row ) ;
		ttrow = row ;
		ttcol = col ;
	}
	return 0 ;
}

/*
 * Erase to end of line.
 */
int
tteeol()
{
	GotoXY( ttcol, ttrow ) ;
	EraseEOL() ;
	return 0 ;
}

int
ttnowindow()
{
	ttrow = HUGE ;
	ttcol = HUGE ;
	tttop = HUGE ;
	ttbot = HUGE ;
	return 0 ;
}

/*
 * Set the current writing color to the
 * specified color. Watch for color changes that are
 * not going to do anything (the color is already right)
 * and don't send anything to the display.
 * The rainbow version does this in putline.s on a
 * line by line basis, so don't bother sending
 * out the color shift.
 */
int
ttcolor( int color )
{
    if ( color != tthue ) {
		tthue = color ;			/* Save the color.	*/
    }
	return 0 ;
}

/*
 * Erase to end of page.
 */
int
tteeop()
{
	GotoXY( ttcol, ttrow ) ;
	EraseEOP() ;
	return 0 ;
}

/*
 * Delete nchunk line(s) from "row", replacing the
 * bottom line on the screen with a blank line.
 * Unless we're using the scrolling region, this is
 * done with a crafty sequences of insert and delete
 * lines.  The presence of the echo area makes a
 * boundry condition go away.
 */
int
ttdell( int row, int bot, int nchunk )
{
	ttrow = HUGE ;
	ttcol = HUGE ;
	return 0 ;
}

/*
 * Insert nchunk blank line(s) onto the
 * screen, scrolling the last line on the
 * screen off the bottom.  Use the scrolling
 * region if possible for a smoother display.
 * If no scrolling region, use a set
 * of insert and delete line sequences
 */
int
ttinsl( int row, int bot, int nchunk )
{
	ttrow = HUGE ;
	ttcol = HUGE ;
	return 0 ;
}

/*
 * Make a noise.
 */
int
ttbeep()
{
  extern DWORD g_beepsound;
  extern TCHAR g_beepfile[];

  switch (g_beepsound) {
  case 0:
    /* no sounds */
    break;

#ifndef TARGET_WCEVER_IS_100
  case 1: /* play specified sound file */
    if (g_beepfile[0]) {
      sndPlaySound(g_beepfile, SND_ASYNC | SND_NODEFAULT);
      break;
    }
#endif

  default:
    MessageBeep(0xFFFFFFFF); /* Default wave sound */
    break;

  case MB_OK + NG_WAVE_OFFSET:
  case MB_ICONASTERISK + NG_WAVE_OFFSET:
  case MB_ICONEXCLAMATION + NG_WAVE_OFFSET:
  case MB_ICONHAND + NG_WAVE_OFFSET:
  case MB_ICONQUESTION + NG_WAVE_OFFSET:
    MessageBeep(g_beepsound - NG_WAVE_OFFSET);
    break;
  }
  return 0;
}

/*
 * This routine is called by the
 * "refresh the screen" command to try and resize
 * the display. The new size, which must be deadstopped
 * to not exceed the NROW and NCOL limits, it stored
 * back into "nrow" and "ncol". Display can always deal
 * with a screen NROW by NCOL. Look in "window.c" to
 * see how the caller deals with a change.
 */
int
ttresize()
{
        extern void setttysize pro((void));
	setttysize();
	return 0;
}
