/* $Id: ttyio.c,v 1.6 2001/03/02 08:48:30 amura Exp $ */
/*
 * Name:	MG 2a
 *		Amiga terminal window I/O, with all kinds o' trimmings.
 *		This module is 'way too big.
 * Last Edit:	01-Dec-87 mic@emx.cc.utexas.edu
 * Created:	21-Apr-86 mic@emx.cc.utexas.edu
 */

/*
 * $Log: ttyio.c,v $
 * Revision 1.6  2001/03/02 08:48:30  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.5  2001/01/20 15:48:46  amura
 * very big terminal supported
 *
 * Revision 1.4  2000/12/21 16:54:20  amura
 * fix usage of strncat()
 *
 * Revision 1.3  2000/10/02 16:13:06  amura
 * ignore mouse event in minibuffer editing
 *
 * Revision 1.2  2000/09/29 17:26:17  amura
 * small patch for new ttymenu.c
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 */
 
/*
 * Lots of includes.
 */

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/tasks.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <devices/console.h>
#include <devices/inputevent.h>
#include <libraries/dos.h>
#include <graphics/clip.h>
#include <graphics/view.h>
#include <graphics/rastport.h>
#include <graphics/layers.h>
#include <graphics/text.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <libraries/diskfont.h>
#ifdef KANJI
# ifdef V2
#include <clib/input_protos.h>
# endif
#endif

#undef	TRUE			/* avoid redefinition messages 		*/
#undef	FALSE
#include "config.h"	/* Dec. 15, 1992 by H.Ohkubo */
#include "def.h"		/* includes sysdef.h and ttydef.h	*/

#ifdef	DO_METAKEY
#define	IEQUALIFIER_ALT		(IEQUALIFIER_RALT | IEQUALIFIER_LALT)
#endif	/* DO_METAKEY */

/* ARexx support. this is mg3b's feature! */
#ifdef	REXX	/* Dec.20,1992 Add by H.Ohkubo */
#include	"key.h"
#endif

/*
 * External Amiga functions.
 */
extern	LONG			 AbortIO();
extern	LONG			 CloseDevice();
extern	LONG			 CloseLibrary();
extern	LONG			 CloseWindow();
extern	struct	MsgPort		*CreatePort();
extern	struct	IOStdReq	*CreateStdIO();
extern	LONG			 DeletePort();
extern	LONG			 DeleteStdIO();
extern	struct	IntuiMessage	*GetMsg();
#ifndef	V11
extern	LONG			GetScreenData();
#endif
extern	int			 OpenConsole();
extern	char			*OpenLibrary();
extern	struct	Window		*OpenWindow();
extern	struct TextFont		*OpenDiskFont();
extern	LONG			 RectFill();
extern	LONG			 ReplyMsg();
extern	LONG			 RawKeyConvert();
extern	LONG			 SetAPen();
extern	LONG			 SetDrMd();
extern	LONG			 Wait();

#ifdef	DO_MENU
extern	LONG			 ClearMenuStrip();	/* menu functions */
extern	struct	Menu		*InitEmacsMenu();
extern	VOID			 DisposeMenus();
extern	struct	MenuItem	*ItemAddress();
extern	LONG			 SetMenuStrip();
#endif

#ifdef	MANX
extern	int	Enable_Abort;		/* Do NOT allow abort!		*/
#endif

/*
 * External MG functions and variables
 */
extern	int	quit();			/* Defined by "main.c"		*/
extern	char	version[];		/* Version information		*/
extern	int	ttrow;			/* Current cursor row		*/
extern	int	use_metakey;		/* Do meta characters?		*/

/*
 * Non-int internal functions.  P?() is used to conditionally indicate
 * ANSI-style prototype arguments for compilers (i.e. Lattice) that
 * support them.
 */
#ifdef	SUPPORT_ANSI
#define	P1(a)	a
#define	P2(a,b)	a,b
#define	P3(a,b,c) a,b,c
#else
#define	P1(a)
#define	P2(a,b)
#define P3(a,b,c)
#endif

/* VOID		panic(P1(char *)); */
VOID		setttysize();
/* VOID		ttclose(); */
/* VOID		ttflush(); */
VOID		ttnflush(P1(int));
/* VOID		ttputc(P1(unsigned char)); */

static VOID	cleanup();
static VOID	firstwin();
#ifndef	KANJI	/* Dec.19,1992 by H.Ohkubo */
static VOID	qkey(P1(KCHAR));
#endif
#ifdef	DO_MENU
static VOID	qmenu(P1(USHORT));
#endif
#ifdef	MOUSE
static VOID	qmouse(P3(SHORT, SHORT, USHORT));
extern int	allow_mouse_event;
#endif
static VOID	ttreopen(P1(int)) ;
static VOID	setmaxima() ;
static struct Screen	*wbscreen();


