/* $Id: fileio.c,v 1.1 2000/06/27 01:47:58 amura Exp $ */
/*
 *		MS-DOS file I/O. (Tested only at MS-DOS 3.1)
 *
 *		I make this file from BSD UNIX fileio.c.
 */

/*
 * $Log: fileio.c,v $
 * Revision 1.1  2000/06/27 01:47:58  amura
 * Initial revision
 *
 */
/* 90.02.11	Modified for Ng 1.0 MS-DOS ver. by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	<dos.h>
#include	<ctype.h>	/* 90.05.30  Add by A.Shirahashi */
#include	<string.h>	/* 90.07.26  Add by N.Kamei */

static	FILE	*ffp;
extern	char	*getenv(), *strncpy();
char	*adjustname();
char	*fftolower();	/* 90.07.01  Add by S.Yoshida */

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
	register char	*nname;
	void		*malloc();	/* 90.03.27  by A.Shirahashi */
	char		*dotp;		/* 90.07.26  Add by N.Kamei */
	VOID		strmfe();	/* 90.07.26  Add by N.Kamei */
	char fns[NFILEN];
	char nnames[NFILEN];

	if ((nname=malloc((unsigned)(strlen(fn)+4+1))) == NULL) {
					/* 90.07.26  1+1 -> 4+1 by N.Kamei */
		ewprintf("Can't get %d bytes", strlen(fn) + 4 +1);
		return (ABORT);
	}
/* 90.07.26  by N.Kamei
   This is not good for MS-DOS.
	(VOID) strcpy(nname, fn);
	(VOID) strcat(nname, "~");
 */
	strmfe(nname, fn, "bak");	/* 90.07.26  by N.Kamei */
	strcpy(nnames, nname);
	strcpy(fns, fn);
#ifdef KANJI
	bufetos(nnames, strlen(nnames)+1);
	bufetos(fns, strlen(fns)+1);
#endif KANJI
	(VOID) unlink(nnames);			/* Ignore errors.	*/
	if (rename(fns, nnames) < 0) {
		free(nname);
		return (FALSE);
	}
	free(nname);
	return (TRUE);
}

/*
 * 90.07.26  Add by N.Kamei.
 * Add or replace file name extention to ext.
 * strmfe() is a standard function of Lattice-C.
 */
VOID
strmfe(newname, oldname, ext)
char	*newname, *oldname, *ext;
{
	char	*tmp_p, *node_p;

	strcpy(newname, oldname);

	/* search nodename top */
	node_p	= newname;
	tmp_p	= strchr(node_p, '\\');
	while(tmp_p) {
		node_p	= &tmp_p[1];
		tmp_p	= strchr(node_p, '\\');
	}
	
	tmp_p	= strrchr(node_p, '.');
	if (tmp_p)
		*tmp_p	= '\0';
	strcat(node_p, ".");
	strcat(node_p, ext);
}

#ifdef	BUGFIX	/* 90.02.16  by S.Yoshida */
#ifndef	_SYS_STAT_H_
#if	!defined(__TURBOC__) || __TURBOC__ >= 0x0200
	/* 90.12.28  For Turbo-C 1.5 by Junn Ohta */
#include <sys/types.h>
#endif	/* __TURBOC__ */
#include <sys/stat.h>
#define	_SYS_STAT_H_
#endif	/* _SYS_STAT_H_ */
/*
 * Get file mode of a file fn.
 */
