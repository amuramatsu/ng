/* $Id: ttymenu.c,v 1.1 2000/06/27 01:48:01 amura Exp $ */
/*   
 * ttymenu.c
 *
 * Incorporates the browser, for rummaging around on disks,
 * and the usual Emacs editing command menu
 *
 *	Copyright (c) 1986, Mike Meyer
 *	Mic Kaczmarczik did a few things along the way.
 *
 * Permission is hereby granted to distribute this program, so long as
 * this source file is distributed with it, and this copyright notice
 * is not removed from the file.
 *
 */

/*
 * $Log: ttymenu.c,v $
 * Revision 1.1  2000/06/27 01:48:01  amura
 * Initial revision
 *
 * Revision 1.2  1999/05/21  00:53:36  amura
 * For KANgee 4.2.1
 *
 * Revision 1.1  1999/05/21  00:51:27  amura
 * Initial revision
 *
 */

#include <exec/types.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <intuition/intuition.h>
#include <exec/memory.h>
#include <intuition/screens.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <graphics/gfxbase.h>
#include <workbench/workbench.h>
/* #include <clib/exec_protos.h> */
#include <clib/wb_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>
#include <string.h>
#include <clib/diskfont_protos.h>
#undef	TRUE
#undef	FALSE
#include	"config.h"	/* Dec.19,1992 Add by H.Ohkubo */
#include "def.h"
#ifndef	NO_MACRO
#include "macro.h"
#endif	

#ifdef ASL
# include <graphics/text.h>
# include <exec/libraries.h>
# include <libraries/asl.h>
# include <clib/asl_protos.h>
#endif /* ASL */ 

#ifdef ASL
static UBYTE ASLDirectory[128]="";
struct FileRequester *fr;
struct Libraries *AslBase=NULL;
#define ASLHEIGHT   200
#define ASLWIDTH    320
#define ASLTOPEDGE    5
#define ASLLEFTEDGE 100
extern APTR		OpenLibrary();
#endif /* ASL */

/* For Remote Box */

# include "sys/amiga/KgMenus.h"

struct Window *Win0 = NULL;
APTR Win0VisualInfo;
struct Gadget *Win0GList;
struct Gadget *Win0Gadgets[21];
extern struct TextFont *EmFont;

UWORD Win0GadgetTypes[] =
	{
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
        INTEGER_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	};

struct NewGadget Win0NewGadgets[] =
	{
	24, 21, 58, 12, (UBYTE *)"Top", &topaz800, Win0_Gad0, 16, NULL,  NULL,
	84, 21, 58, 12, (UBYTE *)"Up", &topaz800, Win0_Gad1, 16, NULL,  NULL,
	24, 65, 58, 12, (UBYTE *)"Redraw", &topaz800, Win0_Gad2, 16, NULL,  NULL,
	24, 35, 29, 12, (UBYTE *)"|<", &topaz800, Win0_Gad3, 16, NULL,  NULL,
	84, 35, 29, 12, (UBYTE *)"<", &topaz800, Win0_Gad4, 16, NULL,  NULL,
	114, 35, 29, 12, (UBYTE *)">", &topaz800, Win0_Gad5, 16, NULL,  NULL,
	174, 35, 29, 12, (UBYTE *)">|", &topaz800, Win0_Gad6, 16, NULL,  NULL,
	84, 49, 58, 12, (UBYTE *)"Down", &topaz800, Win0_Gad7, 16, NULL,  NULL,
	24, 49, 58, 12, (UBYTE *)"Bottom", &topaz800, Win0_Gad8, 16, NULL,  NULL,
	145, 21, 58, 12, (UBYTE *)"PgUp", &topaz800, Win0_Gad9, 16, NULL,  NULL,
	145, 49, 58, 12, (UBYTE *)"PgDown", &topaz800, Win0_Gad10, 16, NULL,  NULL,
	54, 35, 29, 12, (UBYTE *)"<<", &topaz800, Win0_Gad11, 16, NULL,  NULL,
	144, 35, 29, 12, (UBYTE *)">>", &topaz800, Win0_Gad12, 16, NULL,  NULL,
	84, 65, 58, 12, (UBYTE *)"Show", &topaz800, Win0_Gad13, 16, NULL,  NULL,
       	77, 4, 126, 14, (UBYTE *)"Goto:", &topaz800, Win0_Gad14, 1, NULL,  NULL,
	230,  5, 80, 12, (UBYTE *)"SetFont", &topaz800, Win0_Gad15, 16, NULL,  NULL,
	210, 21, 100, 12, (UBYTE *)"SplitWindow", &topaz800, Win0_Gad16, 16, NULL,  NULL,
	210, 35, 100, 12, (UBYTE *)"NextWindow", &topaz800, Win0_Gad17, 16, NULL,  NULL,
	210, 49, 100, 12, (UBYTE *)"CloseWindow", &topaz800, Win0_Gad18, 16, NULL,  NULL,
	145, 65, 58, 12, (UBYTE *)"BORDER", &topaz800, Win0_Gad19, 16, NULL,  NULL,
	252, 70, 58, 12, (UBYTE *)"Done", &topaz800, Win0_Gad20, 19, NULL,  NULL,
	};
UWORD Win0ZoomInfo[4] = { 200, 0, 200, 25 };

struct TextAttr topaz800 = { (STRPTR)"topaz.font", 8, 0, 0 };

