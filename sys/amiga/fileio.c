/* $Id: fileio.c,v 1.7 2001/03/02 08:47:04 amura Exp $ */
/*
 * Name:	MG 2a401
 *		Commodore Amiga file I/O.
 * Last edit:	05-May-88 swalton@solar.stanford.edu
 * Next-to-Last edit:	16-Dec-87 mic@emx.utexas.edu
 * Created:	23-Jul-86 mic@emx.utexas.edu
 *
 * Read and write ASCII files. All of the low level file I/O
 * knowledge is here.  Uses AmigaDOS standard I/O and does its
 * own dynamic buffering; this seems to save about 2K worth
 * of space in the executable image.
 */

/*
 * $Log: fileio.c,v $
 * Revision 1.7  2001/03/02 08:47:04  amura
 * fix some bogus bugs
 *
 * Revision 1.6  2001/02/18 19:29:03  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.5  2000/12/28 07:26:50  amura
 * suffix o is not search in filename complition
 *
 * Revision 1.4  2000/12/27 16:55:41  amura
 * change d_makename() params for conservative reason, and bugfix in dires_()
 *
 * Revision 1.3  2000/12/21 16:54:20  amura
 * fix usage of strncat()
 *
 * Revision 1.2  2000/12/14 18:07:52  amura
 * filename length become flexible
 *
 * Revision 1.1.1.1  2000/06/27 01:48:01  amura
 * import to CVS
 *
 * Revision 1.1  1999/05/21  00:58:37  amura
 * Initial revision
 *
 */

#include	"config.h"	/* Dec.15,1992 Add by H.Ohkubo */
#undef	TRUE
#undef	FALSE
#include	"def.h"

#ifdef SUPPORT_ANSI
# include	<string.h>
# include	<stdlib.h>
# include	<exec/types.h>
#endif
#ifdef AMIGA_STDIO
# include	<stdio.h>
#endif
#include	<exec/memory.h>
#include	<libraries/dos.h>
#include	<libraries/dosextens.h>
#ifdef		USE_ARP
# include	"libraries/arpbase.h"
#else
# define FCHARS	32L
#endif

#define	NIOBUF			4096

extern ULONG			Rename(), UnLock(), Close(), FreeMem();
extern LONG			Write(), Read();
extern UBYTE			*AllocMem();
extern struct FileLock		*Lock();
extern struct FileHandle	*Open();
#ifdef	BUGFIX	/* Dec.17,1992 by H.Ohkubo */
extern LONG	Examine();
#endif

#ifdef AMIGA_STDIO
static FILE	*ffp;
#else
static struct FileHandle *ffh = 0;
static UBYTE	*iobuf;
static int	ibufo, niobuf;
static LONG	iostat, access_mode;
#include	"zz_pointer.h"	/* Dec.20,1992 Add by H.Ohkubo */
extern	struct Window *EmW;
#endif

#ifdef	MANX
extern char *strncpy(), *strncat(), *index(), *rindex();
#endif
#ifdef LATTICE
 extern char *malloc() ;
#endif
#ifdef SUPPORT_ANSI
# define index	strchr
# define rindex	strrchr
#endif

#ifdef AMIGA_STDIO
# define GETC(f)    getc(f)
# define PUTC(c,f)  putc(c,f)
# define FERROR(f)  ferror(f)
#else
# define GETC(f)    (ibufo == niobuf) ? FillBuf() : iobuf[ibufo++]
# define PUTC(c,f)  {if (niobuf == NIOBUF) FlushBuf(); iobuf[niobuf++] = c;}
# define FERROR(f)  (iostat < 0L)
#endif

/*
 * Open the Emacs internal file for reading.
 */
ffropen(fn)
char	*fn;
#ifdef AMIGA_STDIO
{
	if ((ffp=fopen(fn, "r")) == NULL)
		return (FIOFNF);
	return (FIOSUC);
}
#else
{
	if ((iobuf = AllocMem((ULONG) NIOBUF, 0L)) == NULL)
		return (FIOERR);

	if ((ffh = Open(fn, access_mode = MODE_OLDFILE)) == 0L) {
		FreeMem(iobuf, (ULONG) NIOBUF);
		return (FIOFNF);
	}
	ZZ_POINTER(EmW);	/* Dec.20,1992 Add by H.Ohkubo */
	ibufo = niobuf = 0;
	return (FIOSUC);
}
#endif

