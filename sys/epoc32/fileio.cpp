/* $Id: fileio.cpp,v 1.5 2003/02/22 08:09:47 amura Exp $ */
/*
 *	Epoc32 file I/O. (Tested only at Psion 5mx)
 *
 *	I make this file from MSDOS & UNIX fileio.c.
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include <ctype.h>	/* 90.05.30  Add by A.Shirahashi */
#include <string.h>	/* 90.07.26  Add by N.Kamei */

static FILE *ffp;
extern "C" void kputc(int, FILE*, int);

/*
 * Open a file for reading.
 */
int
ffropen(char *fn)
{
    char fns[NFILEN];

    strcpy(fns, fn);
    if ((ffp=fopen(fns, "r")) == NULL)
	return (FIOFNF);
    return (FIOSUC);
}

/*
 * Open a file for writing.
 * Return TRUE if all is well, and
 * FALSE on error (cannot create).
 */
int
ffwopen(char *fn)
{
    char fns[NFILEN];
    
    strcpy(fns, fn);
    if ((ffp=fopen(fns, "w")) == NULL) {
	ewprintf("Cannot open file for writing");
	return (FIOERR);
    }
    return (FIOSUC);
}

/*
 * Close a file.
 * Should look at the status.
 */
int
ffclose()
{
    fclose(ffp);
    return (FIOSUC);
}

/*
 * Write a buffer to the already
 * opened file. bp points to the
 * buffer. Return the status.
 * Check only at the newline and
 * end of buffer.
 */
