/* $Id: menumap.h,v 2.1.4.1 2003/03/08 01:22:36 amura Exp $ */
/*
 * menumap.h
 *   Amiga intuition menu mapping header for Ng 1.x
 *
 * Copyright (C) 2000, MURAMATSU Atsushi  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY "MURAMATSU Atsushi" AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.  
 */

/*
 * $Log: menumap.h,v $
 * Revision 2.1.4.1  2003/03/08 01:22:36  amura
 * NOTAB is always enabled
 *
 * Revision 2.1  2000/10/02 14:05:46  amura
 * create for new ttymenu.c
 *
 */


/*--- Functions called from menu ---*/

#define _PF() pro((int, int))

/*
 * Defined by "buffer.c".
 */
extern	int	listbuffers _PF();	/* Display list of buffers	*/
extern	int	usebuffer _PF();	/* Switch a window to a buffer	*/
extern	int	killbuffer _PF();	/* Make a buffer go away.	*/

#ifndef NO_DIRED
/*
 * defined by "dired.c"
 */
extern	int	dired _PF();		/* dired			*/
#endif

/*
 * Defined by "file.c".
 */
extern	int	filevisit _PF();	/* Get a file, read write	*/
extern	int	filewrite _PF();	/* Write a file			*/
extern	int	filesave _PF();		/* Save current file		*/
extern	int	fileinsert _PF();	/* Insert file into buffer	*/

/*
 * defined by help.c
 */
#ifndef NO_HELP
extern	int	desckey _PF();		/* describe key			*/
extern	int	wallchart _PF();	/* Make wall chart.		*/
extern	int	help_help _PF();	/* help help			*/
extern	int	apropos_command _PF();	/* apropos			*/
#endif

/*
 * defined by "jump.c"         Dec 1991. bsh
 */
#ifdef	JUMPERR
extern	int	compile _PF();		/* compile  Y.Koyanagi */
#endif

/*
 * defined by "kanji.c"
 */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
extern	int	k_set_buffio _PF();	/* Set buffer file I/O code.	*/
extern	int	k_set_input _PF();	/* Set keyboard input code.	*/
extern	int	k_set_display _PF();	/* Set KANJI display code.	*/
#endif	/* KANJI */

/*
 * defined by "line.c"
 */
#ifdef	UNDO
extern	int	do_undo _PF();		/* undo */
#endif

/*
 * defined by "macro.c"
 */
#ifndef NO_MACRO
extern	int	executemacro _PF();	/* Execute macro		*/
#endif

/*
 * Defined by "main.c".
 */
extern	int	quit _PF();		/* Quit				*/

/*
 * defined by "modes.c"
 */
extern	int	indentmode _PF();	/* set auto-indent mode		*/
extern	int	fillmode _PF();		/* set word-wrap mode		*/
extern	int	notabmode _PF();	/* no tab mode			*/
extern	int	overwrite _PF();	/* overwrite mode		*/
#ifdef	C_MODE	/* 90.07.24  by K.Takano */
extern	int	cmode _PF();		/* set c-mode			*/
#endif	/* C_MODE */

/*
 * defined by "paragraph.c" - the paragraph justification code.
 */
extern	int	fillpara _PF();		/* Justify a paragraph.		*/

/*
 * Defined by "random.c".
 */
extern	int	yank _PF();		/* Yank back from killbuffer.	*/

#ifdef	REGEX
/*
 * Defined by "re_search.c"
 */
extern	int	re_forwsearch _PF();	/* Regex search forward		 */
extern	int	re_backsearch _PF();	/* Regex search backwards	 */
extern	int	re_searchagain _PF();	/* Repeat regex search command	 */
extern	int	re_queryrepl _PF();	/* Regex query replace		 */
#endif

/*
 * Defined by "region.c".
 */
extern	int	killregion _PF();	/* Kill region.			*/
extern	int	copyregion _PF();	/* Copy region to kill buffer.	*/
extern	int	lowerregion _PF();	/* Lower case region.		*/
extern	int	upperregion _PF();	/* Upper case region.		*/

