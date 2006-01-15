/* $Id: echo.c,v 1.16.2.17 2006/01/15 11:12:02 amura Exp $ */
/*
 *		Echo line reading and writing.
 *
 * Common routines for reading
 * and writing characters in the echo line area
 * of the display screen. Used by the entire
 * known universe.
 */
/*
 * The varargs lint directive comments are 0 an attempt to get lint to shup
 * up about CORRECT usage of varargs.h.  It won't.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"
#include "echo.h"

#include "i_lang.h"
#include "lang.h"
#include "tty.h"
#include "ttyio.h"
#include "key.h"
#include "kbd.h"
#include "line.h"
#include "macro.h"
#include "window.h"
#include "cinfo.h"

#ifdef SUPPORT_ANSI
#  include <stdarg.h>
#else /* !SUPPPORT_ANSI */
# ifdef	LOCAL_VARARGS
#  include "varargs.h"
# else
# include <varargs.h>
# endif
#endif /* SUPPORT_ANSI */

static int veread _PRO((const char *, NG_WCHAR_t *, int, int, va_list *));

static VOID eformat _PRO((register const char *fp, register va_list *ap));
static VOID eputi _PRO((int, int));
static VOID eputl _PRO((long, int));
static VOID eputs _PRO((const char *));
static VOID eputls _PRO((const NG_WCHAR_t *));
static VOID eputc _PRO((int));
#ifndef NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
static int complt _PRO((int, int, NG_WCHAR_t *, int));
#endif /* NEW_COMPLETE */

#define	GETNUMLEN	6

#ifdef ADDFUNC	/* 91.01.16  by S.Yoshida */
static int earg_exist = FALSE;		/* Extra argument existing flag. */
static NG_WCHAR_t *earg_text;		/* Extra argument text body.	*/

/*
 * Set extra argument using in veread().
 */
VOID
eargset(earg)
NG_WCHAR_t *earg;
{
    if (earg) {
	earg_exist = TRUE;
	earg_text = earg;
    }
    else
	earg_exist = FALSE;
}
#endif /* ADDFUNC */

#ifdef EXTD_DIR
static NG_WCHAR_t *edef_text;		/* Default argument text body */

/*
 * Set extra argument using in veread().
 */
VOID
edefset(earg)
NG_WCHAR_t *earg;
{
    edef_text = earg;
}
#endif /* EXTD_DIR */

int epresf = FALSE;		/* Stuff in echo line flag.	*/
/*
 * Erase the echo line.
 */
VOID
eerase()
{
    ttcolor(CTEXT);
    ttmove(nrow-1, 0);
    tteeol();
    ttflush();
    epresf = FALSE;
}

/*
 * Ask "yes" or "no" question.
 * Return ABORT if the user answers the question
 * with the abort ("^G") character. Return FALSE
 * for "no" and TRUE for "yes". No formatting
 * services are available. No newline required.
 */
int
eyorn(sp)
const char *sp;
{
    register int s;

#ifndef NO_MACRO
    if (inmacro)
	return TRUE;
#endif
    ewprintf("%s? (y or n) ", sp);
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    for (;;) {
	s = getkey(FALSE);
	if (s == 'y' || s == 'Y')
	    return TRUE;
	if (s == 'n' || s == 'N')
	    return FALSE;
	if (s == CCHR('[') || s == CCHR('G'))
	    return ctrlg(FFRAND, 1);
	ewprintf("Please answer y or n.  %s? (y or n) ", sp);
    }
    /*NOTREACHED*/
}

/*
 * Like eyorn, but for more important question. User must type either all of
 * "yes" or "no", and the trainling newline.
 */
int
eyesno(sp)
const char *sp;
{
    register int s;
    NG_WCHAR_t buf[64];

#ifndef NO_MACRO
    if (inmacro)
	return TRUE;
#endif
    s = ereply("%s? (yes or no) ", buf, NG_WCHARLEN(buf), sp);
    for (;;) {
	if (s == ABORT)
	    return ABORT;
	if (s != FALSE) {
#ifndef NO_MACRO
	    if (macrodef) {
		LINE *lp = maclcur;
		
		maclcur = lp->l_bp;
		maclcur->l_fp = lp->l_fp;
		free((char *)lp);
	    }
#endif
	    if ((buf[0] == NG_WCODE('y') || buf[0] == NG_WCODE('Y'))
		&& (buf[1] == NG_WCODE('e') || buf[1] == NG_WCODE('E'))
		&& (buf[2] == NG_WCODE('s') || buf[2] == NG_WCODE('S'))
		&& (buf[3] == NG_EOS))
		return TRUE;
	    if ((buf[0] == NG_WCODE('n') || buf[0] == NG_WCODE('N'))
		&& (buf[1] == NG_WCODE('o') || buf[0] == NG_WCODE('O'))
		&& (buf[2] == NG_EOS))
		return FALSE;
	}
#ifdef notyet
	message("Please answer yes or no.");
	ttwait();
	s = ereply("%s? (yes or no) ", buf, NG_WCHARLEN(buf), sp);
#else
	s = ereply("Please answer yes or no.  %s? (yes or no) ",
		   buf, NG_WCHARLEN(buf), sp);
#endif
    }
    /*NOTREACHED*/
}

/*
 * Write out a prompt, and read back a
 * reply. The prompt is now written out with full "ewprintf"
 * formatting, although the arguments are in a rather strange
 * place. This is always a new message, there is no auto
 * completion, and the return is echoed as such.
 */
/*VARARGS 0*/
#ifdef SUPPORT_ANSI
int
ereply(const char *fp, NG_WCHAR_t *buf, int nbuf, ... )
{
    int i;
    va_list pvar;

    va_start(pvar, nbuf) ;
    i = veread(fp, buf, nbuf, EFNEW|EFCR, &pvar) ;
    va_end(pvar) ;
    return i;
}
#else /* SUPPORT_ANSI */
int
ereply(va_alist)
va_dcl
{
    va_list pvar;
    register const char *fp;
    register NG_WCHAR_t *buf;
    register int nbuf;
    register int i;

    va_start(pvar);
    fp = va_arg(pvar, const char *);
    buf = va_arg(pvar, NG_WCHAR_t *);
    nbuf = va_arg(pvar, int);
    i = veread(fp, buf, nbuf, EFNEW|EFCR, &pvar);
    va_end(pvar);
    return i;
}
#endif /* SUPPORT_ANSI */

/*
 * This is the general "read input from the
 * echo line" routine. The basic idea is that the prompt
 * string "prompt" is written to the echo line, and a one
 * line reply is read back into the supplied "buf" (with
 * maximum length "len"). The "flag" contains EFNEW (a
 * new prompt), an EFFUNC (autocomplete), or EFCR (echo
 * the carriage return as CR).
 */
/* VARARGS 0 */
#ifdef SUPPORT_ANSI
int
eread(const char *fp, NG_WCHAR_t *buf, int nbuf, int flag, ...)
{
    int	    i;
    va_list pvar;
    va_start(pvar, flag);

    i = veread(fp, buf, nbuf, flag, &pvar);
    va_end(pvar) ;
    return i;
}
#else /* SUPPORT_ANSI */
int
eread(va_alist)
va_dcl
{
    va_list pvar;
    char *fp;
    NG_WCHAR_t *buf;
    int nbuf, flag, i;
    
    va_start(pvar);
    fp   = va_arg(pvar, const char *);
    buf  = va_arg(pvar, NG_WCHAR_t *);
    nbuf = va_arg(pvar, int);
    flag = va_arg(pvar, int);
    i = veread(fp, buf, nbuf, flag, &pvar);
    va_end(pvar);
    return i;
}
#endif /* SUPPORT_ANSI */

#ifdef USING_GETNUM
int
getnum (prompt, num)
char *prompt;
int *num;
{
    NG_WCHAR_t numstr[GETNUMLEN];

    if (ereply("%s : ", numstr, NG_WCHARLEN(numstr), prompt) == FALSE)
	return (FALSE);
    *num = watoi(numstr);
    return (TRUE);
}
#undef USING_GETNUM
#endif /* USING_GETNUM */

#ifdef NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
#include "complt.h"

#ifdef MINIBUF_EDIT
/* Minibuffer hack from NGSCM by H.Kakugawa */
struct _Line {
    int  idx;
    int  lno;
    struct _Line *prev;
    struct _Line *next;
};

/* for Minibuffer history */
#define MB_HIST_FUNC		0
#define MB_HIST_BUF		1
#define MB_HIST_FILE		2
#define MB_HIST_MISC		3
#define MB_HIST_NTYPES		4
/* Note: mb_hist_buf[*][0] plays a special role. */
static NG_WCHAR_t *mb_hist_buf[MB_HIST_NTYPES][MB_NHISTS+1];
#define mb_get_hist_buf(flag)					\
	(((flag)&EFFUNC) ? mb_hist_buf[MB_HIST_FUNC] :		\
	(((flag)&EFBUF)  ? mb_hist_buf[MB_HIST_BUF] :		\
	(((flag)&EFFILE) ? mb_hist_buf[MB_HIST_FILE] :		\
			mb_hist_buf[MB_HIST_MISC])))		\

