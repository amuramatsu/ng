/* $Id: ttymenu.c,v 2.3 2000/12/14 18:08:35 amura Exp $ */
/*
 * ttymenu.c
 *   Amiga intuition menu handling routine for Ng 1.x
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
 * $Log: ttymenu.c,v $
 * Revision 2.3  2000/12/14 18:08:35  amura
 * filename length become flexible
 *
 * Revision 2.2  2000/10/11 13:38:59  amura
 * change wildcard in ASL requester
 *
 * Revision 2.1  2000/10/02 14:03:14  amura
 * rewrite from scratch
 *
 */

#include "config.h"
#ifdef	DO_MENU
#include "def.h"

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfxbase.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#define	MENU_SUB_ICON	" \273"
#include "menumap.h"

struct Menu *    InitEmacsMenu(struct Window *);
VOID             DisposeMenus(struct Menu *);
struct MenuItem *MakeMenuItems(MenuMap *, struct TextAttr *);
VOID             DisposeMenuItems(struct MenuItem *);
int amigamenu pro((int, int));

/*
 * internal function prototypes
 * these functions based on menulayout.c included
 *    in "AMIGA DEVELOPER CD 1.2"
 */
static USHORT MaxLength(struct RastPort *textRPort,
			struct MenuItem *first_item, USHORT char_size);
static VOID adjustItems(struct RastPort *textRPort,
			struct MenuItem *first_item, USHORT char_size,
			USHORT height, USHORT level, USHORT left_edge);
static VOID adjustMenus(struct Menu *first_menu, struct Window *win);

struct Menu *
InitEmacsMenu(win)
struct Window *win;
{
    int  i, num;
    struct Menu *menu;

    num = sizeof(MgMenus)/sizeof(MenuMap) - 1;

    menu = (struct Menu *)AllocMem(num*sizeof(struct Menu),
				   MEMF_PUBLIC|MEMF_CLEAR);
    if (menu == NULL)
	return NULL;
    
    for (i=0; i<num; i++)
    {
	menu[i].NextMenu = &menu[i+1];
	menu[i].Flags    = MENUENABLED;
	menu[i].MenuName = MgMenus[i].face;
	menu[i].FirstItem = (MgMenus[i].type==MENU_SUB) ?
	    MakeMenuItems((MenuMap*)MgMenus[i].func, win->WScreen->Font):
	    (struct MenuItem*)MENUNULL;
    }
    /* Menu Last mark */
    menu[num-1].NextMenu = NULL;

    adjustMenus(menu, win);
    return menu;
}


VOID
DisposeMenus(menu)
struct Menu *menu;
{
    long num;
    struct Menu *p;

    if (menu == NULL)
	return;
    for (p=menu,num=0; p!=NULL; p=p->NextMenu,num++)
	DisposeMenuItems(p->FirstItem);
    
    FreeMem(menu, num*sizeof(struct Menu));
}


struct MenuItem *
MakeMenuItems(em, font)
MenuMap *em;
struct TextAttr *font;
{
    int i, num;
    struct IntuiText *texts;
    struct MenuItem  *items;
    MenuMap *p;

    /* counting menu items */
    for (p=em,num=0; p->type!=MENU_END; p++,num++);

    texts  = (struct IntuiText *)AllocMem(num*sizeof(struct IntuiText),
					  MEMF_PUBLIC|MEMF_CLEAR);
    if (texts == NULL)
	return NULL;
    items = (struct MenuItem *)AllocMem(num*sizeof(struct MenuItem),
					MEMF_PUBLIC|MEMF_CLEAR);
    if (items == NULL)
    {
	FreeMem(texts, num*sizeof(struct IntuiText));
	return NULL;
    }

    for (i=0; i<num; i++)
    {
	if (MENU_TYPE(em[i].type) == MENU_SUB)
	{
	    items[i].SubItem  =	MakeMenuItems(em[i].func, font);
	    if (items[i].SubItem == NULL)
	    {
		
		for (i--; i>=0; i--)
		    DisposeMenuItems(items[i].SubItem);
		
		/* Dispose now making Items */
		FreeMem(texts, num*sizeof(struct IntuiText));
		FreeMem(items, num*sizeof(struct MenuItem));
		return NULL;
	    }
	}
	else
	    items[i].SubItem   = (struct MenuItem*)NULL;
	items[i].NextItem      = &items[i+1];
	if (MENU_TYPE(em[i].type) == MENU_LINE)
	    items[i].Flags     = ITEMTEXT|ITEMENABLED|HIGHNONE;
	else
	    items[i].Flags     = ITEMTEXT|ITEMENABLED|HIGHCOMP;
	items[i].ItemFill  = &texts[i];
	
	/* Make IntuiText for menu face */
	texts[i].FrontPen  = 0;
	texts[i].BackPen   = 1;
	texts[i].DrawMode  = JAM2;
	texts[i].LeftEdge  = 0;
	texts[i].TopEdge   = 1;
	texts[i].IText     = (UBYTE*)em[i].face;
	texts[i].ITextFont = font;
    }
    items[num-1].NextItem = NULL;	/* Menu Last mark */

