/* $Id: fileio.c,v 1.12 2001/03/02 08:48:32 amura Exp $ */
/*
 *	unix file I/O. (for configure)
 *
 * Create from BSD UNIX version by amura, 2000
 */

/*
 * $Log: fileio.c,v $
 * Revision 1.12  2001/03/02 08:48:32  amura
 * now AUTOSAVE feature implemented almost all (except for WIN32
 *
 * Revision 1.11  2001/02/18 19:29:04  amura
 * split dir.c to port depend/independ
 *
 * Revision 1.10  2001/02/18 17:07:40  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.9  2001/02/11 15:39:01  amura
 * change getuid() geteuid()
 *
 * Revision 1.8  2001/01/05 13:55:28  amura
 * filename completion fixed
 *
 * Revision 1.7  2000/12/28 07:25:40  amura
 * fix filename complition
 *
 * Revision 1.6  2000/12/27 16:55:43  amura
 * change d_makename() params for conservative reason, and bugfix in dires_()
 *
 * Revision 1.5  2000/12/22 20:02:14  amura
 * readonly check is more correctly
 *
 * Revision 1.4  2000/12/21 16:56:25  amura
 * filename length become flexible
 *
 * Revision 1.3  2000/12/14 18:16:50  amura
 * filename length become flexible and
 * expand HOME directory in completion
 *
 * Revision 1.2  2000/12/01 09:51:23  amura
 * fix problems open "/" and sybolic link directory, remove typos
 *
 * Revision 1.1  2000/11/19 18:35:00  amura
 * support GNU configure system
 *
 */

#include	"config.h"
#include	"def.h"

static	FILE	*ffp;
#ifdef	STDC_HEADERS
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
    if (rename(fn, nname) < 0)
	return (FALSE);
    return (TRUE);
}

#ifdef	BUGFIX	/* 90.02.14  by S.Yoshida */
#ifndef	_SYS_STAT_H_
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
#include <sys/stat.h>
#define	_SYS_STAT_H_
#endif	/* _SYS_STAT_H_ */
#ifndef S_IWUSR
#define S_IWUSR         S_IWRITE
#endif
/*
 * Check whether file is read-only of a file fn.
 */
fchkreadonly(fn)
char	*fn;
{
    struct	stat	filestat;
    
    if (stat(fn, &filestat) == 0) {
	if (filestat.st_mode&S_IWOTH)
	    return FALSE;
	if (filestat.st_mode&S_IWGRP && filestat.st_gid==getegid())
	    return FALSE;
#ifdef HAVE_GETGROUPS
	if (filestat.st_mode & S_IWGRP) {
	    gid_t *gids;
	    int i, num;
	    num = getgroups(0, NULL);
	    if (num!=0 && (gids=alloca(sizeof(gid_t)*num))!=NULL) {
		num = getgroups(num, gids);
		for (i=0; i<num; i++)
		    if (filestat.st_gid == gids[i])
			return FALSE;
	    }
	}
#endif
	if (filestat.st_mode&S_IWUSR && filestat.st_uid==geteuid())
	    return FALSE;
	return TRUE;
    } else {
	return FALSE;
    }
}
#endif	/* READONLY */

#ifndef NO_DIR
#ifdef	HAVE_GETCWD
char	*getcwd();
#else
char	*getwd();
#endif

extern char *wdir;
extern char *startdir;
static char cwd[NFILEN];

/*
 * Initialize anything the directory management routines need
 */
