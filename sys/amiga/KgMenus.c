/* $Id: KgMenus.c,v 1.1 2000/06/27 01:48:00 amura Exp $ */
/*********************************************/
/*                                           */
/*       Designer (C) Ian OConnor 1994       */
/*                                           */
/*      Designer Produced C include file     */
/*                                           */
/*********************************************/

/*
 * $Log: KgMenus.c,v $
 * Revision 1.1  2000/06/27 01:48:00  amura
 * Initial revision
 *
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/screens.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <graphics/gfxbase.h>
#include <workbench/workbench.h>
#include <clib/exec_protos.h>
#include <clib/wb_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>
#include <string.h>
#include <clib/diskfont_protos.h>

#include "KgMenus.h"


struct Window *Win0 = NULL;
APTR Win0VisualInfo;
struct Gadget *Win0GList;
struct Gadget *Win0Gadgets[4];

UWORD Win0GadgetTypes[] =
	{
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	};

struct NewGadget Win0NewGadgets[] =
	{
	24, 21, 58, 12, (UBYTE *)"Top", &topaz800, Win0_Gad0, 16, NULL,  NULL,
	84, 21, 58, 12, (UBYTE *)"Up", &topaz800, Win0_Gad1, 16, NULL,  NULL,
	84, 35, 58, 12, (UBYTE *)"Redraw", &topaz800, Win0_Gad2, 16, NULL,  NULL,
	24, 35, 29, 12, (UBYTE *)"|<", &topaz800, Win0_Gad3, 16, NULL,  NULL,
	};
UWORD Win0ZoomInfo[4] = { 200, 0, 200, 25 };

struct TextAttr topaz800 = { (STRPTR)"topaz.font", 8, 0, 0 };

int OpenWindowWin0( void )
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
			for ( loop=0 ; loop<4 ; loop++ )
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
								WA_Width, 243+offx,
								WA_Height, 88+offy,
								WA_Title, "New Window 0",
								WA_MinWidth, 150,
								WA_MinHeight, 25,
								WA_MaxWidth, 1200,
								WA_MaxHeight, 1200,
								WA_DepthGadget, TRUE,
								WA_Activate, TRUE,
								WA_Dummy+0x30, TRUE,
								WA_SmartRefresh, TRUE,
								WA_AutoAdjust, TRUE,
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

void CloseWindowWin0( void )
{
if (Win0 != NULL)
	{
	CloseWindow( Win0);
	Win0 = NULL;
	FreeVisualInfo( Win0VisualInfo);
	FreeGadgets( Win0GList);
	}
}