/*
 * Open a file for writing.  Return TRUE if all
 * is well, and FALSE on error (cannot create).
 */

ffwopen(fn)
char	*fn;
#ifdef AMIGA_STDIO
{
	if ((ffp=fopen(fn, "w")) == NULL) {
		ewprintf("Cannot open file for writing");
		return (FIOERR);
	}
	return (FIOSUC);
}
#else
{
	if ((iobuf = AllocMem((ULONG) NIOBUF, 0L)) == NULL)
		return (FIOERR);
	if ((ffh = Open(fn, access_mode = MODE_NEWFILE)) == 0L) {
		ewprintf("Cannot open file for writing");
		FreeMem(iobuf, (ULONG) NIOBUF);
		return (FIOERR);
	}
	niobuf = 0;
	iostat = NIOBUF;    /* pretend we wrote out a full buffer last time */
	ZZ_POINTER(EmW);	/* Dec.20,1992 Add by H.Ohkubo */
	return (FIOSUC);
}
#endif

/*
 * Close a file, flushing the output buffer.  Should look at
 * the status.
 */
ffclose()
#ifdef AMIGA_STDIO
{
	(VOID) fclose(ffp);
	return (FIOSUC);
}
#else
{
	if (access_mode == MODE_NEWFILE)
		FlushBuf();
	if (ffh)
		(void) Close(ffh);
	if (iobuf)
		FreeMem(iobuf, (ULONG) NIOBUF);
	CLEAR_POINTER(EmW);	/* Dec.20,1992 add by H.Ohkubo */
	return (FIOSUC);
}
#endif

/*
 * Write a buffer to the already opened file. bp points to the
 * buffer. Return the status. Check only at the newline and
 * end of buffer.
 */
ffputbuf(bp)
BUFFER *bp;
{
    register char *cp;
    register char *cpend;
    register LINE *lp;
    register LINE *lpend;
#ifdef	KANJI	/* Dec. 15, 1992 by H.Ohkubo */
    register int  kfio;
# ifdef   AMIGA_STDIO
#   define  FFP		(ffp)
# else
    My_FILE ffp;
#   define  FFP		(&ffp)
# endif
#endif	/* KANJI */

    lpend = bp->b_linep;
#ifdef	KANJI	/* Dec. 15, 1992 by H.Ohkubo */
    if (bp->b_kfio == NIL)
	ksetbufcode(bp);		/* Set buffer local KANJI code.	*/
    kfio  = bp->b_kfio;
# ifndef   AMIGA_STDIO
    ffp.niobuf = &niobuf;
    ffp.bufmax = NIOBUF;
    ffp.iobuf = iobuf;
# endif
#endif	/* KANJI */
    lp = lforw(lpend);
    do {
	cp = &ltext(lp)[0];		/* begining of line	*/
	cpend = &cp[llength(lp)];	/* end of line		*/
	while(cp != cpend) {
#ifdef	KANJI	/* Dec. 15, 1992 by H.Ohkubo / 19 May 1999 by amura*/
	    kputc(*cp, FFP, kfio);
#else	/* NOT KANJI */
	    PUTC(*cp, FFP);
#endif	/* KANJI */
	    cp++;
	}
#ifdef	KANJI	/* Dec. 15, 1992 by H.Ohkubo */
	if (kfio == JIS) {
		kfselectcode(FFP, FALSE);
	}
#endif	/* KANJI */
	lp = lforw(lp);
	if(lp == lpend) break;		/* no implied newline on last line */
	PUTC('\n', ffp);
    } while(!FERROR(ffp));
    if(FERROR(ffp)) {
	ewprintf("Write I/O error");
	return FIOERR;
    }
    return FIOSUC;
}

/*
 * Read a line from a file, and store the bytes
 * in the supplied buffer. Stop on end of file or end of
 * line.  When FIOEOF is returned, there is a valid line
 * of data without the normally implied \n.
 */