/*
 * Intuition window and menu variables.  MG gets used a lot, because it
 * gets reconfigured on the fly for the amiga-set-font and toggle-border
 * operations.
 */

#define WINDOWGADGETS (WINDOWDRAG | WINDOWDEPTH | WINDOWCLOSE)
#define WINDOWFLAGS	(WINDOWGADGETS | ACTIVATE)

struct NewWindow MG = {
	0,	0,			/* start position       	*/
	0,	0,			/* width, height (set by ttopen)*/
	0,	1,	     		/* detail pen, block pen	*/
#ifdef	DO_MENU
	MENUPICK |			/* If menu is used		*/
#endif
#ifdef	MOUSE
	MOUSEBUTTONS | 			/* If mouse is used		*/
#endif
	INTUITICKS |
#ifndef	KANJI	/* Dec.19,1992 by H.Ohkubo */
	RAWKEY |
#endif
	CLOSEWINDOW | NEWSIZE,		/* IDCMP flags			*/
	0,				/* window flags	(set by ttopen)	*/
	NULL,				/* pointer to first user gadget */
	NULL,				/* pointer to user checkmark	*/ 
	NULL,				/* title (filled in later)	*/
	NULL,				/* pointer to screen (none)	*/
	NULL,				/* pointer to superbitmap	*/
	220,40,				/* minimum size	(small!)	*/
	0, 0,				/* maximum size (set by ttopen)	*/
	WBENCHSCREEN			/* screen in which to open	*/ 
};

static short	borderless = FALSE;	/* Flag for borderless window	*/
static short	toggle_zooms = TRUE;	/* Does toggling border zoom?	*/
static int	last_top, last_left, last_height, last_width;

struct Window	*EmW = NULL;		/* Our window			*/
struct Screen	*EmS = NULL;		/* Our screen (usually WB)	*/
short		toggling = FALSE;	/* Prevent menu wiping		*/
#ifndef	V11
struct Screen	WBInfo;			/* Info about the WB screen	*/
#endif
struct TextFont *EmFont = NULL;		/* Our font (usually TOPAZ_xx)	*/

#ifdef	DO_MENU
static struct Menu	*EmacsMenu = NULL;	/* Our menu		*/
#endif

static ULONG		class;			/* Intuition event	*/
static USHORT		code,			/*   information	*/
			qualifier;
static APTR		address;
static SHORT		x, y;
static LONG		intuitionMsgBit;	/* Signal bit		*/
#define INTUITION_MESSAGE ((LONG) (1L << intuitionMsgBit))

/* * * * * * * * * * * * * console I/O * * * * * * * * * * * * * * * * */

#define	CSI	0x9b			/* Command Sequence Introducer	*/
#define	NOBUF	512			/* About 1/4 screen		*/
#define	NIBUF	256			/* Input buffer			*/

static KCHAR		ibuf[NIBUF];	/* keyboard input buffer	*/
static int		ibufo, nibuf;	/* head, # of bytes in ibuf	*/

#ifndef	PROMPTWAIT
#define	PROMPTWAIT 20			/* ticks to wait before timeout	*/
#endif
static	LONG		tickcount;	/* # intuiticks	since last char	*/

#ifdef	REXX	/* Dec.20,1992 Add by H.Ohkubo */
extern	struct	MsgPort	*rexxport;
#define	REXXPORT_MESSAGE	(1L<<rexxport->mp_SigBit)
#endif
#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
static struct MsgPort	*conReadPort = NULL;	/* I/O ports		*/
static struct IOStdReq	*conReadMsg = NULL;	/* I/O messages		*/
#define	CONREAD_MESSAGE	(1L<<conReadPort->mp_SigBit)
#endif
static struct MsgPort	*conWritePort = NULL;	/* I/O ports 		*/
static struct IOStdReq	*conWriteMsg = NULL;	/* I/O messages		*/
#ifndef KANJI	/* Dec.19,1992 by H.Ohkubo / fixed by amura */
struct Device		*ConsoleDevice;	        /* used by RawKeyConvert*/
#endif
static unsigned char	outbuf[NOBUF+7];	/* output buffer	*/
static unsigned char	*obuf;			/* first output char	*/
int			nobuf;			/* # of bytes in above	*/
int			nrow;			/* Terminal size, rows.	*/
int			ncol;			/* Terminal size, cols.	*/

#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
#define	qkey(k)	{if (nibuf < NIBUF) ibuf[(ibufo + nibuf++)%NIBUF] = (KCHAR)(k);}
#endif
/* * * * * * * * * functions to open/reopen the window * * * * * * * * * */

/*
 * Open up the virtual terminal MG communicates with. Set up the window,
 * console, and menu strip.
 */

