/* $Id: display.c,v 1.14.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 * The functions in this file handle redisplay. The
 * redisplay system knows almost nothing about the editing
 * process; the editing functions do, however, set some
 * hints to eliminate a lot of the grinding. There is more
 * that can be done; the "vtputc" interface is a real
 * pig. Two conditional compilation flags; the GOSLING
 * flag enables dynamic programming redisplay, using the
 * algorithm published by Jim Gosling in SIGOA. The MEMMAP
 * changes things around for memory mapped video. With
 * both off, the terminal is a VT52.
 */

/*
 * $Log: display.c,v $
 * Revision 1.14.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.14  2001/04/28 18:54:27  amura
 * support line-number-mode (based on MATSUURA's patch )
 *
 * Revision 1.13  2001/02/18 17:07:24  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.12  2001/02/14 09:18:55  amura
 * code cleanup around putline()
 *
 * Revision 1.11  2001/02/11 15:40:24  amura
 * some function are changed to static for speed/size
 *
 * Revision 1.10  2001/02/01 16:29:32  amura
 * fix terminal buffer size check
 *
 * Revision 1.9  2001/01/20 18:16:20  amura
 * fix vtsetsize() bug, and make alignment VIDEO buffer
 *
 * Revision 1.8  2001/01/20 15:48:45  amura
 * very big terminal supported
 *
 * Revision 1.7  2001/01/05 14:07:01  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.6  2000/11/16 14:31:12  amura
 * fix some typos which cause compile error when using
 * strict ANSI-C compiler (ex ACK, gcc-1.x)
 *
 * Revision 1.5  2000/10/02 16:24:42  amura
 * bugfix by Tillanosoft(Ng for Win32)
 *
 * Revision 1.4  2000/09/21 17:28:29  amura
 * replace macro _WIN32 to WIN32 for Cygwin
 *
 * Revision 1.3  2000/06/27 01:49:43  amura
 * import to CVS
 *
 * Revision 1.2  2000/06/01  18:29:12  amura
 * support VARIABLE_TAB
 *
 * Revision 1.1  1999/05/19  04:25:31  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	"kbd.h"

/*
 * You can change these back to the types
 * implied by the name if you get tight for space. If you
 * make both of them "int" you get better code on the VAX.
 * They do nothing if this is not Gosling redisplay, except
 * for change the size of a structure that isn't used.
 * A bit of a cheat.
 */
/* These defines really belong in sysdef.h */
#ifndef XCHAR
#  define	XCHAR	int
#  define	XSHORT	int
#endif

#ifdef	STANDOUT_GLITCH
# ifdef TCCONIO
#  undef STANDOUT_GLITCH
# else
extern int SG;				/* number of standout glitches	*/
# endif /* TCCONIO */
#endif /* STANDOUT_GLITCH */
#ifdef GOSLING
# undef GOSLING				/* GOSLING too slow?		*/
#endif

/*
 * A video structure always holds
 * an array of characters whose length is equal to
 * the longest line possible. Only some of this is
 * used if "ncol" isn't the same as "NCOL".
 */
typedef struct	{
	short	v_hash;			/* Hash code, for compares.	*/
	short	v_flag;			/* Flag word.			*/
	short	v_color;		/* Color of the line.		*/
	XSHORT	v_cost;			/* Cost of display.		*/
#ifndef ZEROARRAY
	char	v_text[1];		/* The actual characters.	*/
#else
	char	v_text[];		/* The actual characters.	*/
#endif
}	VIDEO;

#define	MEM_ROUND(n)	(((n)+7)&~7)	/* Memory round bound for 8 bytes*/
#ifdef	SS_SUPPORT
#define SIZEOF_VIDEO	MEM_ROUND(sizeof(VIDEO) + vncol*2)
#define	v_sub(n) v_text[(n)+vncol]	/* 2nd Character of HANKANA	*/
#else	/* SS_SUPPORT */
#define SIZEOF_VIDEO	MEM_ROUND(sizeof(VIDEO) + vncol)
#endif

#define VFCHG	0x0001			/* Changed.			*/
#define VFHBAD	0x0002			/* Hash and cost are bad.	*/
#define VFEXT	0x0004			/* extended line (beond ncol)	*/

/*
 * SCORE structures hold the optimal
 * trace trajectory, and the cost of redisplay, when
 * the dynamic programming redisplay code is used.
 * If no fancy redisplay, this isn't used. The trace index
 * fields can be "char", and the score a "short", but
 * this makes the code worse on the VAX.
 */
typedef struct	{
	XCHAR	s_itrace;		/* "i" index for track back.	*/
	XCHAR	s_jtrace;		/* "j" index for trace back.	*/
	XSHORT	s_cost;			/* Display cost.		*/
}	SCORE;

int	sgarbf	= TRUE;			/* TRUE if screen is garbage.	*/
static int	vtrow	= 0;		/* Virtual cursor row.		*/
static int	vtcol	= 0;		/* Virtual cursor column.	*/
int	tthue	= CNONE;		/* Current color.		*/
int	ttrow	= HUGE;			/* Physical cursor row.		*/
int	ttcol	= HUGE;			/* Physical cursor column.	*/
int	tttop	= HUGE;			/* Top of scroll region.	*/
int	ttbot	= HUGE;			/* Bottom of scroll region.	*/

extern int	ncol;
extern int	nrow;
static int	vncol   = 0;
static int	vnrow	= 0;

static VIDEO	**vscreen = NULL;	/* Edge vector, virtual.	*/
static VIDEO	**pscreen = NULL;	/* Edge vector, physical.	*/
static VIDEO	*video    = NULL;	/* Actual screen data.		*/
static VIDEO	*blanks   = NULL;	/* Blank line image.		*/

#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#define	ASCII	0
#define	K1ST	1
#define	K2ND	2
#define	HAN1ST	3
#define HOJO1ST	4
#define	HOJO2ND	5
int	vtkattr	= ASCII;
#endif	/* KANJI */
/*
 * Some predeclerations to make ANSI compilers happy
 */
VOID	vtinit();
VOID	vttidy();
VOID	vtputc();
VOID	update();
static VOID	vtmove();
static VOID	vtmarkyen();
static VOID	vteeol();
static VOID	ucopy();
static VOID	uline();
static VOID	modeline();
#ifdef	ADDFUNC
static VOID	moderatio();
#endif

#ifdef	GOSLING
/*
 * This matrix is written as an array because
 * we do funny things in the "setscores" routine, which
 * is very compute intensive, to make the subscripts go away.
 * It would be "SCORE	score[vnrow][vnrow]" in old speak.
 * Look at "setscores" to understand what is up.
 */
static VOID	hash();
static VOID	setscores();
static VOID	traceback();

static SCORE	*score = NULL;
#endif

