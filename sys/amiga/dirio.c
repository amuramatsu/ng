/* $Id: dirio.c,v 1.8 2003/02/22 08:09:47 amura Exp $ */
/*
 * Name:	MG 2x
 *		Directory I/O routines, by Stephen Walton
 *		Version of 5-May-88
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