ttopen()
{

#ifdef	MANX
	Enable_Abort = 0;				/* Disable ^C	*/
#endif

	/* firstwin() is only called the very first time we open the window */
	if (toggling == FALSE)
		firstwin();

	/* Set the window size, set the flags and title, and open it */

	setmaxima();
	MG.Flags = WINDOWFLAGS;
	MG.Flags |= borderless ? BORDERLESS : WINDOWSIZING;
	MG.Title = (UBYTE *) &version[0];

	if ((EmW = OpenWindow(&MG)) == NULL)
		cleanup();
	SetFont(EmW->RPort, EmFont);

	/* Once the window is created, get the Intuition signal bit, set up
	 * the menu, and tell the virtual terminal how big it is.
 	 */
	setttysize();
	intuitionMsgBit = EmW->UserPort->mp_SigBit;
#ifdef	DO_MENU
	if (toggling == FALSE)
		EmacsMenu = InitEmacsMenu(EmW);
	if (EmacsMenu == NULL)
		cleanup();
	SetMenuStrip(EmW, EmacsMenu);
#endif

	/* Attach a console device (purely for output now) to our window
	 */

	if ((conWritePort = CreatePort("Emacs.con.write", 0L)) == NULL)
		cleanup();
	if ((conWriteMsg = CreateStdIO(conWritePort)) == NULL)
		cleanup();
#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
	if ((conReadPort = CreatePort("Emacs.con.read", 0L)) == NULL)
		cleanup();
	if ((conReadMsg = CreateStdIO(conReadPort)) == NULL)
		cleanup();
#endif

#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
	if (OpenConsole(conWriteMsg,conReadMsg,EmW) != 0)
#else	/* Original */
	if (OpenConsole(conWriteMsg,NULL,EmW) != 0)
#endif
		cleanup();
#ifndef	KANJI	/* Dec.19,1992 by H.Ohkubo */
	ConsoleDevice = conWriteMsg->io_Device;
#endif
	nibuf = ibufo = 0;

	return (0);
}

/*
 * Set up the initial state of the window.  Opens up libraries, decides how
 * big the initial window should be, and whether it should be borderless.
 */

static VOID firstwin()
{
	/* Get our screen and font, then figure out if we can go borderless
	*/
	if ((EmS = wbscreen()) == NULL)
		cleanup();
	EmFont = OpenDiskFont(EmS->Font);
	if ((EmS->Width >= ((INIT_COLS * EmFont->tf_XSize) + LR_BORDER)) &&
		(EmS->Height >= ((INIT_ROWS * EmFont->tf_YSize) + TB_BORDER)))
		borderless = FALSE;

	/* Set the size of the initial window and fake the last one
	 */
	last_width = MG.Width = EmS->Width;
	last_height = MG.Height = EmS->Height;
	last_left = MG.LeftEdge = 0;
	last_top = MG.TopEdge = 0;

	bcopy(outbuf,"\2330 p", 4);	/* preload cursor off sequence */
	obuf = outbuf + 4;
}

/*
 * Make sure the window isn't bigger than NROW * NCOL, while accounting
 * for borders & such.  Since the window might not be at its largest right
 * now, deadstop both the current width and the maxwidth.
 */

static VOID setmaxima()
{
	register int maxw, maxh;

	MG.MaxWidth = EmS->Width;
	MG.MaxHeight = EmS->Height;
	maxw = NCOL * EmFont->tf_XSize + (borderless ? 0 : LR_BORDER);
	maxh = NROW * EmFont->tf_YSize + (borderless ? TOP_OFFSET : TB_BORDER);

	if (MG.MaxWidth > maxw)		MG.MaxWidth = maxw;
	if (MG.Width > maxw)		MG.Width = maxw;

	if (MG.MaxHeight > maxh)	MG.MaxHeight = maxh;
	if (MG.Height > maxh)		MG.Height = maxh;
}


/* Return a pointer the workbench screen, using GetScreenData() to do
 * things like a good citizen.  Left the V11 code in as a reminder
 * that what works is not always the _best_ way to do things.
 * Thanks to Tom Rokicki for reminding me (mpk) this had to be done.
 */

static struct Screen
*wbscreen()
{
#ifndef	V11
	return GetScreenData(&WBInfo, (ULONG) sizeof(WBInfo),
		WBENCHSCREEN, NULL) ? &WBInfo : ((struct Screen *)NULL);
#else
	register struct Screen	*s;
	extern	struct IntuitionBase *IntuitionBase;/* Dec.20,1992 by H.Ohkubo */

	Forbid();
	for (s = IntuitionBase->FirstScreen; s ; s = s->NextScreen)
		if ((s->Flags & SCREENTYPE) == WBENCHSCREEN)
			break;
	Permit();
	return (s);
#endif
}

/*
 * Hide the window and open it up again.  If resize is TRUE, they're
 * being called as part of a resize operation, so assume that the
 * NewWindow structure is set correctly.  Otherwise, store the current
 * window size and position in the NewWindow structure.
 *
 * These two functions are split so we can do things like ttreopen() and
 * tticon() cleanly.
 */