BOOL Remote_Box();
BOOL handleIDCMP();
int  OpenWindowWin0();
void CloseWindowWin0(); 
void ASLSetFont();

/*
 * Commands for managing REMOTE BOX
 */

extern	int	forwchar pro((int,int));
extern	int	backchar pro((int,int));
extern	int	forwline pro((int,int));
extern	int	backline pro((int,int));

/* For RemotoBox */


extern struct Menu		*AutoMenu ;
extern struct Window		*EmW ;

#ifdef	SUPPORT_ANSI
static VOID	Add_Devices(ULONG) ;
#else
static VOID	Add_Devices() ;
#endif

#define MNUM(menu,item,sub) (SHIFTMENU(menu)|SHIFTITEM(item)|SHIFTSUB(sub))

#ifdef	BROWSER
#define LONGEST_NAME	80	/* Longest file name we can deal with	*/

# ifdef	LATTICE
char *strchr(char *, int);
# else
#  ifndef SUPPORT_ANSI
char *index();			/* find first instance of c in s	*/
#define	strchr(s, c) index(s, c)
#  endif
# endif

# ifdef	MENU
#define	FIRSTMENU	1
# else
#define	FIRSTMENU	0
# endif

#endif	BROWSER

#ifdef	MENU
/*
 * When ttgetc() sees a menu selection event, it stuffs the sequence
 * KMENU <menu><item><subitem> into the input buffer
 *
 * The menu item names are chosen to be relatively close to the extended
 * function names, so a user can usually figure out the key binding of
 * a menu item by searching through the "display-bindings" buffer
 * for something that's close.
 */

/*
 * Commands for managing files and buffers
 */

extern	int	filevisit pro((int,int));
extern	int	poptofile pro((int,int));
extern	int	fileinsert pro((int,int));
extern	int	filesave pro((int,int));
extern	int	filewrite pro((int,int));
#ifndef	NO_DIRED
extern	int	dired pro((int,int));
#endif
extern	int	usebuffer pro((int,int));
extern	int	poptobuffer pro((int,int));
extern	int	killbuffer pro((int,int));
extern	int	listbuffers pro((int,int));
extern	int	savebuffers pro((int,int));
extern	int	quit pro((int,int));

static struct MenuBinding FileItems[] = {
	{ "Find File         C-x C-f",	filevisit	},
	{ "Pop To File       C-x 4 f",	poptofile	},
	{ "Insert File       C-x i",	fileinsert	},
	{ "Save File         C-x C-s",	filesave	},
	{ "Write File        C-x C-w",	filewrite	},
#ifndef	NO_DIRED
	{ "Dired             C-x d",	dired		},
#endif
	{ "Switch To Buffer  C-x b",	usebuffer	},
	{ "Pop To Buffer     C-x 4 b",	poptobuffer	},
	{ "Kill Buffer       C-x k",	killbuffer	},
	{ "List Buffers      C-x C-b",	listbuffers	},
	{ "Save Buffers      C-x s",	savebuffers	},
	{ "Save And Exit     C-x C-c",	quit		}
};

/*
 * Commands for various editing functions
 */

extern	int	yank pro((int,int));
extern	int	openline pro((int,int));
extern	int	killline pro((int,int));
extern	int	deblank pro((int,int));
extern	int	justone pro((int,int));
extern	int	indent pro((int,int));
extern	int	twiddle pro((int,int));
extern	int	quote pro((int,int));

static struct MenuBinding EditItems[] = {
	{ "Yank                 C-y",	yank		},
	{ "Blank Line           C-o ",	openline	},
	{ "Kill Line            C-k",	killline	},
	{ "Delete Blank Lines   C-x C-o",deblank	},
	{ "Delete Blanks        M-SPC",	justone		},
	{ "Newline And Indent   C-j",	indent		},
	{ "Transpose Characters C-t",	twiddle		},
	{ "Quoted Insert        C-q",	quote		}
};

/*
 * Movement commands
 */

extern	int	forwpage pro((int,int));
extern	int	backpage pro((int,int));
extern	int	gotobol pro((int,int));
extern	int	gotobob pro((int,int));
extern	int	gotoeol pro((int,int));
extern	int	gotoeob pro((int,int));
extern	int	gotoline pro((int,int));
extern	int	showcpos pro((int,int));

static struct MenuBinding MoveItems[] = {
	{ "Scroll Up       C-v",	forwpage	},
	{ "Scroll Down     M-v",	backpage	},
	{ "Start Of Line   C-a",	gotobol		},
	{ "Start Of Buffer M-<",	gotobob		},
	{ "End Of Line     C-e",	gotoeol		},
	{ "End Of Buffer   M->",	gotoeob		},
	{ "Goto Line",			gotoline	},
	{ "Show Cursor     C-x =",	showcpos	}
};

/*
 * Commands for searching and replacing
 */

extern	int	forwisearch  pro((int,int));
extern	int	backisearch pro((int,int));
extern	int	searchagain pro((int,int));
extern	int	forwsearch pro((int,int));
extern	int	backsearch pro((int,int));
extern	int	queryrepl pro((int,int));

static struct MenuBinding SearchItems[] = {
	{ "I-Search Forward  C-s",	forwisearch	},
	{ "I-Search Backward C-r",	backisearch	},
	{ "Search Again",		searchagain	},
	{ "Search Forward    M-s",	forwsearch	},
	{ "Search Backward   M-r",	backsearch	},
	{ "Query Replace     M-%",	queryrepl	}
};

