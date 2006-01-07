/* $Id: jump.c,v 1.9.2.4 2006/01/07 18:17:53 amura Exp $ */
/*
 * jump-to-error
 *
 * Written by bsh, Dec 1991.
 */

#include "config.h"

#ifdef JUMPERR
#include "def.h"
#include "jump.h"

#include "in_code.h"
#include "i_line.h"
#include "i_buffer.h"
#include "i_window.h"
#include "buffer.h"
#include "window.h"
#include "echo.h"
#include "file.h"
#include "basic.h"
#include "shell.h"

#define ng
#include "trex.h"

#ifndef	R_OK              /* for access() */
# define R_OK 4
#endif

#define	BUFLEN	100

#ifndef DEFAULT_COMPILE_CMD
#define DEFAULT_COMPILE_CMD	"make -k"
#endif

#ifndef DEFAULT_JMP_REGEXP
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
#define	DEFAULT_JMP_REGEXP   "\\(\\([^ \n]+:\\)?[^ \n]+\\(:? *\\| at line \\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\)"
#elif defined(MSDOS)||defined(HUMAN68K)||defined(WIN32)
#define	DEFAULT_JMP_REGEXP   "\\(\\([a-zA-Z]:\\)?[^ :\n]+\\(:? *\\| at line \\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\)"
#else
#define	DEFAULT_JMP_REGEXP   "\\([^ :\n]+\\(:? *\\| at line \\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\)"
#endif
/* other regexp
#define	DEFAULT_JMP_REGEXP   "\\([^ :\n]+\\(:? *\\|, line \\|(\\)[0-9]+\\)\\|\\([0-9]+ *of *[^ \n]+\\|[^ \n]+ \\(at \\)*line [0-9]+\\)"
*/
#endif /* DEFAULT_JMP_REGEXP */

static TRex *jmp_re_exp;
static NG_WCHAR_t *jmp_re_pat = NULL;
static NG_WCHAR_t compile_command[NLINE];
			/* "compile" command buffer.			*
			 * This	variable is *NOT* buffer local, this is *
			 * global. 	Y.Koyanagi			*/
static int compile_firsttime = TRUE;

static NG_WCHAR_t *grab_filename _PRO((NG_WCHAR_t *));

/*
 *
 */
static int
set_regexp(pat)
const char *pat;
{
    size_t len;
    const NG_WCHAR_t *errorp;

    len = strlen(pat)+1;
    if (jmp_re_pat != NULL)
	free(jmp_re_pat);
    jmp_re_pat = (NG_WCHAR_t *)malloc(len*sizeof(NG_WCHAR_t));
    if (jmp_re_pat == NULL) {
	ewprintf( "Can't get %d bytes", len*sizeof(NG_WCHAR_t));
	return FALSE;
    }
    wstrlcpya(jmp_re_pat, pat, len);
    if ((jmp_re_exp = trex_compile(jmp_re_pat, &errorp)) == NULL) {
	ewprintf("Regex Error at char %d", errorp - jmp_re_pat);
	return FALSE;
    }
    return TRUE;
}

static int
parse_error_message(clp, col, namebuf, ip, parse_end)
LINE *clp;
NG_WCHAR_t *namebuf;
int *ip, *parse_end;
{
    NG_WCHAR_t buf[BUFLEN+1];
    NG_WCHAR_t *filename;
    const NG_WCHAR_t *ep, *eep;
    int i, len;
    
    if (jmp_re_exp == NULL)
	return FALSE;

    if (trex_searchrange(jmp_re_exp, ltext(clp), ltext(clp) + llength(clp) + 1,
			 &ep, &eep) == TRex_False)
	return FALSE;

    if (parse_end != NULL)
	*parse_end = eep - ltext(clp);
    len = eep - ep;
    if (len > BUFLEN)
	len = BUFLEN;
    wstrlcpy(buf, ep, len);

    for (i=len; i>0 && ISDIGIT(buf[i-1]); --i )
	;
    if (i < len) {	/* we are looking filename-first style. */
	*ip = watoi(&buf[i]);
	buf[i] = NG_EOS;
	filename = grab_filename(buf);
    }
    else {		/* line-number-first style */
	*ip = watoi(buf);
	while( ISDIGIT(buf[i]) || buf[i] == NG_WSPACE || buf[i] == NG_WTAB)
	    ++i;
	filename = grab_filename(buf);
    }
    if (*filename) {
	wstrcpy(namebuf, filename);
	return TRUE;
    }
    return FALSE;
}    

/*
 * jump-to-error
 * Parse current line as a error message, then vist correspoding source code.
 */
