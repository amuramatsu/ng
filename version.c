/* $Id: version.c,v 1.12.2.1 2003/03/08 01:22:35 amura Exp $ */
/*
 * This file contains the string that get written
 * out by the emacs-version command.
 */

/*
 * $Log: version.c,v $
 * Revision 1.12.2.1  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.12  2001/05/28 19:02:02  amura
 * edit to 1.5alpha3
 *
 * Revision 1.11  2001/03/09 16:17:22  amura
 * edit to 1.5alpha2
 *
 * Revision 1.10  2001/01/20 18:17:13  amura
 * edit to 1.5alpha1
 *
 * Revision 1.9  2001/01/05 14:07:05  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.8  2000/11/16 14:27:00  amura
 * edit to 1.4.2 release
 *
 * Revision 1.7  2000/10/02 16:29:55  amura
 * can display some Amiga configuration
 *
 *  -- snip --
 *
 * Revision 1.1  1999/05/19  04:17:24  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef	ADDFUNC	/* 90.12.28  by S.Yoshida */
#include	"def.h"
#else	/* NOT ADDFUNC */
#define TRUE	1	/* include "def.h" when things get more complicated */
#endif	/* ADDFUNC */

#define	VERSION		"1.5beta1"

#ifdef	KANJI
# define PROGNAME	"Ng"
# define FORMERLY	"[Nihongo Mg]"
#else
# define PROGNAME	"Mg++"
# define FORMERLY	"(formerly MicroGnuEmacs Adv.)"
#endif

#if defined(MSDOS)
# if defined(IBMPC)
#  ifdef TCCONIO
#   define TARGET	"for IBM PC/TCCONIO"
#  else
#   define TARGET	"for IBM PC"
#  endif
# elif defined(PC9801)
#  ifdef TCCONIO
#   define TARGET	"for PC-9801/TCCONIO"
#  else
#   define TARGET	"for PC-9801"
#  endif
# else
#  define TARGET	"for MS-DOS"
# endif
#elif defined(AMIGA)
# ifdef	V2
#  define TARGET	"for AmigaDOS 2+"
# else
#  define TARGET	"for AmigaDOS 1+"
# endif
#elif defined(WIN)
# define TARGET		"for Win32 Ver.0.5.24"
#endif

#ifdef	TARGET
char version[] = PROGNAME " " VERSION " " TARGET " " FORMERLY ;
#else
char version[] = PROGNAME " " VERSION " " FORMERLY ;
#endif

/*
 * Display the version. All this does
 * is copy the version string onto the echo line.
 */
/*ARGSUSED*/
showversion(f, n)
int f, n;
{
	ewprintf(version);
	return TRUE;
}

#ifdef	ADDFUNC	/* 90.12.28  by S.Yoshida */
#ifndef	NO_BACKUP
static	char *backup_msg = "\tBACKUP\t\t(Enable backup files)";
#endif
#ifdef	CURSOR_POS
static	char *cursor_pos_msg = "\tCURSOR_POS\t(Cursor locates on next CHR after POINT)";
#endif
#ifdef	C_MODE
static	char *c_mode_msg = "\tC_MODE\t\t(Enable \"c-mode\" functions)";
#endif
#ifdef	BUFFER_MODE
static	char *buffer_mode_msg = "\tBUFFER_MODE\t(Enable \"Buffer Menu\" mode)";
#endif
#ifdef	NEW_COMPLETE
static	char *complete_msg = "\tCOMPLETE\t(Enable new version completion)";
#endif
#ifndef	NO_DIR
# ifdef EXTD_DIR
static	char *dir_msg = "\tDIR\t\t(Enable buffer local directory change)";
# else
static	char *dir_msg = "\tDIR\t\t(Enable directory change functions)";
# endif
#endif
#endif	/* ADD_FUNC */
#ifndef	NO_DIRED
static	char *dired_msg = "\tDIRED\t\t(Enable \"dired\" mode functions)";
#endif
#ifndef	NO_DPROMPT
static	char *prompt_msg = "\tDPROMPT\t\t(Enable delayed prompt)";
#endif
#ifdef	FEPCTRL
static	char *fepctrl_msg = "\tFEPCTRL\t\t(Enable FEP auto control)";
#endif
#ifndef	NO_FILECOMP
static	char *filecomp_msg = "\tFILECOMP\t(Enable file name completion)";
#endif
#ifdef	FILLPREFIX
static	char *fillprefix_msg = "\tFILLPREFIX\t(Enable fill prefix function)";
#endif
#ifdef	HANKANA
static	char *hankana_msg = "\tHANKANA\t\t(Enable Hankaku KANA handling)";
#endif
#ifndef	NO_HELP
static	char *help_msg = "\tHELP\t\t(Enable help)";
#endif
#ifdef	KANJI
static	char *kanji_msg = "\tKANJI\t\t(Enable KANJI handling)";
#endif
#ifdef	KINSOKU
static	char *kinsoku_msg = "\tKINSOKU\t\t(Enable KINSOKU handling)";
#endif
#ifndef	NO_MACRO
static	char *macro_msg = "\tMACRO\t\t(Enable keyboard macros)";
#endif
#ifdef	DO_METAKEY
static	char *metakey_msg = "\tMETAKEY\t\t(Enable META key input)";
#endif
#ifdef	PREFIXREGION
static	char *prefixregion_msg = "\tPREFIXREGION\t(Enable \"prefix-region\")";
#endif
#ifdef	READONLY
static	char *readonly_msg = "\tREADONLY\t(Enable read-only buffer mode)";
#endif
#ifdef	REGEX
# ifdef  REGEX_JAPANESE
static	char *regex_msg = "\tREGEX\t\t(Enable Japanese regular expression)";
# else
static	char *regex_msg = "\tREGEX\t\t(Enable regular expression functions)";
# endif
#endif