/*
 * Commands that manipulate words
 */
extern	int	forwword pro((int,int));
extern	int	backword pro((int,int));
extern	int	delfword pro((int,int));
extern	int	delbword pro((int,int));
extern	int	capword pro((int,int));
extern	int	lowerword pro((int,int));
extern	int	upperword pro((int,int));

static struct MenuBinding WordItems[] = {
	{ "Forward Word       M-f",	forwword	},
	{ "Backward Word      M-b",	backword	},
	{ "Kill Word          M-d",	delfword	},
	{ "Backward Kill Word M-DEL",	delbword 	},
	{ "Capitalize Word    M-c",	capword		},
	{ "Downcase Word      M-l",	lowerword	},
	{ "Upcase Word        M-u",	upperword	}
};

/*
 * Commands relating to paragraphs
 */
extern	int	gotoeop pro((int,int));
extern	int	gotobop pro((int,int));
extern	int	fillpara pro((int,int));
extern	int	setfillcol pro((int,int));
extern	int	killpara pro((int,int));
extern	int	fillmode pro((int,int));

static struct MenuBinding ParaItems[] = {
	{ "Forward Paragraph  M-]",	gotoeop		},
	{ "Backward Paragraph M-[",	gotobop		},
	{ "Fill Paragraph     M-q",	fillpara	},
	{ "Set Fill Column    C-x f",	setfillcol	},
	{ "Kill Paragraph",		killpara	},
	{ "Auto Fill Mode",		fillmode	}
};

/*
 * Region stuff
 */
extern	int	setmark pro((int,int));
extern	int	swapmark pro((int,int));
extern	int	killregion pro((int,int));
extern	int	copyregion pro((int,int));
extern	int	lowerregion pro((int,int));
extern	int	upperregion pro((int,int));

static struct MenuBinding RegionItems[] = {
	{ "Set Mark            C-@",	setmark		},
	{ "Exch Point And Mark C-x C-x",swapmark	},
	{ "Kill Region         C-w",	killregion	},
	{ "Copy Region As Kill M-w",	copyregion	},
	{ "Downcase Region     C-x C-l",lowerregion	},
	{ "Upcase Region       C-x C-u",upperregion	}
};

/*
 * Commands for manipulating windows
 */

extern	int	splitwind pro((int,int));
extern	int	delwind pro((int,int));
extern	int	onlywind pro((int,int));
extern	int	nextwind pro((int,int));
#ifdef	PREVWIND
extern	int	prevwind pro((int,int));
#endif
extern	int	enlargewind pro((int,int));
extern	int	shrinkwind pro((int,int));
extern	int	refresh pro((int,int));
extern	int	reposition pro((int,int));
extern	int	togglewindow pro((int,int));
#ifdef	CHANGE_FONT
extern	int	setfont pro((int,int));
#endif

static struct MenuBinding WindowItems[] = {
	{ "Split Window         C-x 2", splitwind	},
	{ "Delete Window        C-x 0",	delwind		},
	{ "Delete Other Windows C-x 1",	onlywind	},
	{ "Next Window          C-x o",	nextwind	},
#ifdef	PREVWIND
	{ "Up Window",			prevwind	},
#endif
	{ "Enlarge Window       C-x ^",	enlargewind	},
	{ "Shrink Window",		shrinkwind	},
	{ "Redraw Display",		refresh		},
	{ "Recenter             C-l",	reposition	},
	{ "Toggle Border",		togglewindow	},
#ifdef	CHANGE_FONT
	{ "Set Font",			setfont		}
#endif
};

/*
 * Miscellaneous commands
 */

#ifndef	NO_MACRO	/* Dec.20,1992 by H.Ohkubo */
extern 	int	definemacro pro((int,int));
extern	int	finishmacro pro((int,int));
extern	int	executemacro pro((int,int));
#endif
extern	int	extend pro((int,int));
extern	int	bindtokey pro((int,int));
extern	int	desckey pro((int,int));
extern	int	wallchart pro((int,int));
extern	int	showversion pro((int,int));
extern	int	spawncli pro((int,int));

static struct MenuBinding MiscItems[] = {
#ifndef	NO_MACRO	/* Dec.20,1992 by H.Ohkubo */
	{ "Start Kbd Macro   C-x (",	definemacro	},
	{ "End Kbd Macro     C-x )",	finishmacro	},
	{ "Call Kbd Macro    C-x e",	executemacro	},
#endif
	{ "Execute Command   M-x",	extend		},
	{ "Global Set Key",		bindtokey	},
	{ "Describe Key      C-h c",	desckey		},
	{ "Describe Bindings C-h b",	wallchart	},
	{ "Emacs Version",		showversion	},
	{ "New CLI           C-z",	spawncli	}
};

#ifdef INCLUDE_SKG
/*
 * Simple Kanji Generator commands.
 */

extern int skginput pro((int,int));
extern int k_rot_buffio pro((int,int));
extern int k_rot_input pro((int,int));
extern int k_rot_display pro((int,int));

static struct MenuBinding SkgItems[] = {
	{ "Kanji Input(SKG) Start ESC C-k  ",	skginput	},
        { "Kanji buffer I/O Code  C-x C-k f",	k_rot_buffio    },
	{ "Kanji Input Code       C-x C-k i",	k_rot_input	},
	{ "Kanji Display Code     C-x C-k d",	k_rot_display	}
};