VOID
vtsetsize(col, row) {
	register VIDEO	*vp;
	register int	i;

	if (col<=vncol && row<=vnrow)
		return;
	vncol = col;
	vnrow = row;

	if (vscreen != NULL) free(vscreen);
	if (pscreen != NULL) free(pscreen);
	if (video != NULL)   free(video);
	if (blanks != NULL)  free(blanks);
	
	vscreen = malloc(sizeof(VIDEO*)*(vnrow-1));
	pscreen = malloc(sizeof(VIDEO*)*(vnrow-1));
	video   = malloc(SIZEOF_VIDEO*2*(vnrow-1));
	blanks  = malloc(SIZEOF_VIDEO);
	if (vscreen==NULL || pscreen==NULL || video==NULL || blanks==NULL)
		panic("Cannot allocate video buffer");
	bzero(video, SIZEOF_VIDEO*2*(vnrow-1));
	bzero(blanks, SIZEOF_VIDEO);
#ifdef	GOSLING
	if (score != NULL) free(score);
	score   = malloc(sizeof(SCORE)*vnrow*vnrow);
	if (score == NULL)
		panic("Cannot allocate video buffer");
	bzero(score, sizeof(SCORE)*vnrow*vnrow);
#endif
	
	vp = video;
	for (i=0; i<vnrow-1; ++i) {
		vscreen[i] = (VIDEO*)vp;
		vp = (VIDEO*)((char*)vp +SIZEOF_VIDEO);
		pscreen[i] = (VIDEO*)vp;
		vp = (VIDEO*)((char*)vp +SIZEOF_VIDEO);
	}
	blanks->v_color = CTEXT;
	for (i=0; i<vncol; ++i) {
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		blanks->v_sub(i) = 0;
#endif  /* SS_SUPPORT */
		blanks->v_text[i] = ' ';
	}
}

/*
 * Initialize the data structures used
 * by the display code. The edge vectors used
 * to access the screens are set up. The operating
 * system's terminal I/O channel is set up. Fill the
 * "blanks" array with ASCII blanks. The rest is done
 * at compile time. The original window is marked
 * as needing full update, and the physical screen
 * is marked as garbage, so all the right stuff happens
 * on the first call to redisplay.
 */
VOID
vtinit() {
	int col, row;
	ttopen();
	ttinit();
	col = ncol;
	row = nrow;
	if (col < NCOL) col = NCOL;
	if (row < NROW) row = NROW;
	vtsetsize(col, row);
}

/*
 * Tidy up the virtual display system
 * in anticipation of a return back to the host
 * operating system. Right now all we do is position
 * the cursor to the last line, erase the line, and
 * close the terminal channel.
 */
VOID
vttidy() {
	ttcolor(CTEXT);
	ttnowindow();				/* No scroll window.	*/
	ttmove(nrow-1, 0);			/* Echo line.		*/
	tteeol();
	tttidy();
	ttflush();
	ttclose();
}

/*
 * Move the virtual cursor to an origin
 * 0 spot on the virtual display screen. I could
 * store the column as a character pointer to the spot
 * on the line, which would make "vtputc" a little bit
 * more efficient. No checking for errors.

 */
static VOID
vtmove(row, col) {
	vtrow = row;
	vtcol = col;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	vtkattr = ASCII;
#endif	/* KANJI */
}

/*
 * Write a character to the virtual display,
 * dealing with long lines and the display of unprintable
 * things like control characters. Also expand tabs every 8
 * columns. This code only puts printing characters into
 * the virtual display image. Special care must be taken when
 * expanding tabs. On a screen whose width is not a multiple
 * of 8, it is possible for the virtual cursor to hit the
 * right margin before the next tab stop is reached. This
 * makes the tab code loop if you are not careful.
 * Three guesses how we found this.
 */
VOID
vtputc(c) register int c; {
	register VIDEO	*vp;
#ifdef VARIABLE_TAB
	int tab = curwp->w_bufp->b_tabwidth;
#endif

	/* vtrow sometimes over-runs the vnrow -1.  In the case, vp at
	   following line becomes an uninitialized pointer.  Then core
	   dump or system error may occur.  To avoid the error.  Some
	   range confirmation should be needed. 
	   By Tillanosoft Sep 9, 2000.  */
	if (vnrow - 1 <= vtrow) {
	  return;
	}

	vp = vscreen[vtrow];
	if (c == '\t' && !(curwp->w_bufp->b_flag & BFNOTAB)) {
		if (
#ifdef  VARIABLE_TAB
		    (vtcol/tab +1)*tab
#else
		    (vtcol | 0x07) + 1
#endif
		    <= ncol - 1 ) {
			do {
				vtputc(' ');
			} while (vtcol<ncol &&
#ifdef  VARIABLE_TAB
				 (vtcol%tab)!=0
#else
				 (vtcol&0x07)!=0
#endif
				 );
		} else {
			vteeol();
			vp->v_text[ncol-1]='\\';
			vtcol=0;
			vtrow++;
		}
	} else if (ISCTRL(c)) {
		vtputc('^');
		vtputc(CCHR(c));
#ifdef	HANKANA  /* 92.11.21  by S.Sasaki */
	} else if (vtkattr == HAN1ST) {
		vp->v_sub(vtcol-1) = c;
		vtkattr = ASCII;
#endif  /* HANKANA */
#ifdef	HOJO_KANJI
	} else if (vtkattr == HOJO1ST) {
		vp->v_sub(vtcol-1) = c;
		vtkattr = HOJO2ND;
	} else if (vtkattr == HOJO2ND) {
		vp->v_text[vtcol++] = SS3;
		vp->v_sub(vtcol)    = c;
		vtkattr = ASCII;
#endif	/* HOJO_KANJI */
	} else {
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		vp->v_sub(vtcol) = 0;
#endif
		vp->v_text[vtcol++] = c;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (vtkattr == K1ST) {
			vtkattr = K2ND;
		} else if (ISKANJI(c)) {
#ifdef	HANKANA  /* 92.11.21  by S.Sasaki */
		        if (ISHANKANA(c)) vtkattr = HAN1ST;
			else
#endif
#ifdef	HOJO_KANJI
			if (ISHOJO(c))	  vtkattr = HOJO1ST;
			else
#endif
			vtkattr = K1ST;
		} else {
			vtkattr = ASCII;
		}
#endif	/* KANJI */
	}
	if (vtcol >= ncol-1 
#ifdef  HANKANA
		&& vtkattr != HAN1ST
#endif
#ifdef	HOJO_KANJI
		&& vtkattr != HOJO1ST
#endif
	) {
		vp->v_text[ncol-1] = '\\';
		vtcol=0;
		vtrow++;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (vtkattr == K1ST
#ifdef	HOJO_KANJI
		    || vtkattr == HOJO2ND
#endif
		    ) {
			vtkattr = ASCII;
			vtputc(vp->v_text[ncol-2]);
			vp->v_text[ncol - 2] = '\\';
		}
#endif	/* KANJI */
	}
}

#if defined(MEMMAP)&&(!defined(PC9801))&&(!defined(WIN32))
static VOID
#ifdef	SS_SUPPORT
putline(row, col, s, t, color)
unsigned char *s;
unsigned char *t;
short color;	/* this is dummy */
#else
putline(row, col, s, color)
unsigned char *s;
short color;	/* this is dummy */
#endif
{
    register int i;
    unsigned char c;
#ifdef	SS_SUPPORT
    unsigned char c1;
#endif
    int oldrow = vtrow;
    int oldcol = vtcol;

    vtrow = row;
    vtcol = col;

    for (i=row; i<=MAXROW; i++)
    {
	c = *s++;
#ifdef	SS_SUPPORT
	if (vtkattr == ASCII) {
	    c1 = *t++;
#ifdef	HANKANA
	    if (ISHANKANA(c) && c1!=0) {
		vtkattr = HAN1ST;
		c = c1;
	    }
#endif
#ifdef	HOJO_KANJI
	    if (ISHOJO(c) && c1!=0) {
		vtkattr = HOJO1ST;
		c = c1;
	    }
#endif
	}
#endif
	vtputc(c);
    }

    vtrow = oldrow;
    vtcol = oldcol;
}
#endif /* MEMMAP && !PC9801 && !WIN32 */