static int mb_init _PRO((int, const char *, va_list *));
static int mb_get_buffer _PRO((NG_WCHAR_t *, int));
static int mb_bufsize _PRO((void));
static NG_WCHAR_t mb_pointchar _PRO((void));
static int mb_pointoverwrite _PRO((NG_WCHAR_ta));
static int mb_gotochar _PRO((int));
static int mb2_gotochar _PRO((int));
static int mb_insert _PRO((int, int));
static int mb2_insert _PRO((int, int));
static int mb_insertstr _PRO((const NG_WCHAR_t *));
static VOID mb_insertcmplmsg _PRO((char *));
static int mb2_insertcmplmsg _PRO((char *));
static int mb_delcmplmsg _PRO((void));
static int mb_appenddiff _PRO((const NG_WCHAR_t *));
static int mb2_appenddiff _PRO((const NG_WCHAR_t *));
static int mb_delc _PRO((int));
static int mb2_delc _PRO((int));
static int mb_erasec _PRO((int));
static int mb2_erasec _PRO((int));
static int mb_killw _PRO((int));
static int mb_delw _PRO((int));
static int mb_forwc _PRO((int));
static int mb_backwc _PRO((int));
static int mb2_forwc _PRO((int));
static int mb2_backwc _PRO((int));
static int mb_forww _PRO((int));
static int mb_backww _PRO((int));
static int mb_upw _PRO((int));
static int mb_downw _PRO((int));
static int mb_cancel _PRO((void));
static int mb2_cancel _PRO((void));
static int mb_kill _PRO((void));
static int mb_yank _PRO((int));
static int mb_trans _PRO((int));
static int mb_matchparen _PRO((int, int, int));
static int mb_col _PRO((struct _Line *, int));
static int mb_putchar _PRO((int));
static VOID mb_move _PRO((int));
/* static VOID mb_movech _PRO((int));*/
static int mb_fixlines _PRO((int, struct _Line *, int, int *, int *));
static VOID mb_redisplay _PRO((void));
static VOID mb_refresh _PRO((int, int));
static VOID mb_hist_save _PRO((NG_WCHAR_t **, const NG_WCHAR_t *));
static NG_WCHAR_t* sformat _PRO((const char *, va_list *));
static int s_put_i _PRO((NG_WCHAR_t *, int, int, int, int));
static int s_put_l _PRO((NG_WCHAR_t *, int, int, long, int));
static int s_put_s _PRO((NG_WCHAR_t *, int, int, const char *));
static int s_put_ls _PRO((NG_WCHAR_t *, int, int, const NG_WCHAR_t *));
static int s_put_c _PRO((NG_WCHAR_t *, int, int, int));

#define mb_iseol() 	(_mb_gapend == _mb_size)
#define mb_isbol() 	(_mb_point == _mb_prompt)
#define mb_isword()	(ISWORD2(_mb_lang, _mb_buf[_mb_gapend]))
#define mb_begl()	(mb_gotochar(_mb_prompt))
#define mb_endl()	(mb_gotochar(_mb_bufsize))
#define mb_flush()	(ttflush())
#define chsize(s)	(_mb_lang->lm_width(*(s)))

static LANG_MODULE *_mb_lang;
static int  _mb_ccol;
static int  _mb_crow;
static NG_WCHAR_t *_mb_buf = NULL;
static int _mb_size      = 0;
static int _mb_point     = 0;
static int _mb_gapend    = 0;
static int _mb_prompt    = 0;
static int _mb_bufsize   = 0;
static struct _Line  Line = {0, 0, NULL, NULL};
static struct _Line  *CLine;

#ifdef CANNA
#include <canna/jrkanji.h>
#define CANBUF 1024

extern jrKanjiStatus ks;
static int mb_henkan _PRO((int));
char mbMode[CANBUF];
int mb_cannamode;
#endif

static int veread_complete _PRO((NG_WCHAR_t *, int, int, int));

