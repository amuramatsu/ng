/* $Id: keymap.c,v 1.9.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 * Keyboard maps.  This is character set dependent.
 * The terminal specific parts of building the
 * keymap has been moved to a better place.
 */

/*
 * $Log: keymap.c,v $
 * Revision 1.9.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.9  2001/04/28 18:54:27  amura
 * support line-number-mode (based on MATSUURA's patch )
 *
 * Revision 1.8  2001/03/02 08:49:04  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.7  2000/11/16 14:31:12  amura
 * fix some typos which cause compile error when using
 * strict ANSI-C compiler (ex ACK, gcc-1.x)
 *
 * Revision 1.6  2000/09/21 17:28:30  amura
 * replace macro _WIN32 to WIN32 for Cygwin
 *
 * Revision 1.5  2000/09/01 19:45:16  amura
 * add default keybind for "undo"
 *
 * Revision 1.4  2000/06/27 01:49:44  amura
 * import to CVS
 *
 * Revision 1.3  2000/06/01  05:30:09  amura
 * Undo support
 *
 * Revision 1.2  2000/03/10  21:29:32  amura
 * some function added
 *
 * Revision 1.1  1999/05/19  03:59:01  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	"kbd.h"

#define _PF() pro((int, int))

/*
 * Defined by "basic.c".
 */
extern	int	gotobol _PF();		/* Move to start of line	*/
extern	int	backchar _PF();		/* Move backward by characters	*/
extern	int	gotoeol _PF();		/* Move to end of line		*/
extern	int	forwchar _PF();		/* Move forward by characters	*/
extern	int	gotobob _PF();		/* Move to start of buffer	*/
extern	int	gotoeob _PF();		/* Move to end of buffer	*/
extern	int	forwline _PF();		/* Move forward by lines	*/
extern	int	backline _PF();		/* Move backward by lines	*/
extern	int	forwpage _PF();		/* Move forward by pages	*/
extern	int	backpage _PF();		/* Move backward by pages	*/
extern	int	pagenext _PF();		/* Page forward next window	*/
extern	int	setmark _PF();		/* Set mark			*/
extern	int	swapmark _PF();		/* Swap "." and mark		*/
extern	int	gotoline _PF();		/* Go to a specified line.	*/
#ifdef	GOSMACS
extern	int	forw1page _PF();	/* move forward by lines	*/
extern	int	back1page _PF();	/* move back by lines		*/
#endif
#ifdef	NEXTLINE
extern	int	nextline _PF();		/* next-line-noexpand		*/
#endif
#ifdef	ADDFUNC
extern	int	linenumbermode _PF();	/* line-number-mode		*/
#endif

/*
 * Defined by "buffer.c".
 */
extern	int	listbuffers _PF();	/* Display list of buffers	*/
extern	int	usebuffer _PF();	/* Switch a window to a buffer	*/
extern	int	poptobuffer _PF();	/* Other window to a buffer	*/
extern	int	killbuffer _PF();	/* Make a buffer go away.	*/
extern	int	savebuffers _PF();	/* Save unmodified buffers	*/
extern	int	bufferinsert _PF();	/* Insert buffer into another	*/
extern	int	notmodified _PF();	/* Reset modification flag	*/
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
extern	int	togglereadonly _PF();	/* Toggle read-only flag	*/
#endif	/* READONLY */
#ifdef  VARIABLE_TAB
extern	int	set_default_tabwidth _PF();/* set default tab width	*/
extern	int	set_tabwidth _PF();	/* set local tab width		*/
extern	int	set_cmode_tabwidth _PF();/* set c-mode tab width	*/
#endif  /* VARIABLE_TAB */
#ifdef	BUFFER_MODE
extern	int	b_del _PF();		/* buffer-mode mark for kill	*/
extern	int	b_undel _PF();		/* buffer-mode unmark		*/
extern	int	b_expunge _PF();	/* buffer-mode kill marked buff	*/
extern	int	b_thiswin _PF();	/* buffer-mode on this window	*/
#endif


/*
 * Defined by "cmode.c".
 */
#ifdef	C_MODE	/* 90.07.24  by K.Takano */
extern	int	cm_use_c_mode _PF();	/* use-c-mode			*/
extern	int	cm_brace _PF();		/* c-mode electric-c-brace	*/
extern	int	cm_brace_blink _PF();	/* c-mode electric-c-brace-blink*/
extern	int	cm_semi _PF();		/* c-mode electric-c-semi	*/
extern	int	cm_term _PF();		/* c-mode electric-c-terminator	*/
extern	int	cm_indent _PF();	/* c-mode indent line		*/
extern	int	cm_lfindent _PF();	/* c-mode newline and indent	*/
extern	int	cm_set_indent _PF();	/* set-c-indent-level		*/
extern	int	cm_set_imagin _PF();	/* set-c-brace-imaginary-offset	*/
extern	int	cm_set_brace _PF();	/* set-c-brace-offset		*/
extern	int	cm_set_arg _PF();	/* set-c-argdecl-indent		*/
extern	int	cm_set_label _PF();	/* set-c-label-offset		*/
extern	int	cm_set_cstat _PF();	/* set-c-continued-statement-offset*/
extern	int	cm_set_cbrace _PF();	/* set-c-continued-brace-offset	*/
extern	int	cm_set_newl _PF();	/* set-c-auto-newline		*/
extern	int	cm_set_tab _PF();	/* set-c-tab-always-indent	*/
extern	int	cm_list_var _PF();	/* list-c-mode-variables	*/
extern	int	cm_indentregion _PF();	/* c-indent-region  Y.Koyanagi	*/
#endif	/* C_MODE */

#ifndef NO_DIR
/*
 * Defined by "dir.c"
 */
extern	int	changedir _PF();	/* change current directory	*/
extern	int	showcwdir _PF();	/* show current directory	*/

#ifndef NO_DIRED
/*
 * defined by "dired.c"
 */
extern	int	dired _PF();		/* dired			*/
extern	int	d_findfile _PF();	/* dired find file		*/
extern	int	d_del _PF();		/* dired mark for deletion	*/
extern	int	d_undel _PF();		/* dired unmark			*/
extern	int	d_undelbak _PF();	/* dired unmark backwards	*/
extern	int	d_expunge _PF();	/* dired expunge		*/
extern	int	d_copy _PF();		/* dired copy			*/
extern	int	d_rename _PF();		/* dired rename			*/
extern	int	d_otherwindow _PF();	/* dired other window		*/
extern	int	d_ffotherwindow _PF();	/* dired find file other window */
extern	int	d_flag _PF();		/* dired mark backup files	*/
extern	int	d_viewfile _PF();	/* dired find file read only	*/
extern	int	d_execute _PF();	/* dired execute command (win32)*/
#endif
#endif

/*
 * Defined by "extend.c".
 */
extern	int	extend _PF();		/* Extended commands.		*/
extern	int	bindtokey _PF();	/* Modify global key bindings.	*/
extern	int	localbind _PF();	/* Modify mode key bindings.	*/
extern	int	define_key _PF();	/* modify any key map		*/
extern	int	unbindtokey _PF();	/* delete global binding	*/
extern	int	localunbind _PF();	/* delete local binding		*/
extern	int	insert _PF();		/* insert string		*/
#ifndef NO_STARTUP
extern	int	evalexpr _PF();		/* Extended commands (again)	*/
extern	int	evalbuffer _PF();	/* Evaluate current buffer	*/
extern	int	evalfile _PF();		/* Evaluate a file		*/
#endif

/*
 * Defined by "file.c".
 */
extern	int	filevisit _PF();	/* Get a file, read write	*/
extern	int	poptofile _PF();	/* Get a file, other window	*/
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
extern	int	filereadonly _PF();	/* Get a file, read only	*/
#endif	/* READONLY */
extern	int	filealternate _PF();	/* Get a file, in this buffer	*/
extern	int	filewrite _PF();	/* Write a file			*/
extern	int	filesave _PF();		/* Save current file		*/
extern	int	fileinsert _PF();	/* Insert file into buffer	*/
#ifndef NO_BACKUP
extern	int	makebkfile _PF();	/* Control backups on saves	*/
#endif

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
extern	int	jumptoerror _PF();	/* parse current line as error message */
extern	int	nexterror _PF();	/* next-error Y.Koyanagi/amura */
extern	int	compile _PF();		/* compile  Y.Koyanagi */
#endif
/*
 * defined by "kanji.c"
 */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
