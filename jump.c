/* $Id: jump.c,v 1.6 2000/09/21 17:28:29 amura Exp $ */
/*
 * jump-to-error
 *
 * Written by bsh, Dec 1991.
 */

/*
 * $Log: jump.c,v $
 * Revision 1.6  2000/09/21 17:28:29  amura
 * replace macro _WIN32 to WIN32 for Cygwin
 *
 * Revision 1.5  2000/07/25 15:04:21  amura
 * fix filevisit() args
 *
 * Revision 1.4  2000/07/22 20:47:32  amura
 * do not use excline()
 *
 * Revision 1.3  2000/07/18 12:38:56  amura
 * remove some compile warning
 *
 * Revision 1.2  2000/06/27 01:49:43  amura
 * import to CVS
 *
 * Revision 1.1  2000/06/01  05:27:47  amura
 * Initial revision
 *
 */

#include "config.h"
#ifdef	JUMPERR

#include "def.h"
#ifndef BSD
# ifndef index
#  define index(s,c)	strchr(s,c)
# endif
#endif	/* BSD */

#ifdef	REGEX_JAPANESE
#include "regex_j.h"
#else
#include "regex_e.h"
#endif
#define BYTEWIDTH 8

#ifndef	R_OK              /* for access() */
#  define R_OK 4
#endif

#define	BUFLEN	100

static struct re_pattern_buffer re_buff;
static char fastmap[(1 << BYTEWIDTH)];
static char compile_command[NLINE]	=  "make";
			/* "compile" command buffer.			*
			 * This	variable is *NOT* buffer local, this is *
			 * global. 	Y.Koyanagi			*/

/* Regular expression for filename/linenumber in error in compilation log.
 * It mathes
 *     filename, line 1234     or
 *     filename: 1234          or
 *     filename  1234          or
 *     filename(1234)          or
 *     1234 of filename        or
 *     filename at line 1234
 */
#if defined(AMIGA)
#define	DEFAULT_REGEXP   "\\(\\([^ \n]+:\\)?[^ \n]+\\(:? *\\| at line \\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\)"
#elif defined(MSDOS)||defined(HUMAN68K)||defined(WIN32)
#define	DEFAULT_REGEXP   "\\(\\([a-zA-Z]:\\)?[^ :\n]+\\(:? *\\| at line \\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\)"
#else
#define	DEFAULT_REGEXP   "\\([^ :\n]+\\(:? *\\| at line \\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\)"
#endif
/*
#define	DEFAULT_REGEXP   "\\([^ :\n]+\\(:? *\\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\|[^ \n]+ \\(at \\)*line [0-9]+\\)"
*/

static	char *grab_filename();

/*
 *
 */
set_regexp( pat )
    char *pat;
{
    char *message;
    
    re_buff.allocated = 40;
    re_buff.buffer = (char *) malloc (re_buff.allocated);
    if( re_buff.buffer == NULL ){
	ewprintf( "Can't get %d bytes", re_buff.allocated );
	re_buff.allocated = 0;
	return FALSE;
    }
    re_buff.fastmap = fastmap;
    re_buff.translate = NULL;
    message = re_compile_pattern (pat, strlen(pat), &re_buff);
    if (message != '\0') {
	ewprintf("Regex Error: %s", message);
	free( re_buff.buffer );
	re_buff.allocated = 0;
	return(FALSE);
    }
    re_compile_fastmap (&re_buff);
    return TRUE;
}

parse_error_message( clp, col, namebuf, ip, parse_end )
    LINE *clp;
    char *namebuf;
    int *ip, *parse_end;
{
    struct re_registers regs;
    char buf[BUFLEN+1];
    int i, len;
    char *filename;
    
    if( re_buff.allocated == 0 && !set_regexp( DEFAULT_REGEXP ) )
	return FALSE;

    i = re_search (&re_buff, ltext(clp), llength(clp),
		   col, llength(clp), &regs);
     if (i < 0)
	 return FALSE;

    if( parse_end ) *parse_end = regs.end[0]+1;
    len = regs.end[0] - regs.start[0];
    if( len > BUFLEN ) len = BUFLEN;
    strncpy( buf, ltext(clp) + regs.start[0], len );
    buf[len] = '\0';

    for(i=len; i>0 && ISDIGIT(buf[i-1]); --i )
	;
    if( i < len ){
	/* we are looking filename-first style. */
	*ip = atoi( &buf[i] );
	buf[i] = '\0';
	filename = grab_filename( buf );
    }
    else {
	/* line-number-first style */
	*ip = atoi(buf);
	while( ISDIGIT(buf[i]) || buf[i] == ' ' || buf[i] == '\t' )
	    ++i;
	filename = grab_filename( buf );
    }
    if( *filename ){
	strcpy( namebuf, filename );
	return TRUE;
    }
    return FALSE;
}    

/*
 * jump-to-error
 * Parse current line as a error message, then vist correspoding source code.
 */
