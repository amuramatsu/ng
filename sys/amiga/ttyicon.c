/* $Id: ttyicon.c,v 1.5 2002/11/06 16:05:23 amura Exp $ */
/*
 * Name:	MG 2a
 *		Iconify the MG window using Leo Schwab's iconify() routine.
 * Last Edit:	07-Jan-88	mic@emx.utexas.edu
 * Created:	04-Jan-88	mic@emx.utexas.edu
 */

/*
 * $Log: ttyicon.c,v $
 * Revision 1.5  2002/11/06 16:05:23  amura
 * compile with newstyle source
 *
 * Revision 1.4  2001/11/23 11:56:45  amura
 * Rewrite all sources
 *
 * Revision 1.3  2000/09/29 17:25:15  amura
 * small patch for new iconify()
 *
 * Revision 1.2  2000/07/28 11:26:37  amura
 * edit icon to Ng 1.4
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 */

#include "config.h"	/* Dec.19,1992 Add by H.Ohkubo */
#ifdef	DO_ICONIFY

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include "iconify.h"
#undef	TRUE
#undef	FALSE
#include "def.h"
#ifdef INLINE_PRAGMAS
#include <pragmas/exec_pragmas.h>
#else
#include <clib/exec_protos.h>
#endif

extern struct SysBase *SysBase;

/*
 * Simple Mg 2a icon image.  We need a more imaginative one.
 */

#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
		/* 27 Jul 2000 rewrite for Ng 1.4 by amura */
static UWORD mg2a[] = {
/* Bit Plane #0 */
    0x3fff, 0xffff, 0xffff, 0xfffe,
    0x3fff, 0xffff, 0xffff, 0xfffe,
    0x3ffd, 0xefff, 0xffff, 0xfffe,
    0x3ff2, 0x97ff, 0xfeff, 0xffbe,
    0x3fe3, 0x2fff, 0xf17f, 0xf85e,
    0x3fe1, 0x3fbf, 0xc2ff, 0xf0be,
    0x3fc2, 0x605f, 0xc2ff, 0xe0be,
    0x3fc0, 0x445f, 0xc5ff, 0xc97e,
    0x3f80, 0x88bf, 0xc5ff, 0x917e,
    0x3f80, 0x88bf, 0x8bff, 0x13be,
    0x3f21, 0x117f, 0x8bfe, 0x00be,
    0x3f21, 0x117f, 0x17fc, 0x017e,
    0x3e62, 0x22ff, 0x17df, 0xc77e,
    0x3e53, 0x02fe, 0x2e2f, 0x8bfe,
    0x398f, 0xc402, 0x2227, 0x881e,
    0x3ffc, 0x0bff, 0xdfdf, 0xf7fe,
    0x3fff, 0xf7ff, 0xffff, 0xfffe,
    0x3fff, 0xffff, 0xffff, 0xfffe,
    0x3fff, 0xffff, 0xffff, 0xfffe,
    
/* Bit Plane #1 */
    
    0x0000, 0x0000, 0x0000, 0x0000,
    0x1fff, 0xffff, 0xffff, 0xfffc,
    0x1ff3, 0x9fff, 0xffff, 0xfffc,
    0x1fed, 0x6fff, 0xf1ff, 0xf87c,
    0x1fdc, 0xdfff, 0xceff, 0xf7bc,
    0x1fde, 0xc07f, 0xbdff, 0xef7c,
    0x1fbd, 0x9fbf, 0xbdff, 0xdf7c,
    0x1fbf, 0xbbbf, 0xbbff, 0xb6fc,
    0x1f7f, 0x777f, 0xbbff, 0x6efc,
    0x1f7f, 0x777f, 0x77fe, 0xec7c,
    0x1ede, 0xeeff, 0x77fd, 0xff7c,
    0x1ede, 0xeefe, 0xeffb, 0xfefc,
    0x1d9d, 0xddfe, 0xee38, 0x38fc,
    0x1dac, 0xfdfd, 0xdddf, 0x77fc,
    0x1800, 0x3801, 0xc1c7, 0x701c,
    0x1ffb, 0xf7fe, 0x3e3f, 0x8ffc,
    0x1ffc, 0x0fff, 0xffff, 0xfffc,
    0x1fff, 0xffff, 0xffff, 0xfffc,
    0x0000, 0x0000, 0x0000, 0x0000,
 };