/* Mark '\\' end of line 
 * whether curcol is not on the top of line.
 */
static VOID
vtmarkyen(fillchar)
char	fillchar;
{
	register VIDEO	*vp;

	vp = vscreen[vtrow];
	if (vtcol > 0) {
		while (vtcol < ncol) {
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
			vp->v_sub(vtcol)=0;
#endif  /* SS_SUPPORT */
			vp->v_text[vtcol++] = fillchar;
		}
	}
}

/* Erase from the end of the
 * software cursor to the end of the
 * line on which the software cursor is
 * located. The display routines will decide
 * if a hardware erase to end of line command
 * should be used to display this.
 */
static VOID
vteeol() {
	register VIDEO	*vp;

	vp = vscreen[vtrow];
	while (vtcol < ncol) {
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		vp->v_sub(vtcol)=0;
#endif  /* SS_SUPPORT */
		vp->v_text[vtcol++] = ' ';
	}
}

/* Calculate offset to col and row
*/
int
#ifdef SUPPORT_ANSI /* for strict compiler */
colrow(LINE *lp, short offset, int *curcol, int *lines)
#else
colrow(lp, offset, curcol, lines)
LINE	*lp;
short	offset;
int	*curcol;
int	*lines;
#endif
{
	register int	i;
	register char	c;
	register int	c1;
#ifdef	VARIABLE_TAB
	int	tab = curbp->b_tabwidth;
#endif

	c1=0;
	*curcol = 0;
	*lines = 0;
	for (i=0; i<offset; ++i) {
		c = lgetc(lp, i);
#ifdef	KANJI
		if ( ISKANJI(c) ) {
		    if (c1==1) c1=0;
		    else c1=1;
		} else c1=0;
#endif	/* KANJI */
#ifdef	HANKANA
		if (ISHANKANA(c) && c1 == 1 )
		    --(*curcol); 
#endif
		if (c == '\t' && !(curbp->b_flag & BFNOTAB)
			) {
#ifdef	VARIABLE_TAB
			*curcol = ((*curcol)/tab +1)*tab -1;
#else
			*curcol |= 0x07;
#endif
			if (*curcol >= ncol -2) {
				*curcol = -1;
				(*lines)++;
			}
		} else if (ISCTRL(c) != FALSE)
			++(*curcol);
		++(*curcol);
		if (*curcol >= ncol-1) {
#ifdef  KANJI
			if (c1 == 1) *curcol=1;
			else	*curcol = *curcol - ncol + 1;
#else  /* not KANJI */
			*curcol = *curcol - ncol + 1;
#endif  /* KANJI */
			++(*lines);
		}
	}
#ifdef  CURSOR_POS
#ifdef  KANJI
	if (*curcol == ncol -2 && i < llength(lp)) {
		c=lgetc(lp, i);
		if (ISKANJI(c)
#ifdef	HANKANA
		    && ISHANKANA(c)
#endif
		)	{
			++(*lines);
			*curcol = 0;
		}
	}			
#endif
#endif  /* CURSOR_POS */
	return (short)*lines;
}

/*
   Returns offset of specified location.  This will be mainly used to
   respond to a mouse click on a screen.  In case the specified
   location does not exist within the `lp', negative value will be
   returned.  The absolute value of the negative result is the number
   of rows left.

   This will return an offset right neighbor to the position, if the
   character at the position is a wide character ,that is, the width
   of the character is more than or equal to a double width character,
   and the position is at the right half of the character.

   By Tillanosoft, March 21, 1999.  */

int
rowcol2offset(lp, row, col)
LINE *lp;
int row, col;
{
  register int	i;
  register int	curcol;
  register char	c;
  register int width, skipbytes;
#ifdef  VARIABLE_TAB
  int tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */
  
  curcol = 0;
  for (i=0; i<llength(lp); i += skipbytes) {
    if (row < 0) {
      return i;
    }
    if (row == 0 && col < curcol) return i;
    c = lgetc(lp, i);
    width = 1;
    skipbytes = 1;
#ifdef	KANJI
    if ( ISKANJI(c) ) {
      width = 2;
      skipbytes = 2;
    }
#ifdef	HANKANA
    if ( ISHANKANA(c) ) {
      width = 1;
      skipbytes = 2;
    }
#endif	/* HANKANA */
#endif	/* KANJI */
    if (c == '\t' && !(curbp->b_flag & BFNOTAB) ) {
#ifdef VARIABLE_TAB
      width = (curcol/tab + 1)*tab - curcol;
#else
      width = (curcol | 0x07) - curcol + 1;
#endif
      /* I should pay more care for the above line.
	 In case, wrapping arround occurs, where should I put the curcol
	 in the next line?
	 By Tillanosoft Mar 21, 1999 */
    } else if (ISCTRL(c) != FALSE) {
      width = 2;
    }
    if (row == 0 && col < curcol + (width + 1) / 2) {
      return i;
    }
    curcol += width;
    if (curcol >= ncol-1
#ifdef	KANJI
	|| (ISKANJI(c) && curcol >= ncol-2)
#endif
	) {
      curcol= ISCTRL(c) ? (curcol - ncol + 1) : 0;
      --row;
    }
  }
  if (row <= 0) {
    return i;
  }
  else {
    return -row;
  }
}

/* Return offset of #th lines
 */
short
skipline(lp, lines)
LINE	*lp;
int	lines;
{
	register int	i;
	register int	curcol;
	register char	c;
	register int	c1;
#ifdef  VARIABLE_TAB
	int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

	c1=0;
	curcol = 0;
	for (i=0; i<llength(lp); ++i) {
		if (lines == 0) return i - curcol;
		c = lgetc(lp, i);
#ifdef	KANJI
		if ( ISKANJI(c) ) {
		    if (c1==1) c1=0;
		    else c1=1;
		} else c1=0;
#endif	/* KANJI */
#ifdef	HANKANA
		if ( ISHANKANA(c) && c1 == 1 )
		    --curcol; 
#endif	/* HANKANA */
		if (c == '\t' && !(curbp->b_flag & BFNOTAB)) {
#ifdef	VARIABLE_TAB
			curcol = (curcol/tab + 1)*tab - 1;
#else
			curcol |= 0x07;
#endif
			if (curcol >= ncol -2) {
				curcol = -1;
				--lines;
			}
		} else if (ISCTRL(c) != FALSE)
			++curcol;
		++curcol;
		if (curcol >= ncol-1) {
#ifdef	KANJI
			if (c1==1) curcol=1;
			else curcol=0;
#else	/* not KANJI */
			curcol=0;
#endif	/* KANJI */
			--lines;
		}
	}
	if (lines == 0) return i - curcol;
	ewprintf("Bug: skipline %d lines left",lines);
	return FALSE;
}
/* Count number of displayed lines on tty.
 * the line which is longer than ncol
 * returns value more than 2 lines
 */