VOID
dirinit()
{
#ifdef HAVE_GETCWD
    if (!(wdir = getcwd(cwd, NFILEN-1)))
#else
    if (!(wdir = getwd(cwd)))
#endif
	panic("Can't get current directory!");
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

rchdir(dir)
char *dir;
{
    int error = chdir(dir);
    if (error == -1)
	return error;
    strncpy(cwd, dir, NFILEN);
    cwd[NFILEN-1] = '\0';
    wdir = cwd;
    return error;
}
#endif

/*
 * The string "fn" is a file name.
 * Perform any required appending of directory name or case adjustments.
 * If NO_DIR is not defined, the same file should be refered to even if the
 * working directory changes.
 */
#ifdef	HAVE_SYMLINK
#ifndef	_SYS_STAT_H_	/* 90.02.15  by S.Yoshida */
#include <sys/stat.h>
#define	_SYS_STAT_H_
#endif	/* _SYS_STAT_H_ */
#ifndef MAXSYMLINKS
# ifdef MAXLINK
#define MAXSYMLINKS MAXLINK	/* maximum symbolic links to follow */
# else
#define MAXSYMLINKS 8		/* maximum symbolic links to follow */
# endif	/* MAXLINK */
#endif	/* MAXSYMLINKS */
#endif	/* HAVE_SYMLINK */
#include <pwd.h>

char *adjustname(fn)
register char *fn;
{
    register char *cp;
    static char fnb[NFILEN];
    struct passwd *pwent;
#ifdef	HAVE_SYMLINK
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
#ifdef HAVE_SYMLINK
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
#endif	/* HAVE_SYMLINK */
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
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifndef F_OK
#define F_OK 04			/* for stupid Sys V		*/
#endif

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
    static char		home[NFILEN];
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

#ifdef	HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef	HAVE_VFORK_H
#include <vfork.h>
#endif
#include "kbd.h"

#ifndef CP_CMD
#define CP_CMD	"/bin/cp"
#endif
copy(frname, toname)
char *frname, *toname;
{
    int pid;
    char *eargv[3];
    int status; 	/* change for Digital UNIX */

#ifdef	BUGFIX	/* 91.01.11  by Y.Kaneko */
    if((pid = vfork()) == 0) {
	execl(CP_CMD, "cp", frname, toname, (char *)NULL);
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

#ifndef LS_CMD
#define LS_CMD "/bin/ls"
#endif
BUFFER *dired_(dirname)
char *dirname;
{
    register BUFFER *bp;
    char line[CMDLINELENGTH];
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
    if(bp->b_cwd != NULL) free(bp->b_cwd);
    if((bp->b_cwd=malloc(strlen(dirname)+1)) == NULL) {
	ewprintf("Could not create buffer");
	return NULL;
    }
    strcpy(bp->b_cwd, dirname);
    ensurecwd();
#endif
#ifdef	BUGFIX	/* 91.02.04  by M.Oki / 2000.12.xx amura */
    (VOID) strncpy(line, LS_CMD " -al '", sizeof(line));
    line[sizeof(line)-1] = '\0';
    (VOID) strncat(line, dirname, sizeof(line)-strlen(line)-1);
    (VOID) strncat(line, "' 2>&1", sizeof(line)-strlen(line)-1);
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
    if(bp->b_fname != NULL) free(bp->b_fname);
    if((bp->b_fname=malloc(strlen(dirname)+1)) != NULL)
	(VOID)strcpy(bp->b_fname, dirname);
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

  /* '30' is a magic number and is not correct always */

  if ( llength( lp ) <= 30 ) {
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
#ifdef	HAVE_SYMLINK
  if (lgetc(lp, 2) == 'l')
    return ffisdir(curbp->b_fname);
#endif
  return lgetc(lp, 2) == 'd';
}

/*
 * I, a System V novice, could only figure out how to do unlinkdir()
 * and rename() as exec's of the appropriate functions.  So sue me.
 * --Stephen Walton, December 1987
 *
 * Now, SystemV has rmdir (from Release3) and rename (from Release 4).
 * I rewrite Ng use them.
 * --amura, 03 Apr 2000
 */

#ifndef HAVE_RMDIR
#ifndef RMDIR_CMD
#define RMDIR_CMD "/bin/rmdir"
#endif
unlinkdir(f)	/* System V Release 2 don't have rmdir(2)? */
char *f;
{
    int status, pid, wpid;

    if ((pid = vfork()) == 0)
	execl(RMDIR_CMD, "rmdir", f, (char *)NULL);
    else if (pid > 0)
	while ((wpid = wait(&status)) && wpid != pid)
	    ;
    else
	return FALSE;
    return status == 0;
}
#endif	/* !HAVE_RMDIR */

#ifndef HAVE_RENAME
#ifndef MV_CMD
#define MV_CMD "/bin/mv"
#endif
rename(f1, f2)	/* System V Release 2/3 don't have rename(2)? */
char *f1, *f2;
{
    int status, pid, wpid;

    if ((pid = vfork()) == 0)
	execl(MV_CMD, "mv", f1, f2, (char *)NULL);
    else if (pid > 0)
	while ((wpid = wait(&status)) && wpid != pid)
	    ;
    else
	return FALSE;
    return status == 0;
}
#endif /* !HAVE_RENAME */
#endif /* NO_DIRED */

#ifndef NO_DIRED	/* 91.01.15  by K.Maeda */

#ifndef	_SYS_STAT_H_
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

#ifdef	HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef	HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef	HAVE_FCNTL_H
#include <fcntl.h>
#endif

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
    char pathbuf[NFILEN], tmpnam[NFILEN];
    char *cp, *dirpart, *nampart;
#ifdef	HAVE_OPENDIR
    DIR *dp;
#ifdef	HAVE_DIRENT_H
    register struct dirent *dirent;
#else
    register struct direct *dirent;
#endif
#else	/* not HAVE_OPENDIR */
    int dp;
    struct direct dirbuf;
    register struct direct *dirent = &dirbuf;
#endif
    int n, len, size, dirpartlen, nampartlen;
    char *buffer;
    struct stat st;
    char *home;
    int homelen;

    if(name[0] == '~' && name[1] == '/' && (home = getenv("HOME"))) {
	homelen = strlen(home) - 1;
	strncpy(pathbuf, home, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
	strncat(pathbuf, &name[1], sizeof(pathbuf)-strlen(pathbuf)-1);
	cp = pathbuf + homelen;
    } else {
	home = NULL;
	homelen = 0;
	strncpy(pathbuf, name, sizeof(pathbuf));
	pathbuf[NFILEN-1] = '\0';
	cp = pathbuf;
    }
    dirpart = NULL;
    for (; *cp; cp++) {
	if (*cp == '/')
	    dirpart = cp;
    }
    if (dirpart) {
	*++dirpart = '\0';
	dirpartlen = dirpart - pathbuf;
    } else {
	strcpy(pathbuf, "./");
	dirpartlen = 0;
    }
    nampart = name + dirpartlen - homelen;
    nampartlen = strlen(nampart);
    
#ifndef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
#ifdef	HAVE_OPENDIR
    if ((dp = opendir(pathbuf)) == NULL)
#else
    if ((dp = open(pathbuf,O_RDONLY)) < 0)
#endif
	return -1;
#endif	/* NOT NEW_COMPLETE */

    buffer = malloc(MALLOC_STEP);
    if (buffer == NULL)
	return -1;
    size = MALLOC_STEP;
    len = 0; n = 0;

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
#ifdef	HAVE_OPENDIR
    if ((dp = opendir(pathbuf)) == NULL) {
#else
    if ((dp = open(pathbuf,O_RDONLY)) < 0) {
#endif
	*buf = buffer;
	buffer[0] = '\0';
	return 0;
    }
#endif	/* NEW_COMPLETE */
#ifdef	HAVE_OPENDIR
    while ((dirent = readdir(dp)) != NULL) {
#else
    while (read(dp,dirent,sizeof(struct direct)) == sizeof(struct direct)) {
#endif
	register int l;
#ifndef	HAVE_OPENDIR
	if (dirent->d_ino == 0) continue;
#endif
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
	if(home) {
	    strcpy(buffer+len, "~");
	    strcat(buffer+len, tmpnam+homelen+1);
	    l -= homelen;
	} else
	    strcpy(buffer+len, tmpnam);
	len += l;
	n++;
nomatch:;
    }
#ifdef	HAVE_OPENDIR
    closedir(dp);
#else
    close(dp);
#endif
    
    *buf = buffer;
    buffer[len] = '\0';
    return n;
}
#endif	/* !NO_FILECOMP */

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

#ifdef	AUTOSAVE
VOID
autosave_name(buff, name, buflen)
char* buff;
char* name;
{
    strcpy(buff, name);
    if (strlen(name)) {
	char *fn = rindex(name, '/');
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