ffgetline(buf, nbuf, nbytes)
register char	*buf;
register int	nbuf;
register int	*nbytes;
{
	register int	c;
	register int	i;

	i = 0;
	while((c = GETC(ffp))!=EOF && c!='\n') {
		buf[i++] = c;
		if (i >= nbuf) return FIOLONG;
	}
	if (c == EOF  && FERROR(ffp) != FALSE) {
		ewprintf("File read error");
		return FIOERR;
	}
	*nbytes = i;
	return c==EOF ? FIOEOF : FIOSUC;
}

#ifndef	NO_BACKUP
/*
 * Rename the current file into a backup copy,
 * possibly after deleting the original file.
 */
fbackupfile(fname)
char	*fname;
{
	struct FileLock *twiddle, *lock;
	char buffer[NFILEN];

	(void) strncpy(buffer,fname,NFILEN - 1);
	(void) strcat(buffer,"~");

	lock = Lock(fname,(ULONG)EXCLUSIVE_LOCK);/* does file exist?	*/
	if (!lock)
		return (FALSE);			/* nope, return error	*/

	twiddle = Lock(buffer,(ULONG)EXCLUSIVE_LOCK);
	if (twiddle) {				/* delete old backup	*/
		UnLock(twiddle);		/* let it go		*/
		if (!DeleteFile(buffer)) {
			UnLock(lock);
			return (FALSE);
		}
		twiddle = NULL;
	}
	/* rename file to backup name (after unlocking the file)
	 */
	UnLock(lock);
	return (int) Rename(fname,buffer);
}

#ifdef	BUGFIX	/* Dec.17,1992 Modified for AmigaDos by H.Ohkubo */
/*
 * Get file mode of a file fn.
 */
fgetfilemode(fn)
char	*fn;
{
	struct FileLock	*lock;
	struct FileInfoBlock	*fib;
	int	retval = -1;

	if ((lock = Lock(fn, SHARED_LOCK)) != NULL) {
		if ((fib = (struct FileInfoBlock *)
			AllocMem(sizeof(struct FileInfoBlock), MEMF_CHIP)) != NULL) {
			if (Examine(lock, fib) != NULL)
				retval = fib->fib_Protection;
			FreeMem(fib, sizeof(struct FileInfoBlock));
		}
		UnLock(lock);
	}
	return retval;
}

/*
 * Set file mode of a file fn to the specified mode.
 */
VOID
fsetfilemode(fn, mode)
char	*fn;
int	mode;
{
	struct FileLock	*lock;

	if ((lock = Lock(fn, EXCLUSIVE_LOCK)) != NULL) {
		SetProtection(fn, mode);
		UnLock(lock);
	}
}
#endif	/* BUGFIX */
#endif	/* NO_BACKUP */

/* Dec.18,1992 Add by H.Ohkubo */
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
/*
 * Check whether file is read-only of a file fn.
 */
fchkreadonly(fn)
char	*fn;
{
	struct Lock	*lock;
	struct FileInfoBlock	*fib;
	int	retval = FALSE;

	if ((lock = (struct Lock*)Lock(fn, SHARED_LOCK)) != NULL) {
		if ((fib = (struct FileInfoBlock *)
			AllocMem(sizeof(struct FileInfoBlock), MEMF_CHIP)) != NULL) {
			if (Examine(lock, fib) != NULL)
				retval = (fib->fib_Protection & FIBF_DELETE);
			FreeMem(fib, sizeof(struct FileInfoBlock));
		}
		UnLock(lock);
	}
	return retval;
}
#endif	/* READONLY */

#ifndef	NO_STARTUP
/*
 * Return name of user's startup file.  On Amiga, make it
 * .mg in the current directory, then s:.mg
 */

#ifdef	KANJI	/* Dec. 15, 1992 by H.Ohkubo */
static char startname[] = ".ng";
static char altstartname[] = "s:ng-startup";
#else	/* NOT KANJI */
static char startname[] = ".mg";
static char altstartname[] = "s:mg-startup";	/* .mg -> mg-startup by H.Ohkubo */
#endif	/* KANJI */