extern	int	k_rot_fio _PF();	/* Change global file I/O code.	*/
extern	int	k_set_fio _PF();	/* Set global file I/O code.	*/
extern	int	k_rot_buffio _PF();	/* Change buffer file I/O code.	*/
extern	int	k_set_buffio _PF();	/* Set buffer file I/O code.	*/
extern	int	k_set_expect _PF();	/* Set expected file input code. */
extern	int	k_rot_input _PF();	/* Change keyboard input code.	*/
extern	int	k_set_input _PF();	/* Set keyboard input code.	*/
extern	int	k_rot_display _PF();	/* Change KANJI display code.	*/
extern	int	k_set_display _PF();	/* Set KANJI display code.	*/
extern	int	k_list_code _PF();	/* List each KANJI code values.	*/
extern	int	k_show_code _PF();	/* Show each KANJI code values.	*/
extern	int	k_set_tokfio _PF();	/* Set file KANJI select char.	*/
extern	int	k_set_toafio _PF();	/* Set file ASCII select char.	*/
extern	int	k_set_tokdisplay _PF();	/* Set disp KANJI select char.	*/
extern	int	k_set_toadisplay _PF();	/* Set disp ASCII select char.	*/
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
extern	int	k_set_tokanafio _PF();	/* Set file KANA select char.	*/
extern	int	k_set_tokanadisplay _PF();/* Set disp KANA select char.	*/
#endif  /* HANKANA */
#endif	/* KANJI */

/*
 * defined by "kbd.c"
 */
#ifdef	DO_METAKEY
extern	int	do_meta _PF();		/* interpret meta keys		*/
#endif
#ifdef	BSMAP
extern	int	bsmap _PF();		/* backspace mapping		*/
#endif
extern	int	universal_argument _PF();/* Ctrl-U			*/
extern	int	digit_argument _PF();	/* M-1, etc.			*/
extern	int	negative_argument _PF();/* M--				*/
extern	int	selfinsert _PF();	/* Insert character		*/
extern	int	rescan _PF();		/* internal try again function	*/

/*
 * defined by "kinsoku.c"
 */
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
extern	int	kc_list_char _PF();	/* List BOL/EOL KINSOKU chars.	*/
extern	int	kc_add_bol _PF();	/* Add BOL KINSOKU chars.	*/
extern	int	kc_del_bol _PF();	/* Delete BOL KINSOKU chars.	*/
extern	int	kc_add_eol _PF();	/* Add EOL KINSOKU chars.	*/
extern	int	kc_del_eol _PF();	/* Delete EOL KINSOKU chars.	*/
#endif	/* KINSOKU */

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
extern	int	definemacro _PF();	/* Begin macro			*/
extern	int	finishmacro _PF();	/* End macro			*/
extern	int	executemacro _PF();	/* Execute macro		*/
#endif

/*
 * Defined by "main.c".
 */
extern	int	ctrlg _PF();		/* Abort out of things		*/
extern	int	quit _PF();		/* Quit				*/

/*
 * Defined by "match.c"
 */
extern	int	showmatch _PF();	/* Hack to show matching paren	 */
#ifdef	GOMATCH
extern	int	gotomatch _PF();	/* goto-matching-fence a la kemacs */
#endif

/* defined by "modes.c" */

extern	int	indentmode _PF();	/* set auto-indent mode		*/
extern	int	fillmode _PF();		/* set word-wrap mode		*/
extern	int	blinkparen _PF();	/* Fake blink-matching-paren var */
extern	int	notabmode _PF();	/* no tab mode			*/
extern	int	overwrite _PF();	/* overwrite mode		*/
#ifdef	C_MODE	/* 90.07.24  by K.Takano */
extern	int	cmode _PF();		/* set c-mode			*/
#endif	/* C_MODE */
extern	int	set_default_mode _PF();	/* set default modes		*/

/*
 * defined by "paragraph.c" - the paragraph justification code.
 */
extern	int	gotobop _PF();		/* Move to start of paragraph.	*/
extern	int	gotoeop _PF();		/* Move to end of paragraph.	*/
extern	int	fillpara _PF();		/* Justify a paragraph.		*/
extern	int	killpara _PF();		/* Delete a paragraph.		*/
extern	int	setfillcol _PF();	/* Set fill column for justify. */
extern	int	fillword _PF();		/* Insert char with word wrap.	*/
#ifdef	FILLPREFIX	/* 90.12.30  by S.Yoshida */
extern	int	setfillprefix _PF();	/* Set fill prefix strings.	*/
#endif	/* FILLPREFIX */

/*
 * Defined by "random.c".
 */
extern	int	showcpos _PF();		/* Show the cursor position	*/
extern	int	twiddle _PF();		/* Twiddle characters		*/
extern	int	quote _PF();		/* Insert literal		*/
extern	int	openline _PF();		/* Open up a blank line		*/
extern	int	newline _PF();		/* Insert newline		*/
extern	int	deblank _PF();		/* Delete blank lines		*/
extern	int	justone _PF();		/* Delete extra whitespace	*/
extern	int	delwhite _PF();		/* Delete all whitespace	*/
extern	int	indent _PF();		/* Insert newline, then indent	*/
extern	int	forwdel _PF();		/* Forward delete		*/
extern	int	backdel _PF();		/* Backward delete in		*/
extern	int	killline _PF();		/* Kill forward			*/
extern	int	killoneline _PF();	/* Kill current line		*/
extern	int	yank _PF();		/* Yank back from killbuffer.	*/
extern	int	space_to_tabstop _PF();
#ifdef	ZAPTOC_A
# define ZAPTOCHAR
#endif
#ifdef ZAPTOCHAR
extern	int	zaptochar _PF();	/* Kill up to CHAR              */
#endif
#ifdef	ADDFUNC	/* 90.02.15  by S.Yoshida */
extern	int	pagelines _PF();	/* Count lines in the page.	*/
extern	int	regionlines _PF();	/* Count lines in the region.	*/
#endif	/* ADDFUNC */

#ifdef	REGEX
/*
 * Defined by "re_search.c"
 */
extern	int	re_forwsearch _PF();	/* Regex search forward		 */
extern	int	re_backsearch _PF();	/* Regex search backwards	 */
extern	int	re_searchagain _PF();	/* Repeat regex search command	 */
extern	int	re_queryrepl _PF();	/* Regex query replace		 */
extern	int	setcasefold _PF();	/* Set case fold in searches	 */
extern	int	delmatchlines _PF();	/* Delete all lines matching	 */
extern	int	delnonmatchlines _PF();	/* Delete all lines not matching */
extern	int	cntmatchlines _PF();	/* Count matching lines		 */
extern	int	cntnonmatchlines _PF();	/* Count nonmatching lines	 */
#endif

/*
 * Defined by "region.c".
 */
extern	int	killregion _PF();	/* Kill region.			*/
extern	int	copyregion _PF();	/* Copy region to kill buffer.	*/
extern	int	lowerregion _PF();	/* Lower case region.		*/
extern	int	upperregion _PF();	/* Upper case region.		*/
extern	int	copybuffer _PF();	/* Copy buffer to kill buffer	*/
#ifdef	PREFIXREGION
extern	int	prefixregion _PF();	/* Prefix all lines in region	*/
extern	int	setprefix _PF();	/* Set line prefix string	*/
#endif

/*
 * Defined by "search.c".
 */
extern	int	forwsearch _PF();	/* Search forward		*/
extern	int	backsearch _PF();	/* Search backwards		*/
extern	int	searchagain _PF();	/* Repeat last search command	*/
extern	int	forwisearch _PF();	/* Incremental search forward	*/
extern	int	backisearch _PF();	/* Incremental search backwards */
extern	int	queryrepl _PF();	/* Query replace		*/

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
/*
 * Defined by "shell.c"
 */
extern	int	shellcmnd _PF();	/* Get shell command result.	*/
#endif	/* NO_SHELL */

/*
 * Defined by "spawn.c".
 */
extern	int	spawncli _PF();		/* Run CLI in a subjob.		*/
#ifdef	VMS
extern	int	attachtoparent _PF();	/* Attach to parent process	*/
#endif

/*
 * Defined by "ttyio.c".
 */
