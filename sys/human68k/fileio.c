/* $Id: fileio.c,v 1.9 2001/03/02 08:48:31 amura Exp $ */
/*
 *		Human68k file I/O
 */

/*
 * $Log: fileio.c,v $
 * Revision 1.9  2001/03/02 08:48:31  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.8  2001/02/18 19:29:03  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.7  2001/01/05 13:55:27  amura
 * filename completion fixed
 *
 * Revision 1.6  2000/12/28 07:27:15  amura
 * homedirctory support with filename complition
 *
 * Revision 1.5  2000/12/27 16:55:42  amura
 * change d_makename() params for conservative reason, and bugfix in dires_()
 *
 * Revision 1.4  2000/12/14 18:10:47  amura
 * filename length become flexible
 *
 * Revision 1.3  2000/07/25 15:06:52  amura
 * handle Kanji filename of initfile
 *
 * Revision 1.2  2000/07/18 12:44:03  amura
 * fix to emacs style backup
 *
 * Revision 1.1.1.1  2000/06/27 01:47:57  amura
 * import to CVS
 *
 */
/* 90.11.09	Modified for Ng 1.2.1 Human68k by Sawayanagi Yosirou */
/*		File I/O for MS-DOS */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	<doslib.h>

#define	A_RDONLY	0x01
#define	A_HIDEN		0x02
#define	A_SYSTEM	0x04
#define	A_VOLNAME	0x08
#define	A_DIR		0x10
#define	A_NORMAL	0x20

static	FILE	*ffp;
extern	char	*getenv(), *strncpy();
char	*adjustname();
char	*tounixfn();
char	*toh68kfn();

/*
 * Open a file for reading.
 */