#endif

/*
 * The following table contains the titles, number of items, and
 * pointers to, the individual menus.
 */

static struct MenuInfo EMInfo[] = {
	{ "File  ",		NITEMS(FileItems),	&FileItems[0]	},
	{ "Edit  ",		NITEMS(EditItems),	&EditItems[0]	},
	{ "Move  ", 		NITEMS(MoveItems),	&MoveItems[0]	},
	{ "Search  ",		NITEMS(SearchItems),	&SearchItems[0] },
	{ "Word  ",		NITEMS(WordItems),	&WordItems[0]	},
	{ "Paragraph  ",	NITEMS(ParaItems),	&ParaItems[0]	},
	{ "Region  ",		NITEMS(RegionItems),	&RegionItems[0]	},
	{ "Window  ",		NITEMS(WindowItems),	&WindowItems[0] },
#ifdef INCLUDE_SKG
	{ "Japanese Env.  ",	NITEMS(SkgItems),	&SkgItems[0] },
#endif
	{ "Miscellaneous  ",	NITEMS(MiscItems),	&MiscItems[0]	}
};

/* There are three cases to deal with; the menu alone, the Browser
 * alone, and both of them together.  We #define some things to make
 * life a little easier to deal with
 */
# ifdef	BROWSER
#  define Edit_Menu_Init() Menu_Add("Edit ", TRUE, FALSE) 
#  define Edit_Menu_Add(n) Menu_Item_Add(n,(USHORT)ITEMENABLED,0L,(BYTE)0,FALSE)
#  define Edit_Item_Add(n) Menu_SubItem_Add(n,(USHORT)ITEMENABLED,0L,(BYTE)0,FALSE)
# else
#  define Edit_Menu_Init() cinf = NULL	/* harmless */
#  define Edit_Menu_Add(n) n[strlen(n)-1] = '\0'; Menu_Add(n, TRUE, FALSE)
#  define Edit_Item_Add(n) Menu_Item_Add(n,(USHORT)ITEMENABLED,0L,(BYTE)0,FALSE)
# endif	BROWSER

#endif	MENU

/*
 * Initialize the Emacs menu
 */

struct Menu * InitEmacsMenu(EmW)
struct Window *EmW;
{
#ifdef	MENU
	register struct MenuInfo *cinf;
	register struct MenuBinding *lastbinding, *cb;
	struct MenuInfo *lastinfo;
#endif

	Menu_Init() ;			/* init the menu		*/

#ifdef	MENU
	Edit_Menu_Init() ;		/* Set up for editing menu	*/
	lastinfo = &EMInfo[NITEMS(EMInfo)];	/* loop sentinel	*/	
	for (cinf = EMInfo; cinf < lastinfo; cinf++) {
		Edit_Menu_Add(cinf->Name);
		lastbinding = &cinf->Items[cinf->NumItems];
		for (cb = cinf->Items; cb < lastbinding; cb++)
			Edit_Item_Add(cb->Command);
	}
#endif	MENU

#ifdef	BROWSER

# ifdef ASL
	Menu_Add("ASL Req. ", TRUE, FALSE) ;/* name is already there */
        Menu_Item_Add("Find File   ",(USHORT) ITEMENABLED, 0L, (BYTE) 0, FALSE );
        Menu_Item_Add("Insert File ",(USHORT) ITEMENABLED, 0L, (BYTE) 0, FALSE );
        Menu_Item_Add("Write File  ",(USHORT) ITEMENABLED, 0L, (BYTE) 0, FALSE );
        Menu_Item_Add("Remote Box  ",(USHORT) ITEMENABLED, 0L, (BYTE) 0, FALSE );
# else
	Menu_Add("Disks ", TRUE, FALSE) ;/* name is already there */
	Add_Devices(DLT_DEVICE);	/* devices first */
	Add_Devices(DLT_VOLUME);	/* mounted volume names next */
	Add_Devices(DLT_DIRECTORY);	/* assigned directories last */
# endif /* ASL */ 
#endif	BROWSER
	return 	AutoMenu ;
}

/*
 * amigamenu() -- handles a menu pick.
 */

