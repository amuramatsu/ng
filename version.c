/*
 * This file contains the string that get written
 * out by the emacs-version command.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

/* $Id: version.c,v 1.1 1999/05/19 04:17:24 amura Exp $ */

/* $Log: version.c,v $
/* Revision 1.1  1999/05/19 04:17:24  amura
/* Initial revision
/*
*/

#include	"config.h"	/* 90.12.20  by S.Yoshida */

#ifdef	ADDFUNC	/* 90.12.28  by S.Yoshida */
#include	"def.h"
#else	/* NOT ADDFUNC */
#define TRUE	1	/* include "def.h" when things get more complicated */
#endif	/* ADDFUNC */

#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#ifdef	MSDOS	/* 90.02.11  by S.Yoshida */
#ifdef	J3100	/* 90.03.10  by S.Yoshida */
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
char version[] = "Ng 1.3L J-3100 version. [Nihongo Mg 2a]  Hankaku KANA Ok";
#else
char version[] = "Ng 1.3L J-3100 version. [Nihongo Mg 2a] ";
#endif  /* HANKANA */
#else	/* NOT J3100 */
#ifdef	PC9801	/* 90.03.10  by S.Yoshida */
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
char version[] = "Ng 1.3L PC-9801 version. [Nihongo Mg 2a]  Hankaku KANA Ok";
#else
char version[] = "Ng 1.3L PC-9801 version. [Nihongo Mg 2a] ";
#endif  /* HANKANA */
#else	/* NOT PC9801 */
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
char version[] = "Ng 1.3L MS-DOS version. [Nihongo Mg 2a]  Hankaku KANA Ok";
#else
char version[] = "Ng 1.3L MS-DOS version. [Nihongo Mg 2a] ";
#endif  /* HANKANA */
#endif	/* PC9801 */
#endif	/* J3100 */
#else	/* NOT MSDOS */
#ifdef	AMIGA	/* Jan.28,1993 by H.Ohkubo */
# ifdef  INCLUDE_SKG
#  ifdef HANKANA
char version[] = "Kg [ KANgee/ based on Ng 1.3L] ver 4.3 Hankaku KANA Ok";
#  else
char version[] = "Kg [ KANgee/ based on Ng 1.3L] ver 4.3 ";
#  endif /* HANKANA */
# else /* WITHOUT SKG */
#  ifdef HANKANA
char version[] = "Ng 1.3L AmigaDos Rev.1 [Nihongo Mg 2a] Hankaku KANA Ok";
#  else
char version[] = "Ng 1.3L AmigaDos Rev.1 [Nihongo Mg 2a] ";
#  endif /* HANKANA */
# endif
#else	/* NOT AMIGA */
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
char version[] = "Ng 1.3L [Nihongo Mg 2a]  Hankaku KANA Ok";
#else
char version[] = "Ng 1.3L [Nihongo Mg 2a] ";
#endif  /* HANKANA */
#endif	/* AMIGA */
#endif	/* MSDOS */
#else	/* NOT KANJI */
char version[] = "Mg 2a (formerly MicroGnuEmacs)";
#endif	/* KANJI */

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
#ifdef	NEW_COMPLETE
static	char *complete_msg = "\tCOMPLETE\t(Enable new version completion)";
#endif
#ifndef	NO_DIR
static	char *dir_msg = "\tDIR\t\t(Enable directory change functions)";
#endif
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
static	char *regex_msg = "\tREGEX\t\t(Enable regular expression functions)";
#endif
#ifndef	NO_SHELL
static	char *shell_msg = "\tSHELL\t\t(Enable shell-command function)";
#endif
#ifndef	NO_STARTUP
static	char *startup_msg = "\tSTARTUP\t\t(Enable startup files)";
#endif
/* Dec.20,1992 Add by H.Ohkubo */
#ifdef	AMIGA
#ifdef	V11
static	char *v11_msg = "\tV11\t\t(Enable AmigaDOS Ver.1.1)";
#endif
#ifdef	MENU
static	char *menu_msg = "\tMENU\t\t(Enable menu selection)";
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
static	char *iconify_msg = "\tICONIFY\t\t(Enable iconify)";
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

	if ((bp = bfind("*Ng Version*", TRUE)) == NULL) return FALSE;
	if (bclear(bp) != TRUE) return FALSE;
	bp->b_flag &= ~BFCHG;		/* Blow away old.	*/

	strcpy(line, "Ng version:");
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
#ifdef	AMIGA	/* Dec.20,1992 By H.Ohkubo */
#ifdef	V11
	strcpy(line, v11_msg);
	if (addline(bp, line) == FALSE) return FALSE;
#endif
#ifdef	MENU
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
	printf("Ng version:\n");
	printf("\t%s\n", version);
	return TRUE;
}

/*
 * Print the Ng compile time options.
 */
printoptions()
{
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
#ifdef	AMIGA	/* Dec.20,1992 By H.Ohkubo */
#ifdef	V11
	printf("%s\n", v11_msg);
#endif
#ifdef	MENU
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
#endif	/* AMIGA */

	return TRUE;
}
#endif	/* ADDFUNC */