VOID
tthide(resize)
int resize;
{
	toggling = TRUE;
	if (resize == FALSE) {		     /* if we're resizing,	*/
		MG.LeftEdge = EmW->LeftEdge; /* use current window size	*/
		MG.TopEdge = EmW->TopEdge;
		MG.Width = EmW->Width;
		MG.Height = EmW->Height;
	}
	ttclose();				/* reset to zero	*/
}

VOID
ttshow(resize)
int resize;
{
	ttopen();				/* re-open tty window	*/
	ttinit();				/* re-initalize tty	*/
	sgarbf = TRUE;				/* screen was trashed	*/
	if (resize == TRUE)
		nrow = ncol = -1;		/* trash screen size	*/
	refresh();				/* and redraw it	*/
	toggling = FALSE;			/* Ok, done		*/
}

/*
 * ttreopen() was split into the two functions above when tticon()
 * was introduced.
 */

static VOID
ttreopen(resize)
int resize;
{
	tthide(resize);
	ttshow(resize);
}

/* * * * * * * * * * * * functions to close the window * * * * * * * * */

/*
 * Close the virtual terminal.  If toggling, don't release all
 * the other resources we've allocated.
 */
/* VOID */
ttclose()
{
	ttflush();
	CloseDevice(conWriteMsg);
	DeleteStdIO(conWriteMsg);	conWriteMsg = NULL;
	DeletePort(conWritePort);	conWritePort = NULL;
#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
	if (CheckIO(conReadMsg)) {
		AbortIO(conReadMsg);
		WaitIO(conReadMsg);
	}
	DeleteStdIO(conReadMsg);	conReadMsg = NULL;
	DeletePort(conReadPort);	conReadPort = NULL;
#endif
#ifdef	DO_MENU
	ClearMenuStrip(EmW);
#endif
	CloseWindow(EmW);
	if (toggling == FALSE)
		cleanup();		/* clean up everything	*/
#ifdef	MANX
	Enable_Abort = 1;
#endif
}


/*
 * Clean up.  Done only when we're really closing up shop
 */

static VOID
cleanup()
{
	if (conWriteMsg)	DeleteStdIO(conWriteMsg);
	if (conWritePort)	DeletePort(conWritePort);
#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
	if (conReadMsg)	{
		if (CheckIO(conReadMsg)) {
			AbortIO(conReadMsg);
			WaitIO(conReadMsg);
		}
		DeleteStdIO(conReadMsg);
	}
	if (conReadPort)	DeletePort(conReadPort);
#endif
#ifdef	DO_MENU
	if (EmacsMenu)		DisposeMenus(EmacsMenu);
#endif
	if (EmFont)		CloseFont(EmFont);
}

/* * * * * * * * functions that diddle the window and reopen it * * * * * */

/*
 * Toggle between a borderless window and a sizeable window. This lets you
 * use the whole screen if you want. Bound to "amiga-toggle-border".
 */

togglewindow(f, n)
{

	if ((borderless = !borderless) == TRUE) {/* *always* save last	 */
		last_top = EmW->TopEdge;	/* bordered window size	 */
		last_left = EmW->LeftEdge;
		last_width = EmW->Width;
		last_height = EmW->Height;
	}

	if (toggle_zooms == FALSE) {		/* just use current size */
		ttreopen(FALSE);	
		return (TRUE);
	}

	/* zooming -- if borderless, go as big as possible.  If
	 * bordered, set to last saved value of bordered window
	 */
	if (borderless) {
		MG.LeftEdge = 0;
		MG.TopEdge = 0;
		MG.Width = MG.MaxWidth;
		MG.Height = MG.MaxHeight;
	} else {
		MG.LeftEdge = last_left;
		MG.TopEdge = last_top;
		MG.Width = last_width;
		MG.Height = last_height;
	}
	ttreopen(TRUE);			/* open with new size	*/
	return (TRUE);
}

/*
 * Modify the action of "amiga-toggle-border", reporting outcome to user.
 * Bound to "amiga-zoom-mode".
 */
togglezooms(f, n)
{
	toggle_zooms = !toggle_zooms;
	ewprintf("Toggling border %s",
		toggle_zooms ?  "expands window to screen size" :
				"retains current window size");
	return (TRUE);
}

#ifdef	CHANGE_FONT
/*
 * Select a different font for the MG window. This does not work very well with
 * proportional fonts, so we ask the user to confirm before he uses one. It's
 * available if you want to be able to use your own disk font (or Topaz 11
 * under 1.2) to edit with.
 */