char *
#ifdef	ADDOPT
startupfile(ngrcfile, suffix)
char* ngrcfile;
#else
startupfile(suffix)
#endif
char *suffix;
{
	struct FileLock *lock;

#ifdef ADDOPT
	if (ngrcfile) {
		if (lock = Lock(ngrcfile,(ULONG)SHARED_LOCK)) {
			UnLock(lock);
			return(ngrcfile);
		}
	}
#endif
	if (lock = Lock(startname,(ULONG)SHARED_LOCK)) {
		UnLock(lock);
		return(startname);
	}
	if (lock = Lock(altstartname,(ULONG)SHARED_LOCK)) { /* alternate */
		UnLock(lock);
		return (altstartname);
	}
	return (NULL);
}
#endif	NO_STARTUP

/*
 * The string "fn" is a file name. Perform any required name adjustments,
 * including conversion to a fully qualified path if NO_DIR isn't defined.
 */

#define MAX_ELEMS	  8		/* Maximum number of elements	*/
extern char MyDirName[];

char *adjustname(fn)
register char	*fn;
{
#ifndef NO_DIR
	static char fnb[MAX_ELEMS*FCHARS + 1];
	struct FileLock *lock;
	long PathName();
	void TackOn();
	char *dup, *p;

	if (!index(fn, ':')) {			/* no device		*/
		strcpy(fnb, MyDirName);
		TackOn(fnb, fn);
		if (!index(fn, '/'))		/* completely bare name */
			return fnb;
	} else
		strcpy(fnb, fn);
	/*
	 * Else fn has some path components in it.  We try to PathName
	 * the whole thing first, but since the file specified by fn
	 * may not exist, we PathName the leading part and TackOn the
	 * trailing part if it doesn't.
	 */
	if (lock = Lock(fnb, SHARED_LOCK)) {
		if (PathName(lock, fnb, (long) MAX_ELEMS) !=0) {
			UnLock(lock);
			return fnb;
		}
		ewprintf("adjustname: PathName() failed!");
		UnLock(lock);
		return fn;
	}
	if (!(p = rindex(fnb, '/')))
		p = index(fnb, ':');
	p++;
	dup = alloca(strlen(p) + 1);
	if (dup == NULL) {
	    ewprintf("adjustname: PathName() failed!");
	    return fn;
	}
	strcpy(dup, p);
	*p = '\0';
	if (lock = Lock(fnb, SHARED_LOCK)) {
		if (PathName(lock, fnb, (long) MAX_ELEMS) != 0) {
			UnLock(lock);
			TackOn(fnb, dup);
			return fnb;
		}
		ewprintf("adjustname: PathName() failed!");
		UnLock(lock);
	}
#endif
	return fn;				/* if all else fails	*/
}

#ifndef	AMIGA_STDIO
/*
 * Functions to read/write into the I/O buffer
 */

int FlushBuf()
{
	if (niobuf > 0) {
		iostat = Write(ffh, iobuf, (ULONG) niobuf);
		niobuf = 0;
	}
}

/*
 * Fill up the input buffer and return the first character in it.
 */
int FillBuf()
{
	if ((iostat = Read(ffh, iobuf, (ULONG) NIOBUF)) <= 0L)
		return (EOF);
	ibufo = 0;
	niobuf = (int) iostat;
	return (int) (iobuf[ibufo++]);
}
#endif /* not AMIGA_STDIO */

#ifndef NO_DIRED

#include "kbd.h"

copy(frname, toname)
char *frname, *toname;
{
#ifdef MANX
	return fexecl("copy", "copy", frname, toname, (char *) 0);
#else
# ifdef LATTICE
	int	error ;
	if (error = forkl("copy", "copy", frname, toname, (char *) 0, (char *) 0, 2))
		return error ;
	return (int) wait(2) ;
# else
	char buf[NFILEN*2];
	sprintf(buf, "copy <>NIL: from %s to %s", frname, toname);
	return system(buf);
# endif	/* LATTICE */
#endif	/* MANX	*/
}