    return items;
}


VOID
DisposeMenuItems(items)
struct MenuItem *items;
{
    long num;
    struct MenuItem *p;
    
    if (items==NULL || items==(struct MenuItem *)MENUNULL)
	return;
    
    /* count menu items */
    for (p=items,num=0; p!=NULL; p=p->NextItem,num++)
	DisposeMenuItems(p->SubItem);

    FreeMem(items->ItemFill, num*sizeof(struct IntuiText));
    FreeMem(items,           num*sizeof(struct MenuItem) );
}



/* Steps thru each item to determine the maximum width of the strip */
static USHORT
MaxLength(struct RastPort *textRPort,
	  struct MenuItem *first_item, USHORT char_size)
{
    USHORT maxLength;
    USHORT total_textlen;
    struct MenuItem  *cur_item;
    struct IntuiText *itext;
    USHORT extra_width;
    USHORT maxCommCharWidth;
    USHORT commCharWidth;

#ifdef	notdef
    extra_width = char_size;  /* used as padding for each item. */
#endif
    
    /*
     * Find the maximum length of a command character, if any.
     * If found, it will be added to the extra_width field.
     */
    maxCommCharWidth = 0;
    for (cur_item=first_item; cur_item!=NULL; cur_item=cur_item->NextItem)
    {
	if (cur_item->Flags & COMMSEQ)
	{
	    commCharWidth = TextLength(textRPort,&(cur_item->Command),1);
	    if (commCharWidth > maxCommCharWidth)
		maxCommCharWidth = commCharWidth;
	}
    }

    /*
     * if we found a command sequence, add it to the extra required space.
     * Add space for the Amiga key glyph plus space for the command
     * character.
     *
     * Note this only works for HIRES screens, for LORES, use LOWCOMMWIDTH.
     */
    if (maxCommCharWidth > 0)
	extra_width += maxCommCharWidth + COMMWIDTH;
    
    /*
     * Find the maximum length of the menu items, given the extra width
     * calculated above.
     */
    maxLength = 0;
    for (cur_item=first_item; cur_item!=NULL; cur_item=cur_item->NextItem)
    {
	itext = (struct IntuiText *)cur_item->ItemFill;
	total_textlen = extra_width + itext->LeftEdge +
	    TextLength(textRPort, itext->IText, strlen(itext->IText));
	
	/* returns the greater of the two */
	if (total_textlen > maxLength)
	    maxLength = total_textlen;
    }
    return maxLength;
}


/* Adjust the MenuItems and SubItems */
static VOID
adjustItems(struct RastPort *textRPort, struct MenuItem *first_item,
	    USHORT char_size, USHORT height,
	    USHORT level, USHORT left_edge)
{
    register USHORT item_num;
    struct   MenuItem *cur_item;
    USHORT   strip_width, subitem_edge;
    
    if (first_item==NULL || first_item==(struct MenuItem *)MENUNULL)
	return;

    /* The width of this strip is the maximum length of its members. */
    strip_width = MaxLength(textRPort, first_item, char_size);

    /* Position the items. */
    for (cur_item=first_item,item_num=0; cur_item!=NULL;
	 cur_item=cur_item->NextItem,item_num++)
    {
	cur_item->TopEdge  = (item_num * height) - level;
	cur_item->LeftEdge = left_edge;
	cur_item->Width    = strip_width - left_edge;
	cur_item->Height   = height;
	
	/* place the sub_item 3/4 of the way over on the item. */
	subitem_edge = strip_width - (strip_width >> 2);
	/* subitem_edge = strip_width >> 2; */

	adjustItems(textRPort, cur_item->SubItem,
		    char_size, height, 1, subitem_edge);
    }
}


/*
 * The following routines adjust an entire menu system to conform to
 * the specified fonts' width and height.  Allows for Proportional Fonts.
 * This is necessary for a clean look regardless of what the users
 * preference in Fonts may be.  Using these routines, you don't need to
 * specify TopEdge, LeftEdge, Width or Height in the MenuItem structures.
 *
 * NOTE that this routine does not work for menus with images, but assumes
 * that all menu items are rendered with IntuiText.
 *
 * This set of routines does NOT check/correct if the menu runs off
 * the screen due to large fonts, too many items, lo-res screen.
 */