#ifdef	IBMPC	/* 91.01.11  by S.Yoshida */
extern	int	j31_set_cursor _PF();	/* Set J-3100 cursor mode.	*/
#endif	/* IBMPC */
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
extern	int	fepmode_set _PF();	/* Set FEP control mode		*/
extern	int	fepmode_chg _PF();	/* Change FEP control mode	*/
extern  int     fepmode_toggle _PF();	/* Toggle FEP on/off		*/
#endif

/* defined by "version.c" */

extern	int	showversion();		/* Show version numbers, etc.	*/
#ifdef	ADDFUNC	/* 90.12.28  by S.Yoshida */
extern	int	showngversion();	/* Show Ng version, etc.	*/
#endif	/* ADDFUNC */

/*
 * Defined by "window.c".
 */
extern	int	reposition _PF();	/* Reposition window		*/
extern	int	refresh _PF();		/* Refresh the screen		*/
extern	int	nextwind _PF();		/* Move to the next window	*/
#ifdef	GOSMACS
extern	int	prevwind _PF();		/* Move to the previous window	*/
#endif
extern	int	onlywind _PF();		/* Make current window only one */
extern	int	splitwind _PF();	/* Split current window		*/
extern	int	delwind _PF();		/* Delete current window	*/
extern	int	enlargewind _PF();	/* Enlarge display window.	*/
extern	int	shrinkwind _PF();	/* Shrink window.		*/

/*
 * Defined by "word.c".
 */
extern	int	backword _PF();		/* Backup by words		*/
extern	int	forwword _PF();		/* Advance by words		*/
extern	int	upperword _PF();	/* Upper case word.		*/
extern	int	lowerword _PF();	/* Lower case word.		*/
extern	int	capword _PF();		/* Initial capitalize word.	*/
extern	int	delfword _PF();		/* Delete forward word.		*/
extern	int	delbword _PF();		/* Delete backward word.	*/

/*
 * Defined by "version.c".
 */
extern	int showversion _PF();		/* Show ng version breifry	*/
extern	int showngversion _PF();	/* Show ng version verbose	*/

/*
 * Defined by "skg.c".
 */
#ifdef INCLUDE_SKG
extern	int	skginput _PF();		/* Get Kanji strings.		*/
extern	int	skg_set_romanname _PF();/* Set Roman->Kana dict name	*/
extern	int	skg_set_dicname _PF();	/* Set Kana->Kanji dict name	*/
#endif

/*
 * Defined by "autosave.c".
 */
#ifdef AUTOSAVE
extern	int	as_set_interval _PF();	/* Autosave interval time	*/
#endif

#ifdef	WIN32
#ifndef NO_STARTUP
extern int ConfigStartupFilePath _PF();
#endif
extern int tticon _PF();
#endif

#ifdef	AMIGA
#ifdef	DO_ICONIFY
extern	int	tticon _PF();
#endif
#ifdef	DO_MENU
extern	int	amigamenu _PF();	/* Menu function		*/
#endif
#ifdef	MOUSE
extern	int	amigamouse _PF();	/* Amiga mouse functions	*/
extern	int	mgotobob _PF();
extern	int	mforwdel _PF();
extern	int	mdelwhite _PF();
extern	int	mdelwind _PF();
extern	int	mgotoeob _PF();
extern	int	menlargewind _PF();
extern	int	mkillline _PF();
extern	int	mkillregion _PF();
extern	int	mdelfword _PF();
extern	int	mreposition _PF();
extern	int	mbackpage _PF();
extern	int	mforwpage _PF();
extern	int	mshrinkwind _PF();
extern	int	msplitwind _PF();
extern	int	myank _PF();
#endif	/* MOUSE */

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

/*
 * This file contains map segment definitions for adding function keys to
 * keymap declarations.  Currently you can add things to the fundamental
 * mode keymap and the dired mode keymap.  See the declaration of
 * diredmap and fundmap for details.
 */
#include "amiga_maps.c"

#endif	/* AMIGA */

#ifdef CANNA
/*
 * Defined by "canna.c".
 */
extern int canna_toggle();
#endif

/* initial keymap declarations, deepest first */

#ifndef NO_HELP
static	PF	cHcG[] = {
	ctrlg,		/* ^G */
	help_help,	/* ^H */
};
static	PF	cHa[]	= {
	apropos_command,/* a */
	wallchart,	/* b */
	desckey,	/* c */
};
static	struct	KEYMAPE(2+IMAPEXT)	helpmap = {
	2,
	2+IMAPEXT,
	rescan,
	{
		{CCHR('G'),CCHR('H'),	cHcG,	(KEYMAP *)NULL},
		{'a',	'c',		cHa,	(KEYMAP *)NULL},
	}
};
#endif

#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
static	PF	cXcKcG[] = {
	ctrlg,		/* ^G */
};
static	PF	cXcKC[] = {
	k_list_code,	/* C */
	rescan,		/* D */
	rescan,		/* E */
	k_rot_fio,	/* F */
};
static	PF	cXcKc[] = {
	k_show_code,	/* c */
	k_rot_display,	/* d */
	rescan,		/* e */
	k_rot_buffio,	/* f */
	rescan,		/* g */
	rescan,		/* h */
	k_rot_input,	/* i */
};
static	struct	KEYMAPE(3+IMAPEXT)	cXcKmap	= {
	3,
	3+IMAPEXT,
	rescan,
	{
		{CCHR('G'), CCHR('G'),	cXcKcG,	(KEYMAP *)NULL},
		{'C',	'F',		cXcKC,	(KEYMAP *)NULL},
		{'c',	'i',		cXcKc,	(KEYMAP *)NULL},
	}
};
#endif	/* KANJI */

static	PF	cX4cF[] = {
	poptofile,	/* ^f */
	ctrlg,		/* ^g */
};
static	PF	cX4b[] = {
	poptobuffer,	/* b */
	rescan,		/* c */
	rescan,		/* d */
	rescan,		/* e */
	poptofile,	/* f */
};
static	struct	KEYMAPE(2+IMAPEXT)	cX4map	= {
	2,
	2+IMAPEXT,
	rescan,
	{
		{CCHR('F'),CCHR('G'),	cX4cF,	(KEYMAP *)NULL},
		{'b',	'f',		cX4b,	(KEYMAP *)NULL},
	}
};