BUFFER *dired_(dirname)
char *dirname;
{
    register BUFFER *bp;
    char line[CMDLINELENGTH];
    BUFFER *findbuffer();
    char *tmpname, *mktemp();
    int i;
    VOID lfree();

    if((dirname = adjustname(dirname)) == NULL) {
	ewprintf("Bad directory name");
	return NULL;
    }
    if(!ffisdir(dirname)) {
	ewprintf("Not a directory: %s", dirname);
	return NULL;
    }
    if((bp = findbuffer(dirname)) == NULL) {
	ewprintf("Could not create buffer");
	return NULL;
    }
    bclear(bp);				/* clear out leftover garbage	*/
    (void) strcpy(line, "list >");
    (void) strncat(line, tmpname = mktemp("ram:mgXXX.XXX"),
		   sizeof(line)-strlen(line)-1);
    (void) strncat(line, " \"", sizeof(line)-strlen(line)-1);
    (void) strncat(line, dirname, sizeof(line)-strlen(line)-1);
    (void) strncat(line, "\"", sizeof(line)-strlen(line)-1);
    Execute(line, 0L, 0L);
    if (ffropen(tmpname) != FIOSUC) {
 	ewprintf("Can't open temporary dir file");
 	return NULL;
    }
    if (ffgetline(line, sizeof(line), &i) != FIOSUC ||
	strncmp(line, "Directory", 9) != 0) {
	ffclose();
	DeleteFile(tmpname);
	ewprintf("No such directory: `%s'", dirname);
    	return NULL;
    }
    line[0] = line[1] = ' ';
    while (ffgetline(&line[2], sizeof(line)-3, &i) == FIOSUC) {
	line[i+2] = '\0';
	(VOID) addline(bp, line);
    }
    ffclose();
    DeleteFile(tmpname);
    bp->b_dotp = lforw(bp->b_linep);		/* go to first line */
    if(bp->b_fname != NULL) free(bp->b_fname);
    if((bp->b_fname=malloc(strlen(dirname)+1)) != NULL)
	(VOID) strcpy(bp->b_fname, dirname);
#ifdef EXTD_DIR
    if(bp->b_cwd != NULL) free(bp->b_cwd);
    bp->b_cwd = NULL;
#endif
    if((bp->b_modes[0] = name_mode("dired")) == NULL) {
	bp->b_modes[0] = &map_table[0];
	ewprintf("Could not find mode dired");
	return NULL;
    }
    bp->b_nmodes = 0;
    return bp;
}

#if defined(LATTICE)||defined(_DCC)
char *
mktemp(pattern)
char *pattern;
{
/* quick hack mktemp for this purpose only */
	register char		*name, *printo ;
	register unsigned short	counter = 0 ;

	if ((name = malloc(strlen(pattern) + 5)) == NULL)
		panic("Manx sucks rocks!") ;
	(VOID) strcpy(name, pattern) ;
	printo = name + strlen(name) ;
	do
		(void) sprintf(printo, "%d", counter += 1) ;
		while (counter > 0 && access(name, 0) == 0) ;
	if (counter == 0) panic("Manx _really_ sucks!") ;
	return name ;
}
#endif

#define LIST_LINE_LENGTH 58			/* Size of line from List */

d_makename(lp, fn, buflen)
register LINE *lp;
register char *fn;
{
    register char *cp;
    int n = 2, len;

    if(llength(lp) < LIST_LINE_LENGTH) return ABORT;
    if(lgetc(lp, 2) == ':') return ABORT;	/* FileNote line	*/
    len = strlen(curbp->b_fname) + llength(lp) - 41;
    if (buflen <= len) return ABORT;
    cp = fn;
    (VOID) strcpy(cp, curbp->b_fname);
    cp += strlen(cp);
    if ((cp[-1] != ':') && (cp[-1] != '/'))	/* append '/' if needed	*/
	*cp++ = '/';
    while (lgetc(lp, n) != ' ') {
	*cp++ = lgetc(lp, n);
	n++;
    }
    *cp = '\0';
    for (n=34; n<llength(lp); n++)
	if (lgetc(lp,n) == ' ')
	    break;
    for (; n<llength(lp); n++)
	if (lgetc(lp,n) != ' ')
	    break;
    return strncmp(&lp->l_text[n], "Dir", 3) == 0;
}

ffisdir(name)
char *name;
{
    struct FileLock *lock;
    struct FileInfoBlock *fib;
    int result;

    if ((lock = Lock(name, ACCESS_READ)) == NULL)
	return FALSE;
    if ((fib = (struct FileInfoBlock *)
#ifdef	BUGFIX	/* Dec.17,1992 by H.Ohkubo */
	   AllocMem((long)sizeof(struct FileInfoBlock),MEMF_CHIP))==NULL) {
#else	/* ORIGINAL Code */
	   AllocMem((long)sizeof(struct FileInfoBlock),MEMF_PUBLIC))==NULL) {
#endif
	UnLock(lock);
	return FALSE;
    }