VOID
adjustMenus(struct Menu *first_menu, struct Window *win)
{
    struct Menu      *cur_menu;
    USHORT            start, char_size, height;

    /* Get the Width of the Font */
    char_size = TextLength(win->RPort, "n", 1);

    /*
     * To prevent crowding of the Amiga key when using COMMSEQ,
     * don't allow the items to be less than 8 pixels high.
     * Also, add an extra pixel for inter-line spacing.
     */
    if (FontHeight(win) > 8)
	height = 1 + FontHeight(win);
    else
	height = 1 + 8;
    
    start = 2;      /* Set Starting Pixel */
    
    /* Step thru the menu structure and adjust it */
    for (cur_menu=first_menu; cur_menu!=NULL;
	 cur_menu=cur_menu->NextMenu)
    {
	cur_menu->LeftEdge = start;
	cur_menu->Width = char_size +
	    TextLength(win->RPort, cur_menu->MenuName,
		       strlen(cur_menu->MenuName));
	cur_menu->Height = height;
	cur_menu->TopEdge = 0;
	adjustItems(win->RPort, cur_menu->FirstItem,
		    char_size, height, 0, 0);
	start += cur_menu->Width + char_size;
    }
}

#ifdef	ASL
#include <libraries/asl.h>
#include <clib/asl_protos.h>

#define	ASL_OPEN	1
#define	ASL_INSERT	2
#define	ASL_WRITE	3

struct Library *AslBase;

static int
alsmenu(dir, mode, buff)
char *dir;
char *buff;
{
    struct FileRequester *fr;
    char *text_OK, *title;
    ULONG asl_flag;

    if (AslBase = OpenLibrary("asl.library", 0L))
    {
	asl_flag = FILF_PATGAD;
	buff[0] = '\0';
	switch (mode)
	{
	  case ASL_OPEN:
	    text_OK = "Open";
	    title   = "Select File to Open";
	    break;
	  case ASL_INSERT:
	    text_OK = "Insert";
	    title   = "Select File to Insert";
	    break;
	  case ASL_WRITE:
	    text_OK = "Save";
	    title   = "Select File to Save to";
	    asl_flag |= FILF_SAVE;
	    break;
	  default:
	    text_OK = "OK";
	    title   = "Select File";
	}
	
	if (dir == NULL)
	    dir = "";
	if (fr = AllocAslRequestTags(ASL_FileRequest,
			ASL_Hail,	(ULONG)title,
			ASL_Dir,	(ULONG)dir,
			ASL_Pattern,	"~(#?'~)",
			ASL_OKText,	(ULONG)text_OK,
			ASL_CancelText,	(ULONG)"Cancel",
			ASL_FuncFlags,	asl_flag,
			TAG_DONE
				     ))
	{
	    if (AslRequest(fr, NULL))
	    {
		strncpy(buff, fr->rf_Dir, NFILEN);
		buff[NFILEN-1] = '\0';
		if (!AddPart(buff, fr->rf_File, NFILEN))
		    buff[0] = '\0';
	    }
	    FreeAslRequest(fr);
	}
	CloseLibrary(AslBase);

	return TRUE;
    }
    return FALSE;
}

int
aslopen(f, c)
{
    char fname[NFILEN];
    
    alsmenu(curbp->b_cwd, ASL_OPEN, fname);
    eargset(fname);
    return filevisit(f, c);
}

int
aslinsert(f, c)
{
    char fname[NFILEN];
    
    alsmenu(curbp->b_cwd, ASL_INSERT, fname);
    eargset(fname);
    return fileinsert(f, c);
}

int
aslwrite(f, c)
{
    char fname[NFILEN];
    
    alsmenu(curbp->b_cwd, ASL_WRITE, fname);
    eargset(fname);
    return filewrite(f, c);
}
#endif	/* ASL */

int
amigamenu(f, c)
int f, c;
{
    MenuMap *em;
    int menuNum,itemNum,subNum;

    menuNum = getkbd() - MN_OFFSET;
    itemNum = getkbd() - MN_OFFSET;
    subNum  = getkbd() - MN_OFFSET;

    if (menuNum == NOITEM)
	return TRUE;
    em = MgMenus;
    switch (MENU_TYPE(em[menuNum].type))
    {
      case MENU_LINE:
	return TRUE;
      case MENU_FUNC:
	return ((int (*)())em[menuNum].func)(f, c);
      case MENU_SUB:
	if (itemNum == NOITEM)
	    return TRUE;
	em = (MenuMap*)(em[menuNum].func);
	switch (MENU_TYPE(em[itemNum].type))
	{
          case MENU_LINE:
	    return TRUE;
	  case MENU_FUNC:
	    return ((int (*)())em[itemNum].func)(f, c);
	  case MENU_SUB:
	    if (subNum == NOITEM)
		return TRUE;
	    em = (MenuMap*)(em[itemNum].func);
	    switch (MENU_TYPE(em[subNum].type))
	    {
	      case MENU_LINE:
		return TRUE;
              case MENU_FUNC:
		return ((int (*)())em[subNum].func)(f, c);
	    }
	}

    }

    ewprintf("BUGS in amiga menu execute");
    return FALSE;
}

#endif	/* DO_MENU */
