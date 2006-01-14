/* $Id: putline.c,v 1.6.2.1 2006/01/14 22:47:48 amura Exp $ */
/*
  VRAM direct write routine for PC9801
  written by A.Shirahashi, KEK
*/
/*
   90.03.31	Modified by A.Shirahashi, support for 'SO' mode line
   90.03.28	Created by A.Shirahashi 
*/

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#ifdef	PC9801	/* 90.03.30  by S.Yoshida ("#ifdef" line only) */
#include <dos.h>

#ifndef	__TURBOC__	/* 90.04.06  by S.Yoshida */
#define MK_FP(seg,ofs)	((void far *) \
			   (((unsigned long)(seg) << 16) | (unsigned)(ofs)))
#define poke(a,b,c)	(*((int  far*)MK_FP((a),(b))) = (int)(c))
#define pokeb(a,b,c)	(*((char far*)MK_FP((a),(b))) = (char)(c))
#endif	/* __TURBOC__ */

#define	stoj(c1, c2)	do {			\
    if ((c1) >= 0xe0)				\
	(c1) -=  0x40;				\
    if ((c2) >= 0x9f) {				\
	(c1) = ((c1) - 0x88)*2 + 0x20;		\
	(c2) = ((c2) + 0x02)&0x7f;		\
    }						\
    else {					\
	if ((c2) >= 0x7f)			\
	    (c2) -= 0x01;			\
	(c1) = ((c1) - 0x89)*2 + 0x21;		\
	(c2) = ((c2) + 0x61)&0x7f;		\
    }						\
} while (/*CONSTCOND*/0)

#define VRAM_SEG	0xa000
#define ATTR_SEG	0xa200

#define C_BLUE		0x02
#define C_RED		0x04
#define C_PURPLE	0x06
#define C_GREEN		0x08
#define C_CYAN		0xa0
#define C_YELLOW	0xc0
#define C_WHITE		0xe0

#define A_NORMAL	0x01
#define A_REVERSE	0x05

#define CNONE		0		/* Unknown color.	*/
#define CTEXT		1		/* Text color.		*/
#define CMODE		2		/* Mode line color.	*/

#ifdef TERMCAP
extern char *SO;
#endif

VOID
putline(int row, const NG_WCHAR_t *s, int color)
{
    unsigned int dest;
    unsigned int c1, c2;
    int attr;

    dest = 160*row;
    if (color == CTEXT) {
        attr = C_WHITE|A_NORMAL;
    }
    else if( color == CMODE ) {
#ifndef TERMCAP
	attr = C_WHITE|A_REVERSE;
#else
    	switch (atoi(SO+2)) {
	case 7: attr = C_WHITE|A_REVERSE; break;
	case 17: case 31: attr = C_RED|A_NORMAL; break;
	case 18: case 34: attr = C_BLUE|A_NORMAL; break;
	case 19: case 35: attr = C_PURPLE|A_NORMAL; break;
	case 20: case 32: attr = C_GREEN|A_NORMAL; break;
	case 21: case 33: attr = C_YELLOW|A_NORMAL; break;
	case 22: case 36: attr = C_CYAN|A_NORMAL; break;
	case 23: case 37: attr = C_WHITE|A_NORMAL; break;
	case 41: attr = C_RED|A_REVERSE; break;
	case 44: attr = C_BLUE|A_REVERSE; break;
	case 45: attr = C_PURPLE|A_REVERSE; break;
	case 42: attr = C_GREEN|A_REVERSE; break;
	case 43: attr = C_YELLOW|A_REVERSE; break;
	case 46: attr = C_CYAN|A_REVERSE; break;
	case 47: attr = C_WHITE|A_REVERSE; break;
	default: attr = C_WHITE|A_REVERSE; break;
	}
#endif
    }
    
    while (*s && dest < 160 * row) {
	if (ISASCII(*s)) {
            pokeb(ATTR_SEG, dest, attr);
#ifdef BACKSLASH
            if (*s == '\\') {
                poke(VRAM_SEG, dest, 0xfc);
                ++s;
            }
	    else
#endif /* BACKSLASH */
                poke(VRAM_SEG, dest, *s++);
            dest += 2;
	}
	else {
	    unsigned char buf[2];
	    i = terminal_lang->lm_get_display_code(*s++, buf, 2);
	    if (i == 1) {
		pokeb(ATTR_SEG, dest, attr);
		poke(VRAM_SEG, dest, buf[0]);
		dest += 2;
	    }
	    else if (i == 2) {
		stoj(buf[0], buf[1]);
		pokeb(ATTR_SEG, dest, attr);
		pokeb(VRAM_SEG, dest++, buf[0] - 0x20);
		pokeb(VRAM_SEG, dest++, buf[1]);
		pokeb(ATTR_SEG, dest, attr);
		pokeb(VRAM_SEG, dest++, buf[0] - 0x20);
		pokeb(VRAM_SEG, dest++, buf[1] + 0x80);
	    }
	}
    }
}
#endif	/* PC9801 */