amigamenu(f, n) {
	unsigned short		menunum, itemnum, subnum, Menu_Number;
	char			*name;
	register PF		fp;

#ifdef	BROWSER
	register unsigned short	level, i, dirp;
	register char		*cp;
	int			stat;
	struct MenuItem		*ItemAddress() ;

	/* State variables that describe the current directory */
	static char		Dir_Name[LONGEST_NAME] ;
	static unsigned short	Menu_Level = 0 ;
#endif

#ifndef	NO_MACRO
	if (inmacro) 
		return (FALSE);	/* menu picks aren't recorded */
#endif

	/* read the menu, item, and subitem codes from the input stream */
	menunum = getkey(FALSE) - MN_OFFSET;
	itemnum = getkey(FALSE) - MN_OFFSET;
	subnum = getkey(FALSE) - MN_OFFSET;

#ifndef	NO_MACRO
	if (macrodef) {		/* menu picks can't be practically recorded */
		ewprintf("Can't record menu selections");
		return (FALSE);
	}
#endif

	Menu_Number = (USHORT)
		(SHIFTMENU(menunum) | SHIFTITEM(itemnum) | SHIFTSUB(subnum));

#ifndef	BROWSER
# ifdef	MENU
	fp = EMInfo[menunum].Items[itemnum].Function;
	return (*(fp)(f, n));
# endif
#else	/* we're using the Browser */
# ifdef	MENU
	/* Handle commands from the Edit menu when using the Browser */
	if (0 == menunum) {
		fp = EMInfo[itemnum].Items[subnum].Function;
		return ((*fp)(f, n));
	}
# endif
	/* Here when a selection was made in a Browser menu */
	name = (char *)((struct IntuiText *)
		(ItemAddress(AutoMenu,(ULONG) Menu_Number) -> ItemFill))
		-> IText ;
	level = MENUNUM(Menu_Number) - FIRSTMENU;

	if (itemnum == 3 ) {		
	   stat = Remote_Box();
	   return ( stat );	
	}
# ifdef ASL
        stat = FALSE;
        if(AslBase = OpenLibrary("asl.library",37L))
 	  {
           if (fr = (struct FileRequester *) AllocFileRequest() )
	     {
               if(AslRequestTags(fr,
                  ASL_Hail,      (ULONG) "Click File name for Read.",
                  ASL_Height,    ASLHEIGHT,
                  ASL_Width,     ASLWIDTH,
                  ASL_LeftEdge,  ASLLEFTEDGE,
                  ASL_TopEdge,   ASLTOPEDGE,
                  ASL_OKText,    (ULONG) "OK",
                  ASL_CancelText,(ULONG) "Cancel",
                  ASL_File      ,(ULONG) "",
                  ASL_Dir       ,(ULONG) ASLDirectory, 
                  TAG_DONE))
		 {
                   strcpy(ASLDirectory,fr->rf_Dir );
		   if ( strlen( fr->rf_File ) == 0 ) stat = FALSE; 
		   else	  
		    {
                     strcpy(Dir_Name , fr->rf_Dir );
                     strcpy(name ,fr->rf_File );
                     switch ( itemnum )
			{
			  case 0: stat = Display_File( Dir_Name , name );
                                  strcpy(name, "Find File   ");
                                  break;
			  case 1: stat = Insert_File( Dir_Name , name );
                                  strcpy(name, "Insert File ");
                                  break; 
			  case 2: stat = Write_File( Dir_Name , name );
                                  strcpy(name, "Write File  ");
                                  break;
			  defalut:stat = FALSE; break;
			}

		    }

		 }
               FreeFileRequest(fr);
	     }
           CloseLibrary(AslBase);
	  }
# else
	name = (char *)((struct IntuiText *)
		(ItemAddress(AutoMenu,(ULONG) Menu_Number) -> ItemFill))
		-> IText ;
	level = MENUNUM(Menu_Number) - FIRSTMENU;

	/* Got what we want, so clear the menu to avoid confusing the user */
	ClearMenuStrip(EmW) ;

	/* set dirp to FALSE if the name is not a directory or disk */
	dirp = (strchr(name, '/') != NULL || strchr(name, ':') != NULL) ;

	/* First, set the directory name right */
	if (level > Menu_Level)			/* Not possible, die */
		panic("impossible menu_level in amigamenu");
	else if (level == 0)			/* picked a new disk */
		Dir_Name[0] = '\0' ;
	else if (level < Menu_Level) {		/* Throw away some levels */
		for (i = 1, cp = strchr(Dir_Name, ':'); i < level; i++) {
			if (cp == NULL) return FALSE;
			cp = strchr(cp, '/') ;
			}
		if (cp == NULL) panic("broken file name in amigamenu");
		*++cp = '\0' ;
		}
	/* else Menu_Level == level, chose a file a current level */

	/* Now, fix up the menu and it's state variable */
	while (Menu_Level > level) {
		Menu_Level-- ;
		Menu_Pop() ;
		}

	/* If we added a file, visit it, else add a
	 * new directory level to the menu.
	 */
	if (!dirp)
		stat = Display_File(Dir_Name, name) ;
	else {
		Menu_Level++ ;
		(void) strncat(Dir_Name, name,
			LONGEST_NAME - strlen(Dir_Name) - 1) ;
		stat = Add_Dir(Dir_Name, name) ;
	}
	SetMenuStrip(EmW, AutoMenu) ;
# endif /* ASL */
	return stat ;
#endif	BROWSER
}

#ifdef	BROWSER
/*
 * Display_File - Go fetch a the requested file into a window.
 */
Display_File(dir, file) char *dir, *file; {
	register BUFFER	*bp;
	BUFFER *findbuffer();
	int		s;
	char		File_Name[LONGEST_NAME], *fn, *adjustname();

#ifdef ASL
	(void) strcpy(File_Name, dir);
        AddPart( File_Name , file , LONGEST_NAME );
	fn = adjustname(File_Name);
	if ((bp = findbuffer(fn)) == NULL) return FALSE;
	curbp = bp;
	if (showbuffer(bp, curwp, WFHARD) != TRUE) return FALSE;
	if (bp->b_fname[0] == 0)
		return (readin(fn));		/* Read it in.	*/
#else
	(void) strcpy(File_Name, dir);
	(void) strncat(File_Name, file, LONGEST_NAME - strlen(File_Name) - 1) ;
	fn = adjustname(File_Name);
	if ((bp = findbuffer(fn)) == NULL) return FALSE;
	curbp = bp;
	if (showbuffer(bp, curwp, WFHARD) != TRUE) return FALSE;
	if (bp->b_fname[0] == 0)
		return (readin(fn));		/* Read it in.	*/
#endif
	return TRUE;
	}
