/* $Id: sysinit.c,v 1.6 2001/02/01 16:31:43 amura Exp $ */
/*
 * Name:	MG 2a
 *
 * 		Very early-on system-specific initialization for whatever's
 *		necessary.
 * Last edit:	05-May-88, Stephen Walton, swalton@solar.stanford.edu
 * Created:	Stephen Walton, 3-Dec-87.
 *	
 */

/*
 * $Log: sysinit.c,v $
 * Revision 1.6  2001/02/01 16:31:43  amura
 * fix small bug? in get current directory
 *
 * Revision 1.5  2000/12/22 19:48:17  amura
 * change "sysdef.h" to "def.h" for get NFILEN
 *
 * Revision 1.4  2000/12/14 18:08:35  amura
 * filename length become flexible
 *
 * Revision 1.3  2000/10/31 07:08:14  amura
 * fix bug around input.device
 *
 * Revision 1.2  2000/09/29 17:25:15  amura
 * small patch for new iconify()
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 */

/* Dec.20,1992 Add by H.Ohkubo */
#include <graphics/gfxbase.h>
#include <intuition/intuitionbase.h>
#include <libraries/diskfont.h>
#include "zz_pointer.h"

#include <libraries/dosextens.h>
#include "config.h"	/* Dec. 16, 1992 by H.Ohkubo */
#include "def.h"

#undef	FALSE
#undef	TRUE
#define	TRUE	1
#define	FALSE	0

#ifdef USE_ARP
#include	"libraries/arpbase.h"
struct ArpBase	*ArpBase;
#endif
#ifdef DO_ICONIFY
# ifdef V2
struct Library 	*WorkbenchBase;
# endif
#endif


#ifdef	KANJI
# ifdef V2
struct Library  *InputBase;
static struct IOStdReq *InputIO;
static struct MsgPort  *InputMP;
# endif
#endif

/*
 * Library bases (used by glue libraries)
 * These valiable from ttyio.c by.H.Ohkubo Dec.20,1992
 */
struct	IntuitionBase	*IntuitionBase;
struct	GfxBase		*GfxBase;
struct	Library		*DiskfontBase;
extern	struct	Library *OpenLibrary();

#ifndef	NO_DIR
extern struct	Task *FindTask();
static BPTR	StartLock;
char		MyDirName[NFILEN];
extern BPTR	DupLock(), CurrentDir();
#endif /* NO_DIR */

sysinit()
{
	long len;
	BPTR MyDirLock;

#ifdef USE_ARP
	if (!(ArpBase = (struct ArpBase *) OpenLibrary("arp.library", 0L)))
		panic("Compiled with USE_ARP, but arp.library not found");
#endif
#ifndef NO_DIR
	/*
	 * The following attempt to be clever assigns the external StartLock
	 * to the lock on the current directory, then switches our CurrentDir
	 * to a duplicate of that lock so we can restore the original lock
	 * on exit.
	 */

	StartLock = ((struct Process *)FindTask(0L))->pr_CurrentDir;
	(void) CurrentDir(MyDirLock = DupLock(StartLock));
	len = PathName(MyDirLock, MyDirName, (NFILEN+31L)/32L - 1);
#endif /* NO_DIR */

/* These from ttyio.c by H.Ohkubo Dec.20,1992 */
#ifdef	USE_ARP	/* Add by H.Ohkubo */
#define	OpenLibrary	ArpOpenLibrary
#endif
	GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 0L);
	if (GfxBase == NULL)				/* Graphics lib	*/
		syscleanup();

	IntuitionBase = (struct IntuitionBase *) 	/* Intuition	*/
		OpenLibrary("intuition.library", 0L);
	if (IntuitionBase == NULL)
		syscleanup();

	DiskfontBase = (struct Library *) OpenLibrary("diskfont.library", 0L);
	if (DiskfontBase == NULL)
		syscleanup();
#ifdef DO_ICONIFY
# ifdef V2
	WorkbenchBase = OpenLibrary("workbench.library",0L);
	if (WorkbenchBase == NULL)
		syscleanup();
# endif
#endif
#ifdef	KANJI
# ifdef V2
	InputBase = NULL;
	if (InputMP=CreatePort(0L,0L)) {
		if (InputIO = (struct IOStdReq*)
		    CreateExtIO(InputMP, sizeof(struct IOStdReq))) {
			if (!OpenDevice("input.device", NULL,
				       (struct IORequest *)InputIO, NULL))
			InputBase = (struct Library *)InputIO->io_Device;
		}
	}
    	if (InputBase == NULL)
		panic("cannot open input device");
# endif /* V2 */
#endif
#ifdef	REXX	/* Dec.20,1992 by H.Ohkubo */
	openrexx();
#endif
	zz_pointer_open();	/* Dec.20,1992 Add by H.Ohkubo */
}

/*
 * System dependent cleanup for the Amiga.
 */
syscleanup()
{
	zz_pointer_close();	/* Dec.20,1992 Add by H.Ohkubo */

	UnLock(CurrentDir(StartLock));	/* restore startup directory	*/

#ifdef	REXX
	closerexx();
#endif
#ifdef KANJI
# ifdef V2
	if (InputIO) {
		CloseDevice(InputIO);
		DeleteExtIO(InputIO);
	}
	if (InputMP)		DeletePort(InputMP);
# endif
#endif
	/* from ttyio.c by H.Ohkubo Dec.20,1992 */
#ifndef	USE_ARP	/* Add by H.Ohkubo */
# ifdef	DO_ICONIFY
#  ifdef V2
	if (WorkbenchBase)	CloseLibrary(WorkbenchBase);
#  endif
# endif
	if (DiskfontBase)	CloseLibrary(DiskfontBase);
	if (IntuitionBase)	CloseLibrary(IntuitionBase);
	if (GfxBase)		CloseLibrary(GfxBase);
#endif
#ifdef USE_ARP
	if (ArpBase)
		CloseLibrary(ArpBase);
#endif
}