int
countlines(lp)
LINE	*lp;
{
	register int	i;
	register int	curcol;
	register int	lines;
	register char	c;
	register int	c1;
#ifdef  VARIABLE_TAB
	int	tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

	c1=0;
	curcol = 0;
	lines=0;
	for (i=0; i<llength(lp); ++i) {
		c = lgetc(lp, i);
#ifdef	KANJI
		if ( ISKANJI(c) ) {
		    if (c1==1) c1=0;
		    else c1=1;
		} else c1=0;
#endif	/* KANJI */
#ifdef	HANKANA
		if ( ISHANKANA(c) && c1 == 1 )
		    --curcol; 
#endif	/* HANKANA */
		if (c == '\t' && !(curbp->b_flag & BFNOTAB) ) { 
#ifdef	VARIABLE_TAB
			curcol = (curcol/tab + 1)*tab - 1;
#else
			curcol |= 0x07;
#endif
			if (curcol >= ncol -2) {
				curcol = -1;
				lines++;
			}
		} else if (ISCTRL(c) != FALSE)
			++curcol;
		++curcol;
		if (curcol >= ncol-1) {
#ifdef	KANJI
			if (c1==1) curcol=1;
			else curcol=0;
#else	/* not KANJI */
			curcol=0;
#endif	/* KANJI */
			++lines;
		}
	}
	return lines+1;
}

#if 0
static VOID
vtpute(c)
int c;
{
    register VIDEO	*vp;

    vp = vscreen[vtrow];

    if (vtcol >= ncol) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (vtkattr == K2ND) {
			vp->v_text[ncol - 2] = '$';
		}
#endif	/* KANJI */
		vp->v_text[ncol - 1] = '$';
    } else if (c == '\t' && !(curbp->b_flag & BFNOTAB) ) {
	do {
	    vtpute(' ');
	}
#ifdef	TABWIDTH	/* 95.08.29 by M.Suzuki	*/
	while (((vtcol + lbound)%tabWidth) != 0 && vtcol < ncol);
#else
	while (((vtcol + lbound)&0x07) != 0 && vtcol < ncol);
#endif	/* TABWIDTH */
    } else if (ISCTRL(c) != FALSE) {
	vtpute('^');
	vtpute(CCHR(c));
    } else {
	if (vtcol >= 0) vp->v_text[vtcol] = c;
	++vtcol;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	if (vtkattr == K1ST) {
		vtkattr = K2ND;
	} else if (ISKANJI(c)) { /* FIXME : unsupported kana */
		vtkattr = K1ST;
	} else {
		vtkattr = ASCII;
	}
	if (vtcol == 1 && vtkattr == K1ST) {
		vtcol--;
		lbound++;
	}
#endif	/* KANJI */
    }
}
#endif

/*
 * Make sure that the display is
 * right. This is a three part process. First,
 * scan through all of the windows looking for dirty
 * ones. Check the framing, and refresh the screen.
 * Second, make sure that "currow" and "curcol" are
 * correct for the current window. Third, make the
 * virtual and physical screens the same.
 */
