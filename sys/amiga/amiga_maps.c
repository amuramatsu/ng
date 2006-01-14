/* $Id: amiga_maps.c,v 1.3.2.1 2006/01/14 17:39:58 amura Exp $ */
/*
 * Name:	MG 2a
 *		keymap.c setup for Amiga-specific function keys.
 * Created:	14-May-1988 Mic Kaczmarczik (mic@emx.utexas.edu)
 * Last edit:	14-May-1988 Mic Kaczmarczik
 *
 * One big table for coding convenience, because the number of different
 * compilation possibilities makes setting up data structures kind of
 * tricky.  Once again for convenience, the file is #included into keymap.c
 * instead of being linked.  I tried to minimize the changes to keymap.c,
 * while making it possible to have the Amiga function/arrow keys in dired
 * mode as well as fundamental mode.  In the future, we might want to
 * rebind some of the function keys in dired mode, but for right now they
 * do the same thing as in fundamental mode.
 */
#ifdef	MOUSE
static PF amiga_keys[] = {
    amigamouse,		/* Mouse		(0x120)	*/
    mforwdel,		/* Ctrl-Mouse			*/
    mreposition,	/* Shift-Mouse			*/
    mdelwhite,		/* Shift-Ctrl-Mouse		*/
    mdelfword,		/* Meta-Mouse			*/
    mkillregion,	/* Meta-Ctrl-Mouse		*/
    mkillline,		/* Meta-Shift-Mouse		*/
    myank,		/* Meta-Shift-Ctrl-Mouse	*/
    mforwpage,		/* Mode-Mouse		(0x128)	*/
    mgotobob,		/* Ctrl-Mode-Mouse		*/
    mbackpage,		/* Shift-Mode-Mouse		*/
    mgotoeob,		/* Shift-Ctrl-Mode-Mouse	*/
    msplitwind,		/* Meta-Mode-Mouse		*/
    menlargewind,	/* Meta-Ctrl-Mode-Mouse		*/
    mdelwind,		/* Meta-Shift-Mode-Mouse	*/
    mshrinkwind,	/* Meta-Shift-Ctrl-Mode-Mouse	*/
#ifdef	DO_ICONIFY
    tticon,		/* Echo-Mouse		(0x130)	*/
#else
    usebuffer,		/* Echo-Mouse		(0x130)	*/
#endif
    spawncli,		/* Ctrl-Echo-Mouse		*/
    killbuffer,		/* Shift-Echo-Mouse		*/
    quit,		/* Shift-Ctrl-Echo-Mouse	*/
    desckey,		/* Meta-Echo-Mouse		*/
    listbuffers,	/* Meta-Ctrl-Echo-Mouse		*/
    wallchart,		/* Meta-Shift-Echo-Mouse	*/
    togglewindow,	/* Meta-Shift-Ctrl-Echo-Mouse	*/
};
#endif	/* MOUSE */

/*
 * Define extra maps for fundamental mode.  Have to provide the number of
 * extra map segments because it's used by the KEYMAPE() macro that builds
 * keymaps.  The keymap setup, while compact, is pretty complex...
 */
#ifdef	MOUSE
#define	NFUND_XMAPS	1
#define	FUND_XMAPS	{KW___MOUSE,	KEASCMOUSE,	amiga_keys,	(KEYMAP*)NULL}
#endif

/*
 * Extra map segments for dired mode -- just use fundamental mode segments
 */
#define	NDIRED_XMAPS	NFUND_XMAPS
#define	DIRED_XMAPS	FUND_XMAPS