/*
 * Defined by "search.c".
 */
extern	int	forwsearch _PF();	/* Search forward		*/
extern	int	backsearch _PF();	/* Search backwards		*/
extern	int	searchagain _PF();	/* Repeat last search command	*/
extern	int	queryrepl _PF();	/* Query replace		*/

/*
 * Defined by "spawn.c".
 */
extern	int	spawncli _PF();		/* Run CLI in a subjob.		*/

/*
 * Defined by "version.c"
 */
extern	int	showversion();		/* Show version numbers, etc.	*/
#ifdef	ADDFUNC	/* 90.12.28  by S.Yoshida */
extern	int	showngversion();	/* Show Ng version, etc.	*/
#endif	/* ADDFUNC */

/*
 * Defined by "window.c".
 */
extern	int	refresh _PF();		/* Refresh the screen		*/
extern	int	onlywind _PF();		/* Make current window only one */
extern	int	splitwind _PF();	/* Split current window		*/
extern	int	delwind _PF();		/* Delete current window	*/
extern	int	enlargewind _PF();	/* Enlarge display window.	*/
extern	int	shrinkwind _PF();	/* Shrink window.		*/

/*
 * Defined by "skg.c".
 */
#ifdef INCLUDE_SKG
extern	int	skginput _PF();		/* Get Kanji strings.		*/
#endif

/*
 * Amiga Special
 */
#ifdef	AMIGA
#ifdef	DO_ICONIFY
extern	int	tticon _PF();
#endif

extern	int	togglewindow _PF();	/* Defined by "ttyio.c"		*/
extern	int	togglezooms _PF();	/*    ""         ""		*/

#ifdef	CHANGE_FONT
extern	int	setfont _PF();		/* Defined by "ttyio.c"		*/
#endif

#ifdef	CHANGE_COLOR
	/* functions to mess with the mode line rendition, window colors*/
extern	int	ttmode _PF();		/* Defined by "tty.c"		*/
extern	int	tttext _PF();		/*  ""				*/
extern	int	textforeground _PF();	/*  ""				*/
extern	int	textbackground _PF();	/*  ""				*/
extern	int	modeforeground _PF();	/*  ""				*/
extern	int	modebackground _PF();	/*  ""				*/
#endif

#ifdef	ASL
extern	int	aslopen _PF();
extern	int	aslinsert _PF();
extern	int	aslwrite _PF();
#endif
#endif	/* AMIGA */


/*--- Menu definitions ---*/

#define MENU_END	0
#define	MENU_FUNC	1
#define	MENU_SUB	2
#define MENU_LINE	0x0F
#define	MENU_TYPE(t)	(t&0x0F)

typedef struct {
    char *face;
    int   type;
    void *func;
    char *key;
} MenuMap;

#ifndef	MENU_SUB_ICON
#define	MENU_SUB_ICON	""
#endif