/* Changed 98/01/23 by amura for Variable Tab handling */
VOID
update()
{
    register LINE	*lp;
    WINDOW		*old_curwp;
    register WINDOW	*wp;
    register VIDEO	*vp1;
    VIDEO		*vp2;
    register int	i;
    register int	j;
    register int	hflag;
    register int	currow;
    register int	curcol;
    register int	offs;
    register int	size;
    int	x,y;
    int	lines;
    VOID traceback ();
    VOID uline ();
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    extern	int	no_k2nd;	/* Defined at kbd.c */
#endif	/* KANJI */

    if (typeahead()) return;
    if (sgarbf) {			/* must update everything */
	wp = wheadp;
	while(wp != NULL) {
	    wp->w_flag |= WFMODE | WFHARD;
	    wp = wp->w_wndp;
	}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	no_k2nd = FALSE;		/* Reset KANJI input condition. */
    } else if (no_k2nd) {		/* We don't have KANJI 2nd byte. */
	return;
#endif	/* KANJI */
    }

    old_curwp = curwp;
    hflag = FALSE;			/* Not hard.		*/
    wp = wheadp;
    while (wp != NULL) {
	if (wp == curwp && wp->w_flag == 0) {
	    lp = wp->w_linep;		/* Cursor location.	*/
	    i = -wp->w_lines;
	    while (lp != wp->w_dotp) {
		i += countlines(lp);
		lp = lforw(lp);
	    }
	    i += colrow(lp, wp->w_doto, &x, &y);
	    if ( i < 0 || i >= wp->w_ntrows )
		wp->w_flag |= WFFORCE;
	}
	if (wp->w_flag != 0) {		/* Need update.		*/
	    if (wp->w_dotlines != countlines(wp->w_dotp))
		wp->w_flag |= WFHARD;
	    if ((wp->w_flag&WFFORCE) == 0) {
		lp = wp->w_linep;
		for (i = -wp->w_lines; i < wp->w_ntrows &&
			lp != wp->w_bufp->b_linep; ) {
		    if (lp == wp->w_dotp) {
			i += colrow(lp, wp->w_doto, &x, &y);
			if (i>=0 && i<wp->w_ntrows)
			    goto out;
			else	break;
		    }
		    i += countlines(lp);
		    lp = lforw(lp);
		}
	    }
	    i = wp->w_force;		/* Reframe this one.	*/
	    if (i > 0) {
		--i;
		if (i >= wp->w_ntrows)
		    i = wp->w_ntrows-1;
	    } else if (i < 0) {
		i += wp->w_ntrows;
		if (i < 0)
		    i = 0;
	    } else
		i = wp->w_ntrows/2;
	    lp = wp->w_dotp;
	    i -= colrow(lp, wp->w_doto, &x, &y);
	    while (i>0 && lback(lp)!=wp->w_bufp->b_linep) {
		lp = lback(lp);
		i -= countlines(lp);
	    }
	    if (i>0) i=0;
	    if (i<0) i = -i;
	    wp->w_linep = lp;
	    wp->w_lines = i;
	    wp->w_flag |= WFHARD;	/* Force full.		*/
out:
	    lp = wp->w_linep;		/* Try reduced update.	*/
	    i  = wp->w_toprow - wp->w_lines;
	    if ((wp->w_flag&~WFMODE) == WFEDIT) {
		while (lp != wp->w_dotp) {
		    i += countlines(lp);
		    lp = lforw(lp);
		}
		j = i + countlines(lp);
		if (j > wp->w_toprow + wp->w_ntrows) {
		    y = skipline(lp, wp->w_toprow + wp->w_ntrows - i);
		    j = wp->w_toprow + wp->w_ntrows;
		} else	y = llength(lp);
		if (i < wp->w_toprow) {
		    x = skipline(lp, wp->w_toprow - i);
		    i = wp->w_toprow;
		} else	x = 0;
		for (lines=i; lines<j; lines++) {
		    vscreen[lines]->v_color = CTEXT;
		    vscreen[lines]->v_flag |= (VFCHG|VFHBAD);
		}
		vtmove(i, 0);
		for (j=x; j<y; ++j)
		    vtputc(lgetc(lp, j));
		if ( y < llength(lp) ) vtmarkyen('\\');
		else	vteeol();
	    } else if ((wp->w_flag&(WFEDIT|WFHARD)) != 0) {
		hflag = TRUE;
		while (i < wp->w_toprow + wp->w_ntrows) {
		    if (lp == wp->w_bufp->b_linep) {
			vtmove(i, 0);
			vscreen[i]->v_color =CTEXT;
			vscreen[i]->v_flag |= (VFCHG|VFHBAD);
			vteeol();
			i++;
			continue;
		    }
		    j = i + countlines(lp);
		    if (j > wp->w_toprow + wp->w_ntrows){
			y = skipline(lp, wp->w_toprow + wp->w_ntrows - i);
			j = wp->w_toprow + wp->w_ntrows;
		    } else	y = llength(lp);
		    if (i < wp->w_toprow) {
			x = skipline(lp, wp->w_toprow - i);
			i = wp->w_toprow;
		    } else	x = 0;
		    for (lines=i; lines<j; lines++) {
			vscreen[lines]->v_color =CTEXT;
			vscreen[lines]->v_flag |= (VFCHG|VFHBAD);
		    }
		    vtmove(i, 0);
		    curwp = wp;		/* for variable tab */
		    for (j=x; j<y; ++j)
			vtputc(lgetc(lp, j));
		    curwp = old_curwp;
		    if ( y < llength(lp) ) vtmarkyen('\\');
		    else	vteeol();
		    i = lines;
		    lp = lforw(lp);
		}
		/* Because vtputc clear modeline, rewrite it */
		if (vtrow >= wp->w_toprow + wp->w_ntrows)
		    wp->w_flag |= WFMODE;
	    }
	    if ((wp->w_flag&WFMODE) != 0)
		modeline(wp);
#ifdef ADDFUNC
	    else if ((wp->w_flag & WFHARD) != 0)
		/* to display the ratio in mode line */
		moderatio(wp);
#endif
	    wp->w_flag  = 0;
	    wp->w_force = 0;
	    wp->w_dotlines = countlines(wp->w_dotp);
	    }
	wp = wp->w_wndp;
    }
    lp = curwp->w_linep;			/* Cursor location.	*/
    currow = curwp->w_toprow - curwp->w_lines;
    while (lp != curwp->w_dotp) {
	currow += countlines(lp);
	lp = lforw(lp);
    }
    currow += colrow(lp, curwp->w_doto, &x, &y);
    curcol = x;
    /* make sure no lines need to be de-extended because the cursor is
	no longer on them */

    wp = wheadp;

    while (wp != NULL) {
        /* if garbaged then fix up mode lines */
	if (sgarbf != FALSE)
	    vscreen[wp->w_toprow+wp->w_ntrows]->v_flag
							|= VFCHG;
	/* and onward to the next window */
	wp = wp->w_wndp;
    }

    if (sgarbf != FALSE) {		/* Screen is garbage.	*/
	sgarbf = FALSE;			/* Erase-page clears	*/
	epresf = FALSE;			/* the message area.	*/
	tttop  = HUGE;			/* Forget where you set */
	ttbot  = HUGE;			/* scroll region.	*/
	tthue  = CNONE;			/* Color unknown.	*/
	ttmove(0, 0);
	tteeop();
	for (i=0; i<nrow-1; ++i) {
	    uline(i, vscreen[i], blanks);
	    ucopy(vscreen[i], pscreen[i]);
	}
	ttmove(currow, curcol);
	ttflush();
#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
	ttykeypadstart();
#endif /* XKEYS */
	return;
    }
#ifdef	GOSLING
    if (hflag != FALSE) {		/* Hard update?		*/
	for (i=0; i<nrow-1; ++i) {	/* Compute hash data.	*/
	    hash(vscreen[i]);
	    hash(pscreen[i]);
	}
	offs = 0;			/* Get top match.	*/
	while (offs != nrow-1) {
	    vp1 = vscreen[offs];
	    vp2 = pscreen[offs];
	    if (vp1->v_color != vp2->v_color
		    ||  vp1->v_hash != vp2->v_hash)
		break;
	    uline(offs, vp1, vp2);
	    ucopy(vp1, vp2);
	    ++offs;
	}
	if (offs == nrow-1) {		/* Might get it all.	*/
	    ttmove(currow, curcol);
	    ttflush();
	    return;
	}
	size = nrow-1;			/* Get bottom match.	*/
	while (size != offs) {
	    vp1 = vscreen[size-1];
	    vp2 = pscreen[size-1];
	    if (vp1->v_color != vp2->v_color
		||  vp1->v_hash	 != vp2->v_hash)
		break;
	    uline(size-1, vp1, vp2);
	    ucopy(vp1, vp2);
	    --size;
	}
	if ((size -= offs) == 0)	/* Get screen size.	*/
	    panic("Illegal screen size in update");
	setscores(offs, size);		/* Do hard update.	*/
	traceback(offs, size, size, size);
	for (i=0; i<size; ++i)
	    ucopy(vscreen[offs+i], pscreen[offs+i]);
	ttmove(currow, curcol);
	ttflush();
	return;
    }
#endif
    for (i=0; i<nrow-1; ++i) {		/* Easy update.		*/
	vp1 = vscreen[i];
	vp2 = pscreen[i];
	if ((vp1->v_flag&VFCHG) != 0) {
	    uline(i, vp1, vp2);
	    ucopy(vp1, vp2);
	}
    }
    ttmove(currow, curcol);
    ttflush();
}

/*
 * Update a saved copy of a line,
 * kept in a VIDEO structure. The "vvp" is
 * the one in the "vscreen". The "pvp" is the one
 * in the "pscreen". This is called to make the
 * virtual and physical screens the same when
 * display has done an update.
 */
static VOID
ucopy(vvp, pvp) register VIDEO *vvp; register VIDEO *pvp; {

	vvp->v_flag &= ~VFCHG;			/* Changes done.	*/
	pvp->v_flag  = vvp->v_flag;		/* Update model.	*/
	pvp->v_hash  = vvp->v_hash;
	pvp->v_cost  = vvp->v_cost;
	pvp->v_color = vvp->v_color;
	bcopy(vvp->v_text, pvp->v_text, ncol);
#ifdef SS_SUPPORT
	bcopy(&vvp->v_sub(0), &pvp->v_sub(0), ncol);
#endif
}

#if 0
/* updext: update the extended line which the cursor is currently
 * on at a column greater than the terminal width. The line
 * will be scrolled right or left to let the user see where
 * the cursor is
 */
static VOID
updext(currow, curcol)
int currow, curcol;
{
    register LINE *lp;			/* pointer to current line */
    register int j;			/* index into line */

    /* calculate what column the left bound should be */
    /* (force cursor into middle half of screen) */
    lbound = curcol - (curcol % (ncol>>1)) - (ncol>>2);
    /* scan through the line outputing characters to the virtual screen */
    /* once we reach the left edge */
    vtmove(currow, -lbound);			/* start scanning offscreen */
    lp = curwp->w_dotp;				/* line to output */
    for (j=0; j<llength(lp); ++j)		/* until the end-of-line */
	vtpute(lgetc(lp, j));
    vteeol();					/* truncate the virtual line */
    vscreen[currow]->v_text[0] = '$';		/* and put a '$' in column 1 */
}
#endif