int
jumptoerror(f,n)
int f, n;
{
    int lineno;
    NG_WCHAR_t wbuf[BUFLEN+1];
    int col;
    LINE *dlp;
    int buflen = 0;
    char *buf;
    int code = curbp->b_lang->lm_get_code(NG_CODE_FOR_FILENAME);
	
    if (jmp_re_exp == NULL) {
	if (set_regexp(DEFAULT_JMP_REGEXP) == FALSE)
	    return FALSE;
    }
    
    dlp = curwp->w_dotp;
    while (dlp != curbp->b_linep) {
    /* get filename and line number to visit */
	col = 0;
	while (col < llength(dlp) &&
	       parse_error_message(dlp, col, wbuf, &lineno, &col)) {
	    int i = curbp->b_lang->lm_out_convert_len(code, wbuf, NULL);
	    if (i > buflen) {
		buflen = i;
		MALLOCROUND(buflen);
		if ((buf = (char *)malloc(buflen)) == NULL) {
		}
	    }
	    curbp->b_lang->lm_out_convert(code, wbuf, NULL, buf);
	    if (access(buf, R_OK) == 0) {
		/* ewprintf( "file:`%ls' line %d", buf, lineno ); */
		/*
		 * All the hairly works to give filename to filevisit()
		 */
		curwp->w_flag |= WFHARD;
		curwp->w_doto = 0;
		curwp->w_linep = dlp;
		curwp->w_lines = 0;
		if (lforw(dlp) != curbp->b_linep)
		    curwp->w_dotp = lforw(dlp);
		eargset(wbuf);
		if (f&FFARG) {
		    if (!filevisit(FFRAND,0))
			return FALSE;
		}
		else {
		    if (!poptofile(FFRAND,0))
			return FALSE;
		}
		gotoline(FFARG, lineno);
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
static NG_WCHAR_t *
grab_filename(buf)
NG_WCHAR_t *buf;
{
    NG_WCHAR_t *p;
#ifdef AMIGA
    int colon_accept = TRUE;
#else
    int colon_accept = FALSE;
#endif

    if (*buf == '"') {
	/* "filename" */
	for(p=buf+1; *p && *p != '"'; ++p )
	    ;
	*p = NG_EOS;
	return buf+1;
    }

    p = buf;
#if defined(MSDOS)||defined(HUMAN68K)||defined(WIN32)
    if (buf[1]==NG_WCODE(':') && buf[2]!='\0' &&
	(ISUPPER(buf[0])||ISLOWER(buf[0])))
	p += 2;
#endif
    for (;*p &&
	  *p!=NG_WSPACE && *p!=NG_WTAB &&
	  *p!=NG_WCODE(',') && *p!=NG_WCODE('('); ++p ) {
	if (*p == NG_WCODE(':')) {
	    if (!colon_accept)
		break;
	    colon_accept = FALSE;
	}
    }
    *p = NG_EOS;
    return buf;
}

/*
 * COMMAND: compile
 * 91.09.14  Modified by Y.Koyanagi
 * "compile" command.
 */
/*ARGSUSED*/
int
compile(f, n)
int f, n;
{
    register BUFFER *bp, *obp;
    register WINDOW *wp, *owp;
    register int s;
    NG_WCHAR_t wbuf[NG_WCHARLEN(compile_command)];
    char *buf, *result;

    /* set default compile command */
    if (compile_firsttime) {
	wstrlcpya(compile_command, DEFAULT_COMPILE_CMD,
		  NG_WCHARLEN(compile_command));
	compile_firsttime = FALSE;
    }
    
    if (compile_command[0] == '\0')
	s = eread("compile: ", wbuf, NLINE, EFNEW);
    else
	s = eread("compile: (%ls) ", wbuf, NLINE, EFNEW, compile_command);
    if (s == ABORT)
	return s;
    if (s == TRUE)
	wstrlcpy(compile_command, wbuf, NG_WCHARLEN(compile_command));
    
    if ((bp = bfind("*compilation*", TRUE)) == NULL)
	return FALSE;
    if ((wp = popbuf(bp)) == NULL)
	return FALSE;
    if (bclear(bp) != TRUE)
	return FALSE;
    obp = curbp; owp = curwp;
    curbp = bp; curwp = wp;
    /* cmode(0,1) */
    if (addline(bp, compile_command) == FALSE)
	return FALSE;
    update();
    LM_OUT_CONVERT_TMP2(bp->b_lang, NG_CODE_FOR_FILENAME, compile_command, buf);
    if (buf == NULL)
	return FALSE;
    if ((result = call_process(buf, NULL)) == NULL)
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
int
nexterror(f, n)
int f, n;
{
    register BUFFER *bp, *obp;
    register WINDOW *wp, *owp;
    register int s;

    if (strcmp(curbp->b_bname,"*compilation*") == 0)
	nextwind(FFRAND,1);
    if ((bp = bfind("*compilation*", TRUE)) == NULL)
	return FALSE;
    if ((wp = popbuf(bp)) == NULL)
	return FALSE;
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