/*
 * Insert File.
 *
 */
#ifdef ASL
Insert_File(dir, file) char *dir, *file; {
	char		File_Name[LONGEST_NAME], *fn, *adjustname();

	(void) strcpy(File_Name, dir);
        AddPart( File_Name , file , LONGEST_NAME );

#ifdef READONLY
	if (curbp->b_flag & BFRONLY ) { /* If This buffer is Read-only, */
              warnreadonly();           /* do only displaying warning.  */
              return TRUE;
	}
#endif 
	fn = adjustname(File_Name);
        return insertfile( fn, (char *)NULL );
                                               /* don't set buffer name */
}

/*
 * Write File.
 *
 */

static char *itos(bufp, num)
char *bufp;
unsigned num;
{
	if (num >= 10) {
		bufp = itos(bufp, num/10);
		num %= 10;
	}
	*++bufp = '0' + num;
	return bufp;
}

Write_File(dir, file) char *dir, *file; {
	int		s;
	char		File_Name[LONGEST_NAME];
        char            *fn, *adjustname();

	(void) strcpy(File_Name, dir);
        AddPart( File_Name , file , LONGEST_NAME );
     
        fn = adjustname( File_Name );
	if((s=writeout( curbp , fn )) == TRUE){
          (VOID) strcpy( curbp->b_fname , fn );
#ifndef NO_BACKUP
          curbp->b_flag &= ~(BFBAK | BFCHG);
#else
          curbp->b_flag &= ~BFCHG;
#endif        
	 {
           BUFFER         *bp;
           char           bname[LONGEST_NAME], *cp;
	   unsigned       count = 1;	  

           makename( bname , fn );
           cp = bname + strlen( bname );
           while((bp= bfind(bname,FALSE)) != NULL ) {
		   if (fncmp(bp->b_fname, fn ) == 0 ) {
			   break;
		   }
		   *cp = '<';
		   (VOID) strcpy( itos(cp, ++count)+1 ,">" );
	   }
	   if ((cp = malloc((unsigned)(strlen(bname)+1))) != NULL ) {
		   (VOID) strcpy( cp ,bname );
		   free( curbp->b_bname );
		   curbp->b_bname = cp;
	   }

	 }
         upmodes(curbp);
	}
       return s; 
}
#endif


/*
 * Add_Dir - given a dir and a name, add the menu name with the files in
 *	dir as entries.  Use AllocMem() in order to make
 *      sure the file info block is on a longword boundary.
 */
static
Add_Dir(dir, name) char *dir, *name; {
	register char			*last_char ;
	register struct FileLock	*my_lock;
	struct FileLock			*Lock();
	unsigned short			count ;
	int				stat = FALSE;
	static char			Name_Buf[LONGEST_NAME] ;
	char				*AllocMem();
	struct	FileInfoBlock		*File_Info;

	if ((File_Info = (struct FileInfoBlock *)
		AllocMem((LONG)sizeof(struct FileInfoBlock), 0L)) == NULL)
		return (FALSE);

	/* Fix up the trailing / if it needs it */
	last_char = &dir[strlen(dir) - 1] ;
	if (*last_char == '/') *last_char = '\0' ;

	/* Now, start on the directory */
	if ((my_lock = Lock(dir, ACCESS_READ)) == NULL) goto out;

	if (!Examine(my_lock, File_Info)) goto out;
	if (File_Info -> fib_DirEntryType < 0L)
		goto out;

	if (Menu_Add(name, TRUE, TRUE) == 0) goto out;
	for (count = 0; ExNext(my_lock, File_Info) 
			|| IoErr() != ERROR_NO_MORE_ENTRIES; count++)
		if (File_Info -> fib_DirEntryType < 0L) {
			if (Menu_Item_Add(File_Info -> fib_FileName,
				(USHORT)ITEMENABLED, 0L, (BYTE)0, TRUE)
					== MNUM(NOMENU, NOITEM, NOSUB))
					break ;
			}
		else {
			(void) strcpy(Name_Buf, File_Info -> fib_FileName) ;
			(void) strcat(Name_Buf, "/") ;
			if (Menu_Item_Add(Name_Buf,
				(USHORT) ITEMENABLED, 0L, (BYTE)0, TRUE)
					 == MNUM(NOMENU, NOITEM, NOSUB))
				break ;
			}
	if (count == 0) Menu_Item_Add("EMPTY", (USHORT)0, 0L, (BYTE)0, FALSE) ;

	/* Put everything back */
	if (*last_char == '\0') *last_char = '/' ;
	stat = TRUE;
out:
	UnLock(my_lock) ;
	FreeMem(File_Info, (LONG) sizeof(struct FileInfoBlock));
	return stat;
	}

/*
 * Add all the devices currently known by the system
 * to the current menu, based on the type of device
 * list entry desired.  Disable multitasking while
 * we look inside the device list, so we don't fly off
 * into space while traversing it.
 */
struct DosLibrary	*DosBase;
#ifndef ASL
extern APTR		OpenLibrary();
#endif