int
ffputbuf(BUFFER *bp)
{
    register char *cp;
    register char *cpend;
    register LINE *lp;
    register LINE *lpend;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    register int  kfio;
#endif	/* KANJI */

    lpend = bp->b_linep;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    if (bp->b_kfio == NIL)
	ksetbufcode(bp);		/* Set buffer local KANJI code.	*/
    kfio  = bp->b_kfio;
#endif	/* KANJI */
    lp = lforw(lpend);
    do {
	cp = &ltext(lp)[0];		/* begining of line	*/
	cpend = &cp[llength(lp)];	/* end of line		*/
	while(cp != cpend) {
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	    kputc(*cp, ffp, kfio);
#else	/* NOT KANJI */
	    putc(*cp, ffp);
#endif	/* KANJI */
	    cp++;	/* putc may evalualte arguments more than once */
	}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	if (kfio == JIS)
	    kfselectcode(ffp, FALSE);
#endif	/* KANJI */
	lp = lforw(lp);
	if (lp == lpend) break;		/* no implied newline on last line */
#ifdef	USE_UNICODE
	if (kfio == UCS2)
	    putc('\0', ffp);
#endif
	putc('\n', ffp);
    } while(!ferror(ffp));
    if (ferror(ffp)) {
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
int
ffgetline(register char *buf, register int nbuf, register int *nbytes)
{
    register int	c;
    register int	i;
    
    i = 0;
    while ((c = getc(ffp))!=EOF && c!='\n') {
	buf[i++] = c;
	if (i >= nbuf) return FIOLONG;
    }
    if (c == EOF  && ferror(ffp) != FALSE) {
	ewprintf("File read error");
	return FIOERR;
    }
    *nbytes = i;
    return c==EOF ? FIOEOF : FIOSUC;
}

#ifndef NO_BACKUP
/*
 * Rename the file "fname" into a backup
 * copy. On Unix the backup has the same name as the
 * original file, with a "~" on the end; this seems to
 * be newest of the new-speak. The error handling is
 * all in "file.c". The "unlink" is perhaps not the
 * right thing here; I don't care that much as
 * I don't enable backups myself.
 */
int
fbackupfile(char *fn)
{
    char nnames[NFILEN];
    strcpy(nnames, fn);
    strcat(nnames, "~");
    unlink(nnames);			/* Ignore errors.	*/
    if (rename(fn, nnames) < 0)
	return (FALSE);
    return (TRUE);
}

#include <sys/types.h>
#include <sys/stat.h>
/*
 * Get file mode of a file fn.
 */
int
fgetfilemode(char *fn)
{
    struct stat	filestat;
    char fns[NFILEN];

    strcpy(fns, fn);

    if (stat(fns, &filestat) == 0)
	return(filestat.st_mode & 0x0fff);
    else
	return(-1);
}

/*
 * Set file mode of a file fn to the specified mode.
 */
extern "C" void fsetfilemode(char *, int);
void
fsetfilemode(char *fn, int mode)
{
    char fns[NFILEN];

    strcpy(fns, fn);
    chmod(fns, mode);
}
#endif

#ifdef	READONLY
#include <sys/types.h>
#include <sys/stat.h>
/*
 * Check whether file is read-only of a file fn.
 */
int
fchkreadonly(char *fn)
{
    struct stat filestat;
    char fns[NFILEN];

    strcpy(fns, fn);
    if (stat(fns, &filestat) == 0)
	return(!(filestat.st_mode & S_IWRITE));
    else
	return FALSE;
}
#endif	/* READONLY */

#ifndef NO_DIR
extern char *wdir;
extern char *startdir;
static char cwd[NFILEN];
extern "C" {
void dirinit(void);
void dirend(void);
int rchdir(char *);
} 

/*
 * Initialize anything the directory management routines need
 */
void
dirinit(void)
{
    /* 90.07.01  Add fftolower() by S.Yoshida */
    if (!(wdir = getcwd(cwd, NFILEN - 1)))
	panic("Can't get current directory!");
    if (wdir[1]==':' && ISUPPER(wdir[0]))
	wdir[0] = TOLOWER(wdir[0]);
    if (startdir == NULL) {
	int len = strlen(cwd);
	startdir = (char*)malloc(len + 1);
	if (startdir == NULL) {
	    ewprintf("Cannot alloc %d bytes", len + 1);
	    return;
	}
	strcpy(startdir, cwd);
    }
}

/*
 * dirend routine
 */
void
dirend(void)
{
}

/*
   rchdir() makes some effects to change directory.  It will affect
   system to set the actual current directory to the specified one.

   This routine is extracted from changedir(), which is currently do
   some virtual chdir but previously do the actual one.

   Both bufc and wdir should have enough space to store file path, that
   is, as long as NFILEN.

   By Tillanosoft, Mar 22, 1999
 */
int
rchdir(char *newdir)
{
    char dir[NFILEN];
    int i;

    strcpy(dir, newdir);
    i = strlen(dir) - 1;
    if (dir[i] == '\\' || dir[i] == '/')
	dir[i] = '\0';
    if (dir[1] == ':' && dir[0] != wdir[0]) {
	int	drive;
	int	ndrive;
	drive = newdir[0];
	if (ISUPPER(drive))
	    drive = TOLOWER(drive);
	drive = drive - 'a' + 1;
	/*XXX*/
    }
    if (dir[1] == ':') {
	if (dir[2] == '\0') {
	    dirinit();
	    return 0;
	} else if (chdir(dir+2) == -1)
	    return -1;
    }
    else if (chdir(dir) == -1)
	return -1;
    dirinit();
    return 0;
}
#endif

/*
 * The string "fn" is a file name.
 * Perform any required appending of directory name or case adjustments.
 * If NO_DIR is not defined, the same file should be refered to even if the
 * working directory changes.
 */
char *
adjustname(register char *fn)
{
    register char *cp;
    static char fnb[NFILEN];

    cp = fnb;
    if (fn[1] == ':') {
	*cp++ = *fn++;
	*cp++ = *fn++;
    }
#ifdef HOMEDIR
    else if (fn[0]=='~' && (fn[1]=='/' || fn[1]=='\\')) {
    	strcpy(fnb, getenv("HOME"));
    	while (*cp) {
	    if (*cp == '/')
		*cp = '\\';
	    cp++;
    	}
    	fn++;
    }
#endif
    switch(*fn) {
    case '/':
	*fn = '\\';
    case '\\':
	/* 91.01.21  Add following if() for bug fix. by S.Yoshida	*/
	/* 		 This fix can consider that when current drive	*/
	/*		 is "a:", "\file" and "a:\file" are same.	*/
	if (cp == fnb) {
	    *cp++ = wdir[0];	/* Current drive name.	*/
	    *cp++ = wdir[1];	/* ':'			*/
	}
	*cp++ = *fn++;
	break;
    default:
#ifndef	NO_DIR
	strcpy(fnb, wdir);
	cp = fnb + strlen(fnb);
	break;
#else
	return fn;				/* punt */
#endif
    }
    if (cp != fnb && cp[-1] != '\\')
	*cp++ = '\\';
    while (*fn) {
    	switch (*fn) {
	case '.':
	    switch (fn[1]) {
	    case '\0':
		*--cp = '\0';
		return fnb;
	    case '/':
	    case '\\':
		fn += 2;
		continue;
	    case '.':
		if (fn[2]=='\\' || fn[2] == '/' || fn[2] == '\0') {
		    --cp;
		    while (cp > fnb && *--cp != '\\') {}
		    ++cp;
		    if (fn[2]=='\0') {
			*--cp = '\0';
			return fnb;
		    }
		    fn += 3;
		    continue;
		}
		break;
	    default:
		break;
	    }
	    break;
	case '/':
	case '\\':
	    fn++;
	    continue;
	default:
	    break;
	}
	while (*fn && (*cp++ = *fn++) != '\\') {
	    if (ISUPPER(cp[-1])) {
		cp[-1] = TOLOWER(cp[-1]);
	    }
	    if (cp[-1] == '/') {
		cp[-1] = '\\';
		break;
	    }
	}
    }
    if (cp[-1]=='\\') {
	/* 91.01.16  bug fix for case only "a:\". by S.Yoshida */
	if (cp != &fnb[3] || fnb[1] != ':')
	    --cp;
    }
    *cp = '\0';
    return fnb;
}

#ifndef NO_STARTUP
/*
 * Find a startup file for the user and return its name. As a service
 * to other pieces of code that may want to find a startup file (like
 * the terminal driver in particular), accepts a suffix to be appended
 * to the startup file name.
 */
#ifdef	ADDOPT
extern "C" char *startupfile(char *ngrcfile, char *suffix);
char *
startupfile(char *ngrcfile, char *suffix)
#else
extern "C" char *startupfile(char *suffix);
char *
startupfile(char *suffix)
#endif
{
    register char *file;
    static char	home[NFILEN];

    if ((file = getenv("NG")) == NULL) {
	if ((file = getenv("HOME")) == NULL) goto notfound;
    }
    if (strlen(file)+7 >= NFILEN - 1) goto notfound;
    strcpy(home, file);
    
#ifdef	ADDOPT
    if (!ngrcfile)
	ngrcfile = getenv("NGRC");
    if (ngrcfile) {
	if (access(ngrcfile, 0) == 0) {
	    strncpy(home, ngrcfile, NFILEN);
	    home[NFILEN-1] = '\0';
	    return home;
	}
#if 0
	strcat(home, "\\");
	strcat(home, ngrcfile);
	if (access(home, 0) == 0)
	     return home;
	strcpy(home, file);
#endif
    }
#endif

#ifdef	KANJI	/* 90.02.10  by S.Yoshida */
    strcat(home, "\\ng.ini");
#else	/* NOT KANJI */
    strcat(home, "\\mg.ini");
#endif	/* KANJI */
    if (suffix != NULL) {
	strcat(home, "-");
	strcat(home, suffix);
    }
    if (access(home, 0) == 0)
	return home;

notfound:
#ifdef	STARTUPFILE
    strcpy(home, STARTUPFILE);
    if (suffix != NULL) {
	strcat(home, "-");
	strcat(home, suffix);
    }
    if (access(home, 0) == 0)
	return home;
#endif
    return NULL;
}
#endif

#ifndef NO_DIRED
int
copy(char *frname, char *toname)
{
    /* IMPLEMENT ME */
    return FALSE;
}

#include "kbd.h"
extern "C" {
BUFFER *dired_(char *dirname);
BUFFER *findbuffer(char *);
}
static char **getfilelist(int *numfiles, char *dirname);

BUFFER *
dired_(char *dirname)
{
    register BUFFER *bp;
    char **files;
    int  numfiles;
    int  i;

    if ((dirname = adjustname(dirname)) == NULL) {
	ewprintf("Bad directory name");
	return NULL;
    }
    if (dirname[strlen(dirname)-1] != '\\')
	strcat(dirname, "\\");
    if ((bp = findbuffer(dirname)) == NULL) {
	ewprintf("Could not create buffer");
	return NULL;
    }
    if (bclear(bp) != TRUE)
	return FALSE;
    if ((files = getfilelist(&numfiles, dirname)) == NULL) {
	ewprintf("Could not get directory info");
	return NULL;
    }
    for (i = 0; i < numfiles; i++) {
	addline(bp, files[i]);
	free(files[i]);
    }
    free(files);
    bp->b_dotp = lforw(bp->b_linep);		/* go to first line */
    if (bp->b_fname != NULL)
	free(bp->b_fname);
    if ((bp->b_fname=(char*)malloc(strlen(dirname)+1)) != NULL)
	strcpy(bp->b_fname, dirname);
#ifdef EXTD_DIR
    if (bp->b_cwd != NULL)
	free(bp->b_cwd);
    bp->b_cwd = NULL;
#endif
    if ((bp->b_modes[0] = name_mode("dired")) == NULL) {
	bp->b_modes[0] = &map_table[0];
	ewprintf("Could not find mode dired");
	return NULL;
    }
    bp->b_nmodes = 0;
    return bp;
}

int
d_makename(register LINE *lp, register char *fn, int buflen)
{
    register char *cp;
    int len;

    if (llength(lp) <= 41) return ABORT;
    len = strlen(curbp->b_fname) + llength(lp) - 41;
    if (buflen <= len) return ABORT;
    cp = fn;
    strcpy(cp, curbp->b_fname);
    cp += strlen(cp);
    bcopy(&lp->l_text[41], cp, llength(lp) - 41);
    int l = llength(lp) - 41; // to avoid GCC (egcs-20000828)
    cp[l] = '\0';
    return lgetc(lp, 2) == 'd';
}

#include <f32file.h>
static void mkfileline(char *line, const TEntry &entry);

static char **
getfilelist(int *numfiles, char *dirname)
{
    char **files;
    RFs fsSession;
    CDir* dirList;
    char fname[NFILEN];

    if (strlen(dirname)+1 > NFILEN-1)
	return NULL;
    strcpy(fname, dirname);
    strcat(fname, "*");

    fsSession.Connect();
    fsSession.GetDir(_L(fname), KEntryAttMaskSupported, ESortByName, dirList);
    fsSession.Close();
    
    *numfiles = 0;
    files = (char**)calloc(dirList->Count(), sizeof(char *));
    
    for (int i=0; i<dirList->Count(); i++) {
	files[i] = (char *)calloc(80, 1);
	if (files[i] == NULL)
	    goto error_end;
	mkfileline(files[i], (*dirList)[i]);
	(*numfiles)++;
    }
    delete dirList;
    return files;

error_end:
    delete dirList;
    for (int i = 0; i < *numfiles; i++) {
	free(files[i]);
    }
    free(files);
    return NULL;
}

static void
mkfileline(char *line, const TEntry &entry)
{
    TBuf<256> tmp;
    line[0] = line[1] = ' ';
    line[2] = entry.IsDir()      ? 'd' : '-';
    line[3] = 'r';
    line[4] = entry.IsReadOnly() ? '-' : 'w';
    line[5] = entry.IsHidden()   ? 'h' : '-';
    line[6] = entry.IsArchive()  ? 'a' : '-';
    sprintf(&line[7], "%15d", entry.iSize);
    entry.iModified.FormatL(tmp, _L("  %Z-%M-%D  %H:%T  "));
    strcpy(&line[22], (const char *)tmp.PtrZ());
    tmp = entry.iName;
    strncpy(&line[41], (const char *)tmp.PtrZ(), 80 - 41);
}

#include <sys/types.h>
#include <sys/stat.h>
/*
 * Check whether file "dn" is directory.
 */
int
ffisdir(char *dn)
{
    struct stat filestat;
    char dns[NFILEN];
    
    strcpy(dns, dn);

    if (stat(dns, &filestat) == 0)
	return ((filestat.st_mode & S_IFMT) == S_IFDIR);
    else
	return FALSE;
}
#endif /* NO_DIRED */

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
#include <f32file.h>

/* 89.11.20	Original code is for X68K (Human68K).
 * 90.04.08	Modified for MS-DOS by S.Yoshida.
 * 90.05.30	Debuged by A.Shirahashi.
 * Find file names starting with name.
 * Result is stored in *buf, got from malloc();
 * Return the number of files found, or
 * -1 of error occured.
 */

#define	MALLOC_STEP	256
extern "C" int fffiles(char *, char **);

int
fffiles(char *name, char **buf)
{
    char pathbuf[NFILEN], tmpnam[NFILEN];
    char *cp, *dirpart, *nampart;
    int n, len, size, dirpartlen, nampartlen;
    char *buffer;
    RFs fsSession;
    CDir* dirList;
#ifdef	HOMEDIR
    char *home;
    int homelen;
	
    if (name[0] == '~' && (name[1]=='/' || name[1]=='\\') &&
	(home = getenv("HOME"))) {
	homelen = strlen(home) - 1;
	strncpy(pathbuf, home, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
	strncat(pathbuf, &name[1], sizeof(pathbuf)-strlen(pathbuf)-1);
    }
    else {
	home = NULL;
	homelen = 0;
	strncpy(pathbuf, name, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
    }
#else
    strncpy(pathbuf, name, sizeof(pathbuf));
    pathbuf[NFILEN-1] = '\0';
#endif
    dirpart = NULL;
    for (cp = pathbuf; *cp; cp++) {
	if (*cp == '/') {
	    *cp = '\\';
	    dirpart = cp;
	} else if (*cp == '\\') {
	    dirpart = cp;
	} else if (dirpart == NULL && *cp == ':') {
	    dirpart = cp;
	}
    }
    if (dirpart) {
	*++dirpart = '\0';
	dirpartlen = dirpart - pathbuf;
    }
    else {
	strcpy(pathbuf, ".\\");	/* 90.05.30  by A.Shirahashi */
	dirpartlen = 0;
    }
#ifdef	HOMEDIR
    nampart = name + dirpartlen - homelen;
#else
    nampart = name + dirpartlen;
#endif
    nampartlen = strlen(nampart);
    for (cp = nampart; *cp; cp++) {		/* _dos_find*() return	*/
	/* 90.05.30  by A.Shirahashi: Use "toupper()". */
	/* 90.06.06  by S.Yoshida: Reused "TOUPPER()".  */
	/*			   And add "ISLOWER()". */
	if (ISLOWER(*cp)) {
	    *cp = TOUPPER(*cp);	/* upper case name.	*/
	}
    }

    buffer = (char*)malloc(MALLOC_STEP);
    if (buffer == NULL)
	return (-1);

    size = MALLOC_STEP;
    len = 0;
    n = 0;
    
    fsSession.Connect();
    fsSession.GetDir(_L(pathbuf), KEntryAttMaskSupported,
		     ESortByName, dirList);
    fsSession.Close();
    
    for (int i=0; i<dirList->Count(); i++) {
	register int l;
	char ff_namee[NFILEN];
	TBuf<256> tmp = (*dirList)[i].iName;
	
	strcpy(ff_namee, (const char *)tmp.PtrZ());
	if (strncmp(nampart, ff_namee, nampartlen) != 0)
	    continue;		/* no-case-sensitive comparison */
	strncpy(tmpnam, pathbuf, dirpartlen);
	
	strcpy(tmpnam + dirpartlen, ff_namee);
	if ((*dirList)[i].IsDir())
	    strcat(tmpnam, "\\");
	l = strlen(tmpnam)+1;
	if (l + len >= size) {
	    /* make room for double null */
	    if ((buffer=(char*)realloc(buffer, size += MALLOC_STEP)) == NULL)
		return(-1);
	}
	/* 90.06.08  by A.Shirahashi: to */
	/* 00.12.28  by amura.. contributed by sahf */
#ifdef HOMEDIR
	if (home) {
	    strcpy(buffer+len, "~");
	    strcat(buffer+len, tmpnam+homelen+1);
	    l -= homelen;
	}
	else
#endif
	strcpy(buffer + len, tmpnam);
	len += l;
	n++;
    }
    
    *buf = buffer;
    buffer[len] = '\0';
    return(n);
}
#endif	/* NO_FILECOMP */

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
extern "C" char *file_name_part(char *);

char *
file_name_part (char *s)
{
    int i;
    for (i = strlen(s) - 1; i > 0; i--) {
	if (*(s+i-1) == '/' || *(s+i-1) == '\\' || *(s+i-1) == ':')
	    break;
    }
    return (s + i);
}

char *
copy_dir_name (char *d, char *s)
{
    int i = file_name_part (s) - s;
    strncpy (d, s, i);
    d[i] = '\0';
    return (d);
}
#endif	/* NEW_COMPLETE */

#ifdef	AUTOSAVE
VOID
autosave_name(char *buff, char *name, int buflen)
{
    strcpy(buff, name);
    if (strlen(name)) {
	char *fn = rindex(name, '/');
	if (fn == NULL)
	    fn = buff;
	else
	    fn++;
	strcpy(&buff[strlen(buff)-strlen(fn)], "#");
	strcat(buff, fn);
	strcat(buff, "#");
    }
}
#endif	/* AUTOSAVE */