/*
 * Update a single line. This routine only
 * uses basic functionality (no insert and delete character,
 * but erase to end of line). The "vvp" points at the VIDEO
 * structure for the line on the virtual screen, and the "pvp"
 * is the same for the physical screen. Avoid erase to end of
 * line when updating CMODE color lines, because of the way that
 * reverse video works on most terminals.
 */
static VOID uline(row, vvp, pvp) VIDEO *vvp; VIDEO *pvp; {
#ifdef	MEMMAP
	ttflush();	/* 90.06.09  by A.Shirahashi */
#ifdef	SS_SUPPORT  /* 92.11.21  by S.Sasaki */
	putline(row+1, 1, &vvp->v_text[0], &vvp->v_sub(0), vvp->v_color);
#else
	putline(row+1, 1, &vvp->v_text[0], vvp->v_color);
#endif
#else   /* not MEMMAP */
	register char	*cp1;
	register char	*cp2;
	register char	*cp3;
	char		*cp4;
	char		*cp5;
	register int	nbflag;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	register int	kselect = FALSE;
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
	char	*ccp1, *ccp2, *ccp3, *ccp4;
#endif	/* SS_SUPPORT */
#endif	/* KANJI */

	if (vvp->v_color != pvp->v_color) {	/* Wrong color, do a	*/
		ttmove(row, 0);			/* full redraw.		*/
#ifdef	STANDOUT_GLITCH
		if (pvp->v_color != CTEXT && SG >= 0) tteeol();
#endif
		ttcolor(vvp->v_color);
#ifdef	STANDOUT_GLITCH
		cp1 = &vvp->v_text[SG > 0 ? SG : 0];
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		ccp1 = &vvp->v_sub(SG > 0 ? SG : 0);
#endif	/* SS_SUPPORT */
		/* the odd code for SG==0 is to avoid putting the invisable
		 * glitch character on the next line.
		 * (Hazeltine executive 80 model 30)
		 */
		cp2 = &vvp->v_text[ncol - (SG >= 0 ? (SG!=0 ? SG : 1) : 0)];
#else	/* not STANDOUT_GLITCH */
		cp1 = &vvp->v_text[0];
		cp2 = &vvp->v_text[ncol];
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		ccp1 = &vvp->v_sub(0);
#endif	/* SS_SUPPORT */
#endif	/* STANDOUT_GLITCH */
		while (cp1 != cp2) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
			kttputc(*cp1);
#ifdef	HANKANA  /* 92.11.21  by S.Sasaki */
			if ( ISHANKANA(*cp1) && *ccp1 != 0)
			    kttputc(*ccp1);
#endif
#ifdef	HOJO_KANJI
			if ( ISHOJO(cp1[0]) && ISHOJO(cp1[1]) &&
			    (cp1+1) != cp2 && ccp1[0] != 0 && ccp1[1] != 0) {
			    kttputc(*ccp1);
			    ++ccp1; ++cp1; ++ttcol;
			    kttputc(*ccp1);
			}
#endif
#ifdef	SS_SUPPORT
			++ccp1;
#endif	/* SS_SUPPORT */
			++cp1;
#else	/* NOT KANJI */
			ttputc(*cp1++);
#endif	/* KANJI */
			++ttcol;
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		kdselectcode(FALSE);
#endif	/* KANJI */
#ifndef MOVE_STANDOUT
		ttcolor(CTEXT);
#endif
	        ttflush(); /* 90.06.09  by A.Shirahashi */
		return;
	}
	cp1 = &vvp->v_text[0];			/* Compute left match.	*/
	cp2 = &pvp->v_text[0];
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
	ccp1 = &vvp->v_sub(0);
	ccp2 = &pvp->v_sub(0);
#endif  /* SS_SUPPORT */
	while (cp1!=&vvp->v_text[ncol] && cp1[0]==cp2[0]) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (!ISKANJI(cp1[0])) {		/* Same ASCII char. */
			++cp1;
			++cp2;
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
			++ccp1;
			++ccp2;
#endif  /* SS_SUPPORT */
#ifdef	SS_SUPPORT  /* 92.11.21  by S.Sasaki */
		} else if ((
#ifdef	HANKANA
			   ISHANKANA(cp1[0]) ||
#endif
#ifdef	HOJO_KANJI
			   ISHOJO(cp1[0]) ||
#endif
			   0) && ccp1[0] != 0 ) {
		    if ( ccp1[0]==ccp2[0] ) {
			++cp1;
			++ccp1;
			++cp2;
			++ccp2;
		    } else break;
#endif	/* SS_SUPPORT */
		} else if (cp1[1] == cp2[1]) {	/* Same KANJI char. */
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
			ccp1 += 2;
			ccp2 += 2;
#endif	/* SS_SUPPORT */
			cp1 += 2;
			cp2 += 2;
		} else {			/* Not same KANJI char. */
			break;
		}
#else	/* NOT KANJI */
		++cp1;
		++cp2;
#endif	/* KANJI */
	}
	if (cp1 == &vvp->v_text[ncol]) {	/* All equal.		*/
	        ttflush(); /* 90.06.09  by A.Shirahashi */
		return;
        }
	nbflag = FALSE;
	cp3 = &vvp->v_text[ncol];		/* Compute right match. */
	cp4 = &pvp->v_text[ncol];
#ifdef	SS_SUPPORT	/* 1999.09.07 by M.Suzuki */
	ccp3 = cp3;
	ccp4 = cp4;
#endif

	while (cp3[-1] == cp4[-1]) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		if (!ISKANJI(cp3[-1])) {	/* Same ASCII char. */
			--cp3;
			--cp4;
			kselect = FALSE;
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		} else if ((
#ifdef	HANKANA
			   ISHANKANA(cp3[-1]) ||
#endif
#ifdef	HOJO_KANJI
			   ISHOJO(cp3[-1]) ||
#endif
			   0) && ccp3[-1] != 0 ) {
		    if (ccp3[-1] == ccp4[-1]) {
			cp3--;
			ccp3--;
			cp4--;
			ccp4--;
		    } else break;
#endif  /* SS_SUPPORT */
		} else if (cp3[-2] == cp4[-2]) { /* Same KANJI char. */
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
			ccp3 -= 2;
			ccp4 -= 2;
#endif  /* SS_SUPPORT */
			cp3 -= 2;
			cp4 -= 2;
			kselect = TRUE;
		} else {			/* Not same KANJI char. */
			break;
		}
#else	/* NOT KANJI */
		--cp3;
		--cp4;
#endif	/* KANJI */
		if (cp3[0] != ' ')		/* Note non-blanks in	*/
			nbflag = TRUE;		/* the right match.	*/
	}
	cp5 = cp3;				/* Is erase good?	*/
	if (nbflag==FALSE && vvp->v_color==CTEXT) {
		while (cp5!=cp1 && cp5[-1]==' ')
			--cp5;
		/* Alcyon hack */
		if ((int)(cp3-cp5) <= tceeol)
			cp5 = cp3;
	}
	/* Alcyon hack */
	ttmove(row, (int)(cp1-&vvp->v_text[0]));
