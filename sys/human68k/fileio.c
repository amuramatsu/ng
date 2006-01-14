/*
 *		Human68k file I/O
 */
/* 90.11.09	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/*		File I/O for MS-DOS */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include <doslib.h>

#define	A_RDONLY	0x01
#define	A_HIDEN		0x02
#define	A_SYSTEM	0x04
#define	A_VOLNAME	0x08
#define	A_DIR		0x10
#define	A_NORMAL	0x20

static FILE *ffp;
char *adjustname _PRO((char *));
char *tounixfn _PRO((char *));
char *toh68kfn _PRO((char *));

/*
 * Open a file for reading.
 */
int
ffropen(fn)
char *fn;
{
    char fns[NFILEN];
    
    strcpy(fns, fn);
#ifdef KANJI
    bufetos(fns, strlen(fns)+1);
#endif
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
ffwopen(fn)
char *fn;
{
    char fns[NFILEN];
    
    strcpy(fns, fn);
#ifdef KANJI
    bufetos(fns, strlen(fns)+1);
#endif KANJI
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
 * opened file. Return the status.
 * Check only at the newline and end of buffer.
 */
int
ffputline(buf, len)
register const char *buf;
register int len;
{
    while (len--) {
	putc(*buf, ffp);
	buf++;	/* putc may evalualte arguments more than once */
    }
    putc('\n', ffp);
    if (ferror(ffp))
	return FIOERR;
    return FIOSUC;
}

/*
 * Read a line from a file, and store the bytes
 * in the supplied buffer. Stop on end of file or end of
 * line.  When FIOEOF is returned, there is a valid line
 * of data without the normally implied \n.
 */
int
ffgetline(buf, nbuf, nbytes)
register char *buf;
register int nbuf;
register int *nbytes;
{
    register int c;
    register int i;
    
    i = 0;
    while ((c = getc(ffp))!=EOF && c!='\n') {
	buf[i++] = c;
	if (i >= nbuf)
	    return FIOLONG;
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
fbackupfile(fn)
char *fn;
{
#ifndef EMACS_BACKUP_STYLE    /* 91.01.29    Sawayanagi Yosirou */
    /* 90.07.26  by N.Kamei */
    char oname[NFILEN];
    char *nname;
    VOID strmfe _PRO((char *, char *, char *));
    
    strcpy(oname, fn);
    toh68kfn(oname);
#ifdef KANJI
    bufetos(oname, strlen(oname)+1);
#endif	
    if ((nname=alloca((unsigned)(strlen(fn)+4+1))) == NULL) {
	ewprintf("Can't get %d bytes", strlen(fn) + 5);
	return (ABORT);
    }
    strmfe(nname, oname, "bak");
    unlink(nname);		   /* Ignore errors.       */
    if (rename(oname, nname) < 0)
	return (FALSE);
    return (TRUE);
#else
    char oname[NFILEN];
    register char *nname;
    
    strcpy(oname,fn);
    toh68kfn(oname);
#ifdef KANJI
    bufetos(oname, strlen(oname)+1);
#endif /* KANJI */
    if ((nname=alloca((unsigned)(strlen(oname)+1+1))) == NULL) {
	ewprintf("Can't get %d bytes", strlen(oname) + 1);
	return (ABORT);
    }
    strcpy(nname, oname);
    strcat(nname, "~");
    unlink(nname);			/* Ignore errors.	*/
    if (rename(oname, nname) < 0)
	return (FALSE);
    return (TRUE);
#endif
}

#ifndef	_STAT_H_
#include <stat.h>
#define	_STAT_H_
#endif	/* _STAT_H_ */
/*
 * Get file mode of a file fn.
 */
int
fgetfilemode(fn)
char *fn;
{
    char fns[NFILEN];
#ifdef BUG_FIXED_CLIB
    struct stat filestat;
#else
    struct FILBUF fi;
#endif
    
    strcpy(fns, fn);
#ifdef KANJI
    bufetos(fns, strlen(fns)+1);
#endif KANJI

#ifdef BUG_FIXED_CLIB
    if (stat(fns, &filestat) == 0)
	return(filestat.st_mode & (S_IREAD | S_IWRITE));
    else
	return(-1);
#else
    if (FILES(&fi, fns, A_NORMAL | A_DIR) < 0)
	return(-1);
    return ((fi.atr & A_RDONLY) ? S_IREAD : (S_IREAD | S_IWRITE));
#endif
}

/*
 * Set file mode of a file fn to the specified mode.
 */
VOID
fsetfilemode(fn, mode)
char *fn;
int mode;
{
    char fns[NFILEN];
    
    strcpy(fns, fn);
#ifdef KANJI
    bufetos(fns, strlen(fns)+1);
#endif KANJI
    chmod(fns, mode);
}
#endif

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
#ifndef	_STAT_H_
#include <stat.h>
#define	_STAT_H_
#endif	/* _STAT_H_ */
/*
 * Check whether file is read-only of a file fn.
 */
int
fchkreadonly(fn)
char *fn;
{
#ifdef BUG_FIXED_CLIB
    struct stat filestat;
#else
    struct FILBUF fi;
#endif
    char fns[NFILEN];

    strcpy(fns, fn);
#ifdef KANJI
    bufetos(fns, strlen(fns)+1);
#endif KANJI

#ifdef BUG_FIXED_CLIB
    if (stat(fn, &filestat) == 0)
	return(!(filestat.st_mode & S_IWRITE));
    else
	return FALSE;
#else
    if (FILES(&fi, fn, A_NORMAL | A_DIR) < 0)
	return FALSE;
    return (fi.atr & A_RDONLY);
#endif
}
#endif	/* READONLY */

/*
 * getcwd() in XC version 1.0 doesn't work.
 *	I suggest not to rely on version 2.0, to avoid confusion.
 *
 * 	getcwd() here is copied (and little modified)
 *	from minsh's get_cwd(), original code by K.Maeda.
 *	This works fine with either version of XC.
 *
 *		91.1.15  by K.Maeda
 */

/*
 * Getcwd gets the current drive together with the current
 * directory.  Human68k's path delimiter character '\' is
 * converted to minsh's '/'.
 */
char *getcwd _PRO((char *, int));

char *
getcwd(cwd, len)
char *cwd;
int len;
{
    register int drive;
    register char *s;
    char buf[NFILEN];

    drive = CURDRV();
    buf[0] = drive + 'a';
    buf[1] = ':';
    buf[2] = '/';
    CURDIR(drive+1, &buf[3]);		/* CURDIR() is a doscall of XC */
    if (strlen(buf) > len-1)
	return NULL;
    strcpy(cwd, buf);
    return (cwd);
}

/*
 * The string "fn" is a file name.
 * Perform any required appending of directory name or case adjustments.
 * If NO_DIR is not defined, the same file should be refered to even if the
 * working directory changes.
 */
#ifndef NO_DIR
#include <doslib.h>

extern char *wdir;
extern char *startdir;
static char cwd[NFILEN];

/*
 * Initialize anything the directory management routines need
 */
VOID
dirinit()
{
    if (!(wdir = tounixfn(getcwd(cwd, NFILEN - 1))))
	panic("Can't get current directory!");
#ifdef	KANJI
    bufstoe(wdir, strlen(wdir)+1);
#endif
    if (startdir == NULL) {
	int len = strlen(cwd);
	startdir = malloc(len + 2);
	if (startdir == NULL) {
	    ewprintf("Cannot alloc %d bytes", len + 2);
	    return;
	}
	strcpy(startdir, cwd);
    }
}

/*
 * dirend routine
 */
VOID
dirend()
{
    rchdir(startdir);
}

/*
   rchdir() makes some effects to change directory.  It will affect
   system to set the actual current directory to the specified one.

   This routine is extracted from changedir(), which is currently do
   some virtual chdir but previously do the actual one.

   Both bufc and wdir should have enough space to store file path, that
   is, as long as NFILEN.

   *** This function has not been completed ***

   By Tillanosoft, Mar 22, 1999
 */
int
rchdir(newdir)
char *newdir;
{
    char dir[NFILEN];
    int i;

    strcpy(dir, newdir);
#ifdef	KANJI
    bufetos(dir, strlen(dir)+1);
#endif
    i = strlen(dir) - 1;
    if (dir[i] == '\\' || dir[i] == '/')
	dir[i] = '\0';
    if (dir[1] == ':' && dir[0] != wdir[0]) {
	int	drive;
	drive = newdir[0];
	/* 90.07.01  Change from TOUPPER() to TOLOWER() */
	/*                                 by S.Yoshida */
	if (ISUPPER(drive))
	    drive = TOLOWER(drive);
	/* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
	drive = drive - 'a' + 1;
	if (CHGDRV(drive) <= drive) {
	    drive = drive - 'a';
	    return(FALSE);
	}
    }
    if (dir[1] == ':') {
	if (dir[2] == '\0') {
	    dirinit();
	    return 0;
	} else if (chdir(dir+2) == -1)
	    return -1;
    } else if (chdir(dir) == -1)
	return -1;
    dirinit();
    return 0;
}
#endif	/* !NO_DIR */

char *
adjustname(fn)
register char *fn;
{
    register char *cp;
    static char fnb[NFILEN];

    fnb[NFILEN-1] = '\0';
    cp = fnb;
    if ((ISUPPER(fn[0]) || ISLOWER(fn[0])) && fn[1] == ':') {
	*cp++ = *fn++;
	*cp++ = *fn++;
    }
    else if (fn[0] == '/' || fn[0] == '\\') {
	char *sysroot = getenv("SYSROOT");
	if (sysroot != NULL) {
	    strncpy(cp, sysroot, NFILEN-1);
	    while (*cp)
		cp++;
	}
    }
    else if (fn[0] == '~' && (fn[0] == '/' || fn[1] == '\\')) {
	char *home = getenv("HOME");
	if (home != NULL) {
	    strncpy(cp, home, NFILEN-1);
	    while (*cp)
		cp++;
	    fn++;
	}
    }
    else {
	*cp++ = wdir[0];
	*cp++ = wdir[1];
    }
    switch (*fn) {
    case '/':
    case '\\':
	*cp++ = *fn++;
	break;
    default:
#ifndef NO_DIR	/* 91.01.22  NODIR -> NO_DIR. by S.Yoshida */
	if (fnb[0] == wdir[0]) {	/* in current drive */
	    strcpy(fnb, wdir);
	    cp = fnb + strlen(fnb);
	}
	else {			/* change drives to get default directory */
	    int drive;
	    int ndrive;
	    drive = fnb[0];
	    if (ISUPPER(drive))
		drive = TOLOWER(drive);
	    drive = drive - 'a';
	    if (CHGDRV(drive) <= drive
		|| tounixfn(getcwd(fnb, NFILEN - 1)) == NULL) {
		cp = fnb;
		/* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
		*cp++ = drive + 'a';
		*cp++ = ':';
	    }
	    else {
		cp = fnb + strlen(fnb);
	    }
	    drive = wdir[0];	/* Reset to current drive. */
	    /* 90.07.01  Change from TOUPPER() to TOLOWER() */
	    /*                                 by S.Yoshida */
	    if (ISUPPER(drive))
		drive = TOLOWER(drive);
	    /* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
	    drive = drive - 'a';
	    if (CHGDRV(drive) <= drive)
		dirinit();
	}
	break;
#else
	return (fn);				/* punt */
#endif
    }
    if (cp[-1] != '/' && cp[-1] != '\\')
        *cp++ = '/';
    while (*fn) {
    	switch (*fn) {
	case '.':
	    switch (fn[1]) {
	    case '\0':
		fn++;
		continue;
	    case '/':
	    case '\\':
		fn += 2;
		continue;
	    case '.':
		if (fn[2] == '\\' || fn[2] == '/' || fn[2] == '\0') {
		    if (cp[-2] != ':') {
			--cp;
			while (cp[-1] != '\\' && cp[-1] != '/')
			    --cp;
		    }
		    if (fn[2] == '\0')
			fn += 2;
		    else
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
	
	while (*fn) {
	    *cp = *fn;
	    if (*cp == '/' || *cp == '\\') {
		fn++;
	        cp++;
	        break;
	    }
	    fn++;
	    cp++;
	}
    }
    if (cp[-1] == '\\' || cp[-1] == '/') {
	/* 91.01.16  bug fix for case only "a:\". by S.Yoshida */
	if (cp != &fnb[3] || fnb[1] != ':')
	    --cp;
    }
    *cp = '\0';
    return (fnb);
}

#ifndef NO_STARTUP
#include <io.h>
/*
 * Find a startup file for the user and return its name. As a service
 * to other pieces of code that may want to find a startup file (like
 * the terminal driver in particular), accepts a suffix to be appended
 * to the startup file name.
 */
char *
#ifdef	ADDOPT
startupfile(ngrcfile, suffix)
char* ngrcfile;
#else
startupfile(suffix)
#endif
char *suffix;
{
    register char *file;
    static char	home[NFILEN];
    
    if ((file = getenv("NG")) == NULL) {
	if ((file = getenv("HOME")) == NULL)
	    goto notfound;
    }
    if (strlen(file)+7 >= NFILEN - 1)
	goto notfound;
    strcpy(home, file);

#ifdef	ADDOPT
    if (!ngrcfile)
	ngrcfile = getenv("NGRC");
    if (ngrcfile) {
	if (access(ngrcfile, 0) == 0) {
	    strncpy(home, ngrcfile, NFILEN);
	    home[NFILEN-1] = '\0';
#ifdef	KANJI
	    bufstoe(home, strlen(home)+1);
#endif
	    return home;
	}
#if 0
	strcat(home, "/");
	strcat(home, ngrcfile);
	if (access(home, 0) == 0) {
#ifdef	KANJI
	    bufstoe(home, strlen(home)+1);
#endif
	    return home;
	}
	strcpy(home, file);
#endif
    }
#endif
#ifdef	KANJI	/* 90.02.10  by S.Yoshida */
    strcat(home, "/ng.ini");
#else	/* NOT KANJI */
    strcat(home, "/mg.ini");
#endif	/* KANJI */
    if (suffix != NULL) {
	strcat(home, "-");
	strcat(home, suffix);
    }
    if (access(home, 0) == 0) {
#ifdef	KANJI
	bufstoe(home, strlen(home)+1);
#endif
	return home;
    }

notfound:
#ifdef	STARTUPFILE
    strcpy(home, STARTUPFILE);
    if (suffix != NULL) {
	(VOID) strcat(home, "-");
	(VOID) strcat(home, suffix);
    }
    if (access(home, 0) == 0) {
#ifdef	KANJI
	bufstoe(home, strlen(home)+1);
#endif
	return file;
    }
#endif

    return NULL;
}
#endif

#ifndef NO_DIRED
#include <process.h>
#include "kbd.h"

int
copy(frname, toname)
char *frname, *toname;
{
    char cmd[CMDLINELENGTH];
    char *ptr;
    char frnames[NFILEN];
    char tonames[NFILEN];

    strcpy(frnames, frname);
    strcpy(tonames, toname);
#ifdef KANJI
    bufetos(frnames, strlen(frnames)+1);
    bufetos(tonames, strlen(tonames)+1);
#endif

    sprintf(cmd, "copy %s %s > NUL", frnames, tonames);
    toh68kfn(cmd);
    return (system(cmd) == 0);
}

BUFFER *
dired_(dirname)
char *dirname;
{
    register BUFFER *bp;
    BUFFER *findbuffer();
    char **filelist, **getfilelist();
    int  numfiles;
    int  i;

    if ((dirname = adjustname(dirname)) == NULL) {
	ewprintf("Bad directory name");
	return NULL;
    }
    if (dirname[strlen(dirname)-1] != '/'
	&& dirname[strlen(dirname)-1] != '\\')
    	strcat(dirname, "/");
    if ((bp = findbuffer(dirname)) == NULL) {
	ewprintf("Could not create buffer");
	return NULL;
    }
    if (bclear(bp) != TRUE)
	return FALSE;
    if ((filelist = getfilelist(&numfiles, dirname)) == NULL) {
	ewprintf("Could not get directory info");
	return NULL;
    }
    for (i = 0; i < numfiles; i++) {
	addline(bp, filelist[i]);
	free(filelist[i]);
    }
    free(filelist);
    bp->b_dotp = lforw(bp->b_linep);		/* go to first line */
    if (bp->b_fname != NULL)
	free(bp->b_fname);
    if ((bp->b_fname=malloc(strlen(dirname+1))) != NULL)
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
d_makename(lp, fn, buflen)
register LINE *lp;
register char *fn;
{
    register char *cp;
    int len;

    if (llength(lp) <= 41)
	return ABORT;
    len = strlen(curbp->b_fname) + llength(lp) - 41;
    cp = malloc(len + 1);
    if (buflen <= len)
	return ABORT;
    cp = fn;
    strcpy(cp, curbp->b_fname);
    cp += strlen(cp);
    bcopy(&lp->l_text[41], cp, llength(lp) - 41);
    cp[llength(lp) - 41] = '\0';
    return lgetc(lp, 2) == 'd';
}

char **
getfilelist(numfiles, dirname)
int *numfiles;
char *dirname;
{
    char **filelist;
    int maxfiles;
    struct FILBUF fileinfo;
    int	i;
    char filename[NFILEN];
#ifdef KANJI
    char filenames[NFILEN];
#endif
    int filelinecmp _PRO((char *, char *));
    VOID mkfileline _PRO((char *, struct FILBUF *));

    if (strlen(dirname) + 4 > NFILEN)
	return (NULL);
    strcpy(filename, dirname);
    strcat(filename, "*.*");
    
    maxfiles = 50;
    filelist = (char **)calloc(maxfiles, sizeof(char *));
    if (filelist == NULL)
	return (NULL);

#ifdef KANJI
    strcpy(filenames, filename);
    bufetos(filenames, strlen(filenames) +1);
    if (FILES(&fileinfo, filenames, A_NORMAL | A_DIR) >= 0)
#else
    if (FILES(&fileinfo, filename, A_NORMAL | A_DIR) >= 0)
#endif
    {
	filelist[0] = (char *)calloc(80, 1);
	if (filelist[0] == NULL) {
	    free(filelist);
	    return(NULL);
	}
	mkfileline(filelist[0], &fileinfo);
	*numfiles = 1;
	while (NFILES(&fileinfo) >= 0) {
	    if (*numfiles >= maxfiles) {
		filelist = (char **) realloc(filelist,
					     sizeof(char *) * (maxfiles + 20));
		if (filelist == NULL)
		    return (NULL);
		maxfiles += 20;
	    }
	    filelist[*numfiles] = (char *)calloc(80, 1);
	    if (filelist[*numfiles] == NULL) {
		for (i = 0; i < *numfiles; i++)
		    free(filelist[i]);
		free(filelist);
		return(NULL);
	    }
	    mkfileline(filelist[*numfiles], &fileinfo);
	    (*numfiles)++;
	}
	qsort(filelist, *numfiles, sizeof (char *), filelinecmp);
	return(filelist);
    }
    return(NULL);
}

VOID
mkfileline(line, info)
char *line;
struct FILBUF *info;
{
    int y;
    
    line[0] = line[1] = ' ';
    line[2] = (info->atr & A_DIR) ? 'd' : '-';
    line[3] = 'r';
    line[4] = (info->atr & A_RDONLY) ? '-' : 'w';
    line[5] = '-';
    line[6] = (info->atr & A_SYSTEM) ? 's' : '-';
    sprintf(&line[7], "%15ld", info->filelen);
    y = 1980 + ((info->date >> 9) & 0x7f);
    y -= (y > 1999) ? 2000 : 1900;
    sprintf(&line[22], "  %02d-%02d-%02d  %02d:%02d  ",
	    y, (info->date >> 5) & 0x0f, info->date & 0x1f,
	    (info->time >> 11) & 0x1f, (info->time >> 5) & 0x3f);
    strcpy(&line[41], info->name);
#ifdef KANJI
    bufstoe(&line[41], strlen(&line[41])+1);
#endif /* KANJI */
}

int
filelinecmp(x, y)
char **x, **y;
{
    register char *xx, *yy;
    
    for (xx = &(*x)[41], yy = &(*y)[41]; *xx && *xx == *yy; xx++, yy++)
	;
    return(*xx - *yy);			
}
#endif

#ifndef NO_DIRED	/* 91.01.15  by K.Maeda */
#ifndef	_STAT_H_
#include <stat.h>
#define	_STAT_H_
#endif	/* _STAT_H_ */
/*
 * Check whether file "dn" is directory.
 */
int
ffisdir(dn)
char *dn;
{
#ifdef	BUG_FIXED_CLIB		/* 91.01.26 by Masaru Oki */
    struct stat filestat;
#else
    struct FILBUF di;
    char pathbuf[NFILEN], *tmpnam;
    int len;
#endif
    char dns[NFILEN];

    strcpy(dns, dn);
#ifdef KANJI
    bufetos(dns, strlen(dns)+1);
#endif

#ifdef	BUG_FIXED_CLIB		/* 91.01.26 by Masaru Oki */
    if (stat(dns, &filestat) == 0)
	return ((filestat.st_mode & S_IFDIR) == S_IFDIR);
    else
	return FALSE;
#else
    len = strlen(dns);
    if (!(len--))
	return FALSE;
    if (dns[len] == '\\' || dns[len] == '/') {
	if (!len || (len == 2 && dns[1] == ':'))
	    return TRUE;
	bcopy(dns, pathbuf, len);
	tmpnam = pathbuf;
    }
    else {
	tmpnam = dn;
    }
    if (FILES(&di, tmpnam, A_DIR) < 0)
	return FALSE;
    return (di.atr & A_DIR);
#endif
}
#endif /* NO_DIRED */

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */

/* 89.11.20	Original code is for X68K (Human68K).
 * 90.04.08	Modified for MS-DOS by S.Yoshida.
 * 90.05.30	Debuged by A.Shirahashi.
 * Find file names starting with name.
 * Result is stored in *buf, got from malloc();
 * Return the number of files found, or
 * -1 of error occured.
 */

#define	MALLOC_STEP	256
int
fffiles(name, buf)
char *name, **buf;
{
    char pathbuf[NFILEN], tmpnam[NFILEN];
#ifdef  KANJI
    char pathbufs[NFILEN];
#endif
    char *cp, *dirpart, *nampart;
    struct FILBUF    fileinfo;
    int n, len, size, dirpartlen, nampartlen;
    char *buffer;
    char *home;
    int homelen;
    
    if (name[0] == '~' && (name[1]=='/' || name[1]=='\\') &&
	  (home = getenv("HOME"))) {
	homelen = strlen(home);
	strncpy(pathbuf, home, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
	strncat(pathbuf, &name[1], sizeof(pathbuf)-strlen(pathbuf)-1);
	name++;
    }
    else {
	home = NULL;
	homelen = 0;
	strncpy(pathbuf, name, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
    }
    dirpart = NULL;
    for (cp = pathbuf; *cp; cp++) {
        if (*cp == '/' || *cp == '\\' || *cp == ':')
            dirpart = cp;
    }
    if (dirpart) {
        *++dirpart = '\0';
        dirpartlen = dirpart - pathbuf;
    }
    else {
        *pathbuf = '\0';
        dirpartlen = 0;
    }
    nampart = name + dirpartlen - homelen;
    nampartlen = strlen(nampart);

    buffer = malloc(MALLOC_STEP);
    if (buffer == NULL)
        return (-1);
    size = MALLOC_STEP;
    len = 0;
    n = 0;
  
    strncat(pathbuf, "*.*", sizeof(pathbuf)-strlen(pathbuf)-1);
#ifdef KANJI
    strcpy(pathbufs, pathbuf);
    bufetos(pathbufs, strlen(pathbufs)+1);
    if (FILES(&fileinfo, pathbufs, A_NORMAL | A_DIR) < 0)
#else
    if (FILES(&fileinfo, pathbuf, A_NORMAL | A_DIR) < 0)
#endif
    {
	*buf = buffer;
	buffer[0] = '\0';
        return (0);
    }

    do {
        register int l;

        if (strncmp(nampart, fileinfo.name, nampartlen) != 0)
            continue;		/* case-sensitive comparison */
        strncpy(tmpnam, pathbuf, dirpartlen);
        strcpy(tmpnam + dirpartlen, fileinfo.name);
#ifdef	KANJI
	bufstoe(tmpnam + dirpartlen, strlen(tmpnam+dirpartlen)+1);
#endif
        if (fileinfo.atr & A_DIR)
            strcat(tmpnam, "/");
        l = strlen(tmpnam) + 1;
	if (l > 3 && (stricmp(&tmpnam[l - 3], ".o") == 0
		|| stricmp(&tmpnam[l - 3], ".x") == 0
		|| stricmp(&tmpnam[l - 3], ".r") == 0
		|| stricmp(&tmpnam[l - 3], ".z") == 0))
	    continue;
        if (l + len >= size) {
            /* make room for double null */
            if ((buffer = realloc(buffer, size += MALLOC_STEP)) == NULL)
                return(-1);
        }
	if (home) {
	    strcpy(buffer + len, "~");
	    strcat(buffer + len, tmpnam + homelen);
	    len += l - homelen + 1;
	}
	else {
	    strcpy(buffer + len, tmpnam);
	    len += l;
	}
        n++;
    } while (NFILES(&fileinfo) >= 0);

    *buf = buffer;
    buffer[len] = '\0';
    return(n);
}
#endif	/* NO_FILECOMP */

/* This function changes file name into unix style's */
char *
tounixfn(name)
char *name;
{
    register char *p;

    if ((p = name) == NULL)
        return (NULL);
    while(*p != '\0') {
        if (*p == '\\')
            *p = '/';
        else if (*(p + 1) == ':') {
            if (ISUPPER(*p))
                *p = TOLOWER(*p);
        }
        p++;
    }
    return(name);
}

char *
toh68kfn(name)
char *name;
{
    register char *p;

    if ((p = name) == NULL)
        return (NULL);
    while (*p != '\0') {
        if (*p == '/')
            *p = '\\';
        else if (*(p + 1) == ':') {
            if (ISLOWER(*p))
                *p = TOUPPER(*p);
        }
        p++;
    }
    return(name);
}

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
char *
file_name_part (s)
register char *s;
{
    register int i;
    
    for (i = strlen(s); i > 0; i--) {
	if (s[i - 1] == '/' || s[i - 1] == '\\' || s[i - 1] == ':')
	    break;
    }
    return (s + i);
}

char *
copy_dir_name (d, s)
char *d;
char *s;
{
    int i;

    i = file_name_part (s) - s;
    strncpy (d, s, i);
    d[i] = '\0';
    return (d);
}
#endif	/* NEW_COMPLETE */

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
	    fn = rindex(name, '\\');
	if (fn == NULL)
	    fn = rindex(name, ':');
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