MenuMap MgFileMenus[] =
{
    {"Open File...      (C-x C-f)", MENU_FUNC, filevisit  , NULL},
#ifndef	NO_DIRED
    {"Open Directory... (C-x d)"  , MENU_FUNC, dired      , NULL},
#endif
    {"Save Buffer       (C-x C-s)", MENU_FUNC, filesave   , NULL},
    {"Save Buffer As... (C-x C-w)", MENU_FUNC, filewrite  , NULL},
    {"Insert File...    (C-x i)"  , MENU_FUNC, fileinsert , NULL},
    {"Kill Buffer...    (C-x k)"  , MENU_FUNC, killbuffer , NULL},
    {"---------------------------", MENU_LINE, NULL       , NULL},
    {"Show All Buffer   (C-x C-b)", MENU_FUNC, listbuffers, NULL},
    {"Switch Buffer     (C-x b)"  , MENU_FUNC, usebuffer  , NULL},
    {"---------------------------", MENU_LINE, NULL       , NULL},
    {"Split Window      (C-x 2)"  , MENU_FUNC, splitwind  , NULL},
    {"One Window        (C-x 1)"  , MENU_FUNC, onlywind   , NULL},
    {"One Window(Alt)   (C-x 0)"  , MENU_FUNC, delwind    , NULL},
#if	defined(AMIGA)||defined(DO_ICONIFY)
    {"---------------------------", MENU_LINE, NULL       , NULL},
#endif
#ifdef	AMIGA
    {"Spawn CLI"                  , MENU_FUNC, spawncli   , NULL},
#endif
#ifdef	DO_ICONIFY
    {"Iconify"                    , MENU_FUNC, tticon     , NULL},
#endif
    {"---------------------------", MENU_LINE, NULL       , NULL},
    {"Exit Emacs        (C-x C-c)", MENU_FUNC, quit       , NULL},
    {NULL                         , MENU_END , NULL       , NULL}
};

MenuMap MgEditMenus[] =
{
#ifdef	UNDO
    {"Undo             (C-x _)", MENU_FUNC, do_undo    , NULL},
#endif
    {"Cut              (C-w)"  , MENU_FUNC, killregion , NULL},
    {"Copy             (M-w)"  , MENU_FUNC, copyregion , NULL},
    {"Paste            (C-y)"  , MENU_FUNC, yank       , NULL},
    {"------------------------", MENU_LINE, NULL       , NULL},
    {"Fill Paragraph   (M-q)"  , MENU_FUNC, fillpara   , NULL},
    {"Up-case Region   (M-u)"  , MENU_FUNC, upperregion, NULL},
    {"Down-case Region (M-l)"  , MENU_FUNC, lowerregion, NULL},
    {NULL                      , MENU_END , NULL       , NULL}
};

MenuMap MgSearchMenus[] =
{
    {"Search..."                       , MENU_FUNC, forwsearch    , NULL},
#ifdef REGEX
    {"Regexp Search..."                , MENU_FUNC, re_forwsearch , NULL},
#endif
    {"Search Backwards..."             , MENU_FUNC, backsearch    , NULL},
#ifdef REGEX
    {"Regexp Search Backwards..."      , MENU_FUNC, re_backsearch , NULL},
#endif
    {"--------------------------------", MENU_LINE, NULL          , NULL},
    {"Repeat Search"                   , MENU_FUNC, searchagain   , NULL},
#ifdef	REGEX
    {"Repeat Regexp"                   , MENU_FUNC, re_searchagain, NULL},
#endif
    {"--------------------------------", MENU_LINE, NULL          , NULL},
    {"Quary Replace...           (M-%)", MENU_FUNC, queryrepl     , NULL},
#ifdef	REGEX
    {"Quary Replace Regexp..."         , MENU_FUNC, re_queryrepl  , NULL},
#endif
    {NULL                              , MENU_END , NULL          , NULL}
};

#ifdef	KANJI
MenuMap MgKanjiMenus[] =
{
    {"Kanji Buffer I/O Code (C-x C-k f)", MENU_FUNC, k_set_buffio , NULL},
    {"Kanji Input Code      (C-x C-k i)", MENU_FUNC, k_set_input  , NULL},
    {"Kanji Display Code    (C-x C-k d)", MENU_FUNC, k_set_display, NULL},
    {NULL                               , MENU_END , NULL         , NULL}
};
#endif	/* KANJI */