#ifdef	STANDOUT_GLITCH
	if (vvp->v_color != CTEXT && SG > 0) {
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		if(cp1 < &vvp->v_text[SG]) {
			cp1 = &vvp->v_text[SG];
			ccp1 = &vvp->v_sub(SG);
		}
#else	/* not SS_SUPPORT */
		if(cp1 < &vvp->v_text[SG]) cp1 = &vvp->v_text[SG];
#endif  /* SS_SUPPORT */
		if(cp5 > &vvp->v_text[ncol-SG]) cp5 = &vvp->v_text[ncol-SG];
	} else if (SG < 0)
#endif
		ttcolor(vvp->v_color);
	while (cp1 != cp5) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		kttputc(*cp1);
#ifdef	HANKANA /* 92.11.21  by S.Sasaki */
		if (ISHANKANA(*cp1) && *ccp1 != 0)
		    kttputc(*ccp1);
#endif	/* HANKANA */
#ifdef	HOJO_KANJI
		if ( ISHOJO(cp1[0]) && ISHOJO(cp1[1]) && (cp1+1) != cp5 &&
		     ccp1[0] != 0 && ccp1[1] != 0) {
		    kttputc(*ccp1);
		    ++ccp1; ++cp1; ++ttcol;
		    kttputc(*ccp1);
		}
#endif
#ifdef	SS_SUPPORT
		++ccp1;
#endif	/* SS_SUPPORT */
		++cp1;
#else	/* NOT KANJI */
		ttputc(*cp1++);
#endif	/* KANJI */
		++ttcol;
	}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	kdselectcode(kselect);
#endif	/* KANJI */
	if (cp5 != cp3)				/* Do erase.		*/
		tteeol();
#ifndef MOVE_STANDOUT	/* 90.03.21  by S.Yoshida */
	ttcolor(CTEXT);
#endif
        ttflush(); /* 90.06.09  by A.Shirahashi */
#endif  /* MEMMAP */
}

/*
 * Redisplay the mode line for
 * the window pointed to by the "wp".
 * This is the only routine that has any idea
 * of how the modeline is formatted. You can
 * change the modeline format by hacking at
 * this routine. Called by "update" any time
 * there is a dirty window.
 * Note that if STANDOUT_GLITCH is defined, first and last SG characters
 * may never be seen.
 */
static VOID
modeline(wp) register WINDOW *wp; {
	register int	n;
	register BUFFER *bp;
	int	mode;
#ifdef CANNA
	extern char currentMode[];
	extern char origMode[];
#endif

	n = wp->w_toprow+wp->w_ntrows;		/* Location.		*/
	vscreen[n]->v_color = CMODE;		/* Mode line color.	*/
	vscreen[n]->v_flag |= (VFCHG|VFHBAD);	/* Recompute, display.	*/
	vtmove(n, 0);				/* Seek to right line.	*/
	bp = wp->w_bufp;
#ifdef CANNA
	if(bp->b_flag & BFCANNA){
		if(bp == curbp) n += vtputs(currentMode);
		else n += vtputs(origMode);
	}
	else n += vtputs("[ -- ]");
#endif
	vtputc('-'); vtputc('-');
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	if ((bp->b_flag&BFRONLY) != 0) {	/* "%" if read-only.	*/
		vtputc('%'); vtputc('%');
	} else
#endif	/* READONLY */
	if ((bp->b_flag&BFCHG) != 0) {		/* "*" if changed.	*/
		vtputc('*'); vtputc('*');
	} else {
		vtputc('-'); vtputc('-');
	}
	vtputc('-');
	n  = 5;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	n += vtputs("Ng: ");
#else	/* NOT KANJI */
	n += vtputs("Mg: ");
#endif	/* KANJI */
	if (bp->b_bname[0] != '\0')
		n += vtputs(&(bp->b_bname[0]));
	while (n < 42) {			/* Pad out with blanks	*/
		vtputc(' ');
		++n;
	}
	vtputc('(');
	++n;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		/* 90.12.28  Move to here like as Nemacs 3.3. by S.Yoshida */
	n += kdispbufcode(bp);			/* Show KANJI code.	*/
#endif	/* KANJI */	
	for (mode=0;;) {
	    n += vtputs(bp->b_modes[mode]->p_name);
	    if(++mode > bp->b_nmodes) break;
	    vtputc('-');
	    ++n;
	}
	vtputc(')');
#ifdef	ADDFUNC
	vtputc('-');
	vtputc('-');
	moderatio(wp);
#endif
	vtmarkyen('-');				/* Pad out.		*/
}

/*
 * Redisplay the mode line  "raito" ONLY for
 * the window pointed to by the "wp".
 * This is the only routine that has any idea
 * of how the modeline is formatted. You can
 * change the modeline format by hacking at
 * this routine. Called by "update" any time
 * there is a dirty window.
 * Note that if STANDOUT_GLITCH is defined, first and last SG characters
 * may never be seen.
 */
static VOID
moderatio(wp) register WINDOW *wp; {
	register int	n;
	register BUFFER *bp;
	int	mode, l;
	extern int line_number_mode;		/* Defined in basic.c	*/

	l = wp->w_toprow+wp->w_ntrows;		/* Location.		*/
	vscreen[l]->v_color = CMODE;		/* Mode line color.	*/
	vscreen[l]->v_flag |= (VFCHG|VFHBAD);	/* Recompute, display.	*/
	bp = wp->w_bufp;
#ifdef	CANNA
	n  = 46 + 6;
#else
	n  = 46;
#endif
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	/* 90.12.28  Move to here like as Nemacs 3.3. by S.Yoshida */
	n += 4;					/* Show KANJI code.	*/
#endif	/* KANJI */	
	for (mode=0;;) {	/* skip mode names */
	    n += strlen(bp->b_modes[mode]->p_name);
	    if (++mode > bp->b_nmodes) break;
	    ++n;
	}
	vtmove(l, n);				/* Seek to right line.	*/
        if (line_number_mode) {
	    extern int get_lineno pro((BUFFER*, LINE*));
	    char linestr[NINPUT];	/* XXX now, support only 32bit int */
	    sprintf(linestr, "L%d", get_lineno(bp, wp->w_dotp)+1);
	    vtputs(linestr);
	}
	{
            extern int windowpos pro((WINDOW *));
	    int ratio = windowpos(wp);

	    vtputc('-'); vtputc('-');
            switch (ratio) {
	    case MG_RATIO_ALL:
	      vtputs("All");
	      break;
	      
	    case MG_RATIO_TOP:
	      vtputs("Top");
	      break;
	      
	    case MG_RATIO_BOT:
	      vtputs("Bot");
	      break;
	      
	    default:
	      if ((ratio / 10) == 0) {
		vtputc(' ');
	      }
	      else {
		vtputc('0' + (ratio / 10));
	      }
	      vtputc('0' + (ratio % 10));
	      vtputc('%');
	    }
            vtputc('-');vtputc('-');
	}
}

/*
 * output a string to the mode line, report how long it was.
 */
vtputs(s) register char *s; {
	register int n = 0;

	while (*s != '\0') {
		vtputc(*s++);
		++n;
	}
	return n;
}
#ifdef	GOSLING
/*
 * Compute the hash code for
 * the line pointed to by the "vp". Recompute
 * it if necessary. Also set the approximate redisplay
 * cost. The validity of the hash code is marked by
 * a flag bit. The cost understand the advantages
 * of erase to end of line. Tuned for the VAX
 * by Bob McNamara; better than it used to be on
 * just about any machine.
 */