#ifndef	NO_SHELL
static	char *shell_msg = "\tSHELL\t\t(Enable shell-command function)";
#endif
#ifndef	NO_STARTUP
static	char *startup_msg = "\tSTARTUP\t\t(Enable startup files)";
#endif
#ifdef	ADDFUNC
static	char *addfunc_msg = "\tADDFUNC\t\t(Enable misc functions)";
#endif
#ifdef	VARIABLE_TAB
static	char *vartab_msg = "\tVARIABLE_TAB\t(Enable buffer-local tabwidth)";
#endif
#ifdef	ADDOPT
static	char *addopt_msg = "\tADDOPT\t\t(Enable additional startup option)";
#endif
#ifdef	CLIPBOARD
static	char *clipboard_msg = "\tCLIPBOARD\t(Enable yank-buffer to/from clipboard)";
#endif
#ifdef  MINIBUF_EDIT
static  char *minibuf_msg = "\tMINIBUF_EDIT\t(Enable minibuffer edit)";
#endif
#ifdef  CANNA
static  char *canna_msg = "\tCANNA\t\t(Enable CANNA)";
#endif
#ifdef	NEXTLINE
static	char *nextline_msg = "\tNEXTLINE\t(Enable \"next-line-add-newlines\")";
#endif
#ifdef	UNDO
static	char *undo_msg = "\tUNDO\t\t(Enable undo)";
#endif
#ifdef	DROPFILES
static	char *dropfiles_msg = "\tDROPFILES\t(Enable Drag&Drop)";
#endif
#ifdef	HOJO_KANJI
static	char *hojokan_msg = "\tHOJO_KANJI\t(Enable Hojo KANJI handling)";
#endif

/* Dec.20,1992 Add by H.Ohkubo */
#ifdef	AMIGA
#ifndef V2
# ifdef	V11
static	char *v1_msg = "\tV11\t\t(Enable AmigaDOS Ver.1.1)";
# else
static	char *v1_msg = "\tV1\t\t(Enable AmigaDOS Ver.1.x)";
# endif
#endif
#ifdef	DO_MENU
static	char *menu_msg = "\tDO_MENU\t\t(Enable menu selection)";
#endif
#ifdef	BROWSER
static	char *browser_msg = "\tBROWSER\t\t(Enable to present a menu of file)";
#endif
#ifdef	MOUSE
static	char *mouse_msg = "\tMOUSE\t\t(Enable to use mouse)";
#endif
#ifdef	FKEYS
static	char *fkeys_msg = "\tFKEYS\t\t(Enable to use function keys)";
#endif
#ifdef	DO_ICONIFY
static	char *iconify_msg = "\tDO_ICONIFY\t(Enable iconify)";
#endif
#ifdef	CHANGE_COLOR
static	char *color_msg = "\tCOLOR\t\t(Enable to color setting)";
#endif
#ifdef	CHANGE_FONT
static	char *font_msg = "\tFONT\t\t(Enable to font setting)";
#endif
#ifdef	USE_ARP
static	char *arp_msg = "\tARP\t\t(using ARP.Library)";
#endif
#ifdef	REXX
static	char *rexx_msg = "\tREXX\t\t(Enable to use AREXX)";
#endif
#ifdef	ASL
static	char *asl_msg = "\tASL\t\t(Enable to use ASL Filerequester)";
#endif
#endif	/* AMIGA */