#ifdef	AMIGA
MenuMap MgAmigaMenus[] =
{
    {"Window Border Toggle"           , MENU_FUNC, togglewindow  , NULL},
    {"Window Zooming"                 , MENU_FUNC, togglezooms   , NULL},
#ifdef	CHANGE_FONT
    {"Set Font"                       , MENU_FUNC, setfont       , NULL},
#endif
#ifdef	CHANGE_COLOR
    {"Set Text Rendering Mode"        , MENU_FUNC, tttext        , NULL},
    {"Set Text Foreground Color"      , MENU_FUNC, textforeground, NULL},
    {"Set Text Background Color"      , MENU_FUNC, textbackground, NULL},
    {"Set Modeline Rendering Mode"    , MENU_FUNC, ttmode        , NULL},
    {"Set Modeline Foreground Color"  , MENU_FUNC, modeforeground, NULL},
    {"Set Modeline Background Color"  , MENU_FUNC, modebackground, NULL},
#endif	/* CHANGE_COLOR */
    {NULL                             , MENU_END , NULL          , NULL}
};
#endif	/* AMIGA */

MenuMap MgEtcMenus[] =
{
#ifndef	NO_MACRO
    {"Execute Kbd Macro  (C-x e)"  , MENU_FUNC, executemacro, NULL},
    {"----------------------------", MENU_LINE, NULL        , NULL},
#endif
    {"Auto Fill Mode"              , MENU_FUNC, fillmode    , NULL},
    {"Auto Indent Mode"            , MENU_FUNC, indentmode  , NULL},
    {"Overwrite Mode"              , MENU_FUNC, overwrite   , NULL},
#ifdef	CMODE
    {"C Mode"                      , MENU_FUNC, cmode       , NULL},
#endif
#ifdef	JUMPERR
    {"Compile..."                  , MENU_FUNC, compile     , NULL},
#endif
#ifdef	INCLUDE_SKG
    {"----------------------------", MENU_LINE, NULL        , NULL},
    {"Simple Kanji Generator..."   , MENU_FUNC, skginput    , NULL},
#endif
#if	defined(AMIGA)||defined(KANJI)
    {"----------------------------", MENU_LINE, NULL        , NULL},
#endif
#ifdef	KANJI
    {"Set Default Kanji Code    " MENU_SUB_ICON,
                                   MENU_SUB , MgKanjiMenus, NULL},
#endif
#ifdef	AMIGA
    {"Set Amiga                 " MENU_SUB_ICON,
                                   MENU_SUB , MgAmigaMenus, NULL},
#endif
    {NULL                        , MENU_END , NULL        , NULL}
};

#ifndef	NO_HELP
MenuMap MgHelpMenus[] =
{
    {"Command Apropos...   (C-h a)", MENU_FUNC, apropos_command, NULL},
    {"Decscribe Bindings   (C-h b)", MENU_FUNC, wallchart      , NULL},
    {"Decscribe Key        (C-h c)", MENU_FUNC, desckey        , NULL},
    {"Show Compile Options"        , MENU_FUNC, showngversion  , NULL},
    {"Show Version"                , MENU_FUNC, showversion    , NULL},
    {NULL                          , MENU_END , NULL           , NULL}
};
#endif

#ifdef	AMIGA
#ifdef	ASL
MenuMap MgAslMenus[] =
{
    {"Open File"  , MENU_FUNC, aslopen  , NULL},
    {"Insert File", MENU_FUNC, aslinsert, NULL},
    {"Write File" , MENU_FUNC, aslwrite , NULL},
    {NULL         , MENU_END , NULL     , NULL}
};
#endif	/* ASL */
#endif	/* AMIGA */

MenuMap MgMenus[] =
{
    {"File"    , MENU_SUB, MgFileMenus  , NULL},
    {"Edit"    , MENU_SUB, MgEditMenus  , NULL},
    {"Search"  , MENU_SUB, MgSearchMenus, NULL},
    {"Etc."    , MENU_SUB, MgEtcMenus   , NULL},
#ifndef	NO_HELP
    {"Help"    , MENU_SUB, MgHelpMenus  , NULL},
#endif
#ifdef	AMIGA
#ifdef	ASL
    {"ASL req.", MENU_SUB, MgAslMenus   , NULL},
#endif	/* ASL */
#endif	/* AMIGA */
    {NULL      , MENU_END, NULL         , NULL}
};

