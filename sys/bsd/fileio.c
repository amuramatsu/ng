/* $Id: fileio.c,v 1.4 2000/12/27 16:55:41 amura Exp $ */
/*
 *		bsd (4.2, others?), Sun (3.2, ?) and Ultrix-32 (?) file I/O.
 */

/*
 * $Log: fileio.c,v $
 * Revision 1.4  2000/12/27 16:55:41  amura
 * change d_makename() params for conservative reason, and bugfix in dires_()
 *
 * Revision 1.3  2000/12/14 18:17:37  amura
 * filename length become flexible and small bugfix
 *
 * Revision 1.2  2000/12/01 09:50:23  amura
 * fix problems open "/" and sybolic link directory
 *
 * Revision 1.1.1.1  2000/06/27 01:48:02  amura
 * import to CVS
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"

static	FILE	*ffp;
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__BSDI__)
#include <unistd.h>
#include <string.h>
#else
 extern	char	*getenv(), *strncpy();
#endif
char	*adjustname();

/*
 * Open a file for reading.
 */
ffropen(fn) char *fn; {
	if ((ffp=fopen(fn, "r")) == NULL)
		return (FIOFNF);
	return (FIOSUC);
}

/*
 * Open a file for writing.
 * Return TRUE if all is well, and
 * FALSE on error (cannot create).
 */
ffwopen(fn) char *fn; {
	if ((ffp=fopen(fn, "w")) == NULL) {
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

	if ((nname=alloca((unsigned)(strlen(fn)+1+1))) == NULL) {
		ewprintf("Can't get %d bytes", strlen(fn) + 1);
		return (ABORT);
	}
	(void) strcpy(nname, fn);
	(void) strcat(nname, "~");
	(void) unlink(nname);			/* Ignore errors.	*/
	if (rename(fn, nname) < 0) {
		free(nname);
		return (FALSE);
	}
	return (TRUE);
}

#ifdef	BUGFIX	/* 90.02.14  by S.Yoshida */
#ifndef	_SYS_STAT_H_
#include <sys/types.h>
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

	stat(fn, &filestat);
	return(filestat.st_mode & 0x0fff);
}

/*
 * Set file mode of a file fn to the specified mode.
 */
fsetfilemode(fn, mode)
char	*fn;
int	mode;
{
	chmod(fn, mode);
}
#endif	/* BUGFIX */
#endif

