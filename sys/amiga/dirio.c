/* $Id: dirio.c,v 1.7 2002/11/06 16:05:23 amura Exp $ */
/*
 * Name:	MG 2x
 *		Directory I/O routines, by Stephen Walton
 *		Version of 5-May-88
 */

/*
 * $Log: dirio.c,v $
 * Revision 1.7  2002/11/06 16:05:23  amura
 * compile with newstyle source
 *
 * Revision 1.6  2001/11/23 11:56:44  amura
 * Rewrite all sources
 *
 * Revision 1.5  2001/10/29 04:30:43  amura
 * let BUGFIX code enable always
 *
 * Revision 1.4  2001/02/18 19:29:03  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.3  2000/12/22 19:48:17  amura
 * change "sysdef.h" to "def.h" for get NFILEN
 *
 * Revision 1.2  2000/12/14 18:07:51  amura
 * filename length become flexible
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 */

#include "config.h"	/* Dec. 15, 1992 by H.Ohkubo */

#ifndef NO_DIR

#include "def.h"
#include <libraries/dosextens.h>
#include <exec/memory.h>
#ifdef INLINE_PRAGMAS
#include <pragmas/exec_pragmas.h>
#else
#include <clib/exec_protos.h>
#endif

extern struct SysBase *SysBase;

extern char MyDirName[NFILEN];
extern char *wdir, *startdir;
long PathName _PRO((BPTR, char *, long));

char *
getcwd(path, len)
char *path;
int len;
{
    strncpy(path, MyDirName, len);
    path[len-1] = '\0';
    return path;
}

int
chdir(path)
char *path;
{
    BPTR AttemptLock;
    struct FileInfoBlock *fib;
    int retval;
    
    AttemptLock = Lock(path, ACCESS_READ);
    if (!AttemptLock)
	return -1;
    fib = (struct FileInfoBlock *)AllocMem((long)sizeof(struct FileInfoBlock),
					   MEMF_CHIP | MEMF_CLEAR);
    if (fib == NULL) {
	UnLock(AttemptLock);
	return -1;
    }
    Examine(AttemptLock, fib);
    if (fib->fib_DirEntryType < 0) {
	retval = -1;
	UnLock(AttemptLock);
	goto clean;
    }
    UnLock(CurrentDir(AttemptLock));	/* do the thing		*/
    if (PathName(AttemptLock, MyDirName, NFILEN/31L) == 0)
	MyDirName[0] = '\0';
    retval = 0;				/* Success!		*/
clean:
    FreeMem((void *)fib, (long)sizeof(struct FileInfoBlock));
    return retval;
}

/*
 * Initialize anything the directory management routines need
 */
VOID
dirinit()
{
    wdir = MyDirName;
    if (startdir == NULL) {
	int len = strlen(wdir);
	startdir = malloc(len + 2);
	if (startdir == NULL) {
	    ewprintf("Cannot alloc %d bytes", len + 2);
	    return;
	}
	strcpy(startdir, wdir);
    }
}
#endif
