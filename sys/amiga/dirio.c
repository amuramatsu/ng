/* $Id: dirio.c,v 1.1 2000/06/27 01:48:01 amura Exp $ */
/*
 * Name:	MG 2x
 *		Directory I/O routines, by Stephen Walton
 *		Version of 5-May-88
 */

/*
 * $Log: dirio.c,v $
 * Revision 1.1  2000/06/27 01:48:01  amura
 * Initial revision
 *
 */

#include	"config.h"	/* Dec. 15, 1992 by H.Ohkubo */

#ifndef NO_DIR

#include "sysdef.h"
#include <libraries/dosextens.h>
#include <exec/memory.h>

extern	char		MyDirName[MAXPATH], *strncat();

char *getcwd(path, len)
char *path;
{
	strncpy(path,MyDirName,len);
	return path;
}

chdir(path)
char *path;
{
	BPTR Lock(), AttemptLock, CurrentDir();
	long PathName(), len;
	struct FileInfoBlock *fib;
	void *AllocMem();
	int retval;

	AttemptLock = Lock(path, ACCESS_READ);
	if (!AttemptLock)
		return -1;
	fib = (struct FileInfoBlock *) AllocMem((long)
					        sizeof(struct FileInfoBlock),
#ifdef	BUGFIX	/* Dec.18,1992 by H.Ohkubo */
						MEMF_CHIP |
#endif
						MEMF_CLEAR);
#ifdef	BUGFIX	/* Dec.20,1992 by H.Ohkubo */
	if (fib == NULL) {
		UnLock(AttemptLock);
		return -1;
	}
#endif
	Examine(AttemptLock, fib);
	if (fib->fib_DirEntryType < 0) {
		retval = -1;
		UnLock(AttemptLock);
		goto clean;
	}
	UnLock(CurrentDir(AttemptLock));	/* do the thing		*/
	if (PathName(AttemptLock, MyDirName, MAXPATH/31L) == 0)
		MyDirName[0] = '\0';
	retval = 0;				/* Success!		*/
    clean:
	FreeMem((void *) fib, (long) sizeof(struct FileInfoBlock));
	return retval;
}
#endif