static	PF	cXcB[] = {
	listbuffers,	/* ^B */
	quit,		/* ^C */
	rescan,		/* ^D */
	rescan,		/* ^E */
	filevisit,	/* ^F */
	ctrlg,		/* ^G */
};
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
static	PF	cXcK[] = {
	prefix,		/* ^K */
#else	/* NOT KANJI */
static	PF	cXcL[] = {
#endif	/* KANJI */
	lowerregion,	/* ^L */
	rescan,		/* ^M */
	rescan,		/* ^N */
	deblank,	/* ^O */
	rescan,		/* ^P */
#ifdef	READONLY
	togglereadonly,	/* ^Q */
	filereadonly,	/* ^R */
#else	/* NOT READONLY */
	rescan,		/* ^Q */
	rescan,		/* ^R */
#endif	/* READONLY */
	filesave,	/* ^S */
	rescan,		/* ^T */
	upperregion,	/* ^U */
	filealternate,	/* ^V */
	filewrite,	/* ^W */
	swapmark,	/* ^X */
};
#ifndef NO_MACRO
static	PF	cXlp[]	= {
	definemacro,	/* ( */
	finishmacro,	/* ) */
};
#endif
#ifdef	FILLPREFIX	/* 90.12.30  by S.Yoshida */
static	PF	cXdot[]	= {
	setfillprefix,	/* . */
	rescan,		/* / */
#else	/* NOT FILLPREFIX */
static	PF	cX0[]	= {
#endif	/* FILLPREFIX */
	delwind,	/* 0 */
	onlywind,	/* 1 */
	splitwind,	/* 2 */
	rescan,		/* 3 */
	prefix,		/* 4 */
};
static	PF	cXeq[]	= {
	showcpos,	/* = */
};
static	PF	cXcar[] = {
	enlargewind,	/* ^ */
	rescan,		/* _ */
#ifdef JUMPERR
	jumptoerror,	/* ` */
#else
	rescan,		/* ` */
#endif
	rescan,		/* a */
	usebuffer,	/* b */
	rescan,		/* c */
#ifndef NO_DIRED
	dired,		/* d */
#else
	rescan,		/* d */
#endif
#ifndef NO_MACRO
	executemacro,	/* e */
#else
	rescan,		/* e */
#endif
	setfillcol,	/* f */
	rescan,		/* g */
	rescan,		/* h */
	fileinsert,	/* i */
	rescan,		/* j */
	killbuffer,	/* k */
#ifdef	ADDFUNC	/* 90.02.15  by S.Yoshida */
	pagelines,	/* l */
#else	/* NOT ADDFUNC */
	rescan,		/* l */
#endif	/* ADDFUNC */
	rescan,		/* m */
	rescan,		/* n */
	nextwind,	/* o */
	rescan,		/* p */
	rescan,		/* q */
	rescan,		/* r */
	savebuffers,	/* s */
#ifdef	UNDO
	rescan,		/* t */
	do_undo,	/* u */
#endif
};
#ifndef NO_MACRO
static	struct	KEYMAPE(6+IMAPEXT)	cXmap = {
	6,
	6+IMAPEXT,
#else
static	struct	KEYMAPE(5+IMAPEXT)	cXmap = {
	5,
	5+IMAPEXT,
#endif
	rescan,
	{
		{CCHR('B'),CCHR('G'),	cXcB,	(KEYMAP *)NULL},
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
		{CCHR('K'),CCHR('X'),	cXcK,	(KEYMAP *)&cXcKmap},
#else	/* NOT KANJI */
		{CCHR('L'),CCHR('X'),	cXcL,	(KEYMAP *)NULL},
#endif	/* KANJI */
#ifndef NO_MACRO
		{'(',	')',		cXlp,	(KEYMAP *)NULL},
#endif
#ifdef	FILLPREFIX	/* 90.12.30  by S.Yoshida */
		{'.',	'4',		cXdot,	(KEYMAP *)&cX4map},
#else	/* NOT FILLPREFIX */
		{'0',	'4',		cX0,	(KEYMAP *)&cX4map},
#endif	/* FILLPREFIX */
		{'=',	'=',		cXeq,	(KEYMAP *)NULL},
#ifdef	UNDO
		{'^',	'u',		cXcar,	(KEYMAP *)NULL},
#else
		{'^',	's',		cXcar,	(KEYMAP *)NULL},
#endif
	}
};

static	PF	metacG[] = {
	ctrlg,		/* ^G */
};
static	PF	metacV[] = {
	pagenext,	/* ^V */
};
static	PF	metasp[] = {
	justone,	/* space */
#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
	shellcmnd,	/* ! */
#endif	/* NO_SHELL */
};
static	PF	metapct[] = {
	queryrepl,	/* % */
};
static	PF	metami[] = {
	negative_argument,	/* - */
	rescan,		/* . */
	rescan,		/* / */
	digit_argument, /* 0 */
	digit_argument, /* 1 */
	digit_argument, /* 2 */
	digit_argument, /* 3 */
	digit_argument, /* 4 */
	digit_argument, /* 5 */
	digit_argument, /* 6 */
	digit_argument, /* 7 */
	digit_argument, /* 8 */
	digit_argument, /* 9 */
	rescan,		/* : */
	rescan,		/* ; */
	gotobob,	/* < */
#ifdef	ADDFUNC	/* 90.02.15  by S.Yoshida */
	regionlines,	/* = */
#else	/* NOT ADDFUNC */
	rescan,		/* = */
#endif	/* ADDFUNC */
	gotoeob,	/* > */
};
static	PF	metalb[] = {
	gotobop,	/* [ */
	delwhite,	/* \ */
	gotoeop,	/* ] */
	rescan,		/* ^ */
	rescan,		/* _ */
	rescan,		/* ` */
	rescan,		/* a */
	backword,	/* b */
	capword,	/* c */
	delfword,	/* d */
	rescan,		/* e */
	forwword,	/* f */
};
static	PF	metal[] = {
	lowerword,	/* l */
	rescan,		/* m */
	rescan,		/* n */
	rescan,		/* o */
	rescan,		/* p */
	fillpara,	/* q */
	backsearch,	/* r */
	forwsearch,	/* s */
	rescan,		/* t */
	upperword,	/* u */
	backpage,	/* v */
	copyregion,	/* w */
	extend,		/* x */
#ifdef ZAPTOCHAR
        rescan,  	/* y */
        zaptochar,	/* z */
#endif	
};
static	PF	metatilde[] = {
	notmodified,	/* ~ */
	delbword,	/* DEL */
};
#ifdef INCLUDE_SKG 
static	PF	metacK[] = {
	skginput,	/* ^K */
};
#define XSKGMAP 1
#else
#define XSKGMAP 0
#endif
static	struct	KEYMAPE(8+IMAPEXT+XSKGMAP)	metamap = {
	8+XSKGMAP,
	8+IMAPEXT+XSKGMAP,
	rescan,
	{
		{CCHR('G'),CCHR('G'),	metacG, (KEYMAP *)NULL},
#ifdef INCLUDE_SKG 
		{CCHR('K'),CCHR('K'),  	metacK, (KEYMAP *)NULL},
#endif
		{CCHR('V'),CCHR('V'),	metacV, (KEYMAP *)NULL},
#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
		{' ',	'!',		metasp, (KEYMAP *)NULL},
#else	/* NO_SHELL */
		{' ',	' ',		metasp, (KEYMAP *)NULL},
#endif	/* NO_SHELL */
		{'%',	'%',		metapct,(KEYMAP *)NULL},
		{'-',	'>',		metami, (KEYMAP *)NULL},
		{'[',	'f',		metalb, (KEYMAP *)NULL},
#ifndef ZAPTOCHAR
		{'l',	'x',		metal,	(KEYMAP *)NULL},
#else
		{'l',	'z',		metal,	(KEYMAP *)NULL},
#endif	    
		{'~',	CCHR('?'),	metatilde,(KEYMAP *)NULL},
	}
};

static	PF	fund_at[] = {
	setmark,	/* ^@ */
	gotobol,	/* ^A */
	backchar,	/* ^B */
	rescan,		/* ^C */
	forwdel,	/* ^D */
	gotoeol,	/* ^E */
	forwchar,	/* ^F */
	ctrlg,		/* ^G */
#ifndef NO_HELP
	prefix,		/* ^H */
#else
	rescan,		/* ^H */
#endif
};
/* ^I is selfinsert */
static	PF	fund_CJ[] = {
	indent,		/* ^J */
	killline,	/* ^K */
	reposition,	/* ^L */
	newline,	/* ^M */
	forwline,	/* ^N */
#ifdef CANNA
	canna_toggle,	/* ^O */
#else
	openline,	/* ^O */
#endif
	backline,	/* ^P */
	quote,		/* ^Q */
	backisearch,	/* ^R */
	forwisearch,	/* ^S */
	twiddle,	/* ^T */
	universal_argument,	/* ^U */
	forwpage,	/* ^V */
	killregion,	/* ^W */
	prefix,		/* ^X */
	yank,		/* ^Y */
#ifndef VMS
	spawncli,	/* ^Z */
#else
	attachtoparent, /* ^Z */
#endif
};
static	PF	fund_esc[] = {
	prefix,		/* esc */
#ifdef	MSDOS	/* 90.02.20  by S.Yoshida */
#ifdef	PC9801	/* 97.09.11  by amura */
#ifdef	FEPCTRL
	fepmode_toggle,	/* ^\ */	/* ^\ is fep-toggle */
#else
	setmark,	/* ^\ */	/* ^\ is also set-mark-command. */
#endif	/* FEPCTRL */
#else	/* NOT PC9801 */
#ifdef	IBMPC
#ifdef	FEPCTRL
	fepmode_toggle,	/* ^\ */	/* ^\ is fep-toggle */
#else
	setmark,	/* ^\ */	/* ^\ is also set-mark-command. */
#endif	/* FEPCTRL */
#else	/* NOT IBMPC and PC9801 */
	setmark,	/* ^\ */	/* ^\ is also set-mark-command. */
#endif	/* IBMPC */
#endif	/* PC9801 */
#else	/* NOT MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
#ifdef	FEPCTRL
	fepmode_toggle,	/* ^\ */	/* ^\ is fep-toggle */
#else
	setmark,	/* ^\ */	/* ^\ is also set-mark-command. */
#endif	/* FEPCTRL */
#else	/* NOT HUMAN68K */
	rescan,		/* ^\ */	/* selfinsert is default on fundamental */
#endif	/* HUMAN68K */
#endif	/* MSDOS */
	rescan,		/* ^] */
	rescan,		/* ^^ */
#ifdef	UNDO
	do_undo,	/* _ */
#else
	rescan,		/* ^_ */
#endif
};
static	PF	fund_del[] = {
	backdel,	/* DEL */
};

#ifndef	FUND_XMAPS
#define NFUND_XMAPS	0	/* extra map sections after normal ones */
#endif

static	struct	KEYMAPE(4+NFUND_XMAPS+IMAPEXT)	fundmap = {
	4 + NFUND_XMAPS,
	4 + NFUND_XMAPS + IMAPEXT,
	selfinsert,
	{
#ifndef NO_HELP
		{CCHR('@'),CCHR('H'),	fund_at, (KEYMAP *)&helpmap},
#else
		{CCHR('@'),CCHR('H'),	fund_at, (KEYMAP *)NULL},
#endif
		{CCHR('J'),CCHR('Z'),	fund_CJ, (KEYMAP *)&cXmap},
		{CCHR('['),CCHR('_'),	fund_esc,(KEYMAP *)&metamap},
		{CCHR('?'),CCHR('?'),	fund_del,(KEYMAP *)NULL},
#ifdef	FUND_XMAPS
		FUND_XMAPS,
#endif
	}
};

static	PF	fill_sp[] = {
	fillword,	/* ' ' */
};
static struct KEYMAPE(1+IMAPEXT)	fillmap = {
	1,
	1+IMAPEXT,
	rescan,
	{
		{' ',	' ',	fill_sp,	(KEYMAP *)NULL},
	}
};

static	PF	indent_lf[] = {
	newline,	/* ^J */
	rescan,		/* ^K */
	rescan,		/* ^L */
	indent,		/* ^M */
};
static	struct	KEYMAPE(1+IMAPEXT)	indntmap = {
	1,
	1+IMAPEXT,
	rescan,
	{
		{CCHR('J'), CCHR('M'),	indent_lf,	(KEYMAP *)NULL},
	}
};
static	PF	blink_rp[] = {
	showmatch,	/* ) */
};
static	struct	KEYMAPE(1+IMAPEXT)	blinkmap = {
	1,
	1+IMAPEXT,
	rescan,
	{
		{')',	')',	blink_rp,	(KEYMAP *)NULL},
	}
};


static	PF	notab_tab[] = {
	space_to_tabstop,	/* ^I */
};
static	struct	KEYMAPE(1+IMAPEXT)	notabmap = {
	1,
	1+IMAPEXT,
	rescan,
	{
		{CCHR('I'),CCHR('I'),	notab_tab,	(KEYMAP *)NULL},
	}
};

static	struct	KEYMAPE(1+IMAPEXT)	overwmap = {
	0,
	1+IMAPEXT,			/* 1 to avoid 0 sized array */
	rescan,
	{
		/* unused dummy entry for VMS C */
		{(KCHAR)0,	(KCHAR)0, (PF *)NULL,	(KEYMAP *)NULL},
	}
};

#ifndef NO_DIRED
static	PF	dirednul[] = {
	setmark,	/* ^@ */
	gotobol,	/* ^A */
	backchar,	/* ^B */
	rescan,		/* ^C */
	d_del,		/* ^D */
	gotoeol,	/* ^E */
	forwchar,	/* ^F */
	ctrlg,		/* ^G */
#ifndef NO_HELP
	prefix,		/* ^H */
#endif
};
static	PF	diredcl[] = {
	reposition,	/* ^L */
	forwline,	/* ^M */
	forwline,	/* ^N */
	rescan,		/* ^O */
	backline,	/* ^P */
	rescan,		/* ^Q */
	backisearch,	/* ^R */
	forwisearch,	/* ^S */
	rescan,		/* ^T */
	universal_argument, /* ^U */
	forwpage,	/* ^V */
	rescan,		/* ^W */
	prefix,		/* ^X */
};
static	PF	diredcz[] = {
#ifndef VMS
	spawncli,	/* ^Z */
#else
	attachtoparent, /* ^Z */
#endif
	prefix,		/* esc */
	rescan,		/* ^\ */
	rescan,		/* ^] */
	rescan,		/* ^^ */
	rescan,		/* ^_ */
	forwline,	/* SP */
};
static	PF	diredc[] = {
	d_copy,		/* c */
	d_del,		/* d */
	d_findfile,	/* e */
	d_findfile,	/* f */
};
static	PF	diredn[] = {
	forwline,	/* n */
	d_ffotherwindow,/* o */
	backline,	/* p */
	rescan,		/* q */
	d_rename,	/* r */
	rescan,		/* s */
	rescan,		/* t */
	d_undel,	/* u */
#ifdef READONLY
	d_viewfile,	/* v */
#else
	rescan,		/* v */
#endif
	rescan,		/* w */
	d_expunge,	/* x */
};
static	PF	diredf[] = {
 	d_flag,		/* ~ */
};
static	PF	direddl[] = {
	d_undelbak,	/* del */
};

#ifndef	DIRED_XMAPS
#define	NDIRED_XMAPS	0	/* number of extra map sections */
#endif

static	struct	KEYMAPE(7 + NDIRED_XMAPS + IMAPEXT)	diredmap = {
	7 + NDIRED_XMAPS,
	7 + NDIRED_XMAPS + IMAPEXT,
	rescan,
	{
#ifndef NO_HELP
		{CCHR('@'),	CCHR('H'),	dirednul, (KEYMAP *)&helpmap},
#else
		{CCHR('@'),	CCHR('G'),	dirednul, (KEYMAP *)NULL},
#endif
		{CCHR('L'),	CCHR('X'),	diredcl,  (KEYMAP *)&cXmap},
		{CCHR('Z'),	' ',		diredcz,  (KEYMAP *)&metamap},
		{'c',		'f',		diredc,   (KEYMAP *)NULL},
		{'n',		'x',		diredn,   (KEYMAP *)NULL},
		{'~',		'~',		diredf,   (KEYMAP *)NULL},
		{CCHR('?'),	CCHR('?'),	direddl,  (KEYMAP *)NULL},
#ifdef	DIRED_XMAPS
		DIRED_XMAPS,	/* map sections for dired mode keys	*/
#endif
	}
};
#endif

#ifdef	C_MODE	/* 90.07.24  by K.Takano */
static	PF	cmodeci[] = {
	cm_indent,	/* ^I */
	cm_lfindent,	/* ^J */
	rescan,		/* ^K */
	rescan,		/* ^L */
	newline,	/* ^M */
};
static	PF	cmodecolon[] = {
	cm_term,	/* : */
	cm_semi,	/* ; */
};
static	PF	cmodepar[] = {
	cm_brace_blink,	/* { */
	rescan,		/* | */
	cm_brace_blink,	/* } */
};
static	struct	KEYMAPE(3+IMAPEXT)	cmodemap = {
	3,
	3 + IMAPEXT,
	rescan,
	{
		{CCHR('I'), CCHR('M'),	cmodeci,	(KEYMAP *)NULL},
		{':',	    ';',	cmodecolon,	(KEYMAP *)NULL},
		{'{',	    '}',	cmodepar,	(KEYMAP *)NULL},
	}
};
#endif	/* C_MODE */

#ifdef BUFFER_MODE /* 1999.5.30  by Tillanosoft */
static	PF	buffernul[] = {
	setmark,	/* ^@ */
	gotobol,	/* ^A */
	backchar,	/* ^B */
	rescan,		/* ^C */
	b_del,		/* ^D */
	gotoeol,	/* ^E */
	forwchar,	/* ^F */
	ctrlg,		/* ^G */
#ifndef NO_HELP
	prefix,		/* ^H */
#endif
};
static	PF	buffercl[] = {
	reposition,	/* ^L */
 	forwline,	/* ^M */
	forwline,	/* ^N */
	rescan,		/* ^O */
	backline,	/* ^P */
	rescan,		/* ^Q */
	backisearch,	/* ^R */
	forwisearch,	/* ^S */
	rescan,		/* ^T */
	universal_argument, /* ^U */
	forwpage,	/* ^V */
	rescan,		/* ^W */
	prefix,		/* ^X */
};
static	PF	buffercz[] = {
#ifndef VMS
	spawncli,	/* ^Z */
#else
	attachtoparent, /* ^Z */
#endif
	prefix,		/* esc */
	rescan,		/* ^\ */
	rescan,		/* ^] */
	rescan,		/* ^^ */
	rescan,		/* ^_ */
	forwline,	/* SP */
};

/*
  1
  2
  d done
  f done
  g
  k done
  n done
  o
  p done
  u done
  x done
  ~
  %
*/

static	PF	bufferd[] = {
	b_del,		/* d */
	rescan,		/* e */
	b_thiswin,	/* f */
	rescan,		/* g */
	rescan,		/* h */
	rescan,		/* i */
	rescan,		/* j */
	b_del,		/* k */
	rescan,		/* l */
	rescan,		/* m */
	forwline,	/* n */
	rescan,		/* o */
	backline,	/* p */
	rescan,		/* q */
	rescan,		/* r */
	rescan,		/* s */
	rescan,		/* t */
	b_undel,	/* u */
	rescan,		/* v */
	rescan,		/* w */
	b_expunge,	/* x */
};

static struct KEYMAPE(4 + IMAPEXT) bufmodemap = {
  4,
  4 + IMAPEXT,
  rescan,
  {
#ifndef NO_HELP
    {CCHR('@'),	CCHR('H'),	buffernul, (KEYMAP *)&helpmap},
#else
    {CCHR('@'),	CCHR('G'),	buffernul, (KEYMAP *)NULL},
#endif
    {CCHR('L'),	CCHR('X'),	buffercl,  (KEYMAP *)&cXmap},
    {CCHR('Z'),	' ',		buffercz,  (KEYMAP *)&metamap},
    {'d',	'x',		bufferd,   (KEYMAP *)NULL},
  }
};
#endif	/* BUF_MODE */

/* give names to the maps, for use by help etc.
 * If the map is to be bindable, it must also be listed in the
 * function name table below with the same name.
 * Maps created dynamicly currently don't get added here, thus are unnamed.
 * Modes are just named keymaps with functions to add/subtract them from
 * a buffer's list of modes.  If you change a mode name, change it in
 * modes.c also.
 */

MAPS	map_table[] = {
	/* fundamental map MUST be first entry */
	{(KEYMAP *)&fundmap,	"fundamental"},
	{(KEYMAP *)&fillmap,	"fill"},
	{(KEYMAP *)&indntmap,	"indent"},
	{(KEYMAP *)&blinkmap,	"blink"},
	{(KEYMAP *)&notabmap,	"notab"},
	{(KEYMAP *)&overwmap,	"overwrite"},
	{(KEYMAP *)&metamap,	"esc prefix"},
	{(KEYMAP *)&cXmap,	"c-x prefix"},
	{(KEYMAP *)&cX4map,	"c-x 4 prefix"},
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{(KEYMAP *)&cXcKmap,	"kanji-prefix"},
#endif	/* KANJI */
#ifndef NO_HELP
	{(KEYMAP *)&helpmap,	"help"},
#endif
#ifndef NO_DIRED
	{(KEYMAP *)&diredmap,	"dired"},
#endif
#ifdef C_MODE	/* 90.07.24  by K.Takano */
	{(KEYMAP *)&cmodemap,	"C"},
#endif	/* C_MODE */
#ifdef BUFFER_MODE
	{(KEYMAP *)&bufmodemap,	"Buffer Menu"},
#endif
};

#define NMAPS	(sizeof map_table/sizeof(MAPS))
int	nmaps = NMAPS;		/* for use by rebind in extend.c */

char *map_name(map)
KEYMAP *map;
{
	MAPS *mp = &map_table[0];

	do {
	    if(mp->p_map == map) return mp->p_name;
	} while(++mp < &map_table[NMAPS]);
	return (char *)NULL;
}

MAPS *name_mode(name)
char *name;
{
	MAPS *mp = &map_table[0];

	do {
	    if(strcmp(mp->p_name,name)==0) return mp;
	} while(++mp < &map_table[NMAPS]);
	return (MAPS *)NULL;
}

KEYMAP *name_map(name)
char *name;
{
	MAPS *mp;
	return (mp=name_mode(name))==NULL ? (KEYMAP *)NULL : mp->p_map;
}


/* Warning: functnames MUST be in alphabetical order!  (due to binary
 * search in name_function.)  If the function is prefix, it must be listed
 * with the same name in the map_table above.
 */

FUNCTNAMES	functnames[] = {
#ifdef BUFFER_MODE
	{b_del,		"Buffer-menu-delete"},
	{b_expunge,	"Buffer-menu-execute"},
	{b_thiswin,	"Buffer-menu-this-window"},
	{b_undel,	"Buffer-menu-unmark"},
#endif
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
	{kc_add_bol,	"add-kinsoku-bol-chars"},
	{kc_add_eol,	"add-kinsoku-eol-chars"},
#endif	/* KINSOKU */
#ifdef	AMIGA
#ifdef	DO_ICONIFY
	{tticon,	"amiga-iconify"},
#endif
#ifdef	DO_MENU
	{amigamenu,	"amiga-menu"},
#endif
#ifdef	CHANGE_COLOR
	{modebackground,"amiga-mode-background"},
	{modeforeground,"amiga-mode-foreground"},
	{ttmode,	"amiga-mode-rendition"},
#endif
#ifdef	CHANGE_FONT
	{setfont,	"amiga-set-font"},
#endif
#ifdef	CHANGE_COLOR
	{textbackground,"amiga-text-background"},
	{textforeground,"amiga-text-foreground"},
	{tttext,	"amiga-text-rendition"},
#endif
	{togglewindow,	"amiga-toggle-border"},
	{togglezooms,	"amiga-zoom-mode"},
#endif	/* AMIGA */
#ifndef	NO_HELP
	{apropos_command, "apropos"},
#endif
	{fillmode,	"auto-fill-mode"},
	{indentmode,	"auto-indent-mode"},
	{backchar,	"backward-char"},
	{delbword,	"backward-kill-word"},
	{gotobop,	"backward-paragraph"},
	{backword,	"backward-word"},
	{gotobob,	"beginning-of-buffer"},
	{gotobol,	"beginning-of-line"},
	{blinkparen,	"blink-matching-paren"},
	{showmatch,	"blink-matching-paren-hack"},
#ifdef	BSMAP
	{bsmap,		"bsmap-mode"},
#endif
#ifdef C_MODE	/* 90.07.24  by K.Takano */
	{cm_indent,	"c-indent-command"},
	{cm_indentregion, "c-indent-region"},	/* Y.Koyanagi */
	{cmode,		"c-mode"},
	{cm_lfindent,	"c-newline-and-indent"},
#endif	/* C_MODE */
	{prefix,	"c-x 4 prefix"},
	{prefix,	"c-x prefix"},
#ifndef NO_MACRO
	{executemacro,	"call-last-kbd-macro"},
#endif
#ifdef CANNA
	{canna_toggle,	"canna-toggle"},
#endif
	{capword,	"capitalize-word"},
#ifndef NO_DIR
	{changedir,	"cd"},
#endif
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{k_rot_fio,	"change-default-fileio-code"},
	{k_rot_display,	"change-display-code"},
#endif	/* KANJI */
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	{fepmode_chg,	"change-fep-control"},
#endif
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{k_rot_buffio,	"change-fileio-code"},
	{k_rot_input,	"change-input-code"},
#endif	/* KANJI */
#ifdef JUMPERR /* 91.09.14  by Y.Koyanagi */
	{compile,	"compile"},
#endif	/* JUMPERR */
#ifndef NO_STARTUP
#ifdef WIN32
	{ConfigStartupFilePath, "configure"},
#endif
#endif
 	{copybuffer,	"copy-buffer-as-kill"},
	{copyregion,	"copy-region-as-kill"},
#ifdef	ADDFUNC	/* 90.02.15  by S.Yoshida */
	{pagelines,	 "count-lines-page"},
	{regionlines,	 "count-lines-region"},
#endif	/* ADDFUNC */
#ifdef	REGEX
	{cntmatchlines, "count-matches"},
	{cntnonmatchlines,"count-non-matches"},
#endif
	{define_key,	"define-key"},
	{backdel,	"delete-backward-char"},
	{deblank,	"delete-blank-lines"},
	{forwdel,	"delete-char"},
	{delwhite,	"delete-horizontal-space"},
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
	{kc_del_bol,	"delete-kinsoku-bol-chars"},
	{kc_del_eol,	"delete-kinsoku-eol-chars"},
#endif	/* KINSOKU */
#ifdef	REGEX
	{delmatchlines, "delete-matching-lines"},
	{delnonmatchlines,"delete-non-matching-lines"},
#endif
	{onlywind,	"delete-other-windows"},
	{delwind,	"delete-window"},
#ifndef NO_HELP
	{wallchart,	"describe-bindings"},
	{desckey,	"describe-key-briefly"},
#endif
	{digit_argument,"digit-argument"},
#ifndef NO_DIRED
	{dired,		"dired"},
	{d_undelbak,	"dired-backup-unflag"},
	{d_copy,	"dired-copy-file"},
	{d_expunge,	"dired-do-deletions"},
	{d_execute,	"dired-do-shell-command"},
	{d_findfile,	"dired-find-file"},
	{d_ffotherwindow, "dired-find-file-other-window"},
	{d_flag,	"dired-flag-backup-files"},
	{d_del,		"dired-flag-file-deleted"},
	{d_otherwindow, "dired-other-window"},
	{d_rename,	"dired-rename-file"},
	{d_undel,	"dired-unflag"},
#endif
	{lowerregion,	"downcase-region"},
	{lowerword,	"downcase-word"},
#ifdef C_MODE	/* 90.07.24  by K.Takano */
	{cm_brace,	"electric-c-brace"},
	{cm_brace_blink,"electric-c-brace-blink"},
	{cm_semi,	"electric-c-semi"},
	{cm_term,	"electric-c-terminator"},
#endif	/* C_MODE */
	{showversion,	"emacs-version"},
#ifndef NO_MACRO
	{finishmacro,	"end-kbd-macro"},
#endif
	{gotoeob,	"end-of-buffer"},
	{gotoeol,	"end-of-line"},
	{enlargewind,	"enlarge-window"},
	{prefix,	"esc prefix"},
#ifndef NO_STARTUP
	{evalbuffer,	"eval-current-buffer"},
	{evalexpr,	"eval-expression"},
#endif
	{swapmark,	"exchange-point-and-mark"},
	{extend,	"execute-extended-command"},
#ifdef FEPCTRL
	{fepmode_toggle,"fep-toggle"},
#else
# ifdef CANNA
	{canna_toggle,	"fep-toggle"},
# endif
#endif
	{fillpara,	"fill-paragraph"},
	{filevisit,	"find-file"},
	{poptofile,	"find-file-other-window"},
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	{filereadonly,	"find-file-read-only"},
#endif	/* READONLY */
	{forwchar,	"forward-char"},
	{gotoeop,	"forward-paragraph"},
	{forwword,	"forward-word"},
	{bindtokey,	"global-set-key"},
	{unbindtokey,	"global-unset-key"},
	{gotoline,	"goto-line"},
#ifdef	GOMATCH    
	{gotomatch,	"goto-matching-fence"},
#endif    
#ifndef NO_HELP
	{prefix,	"help"},
	{help_help,	"help-help"},
#endif
#ifdef	IBMPC	/* 91.01.11  by S.Yoshida */
	{j31_set_cursor, "ibmpc-set-cursor"},
#endif	/* IBMPC */
#if (defined(AMIGA)&&defined(DO_ICONIFY))||defined(WIN32)
	{tticon,	"iconify-frame"},
#endif	/* ICONIFY */
	{insert,	"insert"},
	{bufferinsert,	"insert-buffer"},
	{fileinsert,	"insert-file"},
	{fillword,	"insert-with-wrap"},
	{backisearch,	"isearch-backward"},
	{forwisearch,	"isearch-forward"},
#ifdef	IBMPC	/* 91.01.11  by S.Yoshida */
	{j31_set_cursor, "j31-set-cursor"},
#endif	/* IBMPC */
#ifdef	JUMPERR	
        {jumptoerror,	"jump-to-error"},
#endif
	{justone,	"just-one-space"},
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{prefix,	"kanji-prefix"},
#endif	/* KANJI */
	{ctrlg,		"keyboard-quit"},
	{killbuffer,	"kill-buffer"},
	{killline,	"kill-line"},
	{killoneline,	"kill-oneline"},
	{killpara,	"kill-paragraph"},
	{killregion,	"kill-region"},
	{delfword,	"kill-word"},
	{linenumbermode, "line-number-mode"},
	{listbuffers,	"list-buffers"},
#ifdef	C_MODE	/* 90.07.24  by K.Takano */
	{cm_list_var,	"list-c-mode-variables"},
#endif	/* C_MODE */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{k_list_code,	"list-kanji-codes"},
	{k_show_code,	"list-kanji-codes-briefly"},
#ifdef	KINSOKU	/* 90.01.29  by S.Yoshida */
	{kc_list_char,	"list-kinsoku-chars"},
#endif	/* KINSOKU */
#endif	/* KANJI */
#ifndef NO_STARTUP
	{evalfile,	"load"},
#endif
	{localbind,	"local-set-key"},
	{localunbind,	"local-unset-key"},
#ifndef NO_BACKUP
	{makebkfile,	"make-backup-files"},
#endif
#ifdef	DO_METAKEY
	{do_meta,	"meta-key-mode"},	/* better name, anyone? */
#endif
#ifndef KANJI
	{showngversion,	"mg-version"},
#endif
#ifdef	AMIGA
#ifdef	MOUSE
	{mgotobob,	"mouse-beginning-of-buffer"},
	{mforwdel,	"mouse-delete-char"},
	{mdelwhite,	"mouse-delete-horizontal-space"},
	{mdelwind,	"mouse-delete-window"},
	{mgotoeob,	"mouse-end-of-buffer"},
	{menlargewind,	"mouse-enlarge-window"},
	{mkillline,	"mouse-kill-line"},
	{mkillregion,	"mouse-kill-region"},
	{mdelfword,	"mouse-kill-word"},
	{mreposition,	"mouse-recenter"},
	{mbackpage,	"mouse-scroll-down"},
	{mforwpage,	"mouse-scroll-up"},
	{amigamouse,	"mouse-set-point"},
	{mshrinkwind,	"mouse-shrink-window"},
	{msplitwind,	"mouse-split-window-vertically"},
	{myank,		"mouse-yank"},
#endif
#endif
	{negative_argument, "negative-argument"},
	{newline,	"newline"},
	{indent,	"newline-and-indent"},
#ifdef	JUMPERR /* Y.Koyanagi */
	{nexterror,	"next-error"},
#endif
	{forwline,	"next-line"},
#ifdef	NEXTLINE
	{nextline,	"next-line-add-newlines"},
#endif
#ifdef	ADDFUNC	/* 90.12.28  by S.Yoshida */
#ifdef	KANJI
	{showngversion,	"ng-version"},
#endif
#endif
	{notabmode,	"no-tab-mode"},
	{notmodified,	"not-modified"},
	{openline,	"open-line"},
	{nextwind,	"other-window"},
	{overwrite,	"overwrite-mode"},
#ifdef	PREFIXREGION
	{prefixregion,	"prefix-region"},
#endif
	{backline,	"previous-line"},
#ifdef	GOSMACS
	{prevwind,	"previous-window"},
#endif
#ifdef	VMS
	{spawncli,	"push-to-dcl"},
#endif
#ifndef NO_DIR
	{showcwdir,	"pwd"},
#endif
	{queryrepl,	"query-replace"},
#ifdef	REGEX
	{re_queryrepl,	"query-replace-regexp"},
#endif
	{quote,		"quoted-insert"},
#ifdef	REGEX
	{re_searchagain,"re-search-again"},
	{re_backsearch, "re-search-backward"},
	{re_forwsearch, "re-search-forward"},
#endif
	{reposition,	"recenter"},
	{refresh,	"redraw-display"},
	{filesave,	"save-buffer"},
	{quit,		"save-buffers-kill-emacs"},
	{savebuffers,	"save-some-buffers"},
	{backpage,	"scroll-down"},
#ifdef	GOSMACS
	{back1page,	"scroll-one-line-down"},
	{forw1page,	"scroll-one-line-up"},
#endif
	{pagenext,	"scroll-other-window"},
	{forwpage,	"scroll-up"},
	{searchagain,	"search-again"},
	{backsearch,	"search-backward"},
	{forwsearch,	"search-forward"},
	{selfinsert,	"self-insert-command"},
#ifdef	AUTOSAVE
	{as_set_interval,"set-auto-save-interval"},
#endif
#ifdef	C_MODE	/* 90.07.24  by K.Takano */
	{cm_set_arg,	"set-c-argdecl-indent"},
	{cm_set_newl,	"set-c-auto-newline"},
	{cm_set_imagin,	"set-c-brace-imaginary-offset"},
	{cm_set_brace,	"set-c-brace-offset"},
	{cm_set_cbrace,	"set-c-continued-brace-offset"},
	{cm_set_cstat,	"set-c-continued-statement-offset"},
	{cm_set_indent,	"set-c-indent-level"},
	{cm_set_label,	"set-c-label-offset"},
	{cm_set_tab,	"set-c-tab-always-indent"},
#ifdef	VARIABLE_TAB
	{set_cmode_tabwidth, "set-c-tab-width"},
#endif	
#endif	/* C_MODE */
#ifdef	REGEX
	{setcasefold,	"set-case-fold-search"},
#endif
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{k_set_fio,	"set-default-fileio-code"},
#endif	/* KANJI */
	{set_default_mode, "set-default-mode"},
#ifdef	VARIABLE_TAB
	{set_default_tabwidth,"set-default-tab-width"},
#endif	/* VARIABLE_TAB */
#ifdef	FEPCTRL	/* 90.11.26  by K.Takano */
	{fepmode_set,	"set-fep-control"},
#endif	/* FEPCTRL */
	{setfillcol,	"set-fill-column"},
#ifdef	FILLPREFIX	/* 90.12.30  by S.Yoshida */
	{setfillprefix,	"set-fill-prefix"},
#endif	/* FILLPREFIX */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{k_set_display,	"set-kanji-display-code"},
	{k_set_expect,	"set-kanji-expected-code"},
	{k_set_buffio,	"set-kanji-fileio-code"},
	{k_set_input,	"set-kanji-input-code"},
#endif	/* KANJI */
	{setmark,	"set-mark-command"},
#ifdef	PREFIXREGION
	{setprefix,	"set-prefix-string"},
#endif
#ifdef	INCLUDE_SKG  /* 00.2.16 by amura */
	{skg_set_dicname, "set-skg-kanji-dictionary"},
	{skg_set_romanname, "set-skg-roman-dictionary"},
#endif  /* INCLUDE_SKG */
#ifdef	VARIABLE_TAB
	{set_tabwidth,	"set-tab-width"},
#endif	/* VARIABLE_TAB */
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	{k_set_toadisplay, "set-to-ascii-display"},
	{k_set_toafio,	"set-to-ascii-fileio"},
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	{k_set_tokanadisplay, "set-to-kana-display"},
	{k_set_tokanafio,     "set-to-kana-fileio"},
#endif  /* HANKANA */
	{k_set_tokdisplay, "set-to-kanji-display"},
	{k_set_tokfio,	"set-to-kanji-fileio"},
#endif	/* KANJI */
#ifndef	NO_SHELL	/* 91.01.10  by K.Maeda */
	{shellcmnd,	"shell-command"},
#endif	/* NO_SHELL */
	{shrinkwind,	"shrink-window"},
#ifdef	INCLUDE_SKG  /* 93.09.07  by H.Konishi */
  	{skginput,	"skg-input"},
#endif  /* INCLUDE_SKG */
	{space_to_tabstop, "space-to-tabstop"},
	{splitwind,	"split-window-vertically"},
#ifndef NO_MACRO
	{definemacro,	"start-kbd-macro"},
#endif
#ifdef	VMS
	{attachtoparent,"suspend-emacs"},
#else
	{spawncli,	"suspend-emacs"},
#endif
	{usebuffer,	"switch-to-buffer"},
	{poptobuffer,	"switch-to-buffer-other-window"},
	{fillmode,	"text-mode"},		/* dummy */
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	{togglereadonly, "toggle-read-only"},
#endif	/* READONLY */
	{twiddle,	"transpose-chars"},
#ifdef	UNDO
	{do_undo,	"undo"},
#endif
	{universal_argument, "universal-argument"},
	{upperregion,	"upcase-region"},
	{upperword,	"upcase-word"},
#ifdef	C_MODE
	{cm_use_c_mode,	"use-c-mode"},
#endif
	{showcpos,	"what-cursor-position"},
	{filewrite,	"write-file"},
	{yank,		"yank"},
#ifdef ZAPTOCHAR	/* Nov 91, bsh */
	{zaptochar,	"zap-to-char"},
#endif
};

#define NFUNCT	(sizeof(functnames)/sizeof(FUNCTNAMES))

int	nfunct = NFUNCT;		/* used by help.c */

/*
 * The general-purpose version of ROUND2 blows osk C (2.0) out of the water.
 * (reboot required)  If you need to build a version of mg with less than 32
 * or more than 511 functions, something better must be done.
 * The version that should work, but doesn't is:
 * #define ROUND2(x) (1+((x>>1)|(x>>2)|(x>>3)|(x>>4)|(x>>5)|(x>>6)|(x>>7)|\
 *	(x>>8)|(x>>9)|(x>>10)|(x>>11)|(x>>12)|(x>>13)|(x>>14)|(x>>15)))
 */
#define ROUND2(x) (x<128?(x<64?32:64):(x<256?128:256))

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
name_fent(fname, flag)
#else	/* NOT NEW_COMPLETE */
static	name_fent(fname, flag)
#endif	/* NEW_COMPLETE */
register char *fname;
int	flag;
{
	register int	try;
	register int	x = ROUND2(NFUNCT);
	register int	base = 0;
	register int	notit;

	do {
	    /* + can be used instead of | here if more efficent.	*/
	    if((try = base | x) < NFUNCT) {
		if((notit = strcmp(fname, functnames[try].n_name)) >= 0) {
		    if(!notit) return try;
		    base = try;
		}
	    }
	} while((x>>=1) || (try==1 && base==0));    /* try 0 once if needed */
	return flag ? base : -1;
}

/*
 * Translate from function name to function pointer, using binary search.
 */

PF	name_function(fname)
char	*fname;
{
	int i;
	if((i = name_fent(fname, FALSE)) >= 0) return functnames[i].n_funct;
	return (PF)NULL;
}

#ifndef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
/* complete function name */

complete_function(fname, c)
register char	*fname;
{
	register int i, j, k, l;
	int	oj;

	i = name_fent(fname, TRUE);
	for(j=0; (l=fname[j]) && functnames[i].n_name[j]==l; j++) {}
	if(fname[j]!='\0') {
	    if(++i >= NFUNCT) return -2;	/* no match */
	    for(j=0; (l=fname[j]) && functnames[i].n_name[j]==l; j++) {}
	    if(fname[j]!='\0') return -2;	/* no match */
	}
	if(c==CCHR('M') && functnames[i].n_name[j]=='\0') return -1;
	for(k=i+1; k<NFUNCT; k++) {		/* find last match */
	    for(l=0; functnames[k].n_name[l]==fname[l]; l++) {}
	    if(l<j) break;
	}
	k--;
	oj = j;
	if(k>i) {					/* multiple matches */
	    while((l = functnames[i].n_name[j]) == functnames[k].n_name[j]) {
		fname[j++] = l;
		if(l=='-' && c==' ') break;
	    }
	    if(j==oj) return -3;			/* ambiguous	*/
	} else {					/* single match */
	    while(l = functnames[i].n_name[j]) {
		fname[j++] = l;
		if(l=='-' && c==' ') break;
	    }
	}
	fname[j] = '\0';
	return j - oj;
}
#endif	/* NEW_COMPLETE */

/* translate from function pointer to function name. */

char *function_name(fpoint)
register PF fpoint;
{
	register FUNCTNAMES	*fnp = &functnames[0];

	if(fpoint == prefix) return (char *)NULL;	/* ambiguous */
	do {
	    if(fnp->n_funct == fpoint) return fnp->n_name;
	} while(++fnp < &functnames[NFUNCT]);
	return (char *)NULL;
}