/*
 * Display the Ng version, compile time options.
 */
/*ARGSUSED*/
showngversion(f, n)
int f, n;
{
	register BUFFER	*bp;
	register WINDOW	*wp;
	char	 line[80];

	if ((bp = bfind("*" PROGNAME " Version*", TRUE)) == NULL) return FALSE;
	if (bclear(bp) != TRUE) return FALSE;
	bp->b_flag &= ~BFCHG;		/* Blow away old.	*/
#ifdef READONLY
	bp->b_flag |= BFRONLY;
#endif
	if (bclear(bp) != TRUE) return FALSE;

	strcpy(line, PROGNAME " version:");
	if (addline(bp, line) == FALSE) return FALSE;
	sprintf(line, "\t%s", version);
	if (addline(bp, line) == FALSE) return FALSE;
	strcpy(line, "Compile time options:");
	if (addline(bp, line) == FALSE) return FALSE;
#ifndef	NO_BACKUP
	strcpy(line, backup_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	CURSOR_POS
	strcpy(line, cursor_pos_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	C_MODE
	strcpy(line, c_mode_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	BUFFER_MODE
	strcpy(line, buffer_mode_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	NEW_COMPLETE
	strcpy(line, complete_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_DIR
	strcpy(line, dir_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_DIRED
	strcpy(line, dired_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_DPROMPT
	strcpy(line, prompt_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	FEPCTRL
	strcpy(line, fepctrl_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_FILECOMP
	strcpy(line, filecomp_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	FILLPREFIX
	strcpy(line, fillprefix_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	HANKANA
	strcpy(line, hankana_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_HELP
	strcpy(line, help_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	KANJI
	strcpy(line, kanji_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	KINSOKU
	strcpy(line, kinsoku_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_MACRO
	strcpy(line, macro_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	DO_METAKEY
	strcpy(line, metakey_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	PREFIXREGION
	strcpy(line, prefixregion_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	READONLY
	strcpy(line, readonly_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	REGEX
	strcpy(line, regex_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_SHELL
	strcpy(line, shell_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifndef	NO_STARTUP
	strcpy(line, startup_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	ADDFUNC
	strcpy(line, addfunc_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	VARIABLE_TAB
	strcpy(line, vartab_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	ADDOPT
	strcpy(line, addopt_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	CLIPBOARD
	strcpy(line, clipboard_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	MINIBUF_EDIT
	strcpy(line, minibuf_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	CANNA
	strcpy(line, canna_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	NEXTLINE
	strcpy(line, nextline_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	UNDO
	strcpy(line, undo_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	DROPFILES
	strcpy(line, dropfiles_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	HOJO_KANJI
	strcpy(line, hojokan_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif

#ifdef	AMIGA	/* Dec.20,1992 By H.Ohkubo */
#ifndef	V2
	strcpy(line, v1_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	DO_MENU
	strcpy(line, menu_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	BROWSER
	strcpy(line, browser_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	MOUSE
	strcpy(line, mouse_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	FKEYS
	strcpy(line, fkeys_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	DO_ICONIFY
	strcpy(line, iconify_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	CHANGE_COLOR
	strcpy(line, color_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	CHANGE_FONT
	strcpy(line, font_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	USE_ARP
	strcpy(line, arp_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	REXX
	strcpy(line, rexx_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	ASL
	strcpy(line, asl_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#endif	/* AMIGA */

	if ((wp = popbuf(bp)) == NULL)	return FALSE;
	bp->b_dotp = lforw(bp->b_linep); /* put dot at beginning of buffer */
	bp->b_doto = 0;
	wp->w_dotp = bp->b_dotp;	/* fix up if window already on screen */
	wp->w_doto = bp->b_doto;
	return TRUE;
}

/*
 * Print the Ng version to the stdout.
 */
printversion()
{
#ifdef	WIN32
	char	buf[ 128 ], *ptr = buf ;
	extern	void	MessageOut( const char * ) ;

	sprintf(ptr, PROGNAME " version:\n");
	ptr += strlen( ptr ) ;
	sprintf(ptr, "\t%s\n", version);
	MessageOut( buf ) ;
#else	/* WIN32 */
	printf(PROGNAME " version:\n");
	printf("\t%s\n", version);
#endif	/* WIN32 */
	return TRUE;
}

/*
 * Print the Ng compile time options.
 */
printoptions()
{
#ifndef	WIN32
	printf("Compile time options:\n");
#ifndef	NO_BACKUP
	printf("%s\n", backup_msg);
#endif
#ifdef	CURSOR_POS
	printf("%s\n", cursor_pos_msg);
#endif
#ifdef	C_MODE
	printf("%s\n", c_mode_msg);
#endif
#ifdef	BUFFER_MODE
	printf("%s\n", buffer_mode_msg);
#endif
#ifdef	NEW_COMPLETE
	printf("%s\n", complete_msg);
#endif
#ifndef	NO_DIR
	printf("%s\n", dir_msg);
#endif
#ifndef	NO_DIRED
	printf("%s\n", dired_msg);
#endif
#ifndef	NO_DPROMPT
	printf("%s\n", prompt_msg);
#endif
#ifdef	FEPCTRL
	printf("%s\n", fepctrl_msg);
#endif
#ifndef	NO_FILECOMP
	printf("%s\n", filecomp_msg);
#endif
#ifdef	FILLPREFIX
	printf("%s\n", fillprefix_msg);
#endif
#ifdef	HANKANA
	printf("%s\n", hankana_msg);
#endif
#ifndef	NO_HELP
	printf("%s\n", help_msg);
#endif
#ifdef	KANJI
	printf("%s\n", kanji_msg);
#endif
#ifdef	KINSOKU
	printf("%s\n", kinsoku_msg);
#endif
#ifndef	NO_MACRO
	printf("%s\n", macro_msg);
#endif
#ifdef	DO_METAKEY
	printf("%s\n", metakey_msg);
#endif
#ifdef	PREFIXREGION
	printf("%s\n", prefixregion_msg);
#endif
#ifdef	READONLY
	printf("%s\n", readonly_msg);
#endif
#ifdef	REGEX
	printf("%s\n", regex_msg);
#endif
#ifndef	NO_SHELL
	printf("%s\n", shell_msg);
#endif
#ifndef	NO_STARTUP
	printf("%s\n", startup_msg);
#endif
#ifdef	VARIABLE_TAB
	printf("%s\n", vartab_msg);
#endif
#ifdef	ADDOPT
	printf("%s\n", addopt_msg);
#endif
#ifdef	CLIPBOARD
	printf("%s\n", clipboard_msg);
#endif
#ifdef	MINIBUF_EDIT
	printf("%s\n", minibuf_msg);
#endif
#ifdef	CANNA
	printf("%s\n", canna_msg);
#endif
#ifdef	NEXTLINE
	printf("%s\n", nextline_msg);
#endif
#ifdef	UNDO
	printf("%s\n", undo_msg);
#endif
#ifdef	DROPFILES
	printf("%s\n", dropfiles_msg);
#endif
#ifdef	HOJO_KANJI
	printf("%s\n", hojokan_msg);
#endif

#ifdef	AMIGA	/* Dec.20,1992 By H.Ohkubo */
#ifndef	V2
	printf("%s\n", v1_msg);
#endif
#ifdef	DO_MENU
	printf("%s\n", menu_msg);
#endif
#ifdef	BROWSER
	printf("%s\n", browser_msg);
#endif
#ifdef	MOUSE
	printf("%s\n", mouse_msg);
#endif
#ifdef	FKEYS
	printf("%s\n", fkeys_msg);
#endif
#ifdef	DO_ICONIFY
	printf("%s\n", iconify_msg);
#endif
#ifdef	CHANGE_COLOR
	printf("%s\n", color_msg);
#endif
#ifdef	CHANGE_FONT
	printf("%s\n", font_msg);
#endif
#ifdef	USE_ARP
	printf("%s\n", arp_msg);
#endif
#ifdef	REXX
	printf("%s\n", rexx_msg);
#endif
#ifdef	ASL
	printf("%s\n", asl_msg);
#endif
#endif	/* AMIGA */
#endif	/* WIN32 */

	return TRUE;
}