ffropen(fn) char *fn; {
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
ffwopen(fn) char *fn; {
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
ffclose() {
	(VOID) fclose(ffp);
	return (FIOSUC);
}

/*
 * Write a buffer to the already
 * opened file. bp points to the
 * buffer. Return the status.
 * Check only at the newline and
 * end of buffer.
 */
ffputbuf(bp)
BUFFER *bp;
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
	if (kfio == JIS) {
		kfselectcode(ffp, FALSE);
	}
#endif	/* KANJI */
	lp = lforw(lp);
	if(lp == lpend) break;		/* no implied newline on last line */
	putc('\n', ffp);
    } while(!ferror(ffp));
    if(ferror(ffp)) {
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
	while((c = getc(ffp))!=EOF && c!='\n') {
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
fbackupfile(fn) char *fn; {
#ifndef EMACS_BACKUP_STYLE    /* 91.01.29    Sawayanagi Yosirou */
/* 90.07.26  by N.Kamei */
	char    oname[NFILEN];
	char    *nname;
	VOID    strmfe();

	(VOID) strcpy(oname, fn);
	toh68kfn(oname);
#ifdef KANJI
	bufetos(oname, strlen(oname)+1);
#endif	
	if ((nname=alloca((unsigned)(strlen(fn)+4+1))) == NULL) {
		ewprintf("Can't get %d bytes", strlen(fn) + 5);
		return (ABORT);
	}
	strmfe(nname, oname, "bak");
	(VOID) unlink(nname);		   /* Ignore errors.       */
	if (rename(oname, nname) < 0)
		return (FALSE);
	return (TRUE);
#else
	char    oname[NFILEN];
	register char	*nname;

	strcpy(oname,fn);
	toh68kfn(oname);
#ifdef KANJI
	bufetos(oname, strlen(oname)+1);
#endif /* KANJI */
	if ((nname=alloca((unsigned)(strlen(oname)+1+1))) == NULL) {
		ewprintf("Can't get %d bytes", strlen(oname) + 1);
		return (ABORT);
	}
	(VOID) strcpy(nname, oname);
	(VOID) strcat(nname, "~");
	(VOID) unlink(nname);			/* Ignore errors.	*/
	if (rename(oname, nname) < 0)
		return (FALSE);
	return (TRUE);
#endif
}

#ifdef	BUGFIX	/* 90.02.16  by S.Yoshida */
#ifndef	_STAT_H_
#include <stat.h>
#define	_STAT_H_
#endif	/* _STAT_H_ */
/*
 * Get file mode of a file fn.
 */
fgetfilemode(fn)
char	*fn;
{
	char fns[NFILEN];
#ifdef BUG_FIXED_CLIB
	struct	stat	filestat;
#else
	struct FILBUF    fi;
#endif

	strcpy(fns, fn);
#ifdef KANJI
	bufetos(fns, strlen(fns)+1);
#endif KANJI

#ifdef BUG_FIXED_CLIB
	if (stat(fns, &filestat) == 0) {
		return(filestat.st_mode & (S_IREAD | S_IWRITE));
	} else {
		return(-1);
	}
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
char	*fn;
int	mode;
{
	char fns[NFILEN];

	strcpy(fns, fn);
#ifdef KANJI
	bufetos(fns, strlen(fns)+1);
#endif KANJI
	(VOID) chmod(fns, mode);
}
#endif	/* BUGFIX */
#endif

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
#ifndef	_STAT_H_
#include <stat.h>
#define	_STAT_H_
#endif	/* _STAT_H_ */
/*
 * Check whether file is read-only of a file fn.
 */
fchkreadonly(fn)
char	*fn;
{
#ifdef BUG_FIXED_CLIB
	struct	stat	filestat;
#else
	struct FILBUF    fi;
#endif
	char fns[NFILEN];

	strcpy(fns, fn);
#ifdef KANJI
	bufetos(fns, strlen(fns)+1);
#endif KANJI

#ifdef BUG_FIXED_CLIB
	if (stat(fn, &filestat) == 0) {
		return(!(filestat.st_mode & S_IWRITE));
	} else {
		return FALSE;
	}
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

char *getcwd(cwd, len)
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
    if (strlen(buf) > len-1) return NULL;
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

extern char	*wdir;
extern char	*startdir;
static char	cwd[NFILEN];

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

char *adjustname(fn)
register char *fn;
{
    register char *cp;
    static char fnb[NFILEN];
    char    *p;
    char    *endp;

    if (fn[0] == '\0')
    {
        strcpy (fnb, wdir);
        if (fnb[strlen (fnb) - 1] != '/'
	        && fnb[strlen (fnb) - 1] != '\\')
	    strcat (fnb, "/");
	return (fnb);
    }
#ifdef HOMEDIR
    else if (fn[0]=='~' && (fn[1]=='/' || fn[1]=='\\')) {
    	strcpy(fnb, getenv("HOME"));
    	while (*cp) {
	    if (*cp == '\\')
		*cp = '/';
	    cp++;
    	}
    	fn++;
    }
#endif
    endp = fn + strlen (fn) - 1;
    cp = fnb;
    for (p = endp; p > fn; p--)
    {
        if (*p == ':')
	{
	    *cp++ = *(p - 1);
	    *cp++ = *p;
	    fn = p + 1;
	    break;
        }
    }
    if (p == fn)
#ifndef NO_DIR	/* 91.01.22  NODIR -> NO_DIR. by S.Yoshida */
    {
        *cp++ = wdir[0];
	*cp++ = wdir[1];
    }
#else
	    return (fn);    /* punt */
#endif
    for (p = endp; p > fn; p--)
    {
        if ((*p == '/' || *p == '\\')
	        && (*(p - 1) == '/' || *(p - 1) == '\\'))
        {
	    fn = p;
	    break;
	}
    }
    switch (*fn) {
	case '/':
    	case '\\':
	    *cp++ = *fn++;
	    break;
	default:
#ifndef NO_DIR	/* 91.01.22  NODIR -> NO_DIR. by S.Yoshida */
	    if (fnb[0] == wdir[0])    /* in current drive */
            {
                (VOID) strcpy(fnb, wdir);
                cp = fnb + strlen(fnb);
	    }
	    else    /* change drives to get default directory */
	    {
		int	drive;
		int	ndrive;
		char	*getcwd();
		drive = fnb[0];
		if (ISUPPER(drive))
		{
		    drive = TOLOWER(drive);
		}
		drive = drive - 'a';
		if (CHGDRV(drive) <= drive
		        || tounixfn(getcwd(fnb, NFILEN - 1)) == NULL)
		{
		    cp = fnb;
		    /* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
		    *cp++ = drive + 'a';
		    *cp++ = ':';
		} else {
		    cp = fnb + strlen(fnb);
		}
		drive = wdir[0];	/* Reset to current drive. */
		/* 90.07.01  Change from TOUPPER() to TOLOWER() */
		/*                                 by S.Yoshida */
		if (ISUPPER(drive)) {
			drive = TOLOWER(drive);
		}
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
    if(cp[-1] != '/' && cp[-1] != '\\')
        *cp++ = '/';
    while (*fn)
    {
    	switch (*fn)
	{
	    case '.':
		switch (fn[1])
		{
	            case '\0':
		        fn++;
                        continue;
		    case '/':
	    	    case '\\':
	    	    	fn += 2;
		    	continue;
		    case '.':
		    	if (fn[2] == '\\' || fn[2] == '/' || fn[2] == '\0')
			{
			    if (cp[-2] != ':')
			    {
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
	while (*fn)
	{
	    *cp = *fn;
	    if (*cp == '/' || *cp == '\\')
	    {
		fn++;
	        cp++;
	        break;
	    }
	    fn++;
	    cp++;
	}
    }
    if ((cp[-1] == '\\' || cp[-1] == '/') && cp[-2] != ':'
            && (*endp != '\\' && *endp != '/' && *endp != ':'))
        --cp;
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
	register char	*file;
	static char	home[NFILEN];
	char		*getenv();

	if ((file = getenv("NG")) == NULL) {
		if ((file = getenv("HOME")) == NULL) goto notfound;
	}
	if (strlen(file)+7 >= NFILEN - 1) goto notfound;
	(VOID) strcpy(home, file);

#ifdef	ADDOPT
	if (!ngrcfile)
		ngrcfile = getenv("NGRC");
	if (ngrcfile)
	{
		if (access(ngrcfile, 0) == 0) {
			strncpy(home, ngrcfile, NFILEN);
			home[NFILEN-1] = '\0';
#ifdef	KANJI
			bufstoe(home, strlen(home)+1);
#endif
			return home;
		}
	/*
		strcat(home, "\\");
		strcat(home, ngrcfile);
		if (access(home, 0) == 0) {
#ifdef	KANJI
			bufstoe(home, strlen(home)+1);
#endif
			return home;
		
		(VOID)strcpy(home, file);
	*/
	}
#endif
#ifdef	KANJI	/* 90.02.10  by S.Yoshida */
	(VOID) strcat(home, "\\ng.ini");
#else	/* NOT KANJI */
	(VOID) strcat(home, "\\mg.ini");
#endif	/* KANJI */
	if (suffix != NULL) {
		(VOID) strcat(home, "-");
		(VOID) strcat(home, suffix);
	}
	if (access(home, 0) == 0) {
#ifdef	KANJI
		bufstoe(home, strlen(home)+1);
#endif
		return home;
	}

notfound:
#ifdef	STARTUPFILE
	(VOID) strcpy(home, STARTUPFILE);
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

copy(frname, toname)
char *frname, *toname;
{
    char cmd[256];
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
    toh68kfn (cmd);
    return (system(cmd) == 0);
}

BUFFER *dired_(dirname)
char *dirname;
{
    register BUFFER *bp;
    BUFFER *findbuffer();
    char **filelist, **getfilelist();
    int  numfiles;
    int  i;

    if((dirname = adjustname(dirname)) == NULL) {
	ewprintf("Bad directory name");
	return NULL;
    }
    if(dirname[strlen(dirname)-1] != '/' && dirname[strlen(dirname)-1] != '\\')
    	(VOID) strcat(dirname, "/");
    if((bp = findbuffer(dirname)) == NULL) {
	ewprintf("Could not create buffer");
	return NULL;
    }
    if(bclear(bp) != TRUE) return FALSE;
    if ((filelist = getfilelist(&numfiles, dirname)) == NULL) {
	ewprintf("Could not get directory info");
	return NULL;
    }
    for (i = 0; i < numfiles; i++) {
	(VOID) addline(bp, filelist[i]);
	free(filelist[i]);
    }
    free(filelist);
    bp->b_dotp = lforw(bp->b_linep);		/* go to first line */
    if(bp->b_fname != NULL) free(bp->b_fname);
    if((bp->b_fname=malloc(strlen(dirname+1))) != NULL)
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

d_makename(lp, fn, buflen)
register LINE *lp;
register char *fn;
{
    register char *cp;
    int len;

    if(llength(lp) <= 41) return ABORT;
    len = strlen(curbp->b_fname) + llength(lp) - 41;
    cp = malloc(len + 1);
    if (buflen <= len) return ABORT;
    cp = fn;
    strcpy(cp, curbp->b_fname);
    cp += strlen(cp);
    bcopy(&lp->l_text[41], cp, llength(lp) - 41);
    cp[llength(lp) - 41] = '\0';
    return lgetc(lp, 2) == 'd';
}

char **
getfilelist(numfiles, dirname)
int	*numfiles;
char	*dirname;
{
	char	**filelist;
	int	maxfiles;
	struct FILBUF    fileinfo;
	int	i;
	char	filename[NFILEN];
#ifdef KANJI
	char	filenames[NFILEN];
#endif
	int	filelinecmp();
	void	*calloc(), *realloc();
	VOID	mkfileline();

	if (strlen(dirname) + 4 > NFILEN) return(NULL);
	(VOID) strcpy(filename, dirname);
	(VOID) strcat(filename, "*.*");

	maxfiles = 50;
	filelist = (char **)calloc(maxfiles, sizeof(char *));
	if (filelist == NULL) return(NULL);

#ifdef KANJI
	strcpy(filenames, filename);
	bufetos(filenames, strlen(filenames) +1);
	if (FILES(&fileinfo, filenames, A_NORMAL | A_DIR) >= 0)
#else
	if (FILES(&fileinfo, filename, A_NORMAL | A_DIR) >= 0)
#endif
	{
		filelist[0] = (char *)calloc(80, 1);
		if (filelist[0] == NULL)
		{
			free(filelist);
			return(NULL);
		}
		mkfileline(filelist[0], &fileinfo);
		*numfiles = 1;
		while (NFILES(&fileinfo) >= 0)
		{
			if (*numfiles >= maxfiles) {
				filelist = (char **) realloc(filelist,
					sizeof(char *) * (maxfiles + 20));
				if (filelist == NULL) return(NULL);
				maxfiles += 20;
			}
			filelist[*numfiles] = (char *)calloc(80, 1);
			if (filelist[*numfiles] == NULL) {
				for (i = 0; i < *numfiles; i++) {
					free(filelist[i]);
				}
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
    char    *line;
    struct FILBUF    *info;
{
	int	y;

	line[0] = line[1] = ' ';
	line[2] = (info->atr & A_DIR) ? 'd' : '-';
	line[3] = 'r';
	line[4] = (info->atr & A_RDONLY) ? '-' : 'w';
	line[5] = '-';
	line[6] = (info->atr & A_SYSTEM) ? 's' : '-';
	(VOID) sprintf(&line[7], "%15ld", info->filelen);
	y = 1980 + ((info->date >> 9) & 0x7f);
	y -= (y > 1999) ? 2000 : 1900;
	(VOID) sprintf(&line[22], "  %02d-%02d-%02d  %02d:%02d  ",
		y, (info->date >> 5) & 0x0f, info->date & 0x1f,
		(info->time >> 11) & 0x1f, (info->time >> 5) & 0x3f);
	(VOID) strcpy(&line[41], info->name);
#ifdef  KANJI
	(VOID) bufstoe(&line[41], strlen(&line[41])+1);
#endif /* KANJI */
}

filelinecmp(x, y)
char	**x, **y;
{
	register char	*xx, *yy;

	for (xx = &(*x)[41], yy = &(*y)[41]; *xx && *xx == *yy; xx++, yy++) {}
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
ffisdir(dn)
char *dn;
{
#ifdef	BUG_FIXED_CLIB		/* 91.01.26 by Masaru Oki */
	struct	stat	filestat;
#else
	struct FILBUF	di;
	char	pathbuf[NFILEN], *tmpnam;
	int	len;
#endif
	char dns[NFILEN];

	strcpy(dns, dn);
#ifdef KANJI
	bufetos(dns, strlen(dns)+1);
#endif

#ifdef	BUG_FIXED_CLIB		/* 91.01.26 by Masaru Oki */
	if (stat(dns, &filestat) == 0) {
		return ((filestat.st_mode & S_IFDIR) == S_IFDIR);
	} else {
		return FALSE;
	}
#else
	len = strlen(dns);
	if (!(len--)) return FALSE;
	if (dns[len] == '\\' || dns[len] == '/') {
		if (!len || (len == 2 && dns[1] == ':'))
			return TRUE;
		bcopy(dns, pathbuf, len);
		tmpnam = pathbuf;
	} else {
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

fffiles(name, buf)
    char *name, **buf;
{
    char    pathbuf[NFILEN], tmpnam[NFILEN];
#ifdef  KANJI
    char    pathbufs[NFILEN];
#endif
    char    *cp, *dirpart, *nampart;
    struct FILBUF    fileinfo;
    int    n, len, size, dirpartlen, nampartlen;
    char    *buffer;
    void    *malloc(), *realloc();
#ifdef	HOMEDIR
    char	*home;
    int		homelen;
    
    if(name[0] == '~' && (name[1]=='/' || name[1]=='\\') &&
       (home = getenv("HOME"))) {
	homelen = strlen(home) - 1;
	strncpy(pathbuf, home, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
	strncat(pathbuf, &name[1], sizeof(pathbuf)-strlen(pathbuf)-1);
    } else {
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
    for (cp = pathbuf; *cp; cp++)
    {
        if (*cp == '/' || *cp == '\\' || *cp == ':')
        {
            dirpart = cp;
        }
    }
    if (dirpart)
    {
        *++dirpart = '\0';
        dirpartlen = dirpart - pathbuf;
    }
    else
    {
        *pathbuf = '\0';
        dirpartlen = 0;
    }
#ifdef	HOMEDIR
    nampart = name + dirpartlen - homelen + 1;
#else
    nampart = name + dirpartlen;
#endif
    nampartlen = strlen(nampart);

    buffer = malloc(MALLOC_STEP);
    if (buffer == NULL)
        return (-1);
    size = MALLOC_STEP;
    len = 0;
    n = 0;

    (VOID) strcat(pathbuf, "*.*");
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
        if (l + len >= size)
	{
            /* make room for double null */
            if ((buffer = realloc(buffer, size += MALLOC_STEP)) == NULL)
                return(-1);
        }
#ifdef HOMEDIR
	if(home) {
	    strcpy(buffer+len, "~");
	    strcat(buffer+len, tmpnam+homelen+1);
	    l -= homelen;
	} else
#endif
        strcpy(buffer + len, tmpnam);
        len += l;
        n++;
    } while (NFILES(&fileinfo) >= 0);

    *buf = buffer;
    buffer[len] = '\0';
    return(n);
}
#endif	/* NO_FILECOMP */

/* This function changes file name into unix style's */
char	*
tounixfn(name)
char    *name;
{
    register char    *p;

    if ((p = name) == NULL)
        return (NULL);
    while(*p != '\0')
    {
        if (*p == '\\')
        {
            *p = '/';
        }
        else if (*(p + 1) == ':')
        {
            if (ISUPPER(*p))
            {
                *p = TOLOWER(*p);
            }
        }
        p++;
    }
    return(name);
}

char *
toh68kfn (name)
    char    *name;
{
    register char    *p;

    if ((p = name) == NULL)
        return (NULL);
    while(*p != '\0')
    {
        if (*p == '/')
        {
            *p = '\\';
        }
        else if (*(p + 1) == ':')
        {
            if (ISLOWER(*p))
            {
                *p = TOUPPER(*p);
            }
        }
        p++;
    }
    return(name);
}

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
char *
file_name_part (s)
char	*s;
{
	int	i;

	for (i = strlen (s); i > 0; i--) {
		if (s[i - 1] == '/' || s[i - 1] == '\\' || s[i - 1] == ':')
			break;
	}
	return (s + i);
}

char *
copy_dir_name (d, s)
char	*d;
char	*s;
{
	int	i;

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