#ifdef	BUGFIX	/* Dec.17, 1992 by H.Ohkubo */
    result = FALSE;
    if (Examine(lock, fib) != NULL)
#endif
    result = (fib->fib_DirEntryType > 0L) ? TRUE : FALSE;
    FreeMem(fib,(long)sizeof(struct FileInfoBlock));
    UnLock(lock);
    return result;
}

#endif	/* NO_DIRED */

/* Dec.17,1992 Add by H.Ohkubo */
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */

/* 89.11.20	Original code is for X68K (Human68K).
 * 90.04.08	Modified for MS-DOS by S.Yoshida.
 * 90.05.30	Debuged by A.Shirahashi.
 * Dec.17,1992	Modified for AmigaDos by H.Ohkubo
 * Find file names starting with name.
 * Result is stored in *buf, got from malloc();
 * Return the number of files found, or
 * -1 of error occured.
 */

#define	MALLOC_STEP	256

fffiles(name, buf)
char *name, **buf;
{
	char	pathbuf[NFILEN], tmpnam[NFILEN];
	char	*dirpart, *nampart;
	struct	Lock	*lock;
	struct	FileInfoBlock	*fib;
	int	n, len, size, dirpartlen, nampartlen;
	char	*buffer,*BaseName();
	void	TackOn();

	if ((fib = (struct FileInfoBlock *)
	     AllocMem(sizeof(struct FileInfoBlock), MEMF_CHIP)) == NULL) {
		return -1;
	}

	if (!index(name, ':')) {
		strcpy(pathbuf, MyDirName);
		TackOn(pathbuf, (*name) ? name : "dummy");
	}
	else {
		strcpy(pathbuf, name);
	}
	dirpart = BaseName(pathbuf);
	*dirpart = '\0';
	nampart = BaseName(name);
	nampartlen = strlen(nampart);
	dirpartlen = nampart - name;

	if ((lock = (struct Lock*)Lock(pathbuf, SHARED_LOCK)) == NULL) {
		FreeMem(fib, sizeof(struct FileInfoBlock));
		return -1;
	}
	if (Examine(lock, fib) == NULL) {
		UnLock(lock);
		FreeMem(fib, sizeof(struct FileInfoBlock));
		return -1;
	}
	if ((buffer = malloc(MALLOC_STEP)) == NULL) {
		UnLock(lock);
		FreeMem(fib, sizeof(struct FileInfoBlock));
		return -1;
	}
	size = MALLOC_STEP;
	len = 0;
	n = 0;

	while (ExNext(lock, fib)) {
		register int l;

		if (Strncmp(nampart, fib->fib_FileName, nampartlen) != 0)
			continue;
		strncpy(tmpnam, name, dirpartlen);
		strcpy(tmpnam + dirpartlen, fib->fib_FileName);
		if (fib->fib_DirEntryType > 0) {
			strcat(tmpnam, "/");
		}
		l = strlen(tmpnam) + 1;
		if (l > 3 && (stricmp(&tmpnam[l-3],".O") == 0))
			continue;
		if (l + len >= size) {
				/* make room for double null */
			if ((buffer = realloc(buffer, size += MALLOC_STEP)) == NULL) {
				UnLock(lock);
				FreeMem(fib, sizeof(struct FileInfoBlock));
				return -1;
			}
		}
		strcpy(buffer + len, tmpnam);
		len += l;
		n++;
	}

	UnLock(lock);
	FreeMem(fib, sizeof(struct FileInfoBlock));

	*buf = buffer;
	buffer[len] = '\0';
	return(n);
}
#endif	/* NO_FILECOMP */


#ifdef	NEW_COMPLETE	/* Dec.17,1992 Add by H.Ohkubo */
char *
copy_dir_name (d, s)
char	*d;
char	*s;
{
	int	i;
	char *BaseName();

	i = BaseName(s) - s;
	strncpy (d, s, i);
	d[i] = '\0';
	return (d);
}
#endif	/* NEW_COMPLETE */

#ifndef USE_ARP

/*
 * Here are work-alikes for the few ARP commands now used by the
 * Amiga version of mg.  These may go away if we require ARP in future.
 */
