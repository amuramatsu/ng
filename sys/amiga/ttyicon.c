/* $Id: ttyicon.c,v 1.1 2000/06/27 01:48:01 amura Exp $ */
/*
 * Name:	MG 2a
 *		Iconify the MG window using Leo Schwab's iconify() routine.
 * Last Edit:	07-Jan-88	mic@emx.utexas.edu
 * Created:	04-Jan-88	mic@emx.utexas.edu
 */

/*
 * $Log: ttyicon.c,v $
 * Revision 1.1  2000/06/27 01:48:01  amura
 * Initial revision
 *
 */

#include	"config.h"	/* Dec.19,1992 Add by H.Ohkubo */
#ifdef	DO_ICONIFY

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include "iconify.h"
#undef	TRUE
#undef	FALSE
#include "def.h"

/*
 * Simple Mg 2a icon image.  We need a more imaginative one.
 */

#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
UWORD	mg2a[] = {
/* Bit Plane #0 */

   0x3fff, 0xffff, 0xffff, 0xfffe,
   0x3fff, 0xffff, 0xffff, 0xfffe,
   0x3ffd, 0xefff, 0xffff, 0xfffe,
   0x3ff2, 0x97ff, 0xfeff, 0xff7e,
   0x3fe3, 0x2fff, 0xf17f, 0xe0be,
   0x3fe1, 0x3fbf, 0xc2ff, 0x88be,
   0x3fc2, 0x605f, 0xc2ff, 0x88be,
   0x3fc0, 0x445f, 0xc5ff, 0xf17e,
   0x3f80, 0x88bf, 0xc5ff, 0xf17e,
   0x3f80, 0x88bf, 0x8bff, 0x86fe,
   0x3f21, 0x117f, 0x8bff, 0xe2fe,
   0x3f21, 0x117f, 0x17ff, 0xc5fe,
   0x3e62, 0x22ff, 0x17dc, 0x45fe,
   0x3e53, 0x02fe, 0x2e28, 0x8bfe,
   0x398f, 0xc402, 0x2224, 0x103e,
   0x3ffc, 0x0bff, 0xdfdf, 0xeffe,
   0x3fff, 0xf7ff, 0xffff, 0xfffe,
   0x3fff, 0xffff, 0xffff, 0xfffe,
   0x3fff, 0xffff, 0xffff, 0xfffe,

/* Bit Plane #1 */

   0x0000, 0x0000, 0x0000, 0x0000,
   0x1fff, 0xffff, 0xffff, 0xfffc,
   0x1ff3, 0x9fff, 0xffff, 0xfffc,
   0x1fed, 0x6fff, 0xf1ff, 0xe0fc,
   0x1fdc, 0xdfff, 0xceff, 0x9f7c,
   0x1fde, 0xc07f, 0xbdff, 0x777c,
   0x1fbd, 0x9fbf, 0xbdff, 0x777c,
   0x1fbf, 0xbbbf, 0xbbff, 0x8efc,
   0x1f7f, 0x777f, 0xbbff, 0x8efc,
   0x1f7f, 0x777f, 0x77ff, 0x79fc,
   0x1ede, 0xeeff, 0x77ff, 0x9dfc,
   0x1ede, 0xeefe, 0xeffc, 0x3bfc,
   0x1d9d, 0xddfe, 0xee3b, 0xbbfc,
   0x1dac, 0xfdfd, 0xddd7, 0x77fc,
   0x1800, 0x3801, 0xc1c3, 0xe03c,
   0x1ffb, 0xf7fe, 0x3e3c, 0x1ffc,
   0x1ffc, 0x0fff, 0xffff, 0xfffc,
   0x1fff, 0xffff, 0xffff, 0xfffc,
   0x0000, 0x0000, 0x0000, 0x0000,

   };
#else	/* Original Code */
UWORD mg2a[160] = {
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

/*
 * Iconify MG's window using tthide(), iconify(), and ttshow().
 */

int tticon(f, n)
{
	static UWORD	iconX = 0, iconY = 0;
	UWORD		*chipbitmap;
	struct Image	*chipimg;
	extern short	toggling;
	extern APTR	AllocMem();

	/* copy the bitmap into chip memory */
	if (NULL == (chipbitmap = (UWORD * )
		AllocMem((ULONG) sizeof(mg2a), MEMF_CHIP | MEMF_PUBLIC))) {
		ewprintf("Can't allocate image bitmap");
		return FALSE;
	}
	bcopy((char *)mg2a, (char *)chipbitmap, (int) sizeof(mg2a));

	/* copy the image structure too */
	if (NULL == (chipimg = (struct Image *)
		AllocMem((ULONG) sizeof(iconimg), MEMF_CHIP | MEMF_PUBLIC))) {
		FreeMem(chipbitmap, (ULONG) sizeof(iconimg));
		ewprintf("Can't allocate image structure");
		return FALSE;
	}
	bcopy((char *)&iconimg, (char *)chipimg, (int) sizeof(iconimg));
	chipimg->ImageData = chipbitmap;

	/* hide the window, display the icon, then redisplay the window */
	tthide(FALSE);	/* not resizing */

	iconify(&iconX, &iconY, chipimg->Width, chipimg->Height, NULL,
		(APTR) chipimg, (int) ICON_IMAGE); /* iconify	*/
	FreeMem(chipimg, (ULONG) sizeof(iconimg));
	FreeMem(chipbitmap, (ULONG) sizeof(mg2a));

	ttshow(FALSE);	/* no resize */
	return TRUE;
}
#endif DO_ICONIFY