jumptoerror(f,n)
{
    int  lineno;
    char buf[BUFLEN+1];
    char *p=buf;
    int col;
    LINE *dlp;
    extern int access();
    extern int gotoline();
    extern int filevisit(), poptofile();
	
    dlp = curwp->w_dotp;
    while (dlp != curbp->b_linep)
    {
    /* get filename and line number to visit */
	col = 0;
	while (col < llength(dlp) &&
	       parse_error_message(dlp, col, buf, &lineno, &col ) )
	{
	    if (0 == access( buf, R_OK )){
		/* ewprintf( "file:`%s' line %d", buf, lineno ); */
		/*
		 * All the hairly works to give filename to filevisit()
		 */
		curwp->w_flag |= WFHARD;
		curwp->w_doto = 0;
		curwp->w_linep = dlp;
		curwp->w_lines = 0;
		if (lforw(dlp) != curbp->b_linep)
		    curwp->w_dotp = lforw(dlp);
		eargset(buf);
		if (f&FFARG) {
			if (!filevisit(FFRAND,0)) return FALSE;
		} else {
			if (!poptofile(FFRAND,0)) return FALSE;
		}
		gotoline( FFARG, lineno );
		return TRUE;
	    }
	}
	
	dlp = lforw(dlp);
    }
    curwp->w_flag |= WFHARD;
    curwp->w_doto = 0;
    curwp->w_dotp = lback(curbp->b_linep);
    curwp->w_linep = lback(curbp->b_linep);
    curwp->w_lines = 0;
    
    ewprintf( "No more errors." );
    return FALSE;
}

/*
 * extract filename removing punctuations around.
 */
static char *
grab_filename( buf )
    char *buf;
{
    char *p;
#ifdef AMIGA
    int colon_has = FALSE;
#endif

    if( *buf == '"' ){
	/* "filename" */
	for(p=buf+1; *p && *p != '"'; ++p )
	    ;
	*p = '\0';
	return buf+1;
    }

    p = buf;
#ifdef AMIGA
    for(;*p && !index(" \t,(", *p); ++p )
    {
	if (*p == ':')
	{
	    if (colon_has)
		break;
	    else
		colon_has = TRUE;
	}
    }
    *p = '\0';
#else
# if defined(MSDOS)||defined(HUMAN68K)||defined(WIN32)
    if (buf[1]==':' && buf[2]!='\0' && (ISUPPER(buf[0])||ISLOWER(buf[0])))
	p += 2;
# endif
    for(;*p && !index(" \t:,(", *p); ++p )
	;
    *p = '\0';
#endif
    return buf;
}

/*
 * COMMAND: compile
 * 91.09.14  Modified by Y.Koyanagi
 * "compile" command.
 */
/*ARGSUSED*/
compile(f, n)
{
    register BUFFER	*bp, *obp;
    register WINDOW	*wp, *owp;
    register int	s;
    char	 buf[NLINE],*result;
    extern char *call_process();
    extern int isetmark(), gotobob();

    if (compile_command[0] == '\0')
	s=eread("compile: ", buf, NLINE, EFNEW);
    else
	s=eread("compile: (%s) ", buf, NLINE, EFNEW, compile_command);
    if (s == ABORT) return s;
    if (s == TRUE) strcpy(compile_command, buf);
    
    if ((bp = bfind("*compilation*", TRUE)) == NULL) return FALSE;
    if ((wp = popbuf(bp)) == NULL) return FALSE;
    if (bclear(bp) != TRUE) return FALSE;
    obp = curbp; owp = curwp;
    curbp = bp; curwp = wp;
    /* cmode(0,1) */
    if (addline(bp, compile_command) == FALSE) return FALSE;
    update();
    if ((result = call_process(compile_command, NULL)) == NULL)
	return FALSE;
    isetmark();
    s = insertfile(result, (char *)NULL);
    (VOID) gotobob(0, 1);
    bp->b_dotp = wp->w_dotp;
    bp->b_doto = wp->w_doto;
    curbp = obp;
    curwp = owp;
    bp->b_flag &= ~BFCHG;
    unlink(result);
    return s;
}


/*
 * COMMAND: c-next-error
 * 91.09.14  Modified by Y.Koyanagi
 * 00.03.30  Modified by amura for jump.c
 * goto next error using *compilation* buffer.
 */
/*ARGSUSED*/
nexterror(f, n)
{
    register BUFFER	*bp, *obp;
    register WINDOW	*wp, *owp;
    register int	s;

    if (strcmp(curbp->b_bname,"*compilation*") == 0) {
	nextwind(FFRAND,1);
    }
    if ((bp = bfind("*compilation*", TRUE)) == NULL) return FALSE;
    if ((wp = popbuf(bp)) == NULL) return FALSE;
    obp = curbp; owp = curwp;
    curbp = bp; curwp = wp;
    if ((s=jumptoerror(0, 1)) != TRUE) {
	curbp = obp;
	curwp = owp;
	return s;
    }
    return TRUE;
}

#endif	/* JUMPERR */