setfont(f, n)
{
	register int	s, size;
	register struct TextFont *newfont;
	char		fontname[NFILEN], fontpath[NFILEN], fontsize[3];
	struct TextAttr	ta;

	/* If negative size, reset to default font
	 */
	if ((f & FFARG) && (n <= 0)) {
		CloseFont(EmFont);			/* return old font  */
		EmFont = OpenDiskFont(EmS->Font);	/* screen's default */
		ttreopen(FALSE);			/* no resize	    */
		ewprintf("Now using default font");
		return (TRUE);
	}

	if ((s = ereply("Font name: ",fontname, sizeof(fontname))) != TRUE)
		return (s);
	/* make name */
	strncpy(fontpath,fontname,sizeof(fontpath));
	fontpath[sizeof(fontpath)-1] = '\0';
	strncat(fontpath,".font",sizeof(fontpath)-strlen(fontpath)-1);

	/* Get font size */
	if (f & FFARG)
		size = n;
	else {
		if ((s = ereply("Font size: ",
				fontsize, sizeof(fontsize))) != TRUE)
			return (s);
		size = atoi(fontsize);
	}

	/* Set up text attributes */
	ta.ta_Name = (UBYTE *)fontpath;
	ta.ta_YSize = size;
	ta.ta_Style = FS_NORMAL;
	ta.ta_Flags = 0;

	/* Look for the font */
	ewprintf("Looking for %s %d...",fontname,size);
	if ((newfont = OpenDiskFont(&ta)) == NULL) {
		ewprintf("Can't find %s %d!",fontname,size);
		return (FALSE);
	} 

	/* Found it! Check before using it */
	if ((newfont->tf_YSize != size) &&
		((s = eyesno("Size unavailable - use closest")) != TRUE)) {
		CloseFont(newfont);
		return (FALSE);
	}
	if ((newfont->tf_Flags & FPF_PROPORTIONAL) &&
		(((s = eyesno("Use proportional font")))!= TRUE)) {
			CloseFont(newfont);
			return (FALSE);
	}

	/* Get rid of old font and reopen with the new one */
	CloseFont(EmFont);
	EmFont = newfont;
	ttreopen(FALSE);
	ewprintf("Now using font %s %d",fontname,EmFont->tf_YSize);
	return (TRUE);
}
#endif

/* * * * * * * * * * * * * console output functions  * * * * * * * * * * * * */

/*
 * Write a single character to the screen. Buffered for speed, so ttflush()
 * does all the work.
 */
/* VOID */
ttputc(c)
int c;
{
	obuf[nobuf++] = (unsigned char)c;
	if (nobuf >= NOBUF)
		ttflush();
}

/*
 * Flush characters from the output buffer.  If the # of characters is
 * greater than a certain ad-hoc value, turn the cursor off while doing
 * the write. To avoid extra writes, the output buffer has been preloaded
 * with the cursor-off sequence.  Outbuf is large enough to hold the extra
 * 7 characters.
 */
#define	MIN_OFF	8
/* VOID */
ttflush()
{
	if (nobuf > 0) {
		if (nobuf <= MIN_OFF)	/* don't turn off for short writes */
			ConWrite(conWriteMsg, obuf, nobuf);
		else {
			obuf[nobuf++] = '\x9b';
			obuf[nobuf++] = ' ';
			obuf[nobuf++] = 'p';
			ConWrite(conWriteMsg, outbuf, nobuf + 4);
		}
		nobuf = 0;
	}
}

/*
 * The caller intends to output an escape sequence, but only flush
 * the buffer if there's not enough room to hold the complete sequence.
 * This avoids breaking up escape sequences when we turn the cursor
 * off in ttflush(), at the expense of some extra function calls.
 */
VOID ttnflush(n)
int n;
{
	if ((nobuf + n) > NOBUF)
		ttflush();
}

/* * * * * * * * * * * * * console input functions  * * * * * * * * * * * * */

/* Dec.17,1992 Add by H.Ohkubo */
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
static	int	nkey = 0;		/* The number of ungetc charactor. */
static	int	keybuf[4];		/* Ungetc charactors.		*/
#endif	/* KANJI */

/*
 * Read a character (really a KCHAR, > 8 bits), blocking till a character
 * is put in the input buffer and can be returned.
 */
ttgetc()
{
	static handle_kbd();
/* Dec.17,1992 Add by H.Ohkubo */
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return(keybuf[--nkey]);
	}	/* 91.01.14  by K.Maeda ---remove else */
#endif	/* KANJI */
#ifdef	AUTOSAVE
	while (handle_kbd(TRUE))
		autosave_handler();
#endif	/* AUTOSAVE */
	return handle_kbd(FALSE);
}

/* Dec.17,1992 Add by H.Ohkubo */
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
/*
 * Save pre-readed char to read again.
 */
ttungetc(c)
int	c;
{
	keybuf[nkey++] = c;
}
#endif	/* KANJI */

/*
 * Return TRUE if we've waited for 2 seconds and nothing has happened,
 * else return false.
 */

ttwait()
{
	static handle_kbd();
/* Dec.17,1992 Add by H.Ohkubo */
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return(FALSE);
	}
#endif	/* KANJI */
	return handle_kbd(TRUE);	/* time out after 2 sec */
}

