/* $Id: iconify.c,v 2.1 2000/09/29 17:24:09 amura Exp $ */
/*
 * iconify.c
 *   Leo Schwab's iconify() compatible iconify routine.
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
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
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
 * $Log: iconify.c,v $
 * Revision 2.1  2000/09/29 17:24:09  amura
 * rewrite from scratch
 *
 */

#include "config.h"
#ifdef DO_ICONIFY
#include "def.h"

#include <exec/types.h>
#include <exec/memory.h>
#include "iconify.h"

#ifdef	V2

#include <dos/dosextens.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <clib/wb_protos.h>

extern struct Library *WorkbenchBase;

static struct DiskObject icondef = {
    WB_DISKMAGIC,WB_DISKVERSION,
    {NULL, 0, 0, 0/*width*/, 0/*height*/, GFLG_GADGIMAGE|GFLG_GADGHCOMP,
	GACT_IMMEDIATE, GTYP_BOOLGADGET, NULL/*image*/, NULL, NULL,
	0, NULL, 0, NULL},
    0, NULL, NULL, NO_ICON_POSITION, NO_ICON_POSITION,
    NULL, NULL, 0 };

int
iconify(x, y, width, height, etcptr, ptr, type)
UWORD *x, *y, width, height;
APTR ptr;
APTR etcptr;
{
    struct AppIcon *icon;
    struct MsgPort *myport;
    struct AppMessage *msg;
    int done;

    icondef.do_Gadget.Width  = width;
    icondef.do_Gadget.Height = height;
    icondef.do_Gadget.GadgetRender = ptr;
    icondef.do_CurrentX = NO_ICON_POSITION;
    icondef.do_CurrentY = NO_ICON_POSITION;

    if ((myport=(struct MsgPort *)CreateMsgPort()) == NULL)
	return FALSE;
    if (type == ICON_WB)
	icon = AddAppIconA(0, 0, etcptr, myport,
			   NULL, &icondef, NULL);
    else
	icon = AddAppIconA(0, 0, NULL, myport,
			   NULL, &icondef, NULL);

    if (icon == NULL)
    {
	while (msg=(struct AppMessage *)GetMsg(myport))
	    ReplyMsg((struct Message *)msg);
	DeleteMsgPort(myport);
	return FALSE;
    }
    
    done = FALSE;
    while (!done)
    {
	WaitPort(myport);
	while (msg = (struct AppMessage *)GetMsg(myport))
	{
	    if (msg->am_NumArgs == 0L)
		done = TRUE;
	    ReplyMsg((struct Message *)msg);
	}
    }
    
    RemoveAppIcon(icon);
    while (msg=(struct AppMessage *)GetMsg(myport))
	ReplyMsg((struct Message *)msg);
    DeleteMsgPort(myport);

    return TRUE;
}

#else	/* not V2 */

#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <clib/intuition_protos.h>

static struct Gadget gadget = {
    NULL, 0, 0, 0/*width*/, 0/*height*/, GFLG_GADGIMAGE|GFLG_GADGHCOMP,
    GACT_IMMEDIATE, WDRAGGING, NULL/*image*/, NULL, NULL,
    0, NULL, 0, NULL};

static struct NewWindow icondef = {
	0,	0,		/* start position       	*/
	0,	0,		/* width, height		*/
	0,	1,	     	/* detail pen, block pen	*/
	GADGETDOWN, 		/* mouse is used		*/
	BORDERLESS|SMART_REFRESH|NOCAREREFRESH,
				/* window flags			*/
	&gadget,		/* pointer to first user gadget */
	NULL,			/* pointer to user checkmark	*/ 
	NULL,			/* title (filled in later)	*/
	NULL,			/* pointer to screen (none)	*/
	NULL,			/* pointer to superbitmap	*/
	0, 0,			/* minimum size	(small!)	*/
	0, 0,			/* maximum size (set by ttopen)	*/
	WBENCHSCREEN		/* screen in which to open	*/ 
};

int
iconify(x, y, width, height, etcptr, ptr, type)
UWORD *x, *y, width, height;
APTR etcptr;
APTR ptr;
{
    int done;
    unsigned long secs,secs1,micros,micros1;
    struct IntuiMessage *msg;
    struct Window *icon;

    icondef.Width  = width;
    icondef.Height = height;
    icondef.LeftEdge = *x;
    icondef.TopEdge  = *y;

    if (type==ICON_WB || etcptr==NULL)
	icondef.Screen = (struct Screen *)WBENCHSCREEN;
    else
	icondef.Screen = (struct Screen *)etcptr;
    
    gadget.Width  = width;
    gadget.Height = height;
    gadget.GadgetRender = ptr;
	
    if ((icon=OpenWindow(&icondef)) == NULL)
	return FALSE;
	
    micros1 = secs1 = 0;
    done = FALSE;
    while (!done)
    {
	Wait((long)(1L << icon->UserPort->mp_SigBit));
	while (msg = (struct IntuiMessage *)GetMsg(icon->UserPort))
	{
	    if (msg->Class == IDCMP_GADGETDOWN)
	    {
		CurrentTime(&secs, &micros);
		if (DoubleClick(secs1, micros1, secs, micros))
		    done = TRUE;
		secs1   = secs;
		micros1 = micros;
	    }
	    ReplyMsg((struct Message *)msg);
	}
    }
	
    CloseWindow(icon);
    return TRUE;
}
#endif	/* V2 */

#endif	/* DO_ICONIFY */
