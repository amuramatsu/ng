/* $Id: clipboard.c,v 1.2 2000/09/29 17:22:16 amura Exp $ */
/*   
 * clipboard.c
 *   Amiga clipboard support routine.
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
/* This source Based on:
 *
 * Clipdemo.c
 *   Demonstrate use of clipboard I/O.  Uses general functions
 *   provided in cbio.c
 *
 * Cbio.c
 *   Provide standard clipboard device interface routines
 *   such as Open, Close, Post, Read, Write, etc.
 *
 * included in "AMIGA DEVELOPER CD 1.2"
 */

/*
 * $Log: clipboard.c,v $
 * Revision 1.2  2000/09/29 17:22:16  amura
 * change license to BSD style
 *
 * Revision 1.2  2000/09/29 17:14:40  amura
 * change license to BSD style
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 * Revision 1.1  2000/02/09  18:16:06  amura
 * Initial revision
 *
 */

#include	"config.h"	/* Dec.19,1992 Add by H.Ohkubo */
#include	"def.h"

#ifdef CLIPBOARD

#include <exec/types.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <devices/clipboard.h>
#include <libraries/dosextens.h>
#include <libraries/dos.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>

#include <string.h>

#define MAKE_ID(a,b,c,d)	\
	((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

#define ID_FORM		MAKE_ID('F','O','R','M')
#define ID_FTXT		MAKE_ID('F','T','X','T')
#define ID_CHRS		MAKE_ID('C','H','R','S')


static struct IOClipReq *CBOpen pro((ULONG));
static VOID CBClose pro((struct IOClipReq *));
static int  CBWriteFTXT pro((struct IOClipReq *, char *, long));
static int  WriteLong pro((struct IOClipReq *, long*));
static int  CBQueryFTXT pro((struct IOClipReq *));
static int  CBReadCHRS pro((struct IOClipReq *, char *));
static int  ReadLong pro((struct IOClipReq *, ULONG *));
static int  FillCBData pro((struct IOClipReq *, char *, ULONG));
static VOID CBReadDone pro((struct IOClipReq *));


struct IOClipReq *CBOpen(unit)
ULONG unit;
{
    struct MsgPort *mp;
    struct IOStdReq *ior;

    if (mp = CreatePort(0L,0L))
    {
	if (ior=(struct IOStdReq *)CreateExtIO(mp,sizeof(struct IOClipReq)))
        {
	    if (!(OpenDevice("clipboard.device",unit,
			     (struct IORequest *)ior,0L)))
                return((struct IOClipReq *)ior);
            DeleteExtIO((struct IORequest *)ior);
        }
        DeletePort(mp);
    }
    return(NULL);
}

void CBClose(ior)
struct IOClipReq *ior;
{
    struct MsgPort *mp;

    mp = ior->io_Message.mn_ReplyPort;

    CloseDevice((struct IORequest *)ior);
    DeleteExtIO((struct IORequest *)ior);
    DeletePort(mp);

}


int CBWriteFTXT(ior, string, slen)
struct IOClipReq *ior;
char *string;
long slen;
{
    BOOL odd;
    int success;
    long length;

    odd = (slen & 1);               /* pad byte flag */

    length = (odd) ? slen+1 : slen;

    /* initial set-up for Offset, Error, and ClipID */

    ior->io_Offset = 0;
    ior->io_Error  = 0;
    ior->io_ClipID = 0;


    /* Create the IFF header information */

    WriteLong(ior, (long *) "FORM");     /* "FORM"             */
    length+=12L;                         /* + "[size]FTXTCHRS" */
    WriteLong(ior, &length);             /* total length       */
    WriteLong(ior, (long *) "FTXT");     /* "FTXT"             */
    WriteLong(ior, (long *) "CHRS");     /* "CHRS"             */
    WriteLong(ior, &slen);               /* string length      */

    /* Write string */
    ior->io_Data    = (STRPTR)string;
    ior->io_Length  = slen;
    ior->io_Command = CMD_WRITE;
    DoIO( (struct IORequest *) ior);

    /* Pad if needed */
    if (odd)
    {
        ior->io_Data   = (STRPTR)"";
        ior->io_Length = 1L;
        DoIO( (struct IORequest *) ior);
    }

    /* Tell the clipboard we are done writing */

    ior->io_Command=CMD_UPDATE;
    DoIO( (struct IORequest *) ior);

    /* Check if io_Error was set by any of the preceding IO requests */
    success = ior->io_Error ? FALSE : TRUE;

    return(success);
}


int WriteLong(ior, ldata)
struct IOClipReq *ior;
long *ldata;
{
    ior->io_Data    = (STRPTR)ldata;
    ior->io_Length  = 4L;
    ior->io_Command = CMD_WRITE;
    DoIO( (struct IORequest *) ior);

    if (ior->io_Actual == 4)
	return( ior->io_Error ? FALSE : TRUE);

    return(FALSE);
}


int CBQueryFTXT(ior)
struct IOClipReq *ior;
{
    ULONG cbuff[4];

    /* initial set-up for Offset, Error, and ClipID */

    ior->io_Offset = 0;
    ior->io_Error  = 0;
    ior->io_ClipID = 0;

    /* Look for "FORM[size]FTXT" */

    ior->io_Command = CMD_READ;
    ior->io_Data    = (STRPTR)cbuff;
    ior->io_Length  = 12;

    DoIO( (struct IORequest *) ior);


    /* Check to see if we have at least 16 bytes */

    if (ior->io_Actual == 12L)
    {
        /* Check to see if it starts with "FORM" */
        if (cbuff[0] == ID_FORM)
            {
            /* Check to see if its "FTXT" */
            if (cbuff[2] == ID_FTXT)
                return TRUE;
            }

        /* It's not "FORM[size]FTXT", so tell clipboard we are done */
        }

    CBReadDone(ior);

    return FALSE;
}


int CBReadCHRS(ior, buffer)
struct IOClipReq *ior;
char *buffer;
{
    ULONG chunk,size;
    int looking, res = 0;

    /* Find next CHRS chunk */
    
    looking = TRUE;
    
    while (looking)
    {
	looking = FALSE;
	
	if (ReadLong(ior,&chunk))       /* Is CHRS chunk */
	{
	    if (chunk == ID_CHRS)
	    {
		/* Get size of chunk, and copy data */
		if (ReadLong(ior,&size))
		{
		    if (size)
		    {
			if (buffer)
			    res = FillCBData(ior, buffer, size);
			else
			{
			    res = size;
			    ior->io_Offset += (size&1) ? size+1 : size;
			}
		    }
		    else if (!buffer)
		        looking = TRUE;
                }
            }
            else                        /* If not, skip to next chunk */
            {
                if (ReadLong(ior,&size))
                {
                    looking = TRUE;
                    /* if odd size, add pad byte */
		    ior->io_Offset += (size&1) ? size+1 : size;
                }
            }
        }
    }

    CBReadDone(ior);			/* tell clipboard we are done */

    return res;
}


int ReadLong(ior, ldata)
struct IOClipReq *ior;
ULONG *ldata;
{
    ior->io_Command = CMD_READ;
    ior->io_Data    = (STRPTR)ldata;
    ior->io_Length  = 4L;

    DoIO( (struct IORequest *) ior);

    if (ior->io_Actual == 4)
        return( ior->io_Error ? FALSE : TRUE);

    return(FALSE);
}

int FillCBData(ior,buffer,size)
struct IOClipReq *ior;
char *buffer;
ULONG size;
{
    ULONG length;

    /* if odd size, read 1 more */
    length = (size & 1) ? size+1 : size;

    ior->io_Command = CMD_READ;
    ior->io_Data    = (STRPTR)buffer;
    ior->io_Length  = length;

    if (!DoIO((struct IORequest *)ior) && ior->io_Actual==length)
	return TRUE;
    
    return FALSE;
}

VOID CBReadDone(ior)
struct IOClipReq *ior;
{
    char buffer[256];

    ior->io_Command = CMD_READ;
    ior->io_Data    = (STRPTR)buffer;
    ior->io_Length  = 254;


    /* falls through immediately if io_Actual == 0 */

    while (ior->io_Actual)
    {
        if (DoIO( (struct IORequest *) ior))
            break;
    }
}

/*
 *
 */
size_clipboard_(void)
{
    struct IOClipReq *ior;
    int len, size=0;

    /* Open clipboard.device unit 0 */

    if (ior=CBOpen(0L))
    {
        /* Look for FTXT in clipboard */
        if (CBQueryFTXT(ior))
	{
            /* Obtain a copy of the contents of each CHRS chunk */

	    while (len = CBReadCHRS(ior, NULL))
		size += len;

            /* The next call is not really needed if you are sure */
            /* you read to the end of the clip.                   */

	}
	CBReadDone(ior);
	CBClose(ior);
	return size;
    }
    else
	puts("Error opening clipboard unit 0");

    return 0;
}


/*
 * Read, and output FTXT in the clipboard.
 *
 */
recieve_clipboard_(buf)
char *buf;
{
    struct IOClipReq *ior;
    int len;
    char *p1,*p2;

    /* Open clipboard.device unit 0 */

    if (ior=CBOpen(0L))
    {
        /* Look for FTXT in clipboard */
        if (CBQueryFTXT(ior))
        {
            /* Obtain a copy of the contents of each CHRS chunk */

	    while (len = CBReadCHRS(ior, buf))
	    {
	        for (p1 = p2 = buf; p1<buf+len; *p1++)
		    if (*p1)
			*p2++ = *p1;
		buf = p2;
	    }

            /* The next call is not really needed if you are sure */
            /* you read to the end of the clip.                   */

            CBReadDone(ior);
	}
    CBClose(ior);
    return TRUE;
    }
    else
	puts("Error opening clipboard unit 0");

    return FALSE;
}

/*
 * Write a string to the clipboard
 *
 */
send_clipboard_(string, len)
char *string;
int  len;
{

    struct IOClipReq *ior;

    if (string == NULL)
    {
	panic("No string argument given");
	return FALSE;
    }

    /* Open clipboard.device unit 0 */

    if (ior = CBOpen(0L))
    {
	if (!(CBWriteFTXT(ior, string, len)))
	    panic("Error writing to clipboard: io_Error");
	CBClose(ior);
	return TRUE;
    }
    else
	puts("Error opening clipboard.device");

    return FALSE;
}

#endif /* CLIPBOARD */