/*
 * Common routine for handling character input, with and without timeout.
 * Handle events until:
 *
 *	1) a character is put in the input buffer
 *	2) if timeout == TRUE, PROMPTWAIT IntuiTicks have gone by
 *
 * If timeout == FALSE, the input character is returned and removed from
 *	the input buffer.
 *
 * If timeout == TRUE, returns TRUE if the read timed out, else FALSE.
 *	Leaves any character typed in the input buffer.
 */

static handle_kbd(timeout)
register int timeout;
{
	register struct	IntuiMessage *message;	/* IDCMP message 	*/
	register LONG	wakeupmask;		/* which signals?	*/
	register int	charfound;		/* got a character yet?	*/
	static dispatch(),nextkey();

	tickcount = 0;				/* *always* zero the count */
	if (nibuf)				/* any chars? return if so */
		return timeout ? FALSE : nextkey();

	charfound = FALSE;			/* nope -- have to wait	*/
	while (!charfound) {
#ifdef	KANJI	/* Dec.19,1992 by H.Ohkubo */
#ifdef	REXX	/* Dec.20,1992 by H.Ohkubo */
		wakeupmask = Wait(INTUITION_MESSAGE | CONREAD_MESSAGE | REXXPORT_MESSAGE);
#else	/* NO REXX */
		wakeupmask = Wait(INTUITION_MESSAGE | CONREAD_MESSAGE);
#endif
		if (wakeupmask & CONREAD_MESSAGE) {
			register UBYTE	*ch;
			int	n;
			UBYTE	*ConRead();

			ch = ConRead(conReadPort, &n);
			if (n > 0) {
#ifdef V2
			    unsigned short q = PeekQualifier();
#endif
			    charfound = TRUE;
			    while (n-- > 0) {
#ifdef V2
				if (*ch==' ' && (q & IEQUALIFIER_CONTROL))
				    {qkey((KCHAR)0x00);}
#ifdef 	LAMIGA_META
				else if (use_metakey&&(q&IEQUALIFIER_LCOMMAND))
				{ qkey((KCHAR)((*ch&0x7f)|METABIT));}
#endif
				else
				    {qkey((KCHAR)(*ch & 0xff));}
				ch++;
#else	/* !V2 */
				qkey((KCHAR)(*ch & 0xff)); ch++;
#endif	/* V2 */
			    }
			}
		}
		if (wakeupmask & INTUITION_MESSAGE)
#else	/* Original */
#ifdef	REXX	/* Dec.20,1992 by H.Ohkubo */
		wakeupmask = Wait(INTUITION_MESSAGE|REXXPORT_MESSAGE);
#else
		wakeupmask = Wait(INTUITION_MESSAGE);
#endif
#endif	/* KANJI */
		/*  Handle Intuiticks specially for speed */
		while(message =	GetMsg(EmW->UserPort))
			if (message->Class == INTUITICKS) {
				tickcount++;
				ReplyMsg(message);
			} else if (dispatch(message) == TRUE)
				charfound = TRUE;
#ifdef	REXX	/* Dec.20,1992 by H.Ohkubo */
		/* Now handle any rexx messages if we need them */
		if (wakeupmask & REXXPORT_MESSAGE) {
			struct key	savekey;/* save current keystrokes */

			savekey = key;
			disprexx(rexxport);
			update();
			key = savekey;
		}
#endif

		/* time out if enough ticks have gone by without
		 * any keyboard input.  We do this *after* all the
		 * events in the current list have been dispatched.
		 */
		if (timeout && (tickcount > PROMPTWAIT))
			break;
	}


	/* If called by ttwait(), return FALSE if a character was found.
	 * Else return the next character in the input buffer
	*/
	return timeout ? (!charfound) : nextkey();
}

/*
 * Handle the events we handle...  The result returned indicates if we've put
 * a character in the input buffer.
 */