static VOID
Add_Devices(devtype)
ULONG devtype;
{
	register struct DeviceList	*devlist;
	struct RootNode			*rootnode;
	struct DosInfo			*dosinfo;
	UBYTE				buffer[80];
	int				ramflag = 0;

	/* if you've gotten this far, none of these will be null. */
	DosBase = (struct DosLibrary *) OpenLibrary(DOSNAME,0L);

	Forbid();			/* let's be careful out there... */
	rootnode = (struct RootNode *) DosBase->dl_Root;
	dosinfo = (struct DosInfo *) BADDR(rootnode->rn_Info);
	devlist = (struct DeviceList *) BADDR(dosinfo->di_DevInfo);

	while (devlist) {
		/* select by specified device type */
		if (devlist->dl_Type != devtype) {
			devlist = (struct DeviceList *) BADDR(devlist->dl_Next);
			continue;
		}

		/* convert device's name into AmigaDOS name and concat a ":" */
		btocstr((BPTR) devlist->dl_Name,buffer,sizeof(buffer));
		strcat(buffer,":");

		/* Always add volumes and assigned directories. However,
		 * disks should be the only devices added to the list. Magic
		 * disk test courtesy of Phillip Lindsay, Commodore-Amiga Inc.
		 */
		if (devtype != DLT_DEVICE)
			Menu_Item_Add(buffer, (USHORT)ITEMENABLED,
					0L, (BYTE)0, TRUE);
		else if (devlist->dl_Task) {	/* why does this work? */
			Menu_Item_Add(buffer, (USHORT)ITEMENABLED,
					0L, (BYTE)0, TRUE);
			if (!strcmp(buffer,"RAM:")) ramflag = 1;
		}
		devlist = (struct DeviceList *) BADDR(devlist->dl_Next);
	}
	/* if ramdisk isn't loaded yet, add it anyway */
	if ((devtype == DLT_DEVICE) && !ramflag)
		Menu_Item_Add("RAM:",(USHORT)ITEMENABLED, 0L, (BYTE) 0, FALSE);
	Permit();
	CloseLibrary(DosBase);
}

btocstr(bp,buf,bufsiz)
BPTR bp;
char *buf;
int bufsiz;
{
	register UBYTE	*cp;
	register int	len, i;

	cp = (UBYTE *) BADDR(bp);
	len = (int) *(cp++);
	len = (len > bufsiz) ? bufsiz : len;	/* truncate if necessary */
	for (i = 0; i < len; i++)
		buf[i] = *(cp++);
	buf[i] = '\0';
	return (len < bufsiz);			/* return FALSE if truncated */
}



/* For Remote Box */

BOOL Remote_Box()
{
  ULONG  winsignal, signalmask, signals;
  BOOL done = FALSE;

  OpenWindowWin0();
  ewprintf("Open REMOTE BOX Window...");	
  update();

  winsignal  = 1L << Win0->UserPort->mp_SigBit;
  signalmask = winsignal;

  while (!done)
   {		
     signals = Wait(signalmask);
     done = handleIDCMP( Win0 );
   }
  CloseWindowWin0(); 
  ewprintf("Close REMOTE BOX Window...");	
  return( done );
}

BOOL handleIDCMP( win )
struct Window *win;
{
  BOOL done = FALSE;
  struct IntuiMessage *message = NULL;
  struct Gadget *gad;
  ULONG class;
  int goto_num;

  while( message = (struct IntuiMessage *)GetMsg( win->UserPort ))
	{
	   class = message->Class;

	   if ( (class == IDCMP_GADGETUP) || (class == IDCMP_GADGETDOWN))
              	gad = (struct Gadget *)(message->IAddress);

	   ReplyMsg( (struct Message *)message );
	   switch( class )
		{	
		  case IDCMP_GADGETUP:
			switch( gad->GadgetID )
				{
				 case 0:  /* Top    */	
				  gotobob(FFRAND , 0 );
				  break;	
				 case 1:  /* Up     */	
				  setgoal();	
				  backline( FFRAND , 1 );	
				  break;	
				 case 2:  /* Redraw */	
				  refresh( FFRAND , 0);	
				  update();	
				  break;	
				 case 3:  /* |<     */	
				  gotobol( FFRAND,  0);	
				  break;	
				 case 4:  /*  <	    */
				  backchar(FFRAND , 1);
				  break;	
				 case 5:  /*  >     */	
				  forwchar(FFRAND , 1);
				  break;	
				 case 6:  /*  >|    */ 	
				  gotoeol( FFRAND , 0);
				  break;	
				 case 7:  /* Down   */	
				  setgoal();	
				  forwline( FFRAND ,  1); 
				  break;	
				 case 8:  /* Bottom */	
				  gotoeob( FFRAND,  0);
				  break;	
				 case 9:  /* PgUp   */	
				  backpage( FFRAND,  2); 
				  break;	
				 case 10: /* PgDown */	
				  forwpage( FFRAND,  2); 
				  break;	
				 case 11: /* <<     */	
				  backword( FFRAND,  1); 
				  break;	
				 case 12: /* >>      */	
 				  forwword( FFRAND,  1); 
				  break;	
				 case 13: /* Show    */	
 				  showcpos( FFRAND,  0); 
				  break;	
				 case 14: /* Goto:   */	
				 gotoline(FFARG ,
					  ((struct StringInfo *)(gad->SpecialInfo))->LongInt ); 
				  break;	
				 case 15:  /* SetFont   */	
				  ASLSetFont();
				  break;	
				 case 16:  /* SplitWindow */	
				  splitwind(FFRAND , 0);
				  break;	
				 case 17:  /* NextWindow */	
				  nextwind(FFRAND , 0);
				  break;	
				 case 18:  /* CloseWindow */	
				  delwind(FFRAND, 0);
				  break;	
				 case 19:  /* ToggleWindow */	
				  togglewindow(FFRAND, 0);
 	  			  WindowToFront(Win0);
				  ActivateWindow(Win0);
				  break;	
				 case 20:  /* Done   */	
				  done = TRUE;	
				  break;	
				 default:	
				  ewprintf("Still Not implement...Sorry...");	
				  break;	
				}
			update();
			break;
 		  case IDCMP_CLOSEWINDOW:
			done = TRUE;
			break;
		  default:	
			break;
		}
	}
	return ( done );
}