#else	/* Original Code */
static UWORD mg2a[160] = {
/* Bit Plane #0 */

    0x0000,0x0000,0x0000,0x0000,
    0x3fff,0xffff,0xffff,0xf000,
    0x3fff,0xffff,0xffff,0xf000,
    0x3c00,0x0000,0x0000,0xf000,
    0x3cc1,0x8f80,0x0000,0xf000,
    0x3ce3,0x9ce0,0x0000,0xf000,
    0x3cf7,0xb800,0x0000,0xf000,
    0x3cff,0xb9e0,0x0000,0xf000,
    0x3cfb,0xb8e0,0x0000,0xf000,
    0x3ce3,0x9ce3,0xf000,0xf000,
    0x3ce3,0x8fe7,0x3800,0xf000,
    0x3c00,0x0000,0x39f8,0xf000,
    0x3c00,0x0000,0xf01c,0xf000,
    0x3c00,0x0003,0x80fc,0xf000,
    0x3c00,0x0007,0x3b1c,0xf000,
    0x3c00,0x0007,0xf9ee,0xf000,
    0x3c00,0x0000,0x0000,0xf000,
    0x3fff,0xffff,0xffff,0xf000,
    0x3fff,0xffff,0xffff,0xf000,
    0x0000,0x0000,0x0000,0x0000,

/* Bit Plane #1 */

    0xffff,0xffff,0xffff,0xfc00,
    0xffff,0xffff,0xffff,0xfc00,
    0xf000,0x0000,0x0000,0x3c00,
    0xf000,0x0000,0x0000,0x3c00,
    0xf040,0x8000,0x0000,0x3c00,
    0xf020,0x8420,0x0000,0x3c00,
    0xf010,0x8800,0x0000,0x3c00,
    0xf000,0x8820,0x0000,0x3c00,
    0xf020,0x8820,0x0000,0x3c00,
    0xf020,0x8420,0x1000,0x3c00,
    0xf020,0x8021,0x0800,0x3c00,
    0xf000,0x0000,0x0808,0x3c00,
    0xf000,0x0000,0x1004,0x3c00,
    0xf000,0x0000,0x8004,0x3c00,
    0xf000,0x0001,0x0804,0x3c00,
    0xf000,0x0000,0x0802,0x3c00,
    0xf000,0x0000,0x0000,0x3c00,
    0xf000,0x0000,0x0000,0x3c00,
    0xffff,0xffff,0xffff,0xfc00,
    0xffff,0xffff,0xffff,0xfc00
};
#endif	/* KANJI */

static struct Image iconimg = {		/*  Icon Image  */
    0, 0,
#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
    64, 19, 2,
#else	/* Original */
    54, 20, 2,
#endif
    NULL,	/* filled in later */
    0x3, 0,
    NULL
};

VOID tthide _PRO((int));
VOID ttshow _PRO((int));

/*
 * Iconify MG's window using tthide(), iconify(), and ttshow().
 */
int
tticon(f, n)
int f, n;
{
    static UWORD iconX = 0, iconY = 0;
    UWORD *chipbitmap;
    struct Image *chipimg;

    /* copy the bitmap into chip memory */
    if ((chipbitmap=(UWORD * )
	AllocMem((ULONG)sizeof(mg2a), MEMF_CHIP | MEMF_PUBLIC)) == NULL) {
	ewprintf("Can't allocate image bitmap");
	return FALSE;
    }
    bcopy((char *)mg2a, (char *)chipbitmap, (int) sizeof(mg2a));
    
    /* copy the image structure too */
    if ((chipimg=(struct Image *)
	 AllocMem((ULONG) sizeof(iconimg), MEMF_CHIP | MEMF_PUBLIC)) == NULL) {
	FreeMem(chipbitmap, (ULONG) sizeof(iconimg));
	ewprintf("Can't allocate image structure");
	return FALSE;
    }
    bcopy((char *)&iconimg, (char *)chipimg, (int) sizeof(iconimg));
    chipimg->ImageData = chipbitmap;
    
    /* hide the window, display the icon, then redisplay the window */
    tthide(FALSE);	/* not resizing */
    
#ifdef	ICON_WB
    iconify(&iconX, &iconY, chipimg->Width, chipimg->Height, "Ng iconified",
	    (APTR) chipimg, (int) ICON_WB); /* iconify	*/
#else
    iconify(&iconX, &iconY, chipimg->Width, chipimg->Height, NULL,
	    (APTR) chipimg, (int) ICON_IMAGE); /* iconify	*/
#endif
    FreeMem(chipimg, (ULONG) sizeof(iconimg));
    FreeMem(chipbitmap, (ULONG) sizeof(mg2a));
    
    ttshow(FALSE);	/* no resize */
    return TRUE;
}
#endif /* DO_ICONIFY */