static dispatch(msg)
register struct IntuiMessage *msg;
{
#ifdef	DO_MENU
	register struct	MenuItem	*item;
#endif

	register int			txheight, txwidth;
	register struct RastPort	*rp;
	int				dx, dy, fgpen, drmode;
#ifndef	KANJI	/* Jan.7,1992 by H.Ohkubo */
	static struct InputEvent FakedEvent = { NULL, IECLASS_RAWKEY, 0, 0, 0 };
	unsigned char			keybuf[64], altbuf[64];
	int				keylen, altlen, i;
#ifndef	V11
	APTR				deadcodes;
#endif
#endif	/* KANJI */
	class =	msg->Class;		/* grab the info before we 	*/
	code = msg->Code;		/* reply to the message		*/
	qualifier = msg->Qualifier;
	address = msg->IAddress;
	x = msg->MouseX;
	y = msg->MouseY;
#ifndef	KANJI	/* Jan.7,1993 by H.Ohkubo */
#ifndef	V11
	if (class == RAWKEY)		/* get dead key info		*/
		deadcodes = (APTR)address;
#endif
#endif	/* KANJI */
	ReplyMsg(msg);			/* return it to Intuition	*/

	switch(class) {			/* see what the fuss is about	*/
#ifndef	KANJI	/* Dec.19,1992 by H.Ohkubo */
	case RAWKEY:
		FakedEvent.ie_Code = code;
		FakedEvent.ie_Qualifier = qualifier;
#ifndef	V11
		FakedEvent.ie_EventAddress = deadcodes;
#endif
		keylen = (int) RawKeyConvert(&FakedEvent,
			keybuf,	(LONG)sizeof(keybuf), NULL);
#ifdef	DO_METAKEY
		/* Special mapping for ALT-ed keys.  The intent is to get
		 * around keymaps where the ALT'ed characters map to
		 * things other than (0x80 | (c)).  This may not work
		 * for all possible keymaps, but it seems to be ok
		 * for the keymaps distributed with 1.2.
		 */
#ifdef	LAMIGA_META
		if (use_metakey &&
		    (qualifier & (IEQUALIFIER_ALT|IEQUALIFIER_LCOMMAND))) {
			FakedEvent.ie_Qualifier &=
			    ~(IEQUALIFIER_ALT | IEQUALIFIER_LCOMMAND);
			if (qualifier & IEQUALIFIER_ALT)
			    altlen =
				(int) RawKeyConvert(&FakedEvent, altbuf,
				(LONG)sizeof(altbuf), NULL);
			else
			{
			    altlen = 1;
			    altbuf[0] = keybuf[0];
			}
			if (altlen >= 1)
			    qkey((KCHAR)(altbuf[0]|METABIT));
			for (i = 1; i < altlen ; i++)
			    qkey((KCHAR) altbuf[i]);
			return (altlen > 0) ? TRUE : FALSE;
		}
#else
		if (use_metakey &&
		    (qualifier & IEQUALIFIER_ALT) {
			FakedEvent.ie_Qualifier &= ~IEQUALIFIER_ALT;
			altlen = (int) RawKeyConvert(&FakedEvent, altbuf,
				(LONG)sizeof(altbuf), NULL);
			if (altlen == 1)
				altbuf[0] |= METABIT;
			for (i = 0; i < altlen ; i++)
				qkey((KCHAR) altbuf[i]);
			return (altlen > 0) ? TRUE : FALSE;
		}
#endif	/* LAMIGA_META */
#endif	/* DO_METAKEY */
		if (keybuf[0]==' ' && (qualifier&IEQUALIFIER_CONTROL))
		    keybuf[0] = '\0';
		for (i = 0; i < keylen ; i++)
			qkey((KCHAR) keybuf[i]);
		return (keylen > 0) ? TRUE : FALSE;
		break;
#endif	/* KANJI */

#ifdef	DO_MENU
	case MENUPICK:
		if (code == MENUNULL)
			return (FALSE);
		while (code != MENUNULL) {/* handle multiple selection	*/
			qmenu(code);
			item = ItemAddress(EmacsMenu,(LONG) code);
			code = item->NextSelect;
		}
		return (TRUE);		/* puts KMENU in event queue	*/
		break;
#endif

#ifdef	MOUSE
	case MOUSEBUTTONS:			/* fake the mouse key	*/
		if (code != SELECTDOWN)		/* ignore SELECTUP	*/
			return (FALSE);
		qmouse(x, y, qualifier);
		return (TRUE);
		break;
#endif

	case NEWSIZE:
		/* Sometimes when you resize the window to make it smaller,
		 * garbage is left at the right and bottom sides of the
		 * window. This code is devoted to (somehow) getting rid
		 * of this garbage.  Any suggestions?
		 */

		rp = EmW->RPort;
		fgpen = rp->FgPen;		/* save params		*/
		drmode = rp->DrawMode;
		SetDrMd(rp, (LONG) JAM1);
		SetAPen(rp, (LONG) EmW->RPort->BgPen);

		/* Check the bottom of the window
		 */
		txheight = EmW->Height - EmW->BorderTop - EmW->BorderBottom;
		if (dy = (txheight % FontHeight(EmW)))
			RectFill(rp,
				(LONG) EmW->BorderLeft,
				(LONG) EmW->BorderTop + txheight - dy - 1,
				(LONG) (EmW->Width - 1) - EmW->BorderRight,
				(LONG) (EmW->Height - 1) - EmW->BorderBottom);

		/* Check the right side
		 */
		txwidth = EmW->Width - EmW->BorderLeft - EmW->BorderRight;
		if (dx = txwidth % FontWidth(EmW))
			RectFill(rp,
				(LONG) EmW->BorderLeft + txwidth - dx - 1,
				(LONG) EmW->BorderTop,
				(LONG) (EmW->Width - 1) - EmW->BorderRight,
				(LONG) (EmW->Height - 1) - EmW->BorderBottom);

		SetDrMd(rp, (LONG) drmode);
		SetAPen(rp, (LONG) fgpen);	/* restore colors */

		/* Tell the console device to resize itself */
		ttputc(CSI);
		ttputc('t');
		ttputc(CSI);
		ttputc('u');
		ttflush();

		/* Signal the editor that a new size has occurred.
		 * I may break down and do this asynchronously...
		 */
		qkey(KRESIZE);
		return (TRUE);			/* we done (finally)	*/
		break;

        case CLOSEWINDOW:
		/* Calling quit() directly is not a guaranteed win. */
		quit(FFRAND, 1);
		return (FALSE);
                break;

	default:
		panic("HandleMsg: unknown event!!!");
		break;
	}
	return(FALSE);
}

/*
 * Return the current size of the virtual terminal in nrow and ncol,
 * making sure we don't go beyond the size of the internal video array.
 * Assumes the current font is monospaced.
 */
VOID
setttysize()
{
	nrow = (EmW->Height - TOP_OFFSET
			- EmW->BorderBottom) / FontHeight(EmW);
	ncol = (EmW->Width - EmW->BorderLeft
			- EmW->BorderRight) / FontWidth(EmW);
	if (nrow < 1)		nrow = 1;
	if (ncol < 1)		ncol = 1;
}

/*
 * Exit as soon as possible, after displaying the message.
 */
/* VOID */
panic(s)
char *s;
{
	ewprintf(s);		/* put message at bottom	*/
	Delay((ULONG) 90);	/* wait 1.5 seconds		*/
	ttclose();		/* get rid of window &resources	*/
	exit(10000);		/* go 'way			*/
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *			 Event buffer management		 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Return next key in the input buffer, if any available.  Returns -1 if not.
 */
static int
nextkey()
{
	register KCHAR k;

	if (nibuf <= 0) {		/* shouldn't happen, but could... */
		nibuf = 0;
		return -1;
	} else {
		k = ibuf[ibufo++];
		nibuf--;
		ibufo %= NIBUF;
		return (int) k;
	}
}			

/*
 * Return true if there are some characters available in the input buffer.
 */
typeahead()
{
/* Dec.17,1992 by H.Ohkubo */
#ifdef	KANJI	/* 90.02.05  by S.Yoshida */
	if (nkey > 0) {
		return(TRUE);
	}
#endif	/* KANJI */
	return (nibuf > 0);
}
#ifndef	KANJI	/* Dec.19,1992 by H.Ohkubo */
/*
 * Add a key to the input queue
 */
static VOID
qkey(k)
KCHAR k;
{
	if (nibuf < NIBUF)
		ibuf[(ibufo + nibuf++) % NIBUF] = k;
}
#endif

#ifdef	MOUSE
/*
 * Add a mouse event to the input queue, calculating the row and column
 * value from the current height and width of the window's font.
 */

static VOID
qmouse(x, y, qual)
SHORT x, y;
USHORT qual;
{
	register int	myqual = MQ_NOQUAL;
	register int	row, col;
	register WINDOW	*wp;	

	if (!allow_mouse_event)
		return;
	
	/* get row, column	*/
	col = (x - EmW->BorderLeft) / FontWidth(EmW);
	row = (y - TOP_OFFSET) / FontHeight(EmW);

	/* find out which kind of window was clicked in */
	for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
		if ((row >= wp->w_toprow) && 
			(row <= (wp->w_toprow + wp->w_ntrows)))
			break;
	if (wp == NULL)
		myqual |= MQ_ECHO;
	else if (row == (wp->w_toprow + wp->w_ntrows))
		myqual |= MQ_MODE;
	else
		myqual |= MQ_WINDOW;

	/* figure out qualifiers	*/
	if (qual & IEQUALIFIER_CONTROL)
		myqual |= MQ_CTRL;
	if (qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
		myqual |= MQ_SHIFT;
	if (qual & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))
		myqual |= MQ_ALT;
#ifdef META_LAMIGA
	if (qual & IEQUALIFIER_LCOMMAND)
		myqual |= MQ_ALT;
#endif
	/*
 	 * Queue up the whole mess.  If user didn't click in the echo
	 * line, transmit the x, y values to the mouse function
	 */
	qkey(KW___MOUSE + myqual);
	if (MQ_WHERE(myqual) != MQ_ECHO) {
		qkey(M_X_ZERO + col);
		qkey(M_Y_ZERO + row);
	}
}
#endif

#ifdef	DO_MENU
/*
 * Add a menu event to the queue.
 */
static VOID
qmenu(code)
USHORT code;
{
	qkey(KMENU);		/* menu key sequence	*/
	qkey(((KCHAR) MENUNUM(code)) + MN_OFFSET);
	qkey(((KCHAR) ITEMNUM(code)) + MN_OFFSET);
	qkey(((KCHAR) SUBNUM(code)) + MN_OFFSET);
}
#endif