void ASLSetFont()
{
  struct FontRequester *frqtr;
  register struct TextFont *newfont;
  register int	s;

  if (AslBase = OpenLibrary("asl.library", 37L ) )  
    {
      if ( frqtr = (struct FontRequester *)
	    AllocAslRequestTags(ASL_FontRequest,
				ASL_FontName, (ULONG )"topaz.font",
				ASL_FontHeight, 8L,
				ASL_FuncFlags, FONF_FIXEDWIDTH,
				TAG_DONE ))
	    {
		    if (AslRequest( frqtr , NULL ))
		       {    
			 /* Look for the font */
			 ewprintf("Looking for %s %d...",frqtr->fo_Attr.ta_Name,frqtr->fo_Attr.ta_YSize);
			 if ((newfont = OpenDiskFont(&(frqtr->fo_Attr))) == NULL) {
			    ewprintf("Can't find %s %d!",frqtr->fo_Attr.ta_Name,frqtr->fo_Attr.ta_YSize);
			    goto CanNotOpen;	 
			 } 

			 if ((newfont->tf_Flags & FPF_PROPORTIONAL) &&
			     (((s = eyesno("Use proportional font")))!= TRUE)) {
			    CloseFont(newfont);
			    goto CanNotOpen;
			 }

			 /* Get rid of old font and reopen with the new one */
			 CloseFont(EmFont);
			 EmFont = newfont;

			 tthide ( FALSE );      
			 ttshow ( FALSE );      
			 update();      

			 WindowToFront(Win0);
			 ActivateWindow(Win0);

			 ewprintf("Now using font %s %d",frqtr->fo_Attr.ta_Name,EmFont->tf_YSize);

		        CanNotOpen:			 
		       }
		    FreeAslRequest( frqtr );
	    }
      CloseLibrary( AslBase );
    }   
  else 
    {
      ewprintf("Can't Open asl.library...");	    
    }
  return;	
		
}


int OpenWindowWin0()
{
struct Screen *Scr;
UWORD offx, offy;
UWORD loop;
struct NewGadget newgad;
struct Gadget *Gad;
if (Win0 == NULL)
	{
	if (NULL != (Scr = LockPubScreen(NULL)))
		{
		offx = Scr->WBorLeft;
		offy = Scr->WBorTop + Scr->Font->ta_YSize+1;
		if (NULL != ( Win0VisualInfo = GetVisualInfoA( Scr, NULL)))
			{
			Win0GList = NULL;
			Gad = CreateContext( &Win0GList);
			for ( loop=0 ; loop<21 ; loop++ )
				{
				CopyMem((char * )&Win0NewGadgets[loop], ( char * )&newgad, (long)sizeof( struct NewGadget ));
				newgad.ng_VisualInfo = Win0VisualInfo;
				newgad.ng_LeftEdge += offx;
				newgad.ng_TopEdge += offy;
				Win0Gadgets[ newgad.ng_GadgetID - Win0FirstID ] = Gad = CreateGadgetA( Win0GadgetTypes[loop], Gad, &newgad, newgad.ng_UserData );
				}
			if (Gad != NULL)
				{
				if (NULL != (Win0 = OpenWindowTags( NULL, WA_Left, 300,
								WA_Top, 21,
								WA_Width, 323+offx,
								WA_Height, 88+offy,
								WA_Title, "Kg Remote Box",
								WA_MinWidth, 150,
								WA_MinHeight, 25,
								WA_MaxWidth, 1200,
								WA_MaxHeight, 1200,
								WA_DepthGadget, TRUE,
								WA_Activate, TRUE,
								WA_CloseGadget, TRUE,
								WA_Dummy+0x30, TRUE,
								WA_SmartRefresh, TRUE,
								WA_AutoAdjust, TRUE,
								WA_DragBar, TRUE,
								WA_Gadgets, Win0GList,
								WA_Zoom, Win0ZoomInfo,
								WA_IDCMP,580,
								TAG_END)))
					{
					GT_RefreshWindow( Win0, NULL);
					UnlockPubScreen( NULL, Scr);
					return( 0L );
					}
				}
			FreeGadgets( Win0GList);
			FreeVisualInfo( Win0VisualInfo );
			}
		UnlockPubScreen( NULL, Scr);
		}
	}
else
	{
	WindowToFront(Win0);
	ActivateWindow(Win0);
	return( 0L );
	}
return( 1L );
}

void CloseWindowWin0()
{
if (Win0 != NULL)
	{
	CloseWindow( Win0);
	Win0 = NULL;
	FreeVisualInfo( Win0VisualInfo);
	FreeGadgets( Win0GList);
	}
}

#endif	BROWSER