#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
#ifndef	_SYS_STAT_H_
#include <sys/types.h>
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

	if (stat(fn, &filestat) == 0) {
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
#ifdef SYMBLINK
#ifndef	_SYS_STAT_H_	/* 90.02.15  by S.Yoshida */
#include <sys/types.h>
#include <sys/stat.h>
#define	_SYS_STAT_H_
#endif	/* _SYS_STAT_H_ */
#ifndef MAXSYMLINKS
#define MAXSYMLINKS 8		/* maximum symbolic links to follow */
#endif
#endif
#include <pwd.h>
#ifndef NO_DIR
extern char *wdir;
#endif

char *adjustname(fn)
register char *fn;
{
    register char *cp;
    static char fnb[NFILEN];
    struct passwd *pwent;
#ifdef	SYMBLINK
    struct stat statbuf;
    int i, j;
    char linkbuf[NFILEN];
#endif

    switch(*fn) {
    	case '/':
	    cp = fnb;
	    *cp++ = *fn++;
	    break;
	case '~':
	    fn++;
	    if(*fn == '/' || *fn == '\0') {
		(VOID) strcpy(fnb, getenv("HOME"));
		cp = fnb + strlen(fnb);
	    	if(*fn) fn++;
		break;
	    } else {
		cp = fnb;
		while(*fn && *fn != '/') *cp++ = *fn++;
		*cp = '\0';
		if((pwent = getpwnam(fnb)) != NULL) {
		    (VOID) strcpy(fnb, pwent->pw_dir);
		    cp = fnb + strlen(fnb);
		    break;
		} else {
		    fn -= strlen(fnb) + 1;
		    /* can't find ~user, continue to default case */
		}
	    }
	default:
#ifndef	NO_DIR
	    strcpy(fnb, wdir);
	    cp = fnb + strlen(fnb);
	    break;
#else
	    return fn;				/* punt */
#endif
    }
    if(cp != fnb && cp[-1] != '/') *cp++ = '/';
    while(*fn) {
    	switch(*fn) {
	    case '.':
		switch(fn[1]) {
	            case '\0':
		    	*--cp = '\0';
		    	return fnb;
	    	    case '/':
	    	    	fn += 2;
		    	continue;
		    case '.':
		    	if(fn[2]=='/' || fn[2] == '\0') {
#ifdef SYMBLINK
			    cp[-1] = '\0';
			    for(j = MAXSYMLINKS; j-- && 
			    	    lstat(fnb, &statbuf) != -1 && 
			    	    (statbuf.st_mode&S_IFMT) == S_IFLNK &&
			    	    (i = readlink(fnb, linkbuf, sizeof linkbuf))
				    != -1 ;) {
				if(linkbuf[0] != '/') {
				    --cp;
				    while(cp > fnb && *--cp != '/') {}
				    ++cp;
				    (VOID) strncpy(cp, linkbuf, i);
				    cp += i;
				} else {
				    (VOID) strncpy(fnb, linkbuf, i);
				    cp = fnb + i;
				}
				if(cp[-1]!='/') *cp++ = '\0';
				else cp[-1] = '\0';
			    }
			    cp[-1] = '/';
#endif
			    --cp;
			    while(cp > fnb && *--cp != '/') {}
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
	    	fn++;
	    	continue;
	    default:
	    	break;
	}
	while(*fn && (*cp++ = *fn++) != '/') {}
    }
    if((cp-1)!=fnb && cp[-1]=='/') --cp;
    *cp = '\0';
    return fnb;
}

#ifndef NO_STARTUP
#include <sys/file.h>
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


#ifdef	ADDOPT
	if (ngrcfile == NULL)
		ngrcfile = getenv("NGRC");
	if (ngrcfile != NULL)
		if (access(ngrcfile, F_OK) == 0)  return ngrcfile;
#endif
	if ((file = getenv("HOME")) == NULL) goto notfound;
	if (strlen(file)+7 >= NFILEN - 1) goto notfound;
	(VOID) strcpy(home, file);
#ifdef	KANJI	/* 90.02.10  by S.Yoshida */
	(VOID) strcat(home, "/.ng");
#else	/* NOT KANJI */
	(VOID) strcat(home, "/.mg");
#endif	/* KANJI */
	if (suffix != NULL) {
		(VOID) strcat(home, "-");
		(VOID) strcat(home, suffix);
	}
	if (access(home, F_OK ) == 0) return home;

notfound:
#ifdef	STARTUPFILE
	file = STARTUPFILE;
	if (suffix != NULL) {
		(VOID) strcpy(home, file);
		(VOID) strcat(home, "-");
		(VOID) strcat(home, suffix);
		file = home;
	}
	if (access(file, F_OK ) == 0) return file;
#endif

	return NULL;
}
#endif

#ifndef NO_DIRED
#include <sys/wait.h>
#include "kbd.h"

copy(frname, toname)
char *frname, *toname;
{
    int pid;
    char *eargv[3];
    int status; 	/* change for Digital UNIX */

#ifdef	BUGFIX	/* 91.01.11  by Y.Kaneko */
    if((pid = vfork()) == 0) {
	execl("/bin/cp", "cp", frname, toname, (char *)NULL);
#else	/* ORIGINAL */
    if(pid = vfork()) {
	if(pid == -1)	return	-1;
	eargv[0] = frname;
	eargv[1] = toname;
	eargv[2] = NULL;
	execve("cp", eargv, (char **)NULL);
#endif	/* BUGFIX */
	_exit(1);	/* shouldn't happen */
    }
#ifdef	BUGFIX	/* 91.01.11  by Y.Kaneko */
    if(pid == -1)	return	-1;
#endif	/* BUGFIX */
    while(wait((int*)&status) != pid) {}

    return status == 0;
}

BUFFER *dired_(dirname)
char *dirname;
{
    register BUFFER *bp;
    char line[256];
    BUFFER *findbuffer();
    FILE *dirpipe;
    FILE *popen();

    if((dirname = adjustname(dirname)) == NULL) {
	ewprintf("Bad directory name");
	return NULL;
    }
    if(dirname[strlen(dirname)-1] != '/') (VOID) strcat(dirname, "/");
    if((bp = findbuffer(dirname)) == NULL) {
	ewprintf("Could not create buffer");
	return NULL;
    }
    if(bclear(bp) != TRUE) return FALSE;
#ifdef	EXTD_DIR
    if (bp->b_cwd)
	free(bp->b_cwd);
    if ((bp->b_cwd=malloc(strlen(dirname)+1)) != NULL)
	strcpy(bp->b_cwd, dirname);
    ensurecwd();
#endif
#ifdef	BUGFIX	/* 91.02.04  by M.Oki */
    (VOID) strcpy(line, "/bin/ls -al ");
    (VOID) strcpy(&line[12], dirname);
#else	/* ORIGINAL */
    (VOID) strcpy(line, "ls -al ");
    (VOID) strcpy(&line[7], dirname);
#endif	/* BUGFIX */
    if((dirpipe = popen(line, "r")) == NULL) {
	ewprintf("Problem opening pipe to ls");
	return NULL;
    }
    line[0] = line[1] = ' ';
    while(fgets(&line[2], 254, dirpipe) != NULL) {
	line[strlen(line) - 1] = '\0';		/* remove ^J	*/
	(VOID) addline(bp, line);
    }
    if(pclose(dirpipe) == -1) {
	ewprintf("Problem closing pipe to ls");
	return NULL;
    }
    bp->b_dotp = lforw(bp->b_linep);		/* go to first line */
    if (bp->b_fname)
	free(bp->b_fname);
    if ((bp->b_fname=malloc(strlen(dirname)+1)) != NULL)
	(VOID) strcpy(bp->b_fname, dirname);
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
  char* cp;
  int l,l1,len;
  char c;

  /* '47' is a magic number and is not correct always */

  if ( llength( lp ) <= 47 ) {
    return ABORT ;
  }
  l = llength(lp); 

  if (lgetc(lp, 2) == 'l') {
    do {
      while (l > 2 && lgetc(lp, l) != ' ')
        l--;
      if (bcmp(lp->l_text + l - 3, " -> ", 4) == 0)
        break;
      l--;
    } while (l > 2);
  }
  else {
    do {
      while (l > 2 && lgetc(lp, l)!=' ')
        l--;
      l1 = l;
      while (l > 2 && lgetc(lp, l)==' ')
        l--;
      while (l > 2 && (c=lgetc(lp, l))!=' ') {
        if (c!=':' && (c<'0'||c>'9')) {
          break;
        }
        l--;
      }
    } while (l > 2 && c != ' ');
  l = l1;
  }
  if (l <= 2)
    return ABORT;
  l++;

  len = llength(lp) - l + 1;
  if (buflen <= len+strlen(curbp->b_fname)) return ABORT;
  cp = fn;
  strcpy(cp, curbp->b_fname);
  cp += strlen(cp);
  bcopy(lp->l_text + l, cp, len);
  cp[len-1] = '\0';
#ifdef SYMBLINK
  if (lgetc(lp, 2) == 'l')
    return ffisdir(curbp->b_fname);
#endif
  return lgetc(lp, 2) == 'd';
}
#endif

#ifndef NO_DIRED	/* 91.01.15  by K.Maeda */
#ifndef	_SYS_STAT_H_
#include <sys/types.h>
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

	if (stat(dn, &filestat) == 0) {
		return ((filestat.st_mode & S_IFMT) == S_IFDIR);
	} else {
		return FALSE;
	}
}
#endif /* NO_DIRED */

#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
#include <sys/dir.h>

/* 89.11.20	Original code is for X68K (Human68K).
 * 90.04.04	Modified for BSD UNIX by S.Yoshida
 * Find file names starting with name.
 * Result is stored in *buf, got from malloc();
 * Return the number of files found, or
 * -1 of error occured.
 */

#define	MALLOC_STEP	256

fffiles(name, buf)
char *name, **buf;
{
	char pathbuf[128], tmpnam[128];
	char *cp, *dirpart, *nampart;
	DIR *dp;
#ifdef	BSD4_3
	register struct dirent *dirent;
#else
	register struct direct *dirent;
#endif
	int n, len, size, dirpartlen, nampartlen;
	char *buffer;
	struct stat st;

	strcpy(pathbuf, name);
	dirpart = NULL;
	for (cp = pathbuf; *cp; cp++) {
		if (*cp == '/')
			dirpart = cp;
	}
	if (dirpart) {
		*++dirpart = '\0';
		dirpartlen = dirpart-pathbuf;
	} else {
		strcpy(pathbuf, "./");
		dirpartlen = 0;
	}
	nampart = name + dirpartlen;
	nampartlen = strlen(nampart);

#ifndef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
	if ((dp = opendir(pathbuf)) == NULL)
		return -1;
#endif	/* NOT NEW_COMPLETE */
	
	buffer = malloc(MALLOC_STEP);
	if (buffer == NULL)
		return -1;
	size = MALLOC_STEP;
	len = 0; n = 0;
	
#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
	if ((dp = opendir(pathbuf)) == NULL) {
		*buf = buffer;
		buffer[0] = '\0';
		return 0;
	}
#endif	/* NEW_COMPLETE */

	while ((dirent = readdir(dp)) != NULL) {
		register int l;
		if (strncmp(nampart, dirent->d_name, nampartlen) != 0)
			goto nomatch;		/* case-sensitive comparison */
		strncpy(tmpnam, pathbuf, dirpartlen);
		strcpy(tmpnam+dirpartlen, dirent->d_name);
		if (stat(tmpnam, &st) < -1) goto nomatch;
		if ((st.st_mode & S_IFMT)==S_IFDIR)
			strcat(tmpnam, "/");
		l = strlen(tmpnam)+1;
		if (l > 3 && tmpnam[l-3] == '.' && tmpnam[l-2] == 'o')
			goto nomatch;
		if (l+len >= size) {
				/* make room for double null */
			if ((buffer = realloc(buffer, size += MALLOC_STEP)) == NULL)
				return -1;
		}
		strcpy(buffer+len, tmpnam);
		len += l;
		n++;
	nomatch:;
	}
	closedir(dp);

	*buf = buffer;
	buffer[len] = '\0';
	return n;
}
#endif

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
char *
file_name_part (s)
    char    *s;
{
    int    i;

    for (i = strlen (s); i > 0; i--)
      {
        if (s[i - 1] == '/')
	  break;
      }
    return (s + i);
}

char *
copy_dir_name (d, s)
    char    *d;
    char    *s;
{
    int    i;

    i = file_name_part (s) - s;
    strncpy (d, s, i);
    d[i] = '\0';
    return (d);
}
#endif	/* NEW_COMPLETE */