#define	LOWER(c)	(ISUPPER(c) ? TOLOWER(c) : (c))	/* By H.Ohkubo */

Strcmp(s1, s2)
register char *s1, *s2;
{
	while (LOWER(*s1) == LOWER(*s2)) {	/* tolower -> LOWER */
		if (*s1 == '\0')		/* Modified by H.Ohkubo */
			return 0;
		s1++; s2++;
	}
	return (LOWER(*s1) < LOWER(*s2) ? -1 : 1);
}

Strncmp(s1, s2, n)	/* Dec.20,1992 By H.Ohkubo */
register char	*s1, *s2;
register int	n;
{
	int	c1, c2;

	while (n > 0 && *s1 != '\0' && *s2 != '\0') {
		c1 = LOWER(*s1);
		c2 = LOWER(*s2);
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		s1++; s2++; n--;
	}
	if (n == 0)
		return 0;
	else
		return c1 < c2 ? -1 : 1;
}

/*
 * This PathName function shamelessly stolen from the Matt Dillon Shell.
 * It is a slight mod of that program's get_pwd routine, from comm1.c.
 */
long
PathName(flock, pwdstr, nentries)
struct Lock *flock;
long nentries;
char *pwdstr;
{
   char *ptr;
   char *name;
   int err=0;

   struct FileLock *lock, *newlock, *ParentDir(), *DupLock();
   long Examine();
   struct FileInfoBlock *fib;
   int i, len, n;

   lock = DupLock(flock);
   n = nentries * FCHARS + 1;
         
   fib = (struct FileInfoBlock *)AllocMem((long)sizeof(struct FileInfoBlock),
#ifdef	BUGFIX	/* Dec. 17, 1992 by H.Ohkubo */
					MEMF_CHIP);
#else	/* ORIGINAL Code */
   					  MEMF_PUBLIC);
#endif
   pwdstr[i = n-1] = '\0';

   while (lock) {
      newlock = ParentDir(lock);
      if (!Examine(lock, fib)) ++err;
      name = fib->fib_FileName;
      if (*name == '\0')	    /* HACK TO FIX RAM: DISK BUG */
	 name = "RAM";
      len = strlen(name);
      if (newlock) {
	 if (i == n-1) {
	    i -= len;
	    bcopy(name, pwdstr + i, len);
	 } else {
	    i -= len + 1;
	    bcopy(name, pwdstr + i, len);
	    pwdstr[i+len] = '/';
	 }
      } else {
	 i -= len + 1;
	 bcopy(name, pwdstr + i, len);
	 pwdstr[i+len] = ':';
      }
      UnLock(lock);
      lock = newlock;
   }
   FreeMem(fib, (long)sizeof(struct FileInfoBlock));
   bcopy(pwdstr + i, pwdstr, n - i);
   if (err) return(0L);
   return((long) n - i - 1);
}

void TackOn(path, file)
char *path, *file;
{
	if (*file != '\0') {
		if (path[strlen(path)-1] != ':')
			strcat(path, "/");
		strcat(path, file);
	}
}

#ifdef	NEW_COMPLETE	/* Dec.20,1992 by H.Ohkubo */
char *
BaseName(pathname)
char	*pathname;
{
	char	*cp, *dirpart;

	dirpart = NULL;
	for (cp = pathname; *cp; cp++) {
		if (*cp == '/' || (dirpart == NULL && *cp == ':')) {
			dirpart = cp;
		}
	}
	if (dirpart)
		++dirpart;
	else
		dirpart = pathname;

	return dirpart;
}
#endif	/* NEW_COMPLETE */
#endif	/* USE_ARP */

#ifdef	AUTOSAVE
VOID
autosave_name(buff, name, buflen)
char* buff;
char* name;
{
    strcpy(buff, name);
    if (strlen(name)) {
	char *fn = rindex(name, '/');
	if (fn == NULL)
	    fn = rindex(name, ':');
	if (fn == NULL){
	    fn = buff;
	} else {
	    fn++;
	}
	strcpy(&buff[strlen(buff)-strlen(fn)], "#");
	strcat(buff, fn);
	strcat(buff, "#");
    }
}
#endif	/* AUTOSAVE */