static VOID
hash(vp) register VIDEO *vp; {
	register int	i;
	register int	n;
	register char	*s;

	if ((vp->v_flag&VFHBAD) != 0) {		/* Hash bad.		*/
		s = &vp->v_text[ncol-1];
		for (i=ncol; i!=0; --i, --s)
			if (*s != ' ')
				break;
		n = ncol-i;			/* Erase cheaper?	*/
		if (n > tceeol)
			n = tceeol;
		vp->v_cost = i+n;		/* Bytes + blanks.	*/
		for (n=0; i!=0; --i, --s)
			n = (n<<5) + n + *s;
		vp->v_hash = n;			/* Hash code.		*/
		vp->v_flag &= ~VFHBAD;		/* Flag as all done.	*/
	}
}

/*
 * Compute the Insert-Delete
 * cost matrix. The dynamic programming algorithm
 * described by James Gosling is used. This code assumes
 * that the line above the echo line is the last line involved
 * in the scroll region. This is easy to arrange on the VT100
 * because of the scrolling region. The "offs" is the origin 0
 * offset of the first row in the virtual/physical screen that
 * is being updated; the "size" is the length of the chunk of
 * screen being updated. For a full screen update, use offs=0
 * and size=nrow-1.
 *
 * Older versions of this code implemented the score matrix by
 * a two dimensional array of SCORE nodes. This put all kinds of
 * multiply instructions in the code! This version is written to
 * use a linear array and pointers, and contains no multiplication
 * at all. The code has been carefully looked at on the VAX, with
 * only marginal checking on other machines for efficiency. In
 * fact, this has been tuned twice! Bob McNamara tuned it even
 * more for the VAX, which is a big issue for him because of
 * the 66 line X displays.
 *
 * On some machines, replacing the "for (i=1; i<=size; ++i)" with
 * i = 1; do { } while (++i <=size)" will make the code quite a
 * bit better; but it looks ugly.
 */
static VOID
setscores(offs, size) {
	register SCORE	*sp;
	SCORE		*sp1;
	register int	tempcost;
	register int	bestcost;
	register int	j;
	register int	i;
	register VIDEO	**vp;
	VIDEO		**pp, **vbase, **pbase;

	vbase = &vscreen[offs-1];		/* By hand CSE's.	*/
	pbase = &pscreen[offs-1];
	score[0].s_itrace = 0;			/* [0, 0]		*/
	score[0].s_jtrace = 0;
	score[0].s_cost	  = 0;
	sp = &score[1];				/* Row 0, inserts.	*/
	tempcost = 0;
	vp = &vbase[1];
	for (j=1; j<=size; ++j) {
		sp->s_itrace = 0;
		sp->s_jtrace = j-1;
		tempcost += tcinsl;
		tempcost += (*vp)->v_cost;
		sp->s_cost = tempcost;
		++vp;
		++sp;
	}
	sp = &score[vnrow];			/* Column 0, deletes.	*/
	tempcost = 0;
	for (i=1; i<=size; ++i) {
		sp->s_itrace = i-1;
		sp->s_jtrace = 0;
		tempcost  += tcdell;
		sp->s_cost = tempcost;
		sp += vnrow;
	}
	sp1 = &score[vnrow+1];			/* [1, 1].		*/
	pp = &pbase[1];
	for (i=1; i<=size; ++i) {
		sp = sp1;
		vp = &vbase[1];
		for (j=1; j<=size; ++j) {
			sp->s_itrace = i-1;
			sp->s_jtrace = j;
			bestcost = (sp-vnrow)->s_cost;
			if (j != size)		/* Cd(A[i])=0 @ Dis.	*/
				bestcost += tcdell;
			tempcost = (sp-1)->s_cost;
			tempcost += (*vp)->v_cost;
			if (i != size)		/* Ci(B[j])=0 @ Dsj.	*/
				tempcost += tcinsl;
			if (tempcost < bestcost) {
				sp->s_itrace = i;
				sp->s_jtrace = j-1;
				bestcost = tempcost;
			}
			tempcost = (sp-vnrow-1)->s_cost;
			if ((*pp)->v_color != (*vp)->v_color
			||  (*pp)->v_hash  != (*vp)->v_hash)
				tempcost += (*vp)->v_cost;
			if (tempcost < bestcost) {
				sp->s_itrace = i-1;
				sp->s_jtrace = j-1;
				bestcost = tempcost;
			}
			sp->s_cost = bestcost;
			++sp;			/* Next column.		*/
			++vp;
		}
		++pp;
		sp1 += vnrow;			/* Next row.		*/
	}
}

/*
 * Trace back through the dynamic programming cost
 * matrix, and update the screen using an optimal sequence
 * of redraws, insert lines, and delete lines. The "offs" is
 * the origin 0 offset of the chunk of the screen we are about to
 * update. The "i" and "j" are always started in the lower right
 * corner of the matrix, and imply the size of the screen.
 * A full screen traceback is called with offs=0 and i=j=nrow-1.
 * There is some do-it-yourself double subscripting here,
 * which is acceptable because this routine is much less compute
 * intensive then the code that builds the score matrix!
 */
static VOID traceback(offs, size, i, j) {
	register int	itrace;
	register int	jtrace;
	register int	k;
	register int	ninsl;
	register int	ndraw;
	register int	ndell;

	if (i==0 && j==0)			/* End of update.	*/
		return;
	itrace = score[(vnrow*i) + j].s_itrace;
	jtrace = score[(vnrow*i) + j].s_jtrace;
	if (itrace == i) {			/* [i, j-1]		*/
		ninsl = 0;			/* Collect inserts.	*/
		if (i != size)
			ninsl = 1;
		ndraw = 1;
		while (itrace!=0 || jtrace!=0) {
			if (score[(vnrow*itrace) + jtrace].s_itrace != itrace)
				break;
			jtrace = score[(vnrow*itrace) + jtrace].s_jtrace;
			if (i != size)
				++ninsl;
			++ndraw;
		}
		traceback(offs, size, itrace, jtrace);
		if (ninsl != 0) {
			ttcolor(CTEXT);
			ttinsl(offs+j-ninsl, offs+size-1, ninsl);
		}
		do {				/* B[j], A[j] blank.	*/
			k = offs+j-ndraw;
			uline(k, vscreen[k], blanks);
		} while (--ndraw);
		return;
	}
	if (jtrace == j) {			/* [i-1, j]		*/
		ndell = 0;			/* Collect deletes.	*/
		if (j != size)
			ndell = 1;
		while (itrace!=0 || jtrace!=0) {
			if (score[(nrow*itrace) + jtrace].s_jtrace != jtrace)
				break;
			itrace = score[(vnrow*itrace) + jtrace].s_itrace;
			if (j != size)
				++ndell;
		}
		if (ndell != 0) {
			ttcolor(CTEXT);
			ttdell(offs+i-ndell, offs+size-1, ndell);
		}
		traceback(offs, size, itrace, jtrace);
		return;
	}
	traceback(offs, size, itrace, jtrace);
	k = offs+j-1;
	uline(k, vscreen[k], pscreen[offs+i-1]);
}
#endif