static int
veread(fp, buf, nbuf, flag, ap)
const char *fp;
NG_WCHAR_t *buf;
int nbuf, flag;
va_list *ap;
{
    int MetaPrefix, CtluPrefix, nargs, cmp_msg_len, sign, ctluf;
    int c;
    NG_WCHAR_t **hist_buf;
    int hist_idx;
  
#ifdef ADDFUNC
    /* If an extra argument exists, use it.			*/
    /* This is a very easy way of not getting an argument from	*/
    /* the keyboard.	*/
    if (earg_exist) {
	wstrcpy(buf, earg_text);
	earg_exist = FALSE;
#ifdef EXTD_DIR
        edef_text = NULL;
#endif
	return (TRUE);
    }
#endif /* ADDFUNC */
#ifndef NO_MACRO
    if (inmacro) {
        bcopy (maclcur->l_text, buf, maclcur->l_used*sizeof(NG_WCHAR_t));
	buf[maclcur->l_used] = NG_EOS;
	maclcur = maclcur->l_fp;
#ifdef EXTD_DIR
        edef_text = NULL;
#endif
	return (TRUE);
    }
#endif

    mb_init(nbuf, fp, ap);
    cmp_msg_len = 0;
    MetaPrefix  = FALSE;
    CtluPrefix  = FALSE;
    nargs       = 1;
    sign        = 1;
    ctluf       = FALSE;
#ifdef CANNA
    mb_cannamode  = FALSE;
    mbMode[0] = '\0';
#endif
    hist_buf	= mb_get_hist_buf(flag);
    hist_buf[0] = buf; /* set top of history */
    hist_idx = 0;

    for (;;) {
	ttflush();
	lastflag = thisflag;
	thisflag = 0;
	
	c = getkey(FALSE); 
#ifdef CANNA
	if (mb_cannamode &&
	    (ks.length != 0 || !(c==NG_WSPACE||!ISASCII(c)||ISCTRL(c))) {
	    if (mb_henkan(c))
		continue;
	}
#endif
	/*
	* Check num prefix
	*/
	if (CtluPrefix) {
	    switch (c) {
	    case CCHR('G'):
		nargs = 1;
		sign  = 1;
		ctluf = FALSE;
		CtluPrefix = FALSE;
		thisflag = lastflag;
		continue;
		break;
	    case CCHR('U'):
		if (!ctluf) {
		    ctluf = FALSE;
		    nargs = 4;
		    sign = 1;
		}
		else 
		    nargs *= 4;
		break;
	    case '0':case '1':case '2':case '3':case '4':
	    case '5':case '6':case '7':case '8':case '9':
		if (ctluf) {
		    nargs = (c - '0');
		    ctluf = FALSE;
		}
		else
		    nargs = (10*nargs + (c - '0')); 
		break;
	    case '-':
		if (ctluf) {
		    sign = -1;
		    ctluf = FALSE;
		    nargs = 0;
		    break;
		}
		else {
		    hist_buf[0] = NULL; /* reset top of history */
		    return (ABORT);
		}
	    default: 
		goto Cmd;
	    }
	    thisflag = lastflag;
	    continue;
	}
Cmd:
	nargs = sign * nargs;
	CtluPrefix = FALSE;

	if (MetaPrefix) {
	    switch (c) {
	    case CCHR('G'):      /* Espace from Meta prefix  */
		break;
	    case '<':            /* Beginning of Line */
		mb_begl();
		break;
	    case '>':            /* End of Line */
		mb_endl();
		break;
	    case 'B': case 'b':  /* Backword word */
		mb_backww(nargs);
		break;
	    case 'F': case 'f':  /* Forward word */
		mb_forww(nargs);
		break;
	    case 'D': case 'd':  /* Delete word */
		if ((lastflag&CFKILL) == 0)
		    kdelete();
		thisflag |= CFKILL;
		mb_delw(nargs);
		break;
	    case 'u': case 'U':  /* Upcase word */
		mb_upw(nargs);
		break;
	    case 'l': case 'L':  /* Downcase word */
		mb_downw(nargs);
		break;
	    case CCHR('V'):      /* Scroll down completion window */
		if (nargs > 0)
		    while (nargs-- > 0)
			complete_scroll_down();
		else
		    while (nargs++ < 0)
			complete_scroll_up();
		break;
	    case 'p': case 'P':  /* History, prev */
		if (hist_idx < MB_NHISTS && hist_buf[hist_idx+1] != NULL) {
		    if (hist_idx == 0)
			mb_get_buffer(buf, nbuf);
		    hist_idx++;
		    mb_begl();
		    mb_kill();
		    mb_insertstr(hist_buf[hist_idx]);
		    mb_begl();
		}
		else {
		    message("Beginning of history; no preceding item");
		    ttwait();
		}
		mb_redisplay();
		break;
	    case 'n': case 'N':  /* History, next */
		if (hist_buf[1] == NULL) {
		    message("End of history; no default available");
		    ttwait();
		}
		else if (hist_idx == 0) {
		    message("End of history; no next item");
		    ttwait();
		}
		else {
		    hist_idx--;
		    mb_begl();
		    mb_kill();
		    mb_insertstr(hist_buf[hist_idx]);
		    mb_begl();
		}
		mb_redisplay();
		break;
		    
	    default:
		ttbeep();
	    }
	    MetaPrefix = FALSE;
	}
	else {
	    switch (c) {
	    case CCHR('['):    /* META prefix */
		MetaPrefix = TRUE;
		thisflag = lastflag;
		continue;
	    case CCHR('U'):    /* Ctl-U prefix */
		CtluPrefix = TRUE;
		nargs = 4;
		ctluf = TRUE;
		sign  = 1;
		thisflag = lastflag;
		continue;
	    case ' ':
	    case CCHR('I'):    /* SPC/TAB, or completion */
		if (flag & EFAUTO)
		    cmp_msg_len = veread_complete(buf, nbuf, c, flag);
		else 
		    mb_insert(1, (char) c);
		break;
	    case CCHR('J'):    /* End of input */
		c = CCHR('M');
		/*FALLTHRU*/
	    case CCHR('M'): /* Return, done.	*/
		mb_get_buffer(buf, nbuf);
		if (flag & EFFUNC) {
		    int    matchnum;
		    matchnum = complete(buf, nbuf, flag);
		    if (matchnum != COMPLT_SOLE
			&& matchnum != COMPLT_NOT_UNIQUE) {
		/* complete() will be called again, but i don't mind it */
			cmp_msg_len =
			    veread_complete(buf, nbuf, (int)CCHR('I'), flag);
			break;
		    }
		}
		complete_del_list();
		if (flag & EFCR) {
		    ttputc(CCHR('M'));
		    ttflush ();
		}
	        hist_buf[0] = NULL; /* reset top of history */
#ifndef NO_MACRO
		if (macrodef) {
		    LINE *lp;
		    
		    if ((lp = lalloc(mb_bufsize())) == NULL)
			return FALSE;
		    lp->l_fp = maclcur->l_fp;
		    maclcur->l_fp = lp;
		    lp->l_bp = maclcur;
		    maclcur = lp;
		    bcopy(buf, lp->l_text, mb_bufsize()*sizeof(NG_WCHAR_t));
		}
#endif
		if (mb_bufsize() == 0)
		    return FALSE;
		mb_hist_save(hist_buf, buf);
		return TRUE;
	    case CCHR('G'):	/* Abort */
		(VOID) ctrlg(FFRAND, 0);
		mb_flush();
		complete_del_list();
	        hist_buf[0] = NULL; /* reset top of history */
		return (ABORT);
	    case CCHR('A'):     /* Beginning of line */
		mb_begl();
		break;
	    case CCHR('B'):     /* Backword char */
		mb_backwc(nargs);
		break;
	    case CCHR('D'):     /* Delete */
		mb_delc(nargs);
		break;
	    case CCHR('E'):     /* End of line */
		mb_endl();
		break;
	    case CCHR('F'):     /* Forward char */
		mb_forwc(nargs);
		break;
	    case CCHR('H'):
	    case CCHR('?'):	/* Rubout, erase */
		mb_erasec(nargs);
		break;
	    case CCHR('K'):     /* Kill after point */
		if ((lastflag&CFKILL) == 0)
		    kdelete();
		thisflag |= CFKILL;
		mb_kill();
		break;
	    case CCHR('L'):     /* redraw */
		refresh(FFRAND, 0);
		update();
		mb_redisplay();
		break;
	    case CCHR('N'):     /* Scroll down completion window */
		if (nargs > 0)
		    while (nargs-- > 0)
			complete_scroll_down();
		else
		    while (nargs++ < 0)
			complete_scroll_up();
		break;
	    case CCHR('P'):     /* Scroll up completion window */
		if (nargs > 0)
		    while (nargs-- > 0)
			complete_scroll_up();
		else
		    while (nargs++ < 0)
			complete_scroll_down();
		break;
	    case CCHR('T'):	/* Transpose char */
		mb_trans(nargs);
		break;
	    case CCHR('W'):	/* Kill a word around point */
		mb_killw(nargs);
		break;
	    case CCHR('X'):	/* Kill line */
		mb_cancel();
		break;
	    case CCHR('Y'):     /* Yank */
		mb_yank(nargs);
		break;
	    case ')':
		mb_matchparen(nargs, '(', ')');
		break;
	    case ']':
		mb_matchparen(nargs, '[', ']');
		break;
	    case '}':
		mb_matchparen(nargs, '{', '}');
		break;
#ifdef CANNA
	    case CCHR('\\'):
	    case CCHR('O'):
		mb_cannamode = mb_cannamode ? FALSE : TRUE;
		if (!mb_cannamode)
		    mbMode[0] = '\0';
		break;
#endif
	    case CCHR('Q'):	/* Quote next	*/
		c = getkey(FALSE);
		/*FALLTHRU*/
	    default:		/* All the rest */
		if (mb_insert(nargs, (char) c) < 0)
		    break;
	    }
	}
	nargs = 1;
	ctluf = FALSE;
	sign  = 1;
    }
}

static int
veread_complete(buf, nbuf, c, flag)
NG_WCHAR_t *buf;
int nbuf, c, flag;
{
    int matchnum, wflag;

    mb_endl();
    wflag = (c == ' ');
    if (wflag) {
	mb2_insert(1, ' ');
	mb_get_buffer(buf, nbuf);
	if (complete(buf, nbuf, flag) == COMPLT_NO_MATCH)
	    mb2_erasec(1);
	else {
	    mb_appenddiff(buf);
	    return 0;
	}
    }
    mb_get_buffer(buf, nbuf);
    matchnum = complete(buf, nbuf, flag);
    mb_appenddiff(buf);
    if (wflag) {
	if (matchnum == COMPLT_AMBIGUOUS
	    || matchnum == COMPLT_NOT_UNIQUE)
	    complete_list_names(buf, flag);
	else if (matchnum == COMPLT_NO_MATCH) {
	    mb_insertcmplmsg(complete_message(matchnum));
	    ttwait();
	    mb_delcmplmsg();
	}
    }
    else {
	if (matchnum == COMPLT_AMBIGUOUS)
	    complete_list_names(buf, flag);
	else { 
	    mb_insertcmplmsg(complete_message(matchnum));
	    ttwait();
	    mb_delcmplmsg();
	}
    }
    return 0;
}

/*VARARGS 0*/
#ifdef SUPPORT_ANSI
int
message(const char *fp, ...)
{
    va_list pvar;
    va_start(pvar, fp);
    ewprintf(fp);
    va_end(pvar);
    return 0;
}
#else
int
message(va_alist)
va_dcl
{
    va_list pvar;
    register const char *fp;
    
    va_start(pvar);
    fp = va_arg(pvar, const char *);
    ewprintf(fp);
    va_end(pvar);
    return 0;
}
#endif /* SUPPORT_ANSI */

static int
mb_init(nbuf, fp, ap)
int  nbuf;
register const char *fp;
register va_list *ap;
{
    NG_WCHAR_t *prompt;
    struct _Line   *lp, *lp2;

    if (_mb_lang == NULL)
	_mb_lang = get_default_lang();
    prompt = sformat(fp, ap);
    if (_mb_buf == NULL) {
	_mb_size = nbuf + wstrlen(prompt) + 10;
	if ((_mb_buf = malloc(_mb_size*sizeof(NG_WCHAR_t))) == NULL)
	    return -1;
    }
    if (ttrow != nrow - 1) {
	ttcolor(CTEXT);
	epresf = TRUE;
    }
    
    _mb_ccol = 0;
    _mb_crow = nrow-1;
    _mb_prompt    = 0;
    _mb_point     = 0;
    _mb_gapend    = _mb_size;
    _mb_bufsize   = 0;

    if (Line.next != NULL) {
	lp = Line.next;
	while (lp != &Line) {
	    lp2 = lp->next;
	    free(lp);
	    lp = lp2;
	}
	Line.prev = NULL;
	Line.next = NULL;
    }
    if ((lp = (struct _Line*) malloc(sizeof(struct _Line))) == NULL)
	return 0;
    Line.prev  = lp;
    Line.next  = lp;
    lp->idx    = 0;
    lp->lno    = 1;
    lp->prev   = &Line;
    lp->next   = &Line;
    CLine      = lp;
    
    mb_insertstr(prompt);
    _mb_prompt = wstrlen(prompt);
    free(prompt);
#ifdef EXTD_DIR
    if (edef_text) {
	mb_insertstr(edef_text);
	edef_text = NULL;
    }
#endif
    return 0;
}

static int
mb_get_buffer(buf, nbuf)
NG_WCHAR_t *buf;
int nbuf;
{
    register int  i, j;
    
    j = 0;
    for (i = _mb_prompt; i < _mb_point;) {
	if (j >= nbuf-1) {
	    buf[j] = NG_EOS;
	    return 0;
	}
	buf[j++] = _mb_buf[i++];
    }
    for (i = _mb_gapend; i < _mb_size;) {
	if (j >= nbuf-1) {
	    buf[j] = NG_EOS;
	    return 0;
	}
	buf[j++] = _mb_buf[i++];
    }
    buf[j] = NG_EOS;
    return mb_bufsize();
}

static int
mb_bufsize()
{
    return _mb_bufsize-_mb_prompt;
}

static NG_WCHAR_t
mb_pointchar()
{
    if (_mb_point == 0)
	return NG_EOS;
    return _mb_buf[_mb_point-1];
}

static int
mb_pointoverwrite(ch)
NG_WCHAR_ta ch;
{
    if (_mb_point == 0)
	return NG_EOS;
    _mb_buf[_mb_point-1] = ch;
    return ch;
}

static int
mb_gotochar(i)
int i;
{
    int  col, pt, ocol, opt;
    struct _Line *lp;

    if (i == _mb_point)
	return _mb_point;
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp = CLine;
    mb2_gotochar(i);
    if (i < opt)
	mb_fixlines(0, Line.next, 0, &col, &pt);
    else 
	mb_fixlines(ocol, lp, opt, &col, &pt);
    if (lp == CLine)
	ttmove(_mb_crow, _mb_ccol);
    else
	mb_refresh(col, pt);
    return _mb_point;
}

static int
mb2_gotochar(i)
int i;
{
    if (i != _mb_point)
	mb_move(i-_mb_point);
    return _mb_point;
}

static int
mb_insert(n, c)
int n, c;
{
    int col, pt, ocol, opt;
    struct _Line *lp;

    if (n < 0)
	return -1;
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    mb2_insert(n, c);
    mb_fixlines(ocol, lp, opt, &col, &pt);
    mb_refresh(col, pt);
    return 0;
}

static int
mb2_insert(n, c)
int n, c;
{
    if (n < 0)
	return -1;
    while (n-- > 0)
	mb_putchar(c);
    return _mb_point;
}

static int
mb_insertstr(s)
const NG_WCHAR_t *s;
{
    int  col, pt, ocol, opt;
    struct _Line *lp;
    
    if (*s == NG_EOS)
	return 0;
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    while (*s != NG_EOS)
	mb_putchar(*(s++));
    if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
	mb_refresh(ocol, opt);
    else
	mb_refresh(col, pt);
    return _mb_point;
}

static int _mb_cmpl_msg_len   = 0;

static VOID
mb_insertcmplmsg(s)
char *s;
{
    int  pt, col;

    mb2_insertcmplmsg(s);
    mb_fixlines(_mb_ccol, CLine, _mb_point, &col, &pt);
    mb_refresh(_mb_ccol, _mb_point);
}

static int
mb2_insertcmplmsg(s)
char *s;
{
    int  pt;
    
    _mb_cmpl_msg_len = strlen(s);
    pt = _mb_point;
    while (*s != NG_EOS)
	mb_putchar(*(s++));
    mb2_gotochar(pt);
    return pt;
}

static int
mb_delcmplmsg()
{
    int  col, pt;
    struct _Line  *lp;
    
    if (_mb_cmpl_msg_len == 0)
	return 0;
    _mb_gapend  += _mb_cmpl_msg_len;
    _mb_bufsize -= _mb_cmpl_msg_len;
    _mb_cmpl_msg_len = 0;
    lp = CLine;
    mb_fixlines(_mb_ccol, CLine, _mb_point, &col, &pt);
    if (lp == CLine)
	tteeol();
    else
	mb_refresh(col, pt);
    _mb_cmpl_msg_len = 0;
    return _mb_point;
}

static int
mb_appenddiff(buf)
const NG_WCHAR_t *buf;
{
    int  ocol, opt, col, pt;
    struct _Line *lp;
    
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp = CLine;
    mb2_appenddiff(buf);
    if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
	mb_refresh(ocol, opt);
    else
	mb_redisplay();
    return _mb_point;
}

static int
mb2_appenddiff(buf)
const NG_WCHAR_t *buf;
{
    int p2;
    const NG_WCHAR_t *p1;
    
    for (p1 = buf, p2 = _mb_prompt; p2 < _mb_point; p1++, p2++)
	;
    while (*p1 != NG_EOS)
	mb_putchar(*(p1++));
    return _mb_point;
}

static int
mb_delc(n)
int n;
{
    int col, pt;
    
    if (n < 0)
	return -1;
    if (mb2_delc(n) < 0)
	return -1;
    mb_fixlines(_mb_ccol, CLine, _mb_point, &col, &pt);
    mb_refresh(col, pt);
    return _mb_point;
}

static int
mb2_delc(n)
int n;
{
    while (n-- > 0) {
	if (_mb_gapend == _mb_size)
	    break;
	_mb_gapend++;
	_mb_bufsize--;
    }
    return _mb_point;
}

static int
mb_erasec(n)
int  n;
{
    int  col, pt;
    struct _Line *lp;
    
    lp = CLine;
    mb2_erasec(n);
    mb_fixlines(0, Line.next, 0, &col, &pt);
    if (lp == CLine)
	mb_refresh(_mb_ccol, _mb_point); 
    else 
	mb_redisplay();
    return _mb_point;
}

static int
mb2_erasec(n)
int n;
{
    if (n < 0)
	return -1;
    while (n-- > 0) {
	if (_mb_point <= _mb_prompt)
	    return -1;
	if (mb2_backwc(1) < 0)
	    return -1;
	if (mb2_delc(1) < 0)
	    return -1;
    }
    return _mb_point;
}

static int
mb_killw(n)
int n;
{
    int col, pt;
    struct _Line *lp;
    
    if (n < 0)
	return -1;
    lp = CLine;
    while (n-- > 0) {
	if (!mb_iseol())
	    mb2_forwc(1);
	while (!mb_iseol() && mb_isword())
	    mb2_delc(1);
	if (!mb_isbol())
	    mb2_backwc(1);
	while (!mb_isbol() && !mb_isword()) {
	    mb2_delc(1);
	    mb2_backwc(1);
	}
	while (!mb_isbol() && mb_isword()) {
	    mb2_delc(1);
	    mb2_backwc(1);
	} 
	if (mb_isbol())
	    mb2_delc(1);
    }
    mb_fixlines(0, Line.next, 0, &col, &pt);
    if (CLine != lp)
	mb_refresh(_mb_ccol, _mb_point);    
    else
	mb_refresh(col, pt);
    return _mb_point;
}

static int
mb_delw(n)
int n;
{
    int ocol, opt, col, pt, i;
    struct _Line *lp;
    
    if (n <= 0)
	return _mb_point;
    
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp = CLine;
    while (n-- > 0) {
	if (_mb_gapend == _mb_size)
	    goto End;
	if (!mb_isword()) {
	    while (!mb_iseol() && !mb_isword()) {
		kinsert(_mb_buf[_mb_gapend+i], KFORW);
		_mb_gapend++;
		_mb_bufsize--;
		if (_mb_gapend == _mb_size)
		    goto End;
	    }
	}
	while (!mb_iseol() && mb_isword()) {
	    kinsert(_mb_buf[_mb_gapend+i], KFORW);
	    _mb_gapend++;
	    _mb_bufsize--;
	    if (_mb_gapend == _mb_size)
		goto End;
	}
    }
End:
#ifdef CLIPBOARD
    send_clipboard();
#endif
    mb_fixlines(_mb_ccol, CLine, _mb_point, &col, &pt);
    mb_refresh(col, pt);
    return _mb_point;
}

static int
mb_forwc(n)
int n;
{
    int ocol, opt, col, pt;
    struct _Line  *lp;
    
    if (n < 0)
	return mb_backwc(-n);
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    mb2_forwc(n);
    if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
	ttmove(_mb_crow, _mb_ccol);
    else 
	mb_refresh(col, pt);
    return _mb_point;
}

static int
mb_backwc(n)
int n;
{
    int col, pt;
    struct _Line  *lp;
    
    if (n < 0)
	return mb_forwc(-n);
    lp = CLine;
    mb2_backwc(n);
    mb_fixlines(0, Line.next, 0, &col, &pt);
    if (lp == CLine)
	ttmove(_mb_crow, _mb_ccol);
    else 
	mb_refresh(col, pt);
    return _mb_point;
}

static int
mb2_forwc(n)
int n;
{
    if (_mb_gapend == _mb_size)
	return _mb_point;
    while ((n-- > 0) && (_mb_gapend < _mb_size)) {
	mb_move(1);
    }
    return _mb_point;
}

static int
mb2_backwc(n)
int n;
{
    if (_mb_point == _mb_prompt)
	return _mb_point;

    while ((n-- > 0) && (_mb_point > _mb_prompt)) {
	mb_move(-1);
    }
    return _mb_point;
}

static int
mb_forww(n)
int n;
{
    int ocol, opt, col, pt;
    struct _Line  *lp;
    
    if (n < 0)
	return mb_backww(-n);
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    while (n-- > 0) {
	/**
	if (!mb_iseol())
	mb2_forwc(1);
	**/
	if (!mb_isword()) {
	    while (!mb_iseol() && !mb_isword())
		mb2_forwc(1);
	}
	while (!mb_iseol() && mb_isword())
	    mb2_forwc(1);
    }
    if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
	ttmove(_mb_crow, _mb_ccol);
    else 
	mb_refresh(col, pt);
    return _mb_point;
}

static int
mb_backww(n)
int n;
{
    int ocol, opt, col, pt;
    struct _Line  *lp;
    
    if (n < 0)
	return mb_forww(-n);
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    while (n-- > 0) {
	if (!mb_isbol())
	    mb2_backwc(1);
	if (!mb_isword()) {
	    while (!mb_isbol() && !mb_isword())
		mb2_backwc(1);
	}
	while (!mb_isbol() && mb_isword())
	    mb2_backwc(1);
	if (!mb_isbol())
	    mb2_forwc(1);
    }
    mb_fixlines(0, Line.next, 0, &col, &pt);
    if (lp == CLine)
	ttmove(_mb_crow, _mb_ccol);
    else 
	mb_refresh(col, pt);
    return _mb_point;
}

static int  
mb_upw(n)
int n;
{
    int ocol, opt, col, pt;
    struct _Line  *lp;
    
    if (n <= 0)
	return _mb_point;
    
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    while (n-- > 0) {
	if (!mb_isword()) {
	    while (!mb_iseol() && !mb_isword())
		mb2_forwc(1);
	}
	while (!mb_iseol() && mb_isword()) {
	    mb2_forwc(1);
	    if (ISLOWER(mb_pointchar()))
		mb_pointoverwrite(TOUPPER(mb_pointchar()));
	}
    }
    if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
	mb_refresh(ocol, opt);
    else
	mb_redisplay();
    return _mb_point;
}

static int  
mb_downw(n)
int n;
{
    int ocol, opt, col, pt;
    struct _Line  *lp;
    
    if (n <= 0)
	return _mb_point;
    
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    while (n-- > 0) {
	if (!mb_isword()) {
	    while (!mb_iseol() && !mb_isword())
		mb2_forwc(1);
	}
	while (!mb_iseol() && mb_isword()) {
	    mb2_forwc(1);
	    if (ISUPPER(mb_pointchar()))
		mb_pointoverwrite(TOLOWER(mb_pointchar()));
	}
    }
    if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
	mb_refresh(ocol, opt);
    else
	mb_redisplay();
    return _mb_point;
}

static int
mb_cancel()
{
    struct _Line *lp;
    
    lp = CLine;
    mb2_cancel();
    if (lp == CLine) {
	ttmove(_mb_crow, _mb_ccol);
	tteeol();
    }
    else
	mb_redisplay();
    return _mb_point;
}

static int
mb2_cancel()
{
    int  col, pt;
    
    _mb_point   = _mb_prompt;
    _mb_gapend  = _mb_size;
    _mb_bufsize = _mb_prompt;
    mb_fixlines(0, Line.next, 0, &col, &pt);
    return _mb_point;
}

static int
mb_kill()
{
    int col, pt;
    
    while (_mb_gapend < _mb_size) {
	if (kinsert(_mb_buf[_mb_gapend], KFORW) < 0)
	    break;
	_mb_gapend++;
	_mb_bufsize--;
    }
#ifdef CLIPBOARD
    send_clipboard();
#endif
    if (mb_fixlines(_mb_ccol, CLine, _mb_point, &col, &pt) == 0)
	tteeol();
    else 
	mb_refresh(col, pt);
    return _mb_point;
}

static int
mb_yank(n)
int n;
{
    int col, pt, ocol, opt, i;
    int ch;
    struct _Line *lp;
    
    if (n < 0)
	return -1;
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
#ifdef CLIPBOARD
    receive_clipboard();
#endif
    while (n-- > 0) {
	i = 0;
	while ((ch=kremove(i++)) >= 0) {
	    mb_putchar(ch);
	}
    }
    mb_fixlines(ocol, lp, opt, &col, &pt);
    if (lp == CLine)
	mb_refresh(ocol, opt);
    else
	mb_redisplay();
    return _mb_point;
}

static int
mb_trans(n)
int n;
{
    int ocol, opt, col, pt, i;
    struct _Line *lp;
    char s[2];
    
    if (n > 0) {
	ocol = 0;
	opt  = _mb_point;
	lp   =  CLine;
	while (n-- > 0) {
	    if (CLine == Line.next) {
		lp   = CLine;
		opt  = 0;
	    }
	    else {
		lp   = CLine->prev;
		opt  = lp->idx;
	    }
	    if (mb_isbol())
		break;
	    if (mb_iseol()) {
		mb2_backwc(1);
		if (mb_isbol()) {
		    mb2_forwc(1);
		    break;
		}
	    }
	    s[i] = _mb_buf[--_mb_point];
	    _mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
	    _mb_buf[--_mb_gapend] = s[-i];
	    
	    if (mb_iseol())
		break;
	    mb2_forwc(1);
	}
	if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
	    mb_refresh(ocol, opt);
	else 
	    mb_redisplay();
    } else if (n < 0) {
	while (n++ < 0) {
	    if (mb_isbol())
		break;
	    if (mb_iseol()) {
		mb2_backwc(1);
		if (mb_isbol()) {
		    mb2_forwc(1);
		    break;
		}
	    }
	    s[i] = _mb_buf[--_mb_point];
	    _mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
	    _mb_buf[--_mb_gapend] = s[1-i-1];
	    
	    if (mb_isbol())
		break;
	    mb2_backwc(1);
	}
	if (mb_fixlines(0, Line.next, 0, &col, &pt) == 0)
	    mb_refresh(col, pt);
	else 
	    mb_redisplay();
    }
    return _mb_point;
}

static int
mb_matchparen(n, opar, cpar)
int n;
int opar, cpar;
{
    int i, dep, dep2, instr, oinstr, point;
    int col, ocol, pt, opt, red, match, on;
    struct _Line  *clp, *dlp, *lp;
    
    if (n <= 0)
	return _mb_point;
    
    instr = 0;
    for (i = _mb_prompt; i < _mb_point; i++) {
	if (_mb_buf[i] == '"') {
	    if (instr == 0)
		instr = 1;
	    else 
		instr = 0;
	}
	else if (_mb_buf[i] == '\\')
	    i++;
    }
    oinstr = instr;
    
    on    = n;
    match = 0;
    red   = 0;
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    clp = dlp = CLine;
    while (n-- > 0) {
	clp = CLine;
	point = _mb_point;
	mb_putchar(cpar);
	mb_fixlines(ocol, lp, opt, &col, &pt);
	if (clp != CLine) {
	    red = 1;
	    col = _mb_ccol-1;
	}
	clp = CLine;
	if (red == 0) {
	    mb_refresh(col, pt);
	    dlp = clp;
	}
	dep  = 0;
	dep2 = 0;
	while (point >= _mb_prompt) {
	    if (instr == 0) {
		if (_mb_buf[point] == '"') {
		    if ((point > _mb_prompt) && (_mb_buf[point-1] == '\\'))
			point--;
		    else {
			instr = 1;
			dep2 = dep;
		    }
		} else if ((_mb_buf[point] == cpar) && (oinstr == 0)) {
		    dep++;
		} else if ((_mb_buf[point] == opar) && (oinstr == 0)) {
		    if (--dep == 0) {
			match++;
			if (dlp != clp) {
			    dlp = clp;
			    red = 1;
			    _mb_ccol = mb_col(clp, point);
			    mb_refresh(0, clp->idx);
			} else 
			    ttmove(_mb_crow, mb_col(clp, point));
			ttflush();
			ttwait();
			break;
		    }
		}
	    } else {
		if (_mb_buf[point] == '"') {
		    if ((point > _mb_prompt) && (_mb_buf[point-1] == '\\'))
			point--;
		    else {
			instr = 0;
			dep = dep2;
		    }
		} else if ((_mb_buf[point] == cpar) && (oinstr == 1)) {
		    dep++;
		} else if ((_mb_buf[point] == opar) && (oinstr == 1)) {
		    if (--dep == 0) {
			match++;
			if (dlp != clp) {
			    dlp = clp;
			    red = 1;
			    _mb_ccol = mb_col(clp, point);
			    mb_refresh(0, clp->idx);
			} else 
			    ttmove(_mb_crow, mb_col(clp, point));
			ttflush();
			ttwait();
			break;
		    }
		}
	    }
	    if (point == clp->idx)
		clp = clp->prev; 
	    point--;
	}
    }
    if (red == 1) {
	mb_fixlines(ocol, lp, opt, NULL, NULL);
	mb_redisplay();
    }
    else 
	ttmove(_mb_crow, _mb_ccol);
    if (match != on)
	ttbeep();
    return _mb_point;
}

static int 
mb_col(lp, pt)
struct _Line *lp;
int pt;
{
    int col, point;
    col = 0;
    for (point = lp->idx; point < pt; ) {
	col += chsize(&_mb_buf[point]);
	point++;
    }
    return col;
}

static int
mb_putchar(c)
int c;
{
    int i, more;
    NG_WCHAR_t *new_mb_buf;
    struct _Line *lp;
    
    if (_mb_point == (_mb_gapend-1)) {
	more = _mb_size;
	if ((new_mb_buf =
	     (NG_WCHAR_t *)malloc((_mb_size+more)*sizeof(NG_WCHAR_t))) == NULL)
	    return -1;
	for (i = 0; i < _mb_point; i++)
	    new_mb_buf[i] = _mb_buf[i];
	for (i = _mb_gapend; i < _mb_size; i++)
	    new_mb_buf[i+more] = _mb_buf[i];
	free(_mb_buf);
	_mb_buf     = new_mb_buf;
	_mb_gapend += more;
	_mb_size    = _mb_size + more;
	if (_mb_point != _mb_prompt)
	    for (lp = CLine->next; lp != &Line; lp = lp->next)
		lp->idx += more;
    }
    _mb_buf[_mb_point++] = c;
    _mb_bufsize++;
    return 0;
}

static VOID
mb_move(m)
int m;
{
    if (m > 0) {
	while (m > 0) {
	    _mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
	    --m;
	}
    }
    else {
	while (m < 0) {
	    _mb_buf[--_mb_gapend] = _mb_buf[--_mb_point];
	    ++m;
	}
    }
}

#if 0
static VOID
mb_movech(n)
int n;
{
    if (n > 0) {
	while (n > 0) {
	    _mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
	    --n;
	}
    }
    else {
	while (n < 0) {
	    _mb_buf[--_mb_gapend] = _mb_buf[--_mb_point];
	    ++n;
	}
    }
}
#endif

/*
 * This function redraw minibuf
 */
static int
mb_fixlines(col, line, point, colp, ptp)
int col, point, *colp, *ptp;
struct _Line *line;
{
    int v, bp, ccol, opt, redraw, lno;
    struct _Line *lp0, *lp1;

    if (colp != NULL)
	*colp = col;
    if (ptp != NULL)
	*ptp  = point; 
    ccol   = col;
    redraw = 0;
    bp     = 1;
    lno    = line->lno;
    for (;;) {
	if (point == _mb_point) {
	    _mb_ccol = col;
	    CLine    = line;
	    point    = _mb_gapend;
	    bp = 0;
	}
	if (point == _mb_size)
	    break;
	
	opt   = point;
	v = chsize(&_mb_buf[point]);
	col   += v;
	ccol  += v;
	point++;
    
	if (col >= (ncol-1)-1) {
	    if (line->next == &Line) {
		if ((lp0 = (struct _Line*)malloc(sizeof(struct _Line))) == NULL)
		    return -1;
		line->next = lp0;
		lp0->next  = &Line;
		Line.prev  = lp0;
		lp0->prev  = line;
	    } 
	    line      = line->next;
	    line->lno = ++lno;
	    line->idx = opt;
	    if (bp == 1) {
		if (colp != NULL)
		    *colp = 0;
		if (ptp != NULL)
		    *ptp  = line->idx;
		redraw = 1;
	    }
	    col  = v;
	}
    }
    
    for (lp0 = line->next; lp0 != &Line; lp0 = lp1) {
	lp1 = lp0->next;
	free(lp0);
    }
    line->next = &Line;
    Line.prev  = line;
    if ((_mb_ccol == (ncol-1)-2) && (_mb_gapend < _mb_size)) {
	CLine = CLine->next;
	_mb_ccol = 0;
	if (colp != NULL)
	    *colp = 0;
	if (ptp != NULL)
	    *ptp  = CLine->idx;
	redraw = 1;
    }
    else if ((_mb_ccol == 0) && (_mb_gapend == _mb_size)) {
	lp0 = CLine;
	CLine = CLine->prev;
	free(lp0);
	CLine->next = &Line;
	Line.prev   = CLine;
	mb_fixlines(0, CLine, CLine->idx, colp, ptp);
	redraw = 1;
    } 
    return redraw;
}

static VOID
mb_redisplay()
{
    mb_refresh(0, CLine->idx);
}

static VOID
mb_refresh(col, idx)
int col, idx;
{
    int limit;

    limit = (ncol-1)-2;
    ttmove(_mb_crow, col);
    for ( ; idx < _mb_point; ) {
	if (col >= limit)
	    break;
	col += chsize(&_mb_buf[idx]);
	eputc(_mb_buf[idx++]);
    }
    for (idx = _mb_gapend; idx < _mb_size; ) {
	if (col >= limit)
	    break;
	col += chsize(&_mb_buf[idx]);
	eputc(_mb_buf[idx++]);
    }
    if (CLine->next != &Line) {
	for ( ; col <= (ncol-1); col++)
	    eputc('\\');
    }
    else 
	tteeol();
    ttmove(_mb_crow, _mb_ccol);
    mb_flush();
}

#ifdef CANNA
static int
mb_henkan(c)
int c;
{
    int ilen;
    static char kakutei[CANBUF];
    static int oldlength, oldrevPos;
    int col, pt;
    struct _Line *lp;
    lp = CLine;

    ilen = jrKanjiString(0, c, kakutei, CANBUF, &ks);

    if (oldrevPos == 0)
	oldrevPos = _mb_point;
    if (oldlength == 0)
	oldlength = _mb_point;

    if (ilen < 0)
	return FALSE;
    if (ks.info & KanjiModeInfo) {
	jrKanjiControl(0,KC_QUERYMODE, mbMode);
    }
	
    if (ilen > 0) {
	mb2_gotochar(oldlength);
	while (_mb_point > oldrevPos)
	    mb2_erasec(1);
	mb_fixlines(0, Line.next, 0, &col, &pt);
	if (lp == CLine)
	    mb_refresh(_mb_ccol, _mb_point); 
	else 
	    mb_redisplay();
	mb_insertstr(kakutei);
	oldlength = oldrevPos = 0;
    }
    if (ks.length > 0) {
	mb2_gotochar(oldlength);
	while (_mb_point > oldrevPos)
	    mb2_erasec(1);
	mb_fixlines(0, Line.next, 0, &col, &pt);
	if (lp == CLine)
	    mb_refresh(_mb_ccol, _mb_point); 
	else 
	    mb_redisplay();
	oldrevPos = _mb_point;
	mb_insert( 1, '|');
	mb_insertstr(ks.echoStr);
	mb_insert( 1, '|');
	oldlength = _mb_point;
	mb_backwc(1);
    }
    else if ( ks.length == 0 && ilen == 0) {
	mb2_gotochar(oldlength);
	while (_mb_point > oldrevPos)
	    mb2_erasec(1);
	mb_fixlines(0, Line.next, 0, &col, &pt);
	if (lp == CLine)
	    mb_refresh(_mb_ccol, _mb_point); 
	else 
	    mb_redisplay();
	oldlength = oldrevPos = 0;
    }

    return TRUE;
}
#endif /* CANNA */

static NG_WCHAR_t*
sformat(fp, ap)
register const char *fp;
register va_list *ap;
{
    register NG_WCHAR_t *s;
    register int n;
    int c, idx;
    char kname[NKNAME];
    char *cp;

    n = ncol + 1;
    if ((s = malloc(n*sizeof(NG_WCHAR_t))) == NULL)
	return NULL;
    idx = 0;
    while ((c = *fp++) != '\0') {
	if (c != '%')
	    idx = s_put_c(s, idx, n, c);
	else {
	    c = *fp++;
	    switch (c) {
	    case 'c':
		(VOID) keyname(kname, va_arg(*ap, int));
		idx = s_put_s(s, idx, n, kname);
		break;
	    case 'k':
		cp = kname;
		for (c=0; c < key.k_count; c++) {
		    cp = keyname(cp, key.k_chars[c]);
		    *cp++ = ' ';
		}
		*--cp = '\0';
		idx = s_put_s(s, idx, n, kname);
		break;
	    case 'd':
		idx = s_put_i(s, idx, n, va_arg(*ap, int), 10);
		break;
	    case 'o':
		idx = s_put_i(s, idx, n, va_arg(*ap, int), 8);
		break;
	    case 's':
		idx = s_put_s(s, idx, n, va_arg(*ap, const char *));
		break;
	    case 'l':/* explicit longword */
		c = *fp++;
		switch(c) {
		case 'd':
		    idx = s_put_l(s, idx, n, (long)va_arg(*ap, long), 10);
		    break;
		case 's': /* wide str */
		    idx = s_put_ls(s, idx, n, va_arg(*ap, const NG_WCHAR_t *));
		    break;
		default:
		    idx = s_put_c(s, idx, n, c);
		    break;
		}
		break;
	    default:
		idx = s_put_c(s, idx, n, c);
	    }
	}
    }
    s[idx] = '\0';
    return s;
}

static int
s_put_i(p, idx, n, i, r)
register NG_WCHAR_t *p;
register int idx, n;
register int i;
register int r;
{
    register int q;
    
    if (i<0) {
	idx = s_put_c(p, idx, n, '-');
	i = -i;
    }
    if ((q=i/r) != 0)
	idx = s_put_i(p, idx, n, q, r);
    return s_put_c(p, idx, n, i%r+'0');
}

static int
s_put_l(p, idx, n, l, r)
register NG_WCHAR_t *p;
register int idx, n;
register long l;
register int r;
{
    register long q;
    
    if (l < 0) {
	idx = s_put_c(p, idx, n, '-');
	l = -l;
    }
    if ((q=l/r) != 0)
	idx = s_put_l(p, idx, n, q, r);
    return s_put_c(p, idx, n, (int)(l%r)+'0');
}

static int
s_put_s(p, idx, n,  s)
register NG_WCHAR_t *p;
register int idx, n;
register const char *s;
{
    while (*s != '\0')
	idx = s_put_c(p, idx, n, *s++);
    return idx;
}

static int
s_put_ls(p, idx, n,  s)
register NG_WCHAR_t *p;
register int idx, n;
register const NG_WCHAR_t *s;
{
    while (*s != NG_EOS)
	idx = s_put_c(p, idx, n, *s++);
    return idx;
}

static int
s_put_c(p, idx, n, c)
register NG_WCHAR_t *p;
register int idx, n;
register int c;
{
    epresf = TRUE;

    if (idx < n) {
#if 0
	if (ISCTRL(c)) {
	    idx = s_put_c(p, idx, n, '^');
	    c = CCHR(c);
	}
#endif
	p[idx++] = c;
    }
    return idx;
}

static VOID
mb_hist_save(hist_buf, buf)
NG_WCHAR_t *hist_buf[];
const NG_WCHAR_t *buf;
{
    int i;
    if (hist_buf[MB_NHISTS] != NULL)
	free(hist_buf[MB_NHISTS]);
    /* rotate history */
    for (i = MB_NHISTS; i > 1; i--)
	hist_buf[i] = hist_buf[i-1];
    /* and insert new history to head */
    hist_buf[1] = malloc((wstrlen(buf)+1) * sizeof(NG_WCHAR_t));
    wstrcpy(hist_buf[1], buf);
}
#else   /* NOT MINIBUF_EDIT */
static int veread_del_char ();
static int veread_complete ();

static int
veread(fp, buf, nbuf, flag, ap)
char *fp;
char *buf;
int nbuf;
int flag;
va_list *ap;
{
    int cpos, c;

#ifdef ADDFUNC	/* 91.01.16  by S.Yoshida */
    /* If an extra argument exists, use it.			*/
    /* This is a very easy way of not getting an argument from	*/
    /* the keyboard.	*/
    if (earg_exist) {
	strcpy(buf, earg_text);
	earg_exist = FALSE;
        edef_text = NULL;
	return (TRUE);
    }
#endif /* ADDFUNC */
#ifndef NO_MACRO
    if (inmacro) {
        bcopy (maclcur->l_text, buf, maclcur->l_used*sizeof(NG_WCHAR_t));
	buf[maclcur->l_used] = '\0';
	maclcur = maclcur->l_fp;
#ifdef EXTD_DIR
        edef_text = NULL;
#endif
	return (TRUE);
    }
#endif
    if ((flag & EFNEW) || ttrow != nrow - 1) {
	ttcolor(CTEXT);
	ttmove(nrow - 1, 0);
	epresf = TRUE;
    }
    else
	eputc(' ');
    eformat(fp, ap);
    tteeol();

#ifdef EXTD_DIR
    if (edef_text) {
	strcpy(buf, edef_text);
	edef_text = NULL;
	eputs(buf);
	cpos = strlen(buf);
    }
    else 
#endif
    {
	buf[0] = '\0';
	cpos = 0;
    }
    for (;;) {
	ttflush();
        c = getkey(FALSE);
	tteeol();
        switch (c) {
	case ' ':
	case CCHR('I'):
	    if (flag & EFAUTO) {
#if 0    /* 90.12.10    Sawayanagi Yosirou */
		if (flag & EFFILE) {
		    char *p;

		    p = adjustname (buf);
		    while (cpos > 0)
			cpos = veread_del_char (buf, cpos);
		    strcpy (buf, p);
		    eputs (buf);
		    cpos = strlen (buf);
		}
#endif
		cpos = veread_complete (buf, cpos, c, flag);
	    }
	    else if (cpos < nbuf - 1) {
		buf[cpos++] = (char) c;
		buf[cpos] = '\0';
		eputc((char) c);
	    }
	    break;
	case CCHR('J'):
	    c = CCHR('M');
	    /*FALLTHRU*/
	case CCHR('M'):	/* Return, done.	*/
	    if (flag & EFFUNC) {
		int  matchnum;
		
		matchnum = complete (buf, flag);
		eputs (&buf[cpos]);
		tteeol();
		cpos = strlen (buf);
		if (matchnum != COMPLT_SOLE
		    && matchnum != COMPLT_NOT_UNIQUE) {
		    /* complete() will be called again, but i don't mind it */
		    cpos = veread_complete (buf, cpos,
					    (int)CCHR('I'), flag);
		    break;
		    }
	    }
	    complete_del_list ();
	    if (flag & EFCR) {
		ttputc(CCHR('M'));
		ttflush ();
	    }
#ifndef NO_MACRO
	    if (macrodef) {
		LINE *lp;
		
		if ((lp = lalloc(cpos)) == NULL)
		    return FALSE;
		lp->l_fp = maclcur->l_fp;
		maclcur->l_fp = lp;
		lp->l_bp = maclcur;
		maclcur = lp;
		bcopy(buf, lp->l_text, cpos*sizeof(NG_WCHAR_t));
	    }
#endif
	    return ((buf[0] == '\0') ? FALSE : TRUE);
	case CCHR('G'):	/* Bell, abort.		*/
	case CCHR('['):
	    (VOID) ctrlg(FFRAND, 0);
	    ttflush();
	    complete_del_list ();
	    return (ABORT);
	case CCHR('N'):
	    complete_scroll_down ();
	    break;
	case CCHR('P'):
	    complete_scroll_up ();
	    break;
	case CCHR('H'):
	case CCHR('?'):	/* Rubout, erase.	*/
	    cpos = veread_del_char (buf, cpos);
	    break;
	case CCHR('X'):	/* C-X			*/
	case CCHR('U'):	/* C-U, kill line.	*/
	    while (cpos > 0)
		cpos = veread_del_char (buf, cpos);
	    break;
	case CCHR('W'):	/* C-W, kill to beginning of */
			/* previous word	*/
			/* back up to first word character or beginning	*/
	    while ((cpos > 0) && !ISWORD2(_mb_lang, buf[cpos - 1]))
		cpos = veread_del_char (buf, cpos);
	    while ((cpos > 0) && ISWORD2(_mb_lang, buf[cpos - 1]))
		    cpos = veread_del_char (buf, cpos);
	    break;
	case CCHR('\\'):
	case CCHR('Q'):	/* C-Q, quote next	*/
	    c = getkey(FALSE);
	    /*FALLTHRU*/
	default:	/* All the rest.	*/
	    if (cpos < nbuf - 1) {
		buf[cpos++] = (char) c;
		buf[cpos] = '\0';
		eputc((char) c);
	    }
	}
    }
}

static int
veread_del_char(buf, cpos)
char *buf;
int cpos;
{
    int w;
    if (cpos <= 0)
	return (0);
    w = terminal_lang->lm_width(buf[cpos]);
    while (w--) {
	ttputc('\b');
	ttputc(' ');
	ttputc('\b');
	--ttcol;
    }
    buf[--cpos] = '\0';
    return (cpos);
}

static int
veread_complete(buf, cpos, c, flag)
char *buf;
int cpos;
int c;
int flag;
{
    int    matchnum;
    int    i;
    int    wflag;
    int    cur_ttrow;
    int    cur_ttcol;

    wflag = (c == ' ');
    if (wflag) {
	buf[cpos] = ' ';
	buf[cpos + 1] = '\0';
	if (complete (buf, flag) == COMPLT_NO_MATCH)
	    buf[cpos] = '\0';
	else {
	    buf[cpos + 1] = '\0';
	    eputs (&buf[cpos]);
	    tteeol();
	    ++cpos;
	    return (cpos);
	}
    }
    matchnum = complete (buf, flag);
    if (buf[cpos] == '\0') {
	if (wflag) {
	    if (matchnum == COMPLT_AMBIGUOUS
	        || matchnum == COMPLT_NOT_UNIQUE)
		complete_list_names (buf, flag);
	    else if (matchnum == COMPLT_NO_MATCH) {
		cur_ttrow = ttrow;
		cur_ttcol = ttcol;
		eputs (complete_message (matchnum));
		tteeol();
		ttmove (cur_ttrow, cur_ttcol);
	    }
	}
	else {
	    if (matchnum == COMPLT_AMBIGUOUS)
		complete_list_names (buf, flag);
	    else {
		cur_ttrow = ttrow;
		cur_ttcol = ttcol;
		eputs (complete_message (matchnum));
		tteeol();
		ttmove (cur_ttrow, cur_ttcol);
	    }
	}
    }
    else {
	if (wflag) {
	    for (i = cpos + 1; buf[i] != NG_EOS; i++) {
	        if (i > cpos && ! ISWORD2(_mb_lang, buf[i - 1])) {
		    buf[i] = NG_EOS;
		    break;
		}
	    }
	}
	eputs (&buf[cpos]);
	tteeol();
	cpos = strlen (buf);
    }
    return (cpos);
}
#endif  /* MINIBUF_EDIT */
#else /* NOT NEW_COMPLETE */
static int
veread(fp, buf, nbuf, flag, ap)
char *fp;
NG_WCHAR_t *buf;
int nbuf, flag;
va_list *ap;
{
    register int cpos;
    register int i;
    register int c;

#ifdef ADDFUNC	/* 91.01.16  by S.Yoshida */
    /* If extra argument is exist, use it.			*/
    /* This is very easy way to not get argument from keyboard.	*/
    if (earg_exist) {
	strcpy(buf, earg_text);
	earg_exist = FALSE;
	return (TRUE);
      }
#endif /* ADDFUNC */
#ifndef NO_MACRO
    if (inmacro) {
	bcopy(maclcur->l_text, buf, maclcur->l_used*sizeof(NG_WCHAR_t));
	buf[maclcur->l_used] = '\0';
	maclcur = maclcur->l_fp;
	return TRUE;
    }
#endif
    cpos = 0;
    if ((flag&EFNEW)!=0 || ttrow!=nrow-1) {
	ttcolor(CTEXT);
	ttmove(nrow-1, 0);
	epresf = TRUE;
    }
    else
	eputc(' ');
    eformat(fp, ap);
    tteeol();
    ttflush();
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
    fepmode_off();
#endif
    for (;;) {
	c = getkey(FALSE);
	if ((flag&EFAUTO) != 0 && (c == ' ' || c == CCHR('I'))) {
	    cpos += complt(flag, c, buf, cpos);
	    continue;
	}
	switch (c) {
	case CCHR('J'):
	    c = CCHR('M');
	    /*FALLTHRU*/
	case CCHR('M'):	/* Return, done.	*/
	    if ((flag&EFFUNC) != 0) {
		if ((i = complt(flag, c, buf, cpos)) == 0)
		    continue;
		if (i > 0) cpos += i;
	    }
	    buf[cpos] = '\0';
	    if ((flag&EFCR) != 0) {
		ttputc(CCHR('M'));
		ttflush();
	    }
#ifndef NO_MACRO
	    if (macrodef) {
		LINE *lp;

		if ((lp = lalloc(cpos)) == NULL)
		    return FALSE;
		lp->l_fp = maclcur->l_fp;
		maclcur->l_fp = lp;
		lp->l_bp = maclcur;
		maclcur = lp;
		bcopy(buf, lp->l_text, cpos*sizeof(NG_WCHAR_t));
	    }
#endif
	    goto done;

	case CCHR('G'):	/* Bell, abort.		*/
	case CCHR('['):
	    eputc(CCHR('G'));
	    (VOID) ctrlg(FFRAND, 0);
	    ttflush();
	    return ABORT;
	    
	case CCHR('H'):
	case CCHR('?'):	/* Rubout, erase.	*/
	    if (cpos != 0) {
		int w = terminal_lang->lm_width(buf[cpos]);
		while (w--) {
		    ttputc('\b');
		    ttputc(' ');
		    ttputc('\b');
		    --ttcol;
		}
		--cpos;
		ttflush();
	    }
	    break;

	case CCHR('X'):	/* C-X			*/
	case CCHR('U'):	/* C-U, kill line.	*/
	    while (cpos != 0) {
		int w = terminal_lang->lm_width(buf[cpos]);
		while (w--) {
		    ttputc('\b');
		    ttputc(' ');
		    ttputc('\b');
		    --ttcol;
		}
		--cpos;
	    }
	    ttflush();
	    break;
	    
	case CCHR('W'):	/* C-W, kill to beginning of */
			/* previous word	*/
			/* back up to first word character or beginning */
	    while ((cpos > 0) && !ISWORD2(_mb_lang, buf[cpos - 1])) {
		int w = terminal_lang->lm_width(buf[cpos]);
		while (w--) {
		    ttputc('\b');
		    ttputc(' ');
		    ttputc('\b');
		    --ttcol;
		}
		--cpos;
	    }
	    while ((cpos > 0) && ISWORD2(_mb_lang, buf[cpos - 1])) {
		int w = terminal_lang->lm_width(buf[--cpos]);
		while (w--) {
		    ttputc('\b');
		    ttputc(' ');
		    ttputc('\b');
		    --ttcol;
		}
		--cpos;
	    }
	    ttflush();
	    break;

	case CCHR('\\'):
	case CCHR('Q'):	/* C-Q, quote next	*/
	    c = getkey(FALSE);
	    /*FALLTHRU*/
	default:	/* All the rest.	*/
	    if (cpos < nbuf-1) {
		buf[cpos++] = (char) c;
		eputc((char) c);
		ttflush();
	    }
	}
    }
done:
    return buf[0] != '\0';
}

/*
 * do completion on a list of objects.
 */
static int
complt(flags, c, buf, cpos)
int flags, c;
register NG_WCHAR_t *buf;
register int cpos;
{
    register LIST *lh, *lh2;
    int i, nxtra;
    int nhits, bxtra;
    int wflag = FALSE;
    int msglen, nshown;
    char *msg;
    
    if ((flags&EFFUNC) != 0) {
	buf[cpos] = '\0';
	i = complete_function(buf, c);
	if (i>0) {
	    eputs(&buf[cpos]);
	    ttflush();
	    return i;
	}
	switch(i) {
	case -3:
	    msg = " [Ambiguous]";
	    break;
	case -2:
	    i=0;
	    msg = " [No match]";
	    break;
	case -1:
	case 0:
	    return i;
	default:
	    msg = " [Internal error]";
	    break;
	}
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
    }
    else if ((flags&EFFILE) != 0) {
	if (c != '\t' && c != ' ')
	    panic("broken complt call: c");
	buf[cpos] = '\0';
	i = complete_filename(buf, cpos, c);
	if (i>0) {
	    eputs(&buf[cpos]);
	    ttflush();
	    return i;
	}
	switch(i) {
	case -3:
	    msg = " [Ambiguous]";
	    break;
	case -2:	/* no match */
#ifdef HUMAN68K	/* 90.11.15    Sawayanagi Yosirou */
	    msg = " [No match]";
	    break;
#else /* NOT HUMAN68K */
	    i = 0;
#endif /* HUMAN68K */
	case -1:	/* single match */
	case 0:
	    return i;
	default:
	    msg = " [Internal error]";
	    break;
	}
#endif /* NO_FILECOMP */
    }
    else {
	if ((flags&EFBUF) != 0)
	    lh = &(bheadp->b_list);
	else
	    panic("broken complt call: flags");
	
	if (c == ' ')
	    wflag = TRUE;
	else if (c != '\t' && c != CCHR('M'))
	    panic("broken complt call: c");
	
	nhits = 0;
	nxtra = HUGE;

	while (lh != NULL) {
	    for (i=0; i<cpos; ++i) {
		if (buf[i] != lh->l_name[i])
		    break;
	    }
	    if (i == cpos) {
		if (nhits == 0)
		    lh2 = lh;
		++nhits;
		if (lh->l_name[i] == '\0')
		    nxtra = -1;
		else {
		    bxtra = getxtra(lh, lh2, cpos, wflag);
		    if (bxtra < nxtra) nxtra = bxtra;
		    lh2 = lh;
		}
	    }
	    lh = lh->l_next;
	}
	if (nhits == 0)
	    msg = " [No match]";
	else if (nhits > 1 && nxtra == 0)
	    msg = " [Ambiguous]";
	else {		/* Got a match, do it to it */
	    /*
	    * Being lazy - ought to check length, but all things
	    * autocompleted have known types/lengths.
	    */
	    if (nxtra < 0 && nhits > 1 && c == ' ')
		nxtra = 1;
	    for (i = 0; i < nxtra; ++i) {
		buf[cpos] = lh2->l_name[cpos];
		eputc(buf[cpos++]);
	    }
	    ttflush();
	    if (nxtra < 0 && c != CCHR('M'))
		return 0;
	    return nxtra;
	}
    }
    /* Set up backspaces, etc., being mindful of echo line limit */
    msglen = strlen(msg);
    nshown = (ttcol + msglen + 2 > ncol) ? ncol - ttcol - 2 : msglen;
    eputs(msg);
    ttcol -= (i = nshown);	/* update ttcol!		*/
    while (i--)			/* move back before msg		*/
	ttputc('\b');
    ttflush();			/* display to user		*/
    i = nshown;
    while (i--)			/* blank out	on next flush	*/
	eputc(' ');
    ttcol -= (i = nshown);	/* update ttcol on BS's		*/
    while (i--)
	ttputc('\b');		/* update ttcol again!		*/
    return 0;
}

/*
 * The "lp1" and "lp2" point to list structures. The
 * "cpos" is a horizontal position in the name.
 * Return the longest block of characters that can be
 * autocompleted at this point. Sometimes the two
 * symbols are the same, but this is normal.
  */
int
getxtra(lp1, lp2, cpos, wflag)
register LIST *lp1, *lp2;
int cpos;
register int wflag;
{
    register int i;
    
    i = cpos;
    for (;;) {
	if (lp1->l_name[i] != lp2->l_name[i] || lp1->l_name[i] == '\0')
	    break;
	++i;
	if (wflag && !ISWORD2(_mb_lang, lp1->l_name[i-1]))
	    break;
    }
    return (i - cpos);
}
#endif /* NEW_COMPLETE */

/*
 * Special "printf" for the echo line.
 * Each call to "ewprintf" starts a new line in the
 * echo area, and ends with an erase to end of the
 * echo line. The formatting is done by a call
 * to the standard formatting routine.
 */
/*VARARGS 0 */
#ifdef SUPPORT_ANSI
VOID
ewprintf(const char *fp, ...)
{
    va_list pvar;

#ifndef NO_MACRO
    if (inmacro)
	return;
#endif
    va_start(pvar, fp);
    ttcolor(CTEXT);
    ttmove(nrow - 1, 0);
    eformat(fp, &pvar);
    va_end(pvar);
    tteeol();
    ttflush();
    epresf = TRUE;
}
#else /* SUPPORT_ANSI */
VOID
ewprintf(va_alist)
va_dcl
{
    va_list pvar;
    register char *fp;

#ifndef NO_MACRO
    if (inmacro)
	return;
#endif
    va_start(pvar);
    fp = va_arg(pvar, char *);
    ttcolor(CTEXT);
    ttmove(nrow-1, 0);
    eformat(fp, &pvar);
    va_end(pvar);
    tteeol();
    ttflush();
    epresf = TRUE;
}
#endif /* SUPPORT_ANSI */

/*
 * Printf style formatting. This is
 * called by both "ewprintf" and "ereply" to provide
 * formatting services to their clients. The move to the
 * start of the echo line, and the erase to the end of
 * the echo line, is done by the caller.
 * Note: %c works, and prints the "name" of the character.
 * %k prints the name of a key (and takes no arguments).
 */
static VOID
eformat(fp, ap)
register const char *fp;
register va_list *ap;
{
    register int c;
    char kname[NKNAME];
    char *cp;
    
    while ((c = *fp++) != '\0') {
	if (c != '%')
	    eputc(c);
	else {
	    c = *fp++;
	    switch (c) {
	    case 'c':
		(VOID) keyname(kname, va_arg(*ap, int));
		eputs(kname);
		break;

	    case 'k':
		cp = kname;
		for (c=0; c < key.k_count; c++) {
		    cp = keyname(cp, key.k_chars[c]);
		    *cp++ = ' ';
		}
		*--cp = '\0';
		eputs(kname);
		break;
		
	    case 'd':
		eputi(va_arg(*ap, int), 10);
		break;
		
	    case 'o':
		eputi(va_arg(*ap, int), 8);
		break;

	    case 's':
		eputs(va_arg(*ap, char *));
		break;
		
	    case 'l':/* explicit longword */
		c = *fp++;
		switch(c) {
		case 'd':
		    eputl((long)va_arg(*ap, long), 10);
		    break;
		    
		case 'c':/* wide char */
		    eputc(va_arg(*ap, int));
		    break;
		    
		case 's':/* wide string */
		    eputls(va_arg(*ap, NG_WCHAR_t *));
		    break;
		    
		default:
		    eputc(c);
		    break;
		}
		break;
		
	    default:
		eputc(c);
	    }
	}
    }
}

/*
 * Put integer, in radix "r".
 */
static VOID
eputi(i, r)
register int i;
register int r;
{
    register int q;

    if (i<0) {
	eputc('-');
	i = -i;
    }
    if ((q=i/r) != 0)
	eputi(q, r);
    eputc(i%r+'0');
}

/*
 * Put long, in radix "r".
 */
static VOID
eputl(l, r)
register long l;
register int  r;
{
    register long q;
    
    if (l < 0) {
	eputc('-');
	l = -l;
    }
    if ((q=l/r) != 0)
	eputl(q, r);
    eputc((int)(l%r)+'0');
}

/*
 * Put string.
 */
static VOID
eputs(s)
register const char *s;
{
    register int c;
    
    while ((c = *s++) != '\0')
	eputc(c);
}

/*
 * Put wide string.
 */
static VOID
eputls(s)
register const NG_WCHAR_t *s;
{
    register NG_WCHAR_t c;
    
    while ((c = *s++) != '\0')
	eputc(c);
}

/*
 * Put character. Watch for
 * control characters, and for the line
 * getting too long.
 */
static VOID
eputc(c)
register int c;
{
    epresf = TRUE;
#ifdef  MINIBUF_EDIT
    if (ttcol <= (ncol-1)-1) 
#else
    if (ttcol+2 < ncol) 
#endif
    {
	if (ISCTRL(c)) {
	    eputc('^');
	    c = CCHR(c);
	}
	ttputc(c);
	++ttcol;
    }
}

#ifndef NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
int
complete_filename(buf, cpos, c)
NG_WCHAR_t *buf;
int cpos, c;
{
    register int nhits, i, j;
    char *files, *try, *hit;
    int nxtra, wflag, bxtra, res;
    
    if ((nhits=fffiles(buf, &files)) < 0)
	return -4;	/* error */
    if (nhits == 0) {
	res = -2;	/* no match */
	goto end;
    }
    
    if (c == ' ')
	wflag = TRUE;
    else
	wflag = FALSE;
    nxtra = HUGE; hit = files;
    
    for (try=files, i=0; i<nhits; i++) {
	if (try[cpos] == '\0')
	    nxtra = -1;
	else {
	    bxtra = cpos;
	    for (;;) {
		if (try[bxtra] != hit[bxtra] || try[bxtra] == '\0')
		    break;
		bxtra++;
		if (wflag && !ISWORD2(_mb_lang, try[bxtra-1]))
		    break;
	    }
	    if (hit[bxtra] == '\0')
		hit = try;
	    bxtra -= cpos;
	    if (bxtra < nxtra)
		nxtra = bxtra;
	}
	try += strlen(try)+1;
    }
    if (nhits > 1 && nxtra == 0) {
	res = -3;	/* ambiguous */
	goto end;
    }
    if (nxtra < 0 && nhits > 1 && c == ' ')
	nxtra = 1;
    if (nxtra >= 0) {
	for (i=cpos; i<cpos+nxtra; i++)
	    buf[i] = hit[i];
	buf[i] = '\0';
	res = nxtra;
    }
    else { /* sole completion */
	for (i=cpos, j=strlen(files); i<j;) {
	    buf[i] = files[i];
	    i++;
	    if (wflag && !ISWORD2(_mb_lang, buf[i-1]))
		break;
	}
	buf[i] = '\0';
	res = i-cpos;
    }
end:
    free(files);
    return res;
}
#endif /* NO_FILECOMP */
#endif /* NEW_COMPLETE */
