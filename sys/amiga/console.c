/* $Id: console.c,v 1.2 2001/10/29 04:30:42 amura Exp $ */
/*
 * These functions are taken directly from the
 * console.device chapter in the Amiga V1.1
 * ROM Kernel Manual.
 */

/*
 * $Log: console.c,v $
 * Revision 1.2  2001/10/29 04:30:42  amura
 * let BUGFIX code enable always
 *
 * Revision 1.1.1.1  2000/06/27 01:48:00  amura
 * import to CVS
 *
 */

#include <exec/types.h>
#include <exec/io.h>
#include <devices/console.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>

#include	"config.h"	/* Dec. 15, 1992 by H.Ohkubo */
#ifdef	KANJI	/* Dec.27,1992 Add by H.Ohkubo */
#define	READBUF	64
static	UBYTE	getbuf[READBUF];
#endif

extern	LONG	OpenDevice();
extern	LONG	DoIO();
extern	LONG	SendIO();

/*
 * Open a console device, given a read request
 * and a write request message.
 */

int OpenConsole(writerequest,readrequest,window)
struct IOStdReq *writerequest;
struct IOStdReq *readrequest;
struct Window *window;
{
	LONG error; 
	writerequest->io_Data = (APTR) window;
	writerequest->io_Length = (ULONG) sizeof(*window);
	error = OpenDevice("console.device", 0L, writerequest, 0L);

	/* clone required parts of the request */
	if (readrequest) {
		readrequest->io_Device = writerequest->io_Device;
		readrequest->io_Unit   = writerequest->io_Unit;
#ifdef	KANJI	/* Dec.19,1992 Add by H.Ohkubo */
		QueueRead(readrequest, getbuf);
#endif
	}
	return((int) error);
}

/*
 * Write out a string of predetermined
 * length to the console
 */
 
int ConWrite(request,string,len)
struct IOStdReq *request;
char *string;
int len;
{
#ifdef	V11
	register int x;
#endif
	request->io_Command = CMD_WRITE;
	request->io_Data = (APTR)string;
	request->io_Length = (LONG)len;
	DoIO(request);
	return(0);
}

/*
 * Queue up a read request 
 * to a console
 */

int QueueRead(request,whereto)
struct IOStdReq *request;
char *whereto;
{
#ifdef	V11
	register int x;
#endif
	request->io_Command = CMD_READ;
	request->io_Data = (APTR)whereto;
#ifdef	KANJI	/* Dec.27,1992 by H.Ohkubo */
	request->io_Length = (LONG)READBUF;
#else	/* ORIGINAL Code */
	request->io_Length = (LONG)1;
#endif	/* KANJI */
	SendIO(request);
	return(0);
}

#ifdef	KANJI	/* Dec.27,1992 Add by H.Ohkubo */
UBYTE	*ConRead(mport, n)
struct	MsgPort	*mport;
int	*n;
{
	static UBYTE	readbuf[READBUF];
	register UBYTE	*p;
	register struct IOStdReq *readreq;

	p = readbuf;
	if (readreq = (struct IOStdReq *)GetMsg(mport)) {
		memcpy(p, getbuf, readreq->io_Actual);
		p += (*n = readreq->io_Actual);
		QueueRead(readreq, getbuf);
	}
	return readbuf;
}
#endif