fgetfilemode(fn)
char	*fn;
{
	struct	stat	filestat;
	char fns[NFILEN];

	strcpy(fns, fn);
#ifdef KANJI
	bufetos(fns, strlen(fns)+1);
#endif KANJI

	if (stat(fns, &filestat) == 0) {
		return(filestat.st_mode & 0x0fff);
	} else {
		return(-1);
	}
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
#ifndef	_SYS_STAT_H_
#if	!defined(__TURBOC__) || __TURBOC__ >= 0x0200
	/* 90.12.28  For Turbo-C 1.5 by Junn Ohta */
#include <sys/types.h>
#endif	/* __TURBOC__ */
#include <sys/stat.h>
#define	_SYS_STAT_H_
#endif	/* _SYS_STAT_H_ */
/*
 * Check whether file is read-only of a file fn.
 */
fchkreadonly(fn)
char	*fn;
{
	struct	stat	filestat;
	char fns[NFILEN];

	strcpy(fns, fn);
#ifdef KANJI
	bufetos(fns, strlen(fns)+1);
#endif KANJI

	if (stat(fns, &filestat) == 0) {
		return(!(filestat.st_mode & S_IWRITE));
	} else {
		return FALSE;
	}
}
#endif	/* READONLY */

/*
 * The string "fn" is a file name.
 * Perform any required appending of directory name or case adjustments.
 * If NO_DIR is not defined, the same file should be refered to even if the
 * working directory changes.
 */
#ifndef NO_DIR
extern char *wdir;
#endif

char *adjustname(fn)
register char *fn;
{
    register char *cp;
    static char fnb[NFILEN];

    cp = fnb;
    if (fn[1] == ':') {
	*cp++ = *fn++;
	*cp++ = *fn++;
    }
#ifdef MSDOS
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
#ifndef	NO_DIR	/* 91.01.17  NODIR -> NO_DIR. by S.Yoshida */
	    if (cp == fnb || fnb[0] == wdir[0]) {
		(VOID) strcpy(fnb, wdir);
		cp = fnb + strlen(fnb);
	    } else {			/* Different drive. */
		int	drive;
		int	ndrive;
		char	*getcwd();
		drive = fnb[0];
		/* 90.07.01  Change from TOUPPER() to TOLOWER() */
		/*                                 by S.Yoshida */
		if (ISUPPER(drive)) {
			drive = TOLOWER(drive);
		}
		/* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
		drive = drive - 'a' + 1;
#ifdef	__TURBOC__	/* 90.03.27  by A.Shirahashi */
		(void) setdisk(drive - 1);
#else	/* NOT __TURBOC__ */
		_dos_setdrive(drive, &ndrive);	/* Need MSC 5.1 */
#endif	/* __TURBOC__ */
		/* 90.07.01  Add fftolower() by S.Yoshida */
		if (!fftolower(getcwd(fnb, NFILEN - 1))) {
		    cp = fnb;
		    /* 90.07.01  Change from 'A' to 'a' by S.Yoshida */
		    *cp++ = drive + 'a' - 1;
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
		drive = drive - 'a' + 1;
#ifdef	__TURBOC__	/* 90.03.27  by A.Shirahashi */
		(void) setdisk(drive - 1);
#else	/* NOT __TURBOC__ */
		_dos_setdrive(drive, &ndrive);	/* Need MSC 5.1 */
#endif	/* __TURBOC__ */
	    }
	    break;
#else
	    return fn;				/* punt */
#endif
    }
    if(cp != fnb && cp[-1] != '\\') *cp++ = '\\';
    while(*fn) {
    	switch(*fn) {
	    case '.':
		switch(fn[1]) {
	            case '\0':
		    	*--cp = '\0';
		    	return fnb;
		    case '/':
	    	    case '\\':
	    	    	fn += 2;
		    	continue;
		    case '.':
		    	if(fn[2]=='\\' || fn[2] == '/' || fn[2] == '\0') {
			    --cp;
			    while(cp > fnb && *--cp != '\\') {}
			    ++cp;
			    if(fn[2]=='\0') {
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
	while(*fn && (*cp++ = *fn++) != '\\') {
	    /* 90.06.05  by S.Yoshida */
	    /* 90.06.08  by A.Shirahashi, convert to lower case */
	    if (ISUPPER(cp[-1])) {
		cp[-1] = TOLOWER(cp[-1]);
	    }
	    if (cp[-1] == '/') {
		cp[-1] = '\\';
		break;
	    }
	}
    }
    if(cp[-1]=='\\') {
	/* 91.01.16  bug fix for case only "a:\". by S.Yoshida */
	if (cp != &fnb[3] || fnb[1] != ':') {
	    --cp;
	}
    }
    *cp = '\0';
    return fnb;
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
		if (access(ngrcfile, 0) == 0) return ngrcfile;
	/*
		strcat(home, "\\");
		strcat(home, ngrcfile);
		if (access(home, 0) == 0) return home;
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
	if (access(home, 0) == 0) return home;

notfound:
#ifdef	STARTUPFILE
	file = STARTUPFILE;
	if (suffix != NULL) {
		(VOID) strcpy(home, file);
		(VOID) strcat(home, "-");
		(VOID) strcat(home, suffix);
		file = home;
	}
	if (access(file, 0) == 0) return file;
#endif

	return NULL;
}
#endif

#ifndef NO_DIRED
#include <process.h>
#ifdef	__TURBOC__	/* 90.03.27  by A.Shirahashi */
#include <dir.h>
#define	find_t ffblk
#define	_dos_findfirst(p,a,f)	findfirst(p,f,a)
#define	_dos_findnext(f)	findnext(f)
#define	_A_NORMAL	0
#define	_A_RDONLY	FA_RDONLY
#define	_A_SUBDIR	FA_DIREC
#define	_A_ARCH		FA_ARCH
#define	attrib		ff_attrib
#define	wr_date		ff_fdate
#define	wr_time		ff_ftime
/*#define size		ff_fsize*/ /* 90.05.30  remove by A.Shirahashi */
/*#define name		ff_name*/ /* 90.05.30  remove by A.Shirahashi */
#else	/* NOT __TURBOC__ */
#include <search.h>
#endif	/* __TURBOC__ */
#include "kbd.h"

copy(frname, toname)
char *frname, *toname;
{
    char cmd[256];
    char frnames[NFILEN];
    char tonames[NFILEN];

    strcpy(frnames, frname);
    strcpy(tonames, toname);
#ifdef KANJI
    bufetos(frnames, strlen(frnames)+1);
    bufetos(tonames, strlen(tonames)+1);
#endif

    sprintf(cmd, "copy %s %s", frnames, tonames);
    return (system(cmd) == 0);
}

BUFFER *dired_(dirname)
char *dirname;
{
    register BUFFER *bp;
    BUFFER *findbuffer();
    char **files, **getfilelist();
    int  numfiles;
    int  i;

    if((dirname = adjustname(dirname)) == NULL) {
	ewprintf("Bad directory name");
	return NULL;
    }
    if(dirname[strlen(dirname)-1] != '\\') (VOID) strcat(dirname, "\\");
    if((bp = findbuffer(dirname)) == NULL) {
	ewprintf("Could not create buffer");
	return NULL;
    }
    if(bclear(bp) != TRUE) return FALSE;
    if ((files = getfilelist(&numfiles, dirname)) == NULL) {
	ewprintf("Could not get directory info");
	return NULL;
    }
    for (i = 0; i < numfiles; i++) {
	(VOID) addline(bp, files[i]);
	free(files[i]);
    }
    free(files);
    bp->b_dotp = lforw(bp->b_linep);		/* go to first line */
    (VOID) strncpy(bp->b_fname, dirname, NFILEN);
#ifdef EXTD_DIR
    bp->b_cwd[0] = '\0';
#endif
    if((bp->b_modes[0] = name_mode("dired")) == NULL) {
	bp->b_modes[0] = &map_table[0];
	ewprintf("Could not find mode dired");
	return NULL;
    }
    bp->b_nmodes = 0;
    return bp;
}

d_makename(lp, fn)
register LINE *lp;
register char **fn;
{
    register char *cp;
    int len;

    if(llength(lp) <= 41) return ABORT;
    len = strlen(curbp->b_fname) + llength(lp) - 41;
    cp = malloc(len + 1);
    if (cp) {
      *fn = cp;
      (VOID)strcpy(cp, curbp->b_fname);
      cp += strlen(cp);
      bcopy(&lp->l_text[41], cp, llength(lp) - 41);
      cp[llength(lp) - 41] = '\0';
      return lgetc(lp, 2) == 'd';
    }
    else {
      return ABORT;
    }
}

char **
getfilelist(numfiles, dirname)
int	*numfiles;
char	*dirname;
{
	char	**files;
	int	maxfiles;
	struct	find_t	fileinfo;
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
	files = (char **)calloc(maxfiles, sizeof(char *));
	if (files == NULL) return(NULL);

#ifdef KANJI
	strcpy(filenames, filename);
	bufetos(filenames, strlen(filenames) +1);
	if (_dos_findfirst(filenames, _A_NORMAL | _A_RDONLY | _A_SUBDIR,
			&fileinfo) == 0) {
#else /* not KANJI */
	if (_dos_findfirst(filename, _A_NORMAL | _A_RDONLY | _A_SUBDIR,
			&fileinfo) == 0) {
#endif /* KANJI */
		files[0] = (char *)calloc(80, 1);
		if (files[0] == NULL) {
			free(files);
			return(NULL);
		}
		mkfileline(files[0], &fileinfo);
		*numfiles = 1;
		while(_dos_findnext(&fileinfo) == 0) {
			if (*numfiles >= maxfiles) {
				files = (char **) realloc(files,
					sizeof(char *) * (maxfiles + 20));
				if (files == NULL) return(NULL);
				maxfiles += 20;
			}
			files[*numfiles] = (char *)calloc(80, 1);
			if (files[*numfiles] == NULL) {
				for (i = 0; i < *numfiles; i++) {
					free(files[i]);
				}
				free(files);
				return(NULL);
			}
			mkfileline(files[*numfiles], &fileinfo);
			(*numfiles)++;
		}
		qsort(files, *numfiles, sizeof (char *), filelinecmp);
		return(files);
	}
	return(NULL);
}

VOID
mkfileline(line, info)
char	*line;
struct	find_t	*info;
{
	int	y;

	line[0] = line[1] = ' ';
	line[2] = (info->attrib & _A_SUBDIR) ? 'd' : '-';
	line[3] = 'r';
	line[4] = (info->attrib & _A_RDONLY) ? '-' : 'w';
	line[5] = '-';
	line[6] = (info->attrib & _A_ARCH) ? 'a' : '-';
#ifdef __TURBOC__	/* 90.05.30  by A.Shirahashi */
	(VOID) sprintf(&line[7], "%15ld", info->ff_fsize);
#else
	(VOID) sprintf(&line[7], "%15ld", info->size);
#endif
	y = 1980 + ((info->wr_date >> 9) & 0x7f);
	y -= (y > 1999) ? 2000 : 1900;
	(VOID) sprintf(&line[22], "  %02d-%02d-%02d  %02d:%02d  ",
		y, (info->wr_date >> 5) & 0x0f, info->wr_date & 0x1f,
		(info->wr_time >> 11) & 0x1f, (info->wr_time >> 5) & 0x3f);
#ifdef __TURBOC__	/* 90.05.30  by A.Shirahashi */
	/* 90.07.01  Add fftolower() by S.Yoshida */
	(VOID) strcpy(&line[41], info->ff_name);
#else
	/* 90.07.01  Add fftolower() by S.Yoshida */
	(VOID) strcpy(&line[41], info->name);
#endif
#ifdef  KANJI
	(VOID) bufstoe(&line[41], strlen(&line[41])+1);
#endif /* KANJI */
	(VOID) fftolower(&line[41]);
}

filelinecmp(x, y)
char	**x, **y;
{
	register unsigned char	*xx, *yy;

	xx = (unsigned char*)&(*x)[41];
	yy = (unsigned char*)&(*y)[41];

	if (*xx != *yy) {		/* for "." ".." directories */
		if (*xx == '.') return -1;
		if (*yy == '.') return 1;
	  	return(*xx - *yy);
	}

	if (*xx) {
		for (xx++, yy++; *xx && *xx == *yy; xx++, yy++) {}
	}
	return(*xx - *yy);
}
#endif

#ifndef NO_DIRED	/* 91.01.15  by K.Maeda */
#ifndef	_SYS_STAT_H_
#if	!defined(__TURBOC__) || __TURBOC__ >= 0x0200
	/* 90.12.28  For Turbo-C 1.5 by Junn Ohta */
#include <sys/types.h>
#endif	/* __TURBOC__ */
#include <sys/stat.h>
#define	_SYS_STAT_H_
#endif	/* _SYS_STAT_H_ */
/*
 * Check whether file "dn" is directory.
 */
ffisdir(dn)
char *dn;
{
	struct	stat	filestat;
	char dns[NFILEN];

	strcpy(dns, dn);
#ifdef KANJI
	bufetos(dns, strlen(dns)+1);
#endif

	if (stat(dns, &filestat) == 0) {
		return ((filestat.st_mode & S_IFMT) == S_IFDIR);
	} else {
		return FALSE;
	}
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
	char	pathbuf[NFILEN], tmpnam[NFILEN];
#ifdef  KANJI
	char	pathbufs[NFILEN];
#endif
	char	*cp, *dirpart, *nampart;
	struct	find_t	fileinfo;
	int	n, len, size, dirpartlen, nampartlen;
	char	*buffer;
	void	*malloc(), *realloc();

	strcpy(pathbuf, name);
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
	} else {
		strcpy(pathbuf, ".\\");	/* 90.05.30  by A.Shirahashi */
		dirpartlen = 0;
	}
	nampart = name + dirpartlen;
	nampartlen = strlen(nampart);
	for (cp = nampart; *cp; cp++) {		/* _dos_find*() return	*/
		/* 90.05.30  by A.Shirahashi: Use "toupper()". */
		/* 90.06.06  by S.Yoshida: Reused "TOUPPER()".  */
		/*			   And add "ISLOWER()". */
		if (ISLOWER(*cp)) {
			*cp = TOUPPER(*cp);	/* upper case name.	*/
		}
	}

#ifndef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
	(VOID) strcat(pathbuf, "*.*");
#ifdef KANJI
	strcpy(pathbufs, pathbuf);
	bufetos(pathbufs, strlen(pathbufs)+1);
	if (_dos_findfirst(pathbufs, _A_NORMAL | _A_RDONLY | _A_SUBDIR,
			&fileinfo) != 0) {
#else /* not KANJI */
	if (_dos_findfirst(pathbuf, _A_NORMAL | _A_RDONLY | _A_SUBDIR,
			&fileinfo) != 0) {
#endif /* KANJI */
		return (-1);
	}
#endif	/* NEW_COMPLETE */

	buffer = malloc(MALLOC_STEP);
	if (buffer == NULL) {
		return (-1);
	}
	size = MALLOC_STEP;
	len = 0;
	n = 0;

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
	(VOID) strcat(pathbuf, "*.*");
#ifdef KANJI
	strcpy(pathbufs, pathbuf);
	bufetos(pathbufs, strlen(pathbufs)+1);
	if (_dos_findfirst(pathbufs, _A_NORMAL | _A_RDONLY | _A_SUBDIR,
			&fileinfo) != 0) {
#else /* not KANJI */
	if (_dos_findfirst(pathbuf, _A_NORMAL | _A_RDONLY | _A_SUBDIR,
			&fileinfo) != 0) {
#endif /* KANJI */
		*buf = buffer;
		buffer[0] = '\0';
		return (0);
	}
#endif	/* NEW_COMPLETE */

	do {
		register int l;
		char ff_namee[NFILEN];

#ifdef __TURBOC__ /* 90.05.30  by A.Shirahashi */
		strcpy(ff_namee, fileinfo.ff_name);
#else
		strcpy(ff_namee, fileinfo.name);
#endif
		bufstoe(ff_namee, strlen(ff_namee)+1);
		if (strncmp(nampart, ff_namee, nampartlen) != 0)
			continue;		/* no-case-sensitive comparison */
		strncpy(tmpnam, pathbuf, dirpartlen);

		strcpy(tmpnam + dirpartlen, ff_namee);
		if (fileinfo.attrib & _A_SUBDIR) {
			strcat(tmpnam, "\\");
		}
		l = strlen(tmpnam)+1;
		/* 90.05.30  by A.Shirahashi */
		if (l > 5 && (stricmp(&tmpnam[l-5],".OBJ") == 0 ||
			      stricmp(&tmpnam[l-5],".EXE") == 0 ||
			      stricmp(&tmpnam[l-5],".COM") == 0))
		/* 90.05.30  by A.Shirahashi */
			continue;
		if (l + len >= size) {
				/* make room for double null */
			if ((buffer = realloc(buffer, size += MALLOC_STEP)) == NULL) {
				return(-1);
			}
		}
		/* 90.06.08  by A.Shirahashi: from */
                for (cp = tmpnam; *cp; cp++) {
                    if (ISUPPER(*cp)) {
                        *cp = TOLOWER(*cp);
                    }
                }
		/* 90.06.08  by A.Shirahashi: to */
		strcpy(buffer + len, tmpnam);
		len += l;
		n++;
	} while (_dos_findnext(&fileinfo) == 0);

	*buf = buffer;
	buffer[len] = '\0';
	return(n);
}
#endif	/* NO_FILECOMP */

/* 90.07.01  Add function to convert strings into lower case by S.Yoshida */
char	*
fftolower(name)
char	*name;
{
	register char	*p;

	if ((p = name) != NULL) {
		while(*p != '\0') {
			if (ISUPPER(*p)) {
				*p = TOLOWER(*p);
			}
			p++;
		}
	}
	return(name);
}

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
char *
file_name_part (s)
char	*s;
{
	int	i;

	for (i = strlen (s) - 1; i > 0; i--) {
		if (s[i - 1] == '/')
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
