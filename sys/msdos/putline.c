/* $Id $ */
/*
  VRAM direct write routine for PC9801
  written by A.Shirahashi, KEK
*/

/*
 * $Log: putline.c,v $
 * Revision 1.3  2001/03/09 15:53:14  amura
 * enable to really work autosave feature
 *
 * Revision 1.2  2001/01/20 15:47:23  amura
 * putline() support Hojo Kanji now
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */
/*
   90.03.31	Modified by A.Shirahashi, support for 'SO' mode line
   90.03.28	Created by A.Shirahashi 
*/

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"

#include <dos.h>

#ifndef	__TURBOC__	/* 90.04.06  by S.Yoshida */
#define MK_FP(seg,ofs)	((void far *) \
			   (((unsigned long)(seg) << 16) | (unsigned)(ofs)))

#define poke(a,b,c)	(*((int  far*)MK_FP((a),(b))) = (int)(c))
#define pokeb(a,b,c)	(*((char far*)MK_FP((a),(b))) = (char)(c))
#endif	/* __TURBOC__ */

#ifdef	PC9801	/* 90.03.30  by S.Yoshida ("#ifdef" line only) */
#define	iseuc1st(c)	((c) >= 0xa1 && (c) <= 0xfe)
#define	etoj(c1, c2)	{c1 &= 0x7f; c2 &= 0x7f;}

#define VRAM_SEG 0xa000
#define ATTR_SEG 0xa200

#define C_BLUE   0x02
#define C_RED    0x04
#define C_PURPLE 0x06
#define C_GREEN  0x08
#define C_CYAN   0xa0
#define C_YELLOW 0xc0
#define C_WHITE  0xe0

#define A_NORMAL 0x01
#define A_REVERSE 0x05

#define CNONE   0                       /* Unknown color.               */
#define CTEXT   1                       /* Text color.                  */
#define CMODE   2                       /* Mode line color.             */

#ifndef TCCONIO
extern char *SO;
#endif

#ifdef HOJO_KANJI
#include "kinit.h"	/* for TOUFU charactor */
#endif

VOID
#ifdef SS_SUPPORT  /* 92.11.21  by S.Sasaki */
putline(int row, int column, unsigned char *s, unsigned char *t, short color)
#else  /* not  SS_SUPPORT */
putline(int row, int column, unsigned char *s, short color)
#endif /* SS_SUPPORT */    
{
    unsigned int dest;
    unsigned int c1, c2;
    int attr;

    dest = 160 * (row - 1);
    if( color == CTEXT ) {
        attr = C_WHITE|A_NORMAL;
    } else if( color == CMODE ) {
#ifdef TCCONIO
	    attr = C_WHITE|A_REVERSE;
#else
    	switch( atoi(SO+2) ) {
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
    
    while( *s && dest < 160 * row) {
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
	if ( ISHANKANA(*s) ) {
            pokeb(ATTR_SEG, dest, attr);
            poke(VRAM_SEG, dest, *t++);
	    ++s;
	    dest += 2;
	} else
#endif /* HANKANA */    
#ifdef HOJO_KANJI
	if ( ISHOJO(*s) ) {
            pokeb(ATTR_SEG, dest, attr);
            pokeb(VRAM_SEG, dest++, TOUFU1ST - 0x20);
            pokeb(VRAM_SEG, dest++, TOUFU2ND);
            pokeb(ATTR_SEG, dest, attr);
            pokeb(VRAM_SEG, dest++, TOUFU1ST - 0x20);
            pokeb(VRAM_SEG, dest++, TOUFU2ND + 0x80);
	    s += 3;
	} else
#endif
        if( iseuc1st(*s) ) {
            c1 = *s++;
            c2 = *s++;
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
	    t += 2;
#endif /* HANKANA */    
            etoj(c1, c2);
            pokeb(ATTR_SEG, dest, attr);
            pokeb(VRAM_SEG, dest++, c1 - 0x20);
            pokeb(VRAM_SEG, dest++, c2);
            pokeb(ATTR_SEG, dest, attr);
            pokeb(VRAM_SEG, dest++, c1 - 0x20);
            pokeb(VRAM_SEG, dest++, c2 + 0x80);
        } else {
            pokeb(ATTR_SEG, dest, attr);
#ifdef BACKSLASH
            if (*s == '\\') {
                poke(VRAM_SEG, dest, 0xfc);
                ++s;
            } else
#endif /* BACKSLASH */
                poke(VRAM_SEG, dest, *s++);
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
	    ++t;
#endif /* HANKANA */    
            dest += 2;
        }
    }
}
#endif	/* PC9801 */
