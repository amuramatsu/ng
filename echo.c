/* $Id: echo.c,v 1.8.2.1 2003/03/02 15:29:37 amura Exp $ */
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

/*
 * $Log: echo.c,v $
 * Revision 1.8.2.1  2003/03/02 15:29:37  amura
 * add minibuffer history (thanks to kakugawa)
 *
 * Revision 1.8  2001/01/05 14:07:01  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.7  2000/11/19 18:15:11  amura
 * rename sput?() functions because HP-UX have sputl() function
 * in its library
 *
 * Revision 1.6  2000/07/18 12:38:56  amura
 * remove some compile warning
 *
 * Revision 1.5  2000/06/27 01:49:43  amura
 * import to CVS
 *
 * Revision 1.4  2000/06/04  06:21:47  amura
 * To be able to compile without KANJI
 *
 * Revision 1.3  2000/06/01  05:26:18  amura
 * Debug CANNA support
 *
 * Revision 1.2  2000/03/28  02:38:37  amura
 * CANNA support.
 * ,
 *
 * Revision 1.1  2000/03/10  21:32:52  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	"config.h"	/* 90.12.20  by S.Yoshida */
#include	"def.h"
#include	"key.h"
#ifdef SUPPORT_ANSI
#  include	<stdarg.h>
#else	/* !SUPPPORT_ANSI */
# ifdef	LOCAL_VARARGS
#  include	"varargs.h"
# else
# include	<varargs.h>
# endif
#endif	/* SUPPORT_ANSI */
#ifndef NO_MACRO
#  include	"macro.h"
#endif

static int	veread();
#ifdef SUPPORT_ANSI
VOID		ewprintf(char *fp, ...);
int		message(char *fp,...);
#else
VOID		ewprintf();
int		message();
#endif

static VOID	eformat pro((register char *fp, register va_list *ap));
static VOID	eputi();
static VOID	eputl();
static VOID	eputs();
static VOID	eputc();
#ifndef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
static int	complt();
#endif	/* NEW_COMPLETE */

#ifdef	ADDFUNC	/* 91.01.16  by S.Yoshida */
int	earg_exist = FALSE;		/* Extra argument existing flag. */
char	*earg_text;			/* Extra argument text body.	*/

/*
 * Set extra argument using in veread().
 */
VOID
eargset(earg)
char	*earg;
{
	earg_exist = TRUE;
	earg_text = earg;
}
#endif	/* ADDFUNC */
#ifdef EXTD_DIR
static char *edef_text;			/* Default argument text body */

/*
 * Set extra argument using in veread().
 */
VOID
edefset(earg)
char	*earg;
{
  edef_text = earg;
}
#endif /* EXTD_DIR */

int	epresf	= FALSE;		/* Stuff in echo line flag.	*/
/*
 * Erase the echo line.
 */
VOID
eerase() {
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
eyorn(sp) char *sp; {
	register int	s;

#ifndef NO_MACRO
	if(inmacro) return TRUE;
#endif
	ewprintf("%s? (y or n) ", sp);
#ifdef FEPCTRL	/* 90.11.26  by K.Takano */
	fepmode_off();
#endif
	for (;;) {
		s = getkey(FALSE);
		if (s == 'y' || s == 'Y') return TRUE;
		if (s == 'n' || s == 'N') return FALSE;
		if (s == CCHR('[') || s == CCHR('G')) return ctrlg(FFRAND, 1);
		ewprintf("Please answer y or n.  %s? (y or n) ", sp);
	}
	/*NOTREACHED*/
}

/*
 * Like eyorn, but for more important question. User must type either all of
 * "yes" or "no", and the trainling newline.
 */
eyesno(sp) char *sp; {
	register int	s;
	char		buf[64];

#ifndef NO_MACRO
	if(inmacro) return TRUE;
#endif
	s = ereply("%s? (yes or no) ", buf, sizeof(buf), sp);
	for (;;) {
		if (s == ABORT) return ABORT;
		if (s != FALSE) {
#ifndef NO_MACRO
			if (macrodef) {
			    LINE *lp = maclcur;

			    maclcur = lp->l_bp;
			    maclcur->l_fp = lp->l_fp;
			    free((char *)lp);
			}
#endif
			if ((buf[0] == 'y' || buf[0] == 'Y')
			    &&	(buf[1] == 'e' || buf[1] == 'E')
			    &&	(buf[2] == 's' || buf[2] == 'S')
			    &&	(buf[3] == '\0')) return TRUE;
			if ((buf[0] == 'n' || buf[0] == 'N')
			    &&	(buf[1] == 'o' || buf[0] == 'O')
			    &&	(buf[2] == '\0')) return FALSE;
		}
#ifdef  notyet
		message("Please answer yes or no.");
		ttwait();
		s = ereply("%s? (yes or no) ", buf, sizeof(buf), sp);
#else
		s = ereply("Please answer yes or no.  %s? (yes or no) ",
			   buf, sizeof(buf), sp);
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
ereply(char *fp, char *buf, int nbuf, ... )
{
    int	    i;
    va_list pvar;

    va_start(pvar, nbuf) ;
    i = veread(fp, buf, nbuf, EFNEW|EFCR, &pvar) ;
    va_end(pvar) ;
    return i;
}
#else	/* SUPPORT_ANSI */
ereply(va_alist)
va_dcl
{
	va_list pvar;
	register char *fp, *buf;
	register int nbuf;
	register int i;

	va_start(pvar);
	fp = va_arg(pvar, char *);
	buf = va_arg(pvar, char *);
	nbuf = va_arg(pvar, int);
	i = veread(fp, buf, nbuf, EFNEW|EFCR, &pvar);
	va_end(pvar);
	return i;
}
#endif	/* SUPPORT_ANSI */

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
eread(char *fp, char *buf, int nbuf, int flag, ...)
{
    int	    i;
    va_list pvar;
    va_start(pvar, flag);

    i = veread(fp, buf, nbuf, flag, &pvar);
    va_end(pvar) ;
    return i;
}
#else	/* SUPPORT_ANSI */
eread(va_alist)
va_dcl
{
	va_list pvar;
	char *fp, *buf;
	int nbuf, flag, i;
	
	va_start(pvar);
	fp   = va_arg(pvar, char *);
	buf  = va_arg(pvar, char *);
	nbuf = va_arg(pvar, int);
	flag = va_arg(pvar, int);
	i = veread(fp, buf, nbuf, flag, &pvar);
	va_end(pvar);
	return i;
}
#endif	/* SUPPORT_ANSI */

#ifdef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
#include    "complt.h"

#ifdef MINIBUF_EDIT
/* Minibuffer hack from NGSCM by H.Kakugawa */

#define MB_NHISTS		10
#define MB_HIST_FUNC		0
#define MB_HIST_BUF		1
#define MB_HIST_FILE		2
#define MB_HIST_MISC		3
#define MB_HIST_NTYPES		4
/* Note: mb_hist_buf[*][0] plays a special role. */
static char *mb_hist_buf[MB_HIST_NTYPES][MB_NHISTS+1];
#define mb_get_hist_buf(flag)					\
	(((flag)&EFFUNC) ? mb_hist_buf[MB_HIST_FUNC] :		\
	(((flag)&EFBUF)  ? mb_hist_buf[MB_HIST_BUF] :		\
	(((flag)&EFFILE) ? mb_hist_buf[MB_HIST_FILE] :		\
			mb_hist_buf[MB_HIST_MISC])))		\

extern int  refresh();

static int  mb_init();
static int  mb_get_buffer();
static int  mb_bufsize();
static int  mb_pointchar();
static int  mb_pointoverwrite();
static int  mb_point();
static int  mb_iseol();
static int  mb_isbol();
static int  mb_isword();
static int  mb_gotochar();
static int  mb2_gotochar();
static int  mb_insert();
static int  mb2_insert();
static int  mb_insertstr();
static VOID mb_insertcmplmsg();
static int  mb2_insertcmplmsg();
static int  mb_delcmplmsg();
static int  mb_appenddiff();
static int  mb2_appenddiff();
static int  mb_delc();
static int  mb2_delc();
static int  mb_erasec();
static int  mb2_erasec();
static int  mb_killw();
static int  mb_delw();
static int  mb_forwc();
static int  mb_backwc();
static int  mb2_forwc();
static int  mb2_backwc();
static int  mb_forww();
static int  mb_backww();
static int  mb_begl();
static int  mb_endl();
static int  mb_upw();
static int  mb_downw();
static int  mb_cancel();
static int  mb2_cancel();
static int  mb_kill();
static int  mb_yank();
static int  mb_trans();
static int  mb_matchparen();
static int  mb_col();
static int  mb_putchar();
static VOID mb_move();
static VOID mb_movech();
static int  mb_fixlines();
static VOID mb_redisplay();
static VOID mb_refresh();
static VOID mb_flush();
static VOID mb_hist_save();
static char* sformat();
static int  s_put_i();
static int  s_put_l();
static int  s_put_s();
static int  s_put_c();
static VOID chsize();
static VOID chsize2();

#ifdef CANNA
#include <canna/jrkanji.h>
#define CANBUF 1024

extern jrKanjiStatus ks;
static int mb_henkan();
char mbMode[CANBUF];
int  mb_cannamode;
#endif

#ifdef	CLIPBOARD
extern int	send_clipboard();
extern int	receive_clipboard();
#endif

static int veread_complete();

static int
veread(fp, buf, nbuf, flag, ap)
  char    *fp;
  char    *buf;
  int    nbuf;
  int    flag;
  va_list *ap;
{
  int        MetaPrefix, CtluPrefix, nargs, cmp_msg_len, sign, ctluf;
  int        c;
  char       **hist_buf;
  int        hist_idx;
  
#ifdef	ADDFUNC
  /* If an extra argument exists, use it.			*/
  /* This is a very easy way of not getting an argument from	*/
  /* the keyboard.	*/
    if(earg_exist)
      {
	strcpy(buf, earg_text);
	earg_exist = FALSE;
#ifdef EXTD_DIR
        edef_text = NULL;
#endif
	return (TRUE);
      }
#endif	/* ADDFUNC */
#ifndef NO_MACRO
    if(inmacro)
      {
        bcopy (maclcur->l_text, buf, maclcur->l_used);
	buf[maclcur->l_used] = '\0';
	maclcur = maclcur->l_fp;
#ifdef EXTD_DIR
        edef_text = NULL;
#endif
	return (TRUE);
      }
#endif

  mb_init(nbuf, fp, ap);
  cmp_msg_len = 0;
  MetaPrefix  = 0;
  CtluPrefix  = 0;
  nargs       = 1;
  sign        = 1;
  ctluf       = 0;
#ifdef CANNA
  mb_cannamode  = FALSE;
  mbMode[0] = '\0';
#endif
  hist_buf    = mb_get_hist_buf(flag);
  hist_buf[0] = buf;
  hist_idx = 0;

  for (;;){
    ttflush();
    lastflag = thisflag;
    thisflag = 0;

    c = getkey(FALSE); 
#ifdef CANNA
    if(mb_cannamode &&
       (ks.length != 0 || !(c==' '||ISCTRL(c)||ISKANJI(c))) ){
	if (mb_henkan(c))
	    continue;
    }
#endif

    if (CtluPrefix == 1){
      switch (c){
      case CCHR('G'):
	nargs = 1;
	sign  = 1;
	ctluf = 0;
	CtluPrefix = 0;
	thisflag = lastflag;
	continue;
	break;
      case CCHR('U'):
	if (ctluf == 0){
	  ctluf = 1;
	  nargs = 4;
	  sign = 1;
	} else 
	  nargs *= 4;
	break;
      case '0':case '1':case '2':case '3':case '4':
      case '5':case '6':case '7':case '8':case '9':
	if (ctluf == 1){
	  nargs = (c - '0');
	  ctluf = 0;
	} else {
	  nargs = (10*nargs + (c - '0')); 
	}
	break;
      case '-':
	if (ctluf == 1){
	  sign = -1;
	  ctluf = 0;
	  nargs = 0;
	  break;
	} else {
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
    CtluPrefix = 0;

    if (MetaPrefix == 1){
      switch (c){
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
      MetaPrefix = 0;
    } else {
      switch (c){
      case CCHR('['):    /* META prefix */
	MetaPrefix = 1;
	thisflag = lastflag;
	continue;
      case CCHR('U'):    /* Ctl-U prefix */
	CtluPrefix = 1;
	nargs = 4;
	ctluf = 1;
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
      case CCHR('M'):	/* Return, done.	*/
	mb_get_buffer(buf, nbuf);
	if (flag & EFFUNC){
	  int    matchnum;
	  matchnum = complete(buf, flag);
	  if (matchnum != COMPLT_SOLE
	      && matchnum != COMPLT_NOT_UNIQUE){
	    /* complete() will be called again, but i don't mind it */
	    cmp_msg_len = veread_complete(buf, nbuf, (int)CCHR('I'), flag);
	    break;
	  }
	}
	complete_del_list();
	if (flag & EFCR){
	  ttputc(CCHR('M'));
	  ttflush ();
	}
	hist_buf[0] = NULL; /* reset top of history */
#ifndef NO_MACRO
	if (macrodef){
	  LINE *lp;
	  
	  if((lp = lalloc(mb_bufsize())) == NULL)
	    return FALSE;
	  lp->l_fp = maclcur->l_fp;
	  maclcur->l_fp = lp;
	  lp->l_bp = maclcur;
	  maclcur = lp;
	  bcopy(buf, lp->l_text, mb_bufsize());
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
      case CCHR('\\'):
#ifdef CANNA
      case CCHR('O'):
	mb_cannamode = mb_cannamode ? FALSE : TRUE;
	if (!mb_cannamode)
	  mbMode[0] = '\0';
	break;
#endif
      case CCHR('Q'):	/* Quote next	*/
	c = getkey(FALSE);
      default:		/* All the rest */
	if (mb_insert(nargs, (char) c) < 0)
	  break;
      }
    }
    nargs = 1;
    ctluf = 0;
    sign  = 1;
  }
}

static int
veread_complete(buf, nbuf, c, flag)
  char   *buf;
  int    nbuf, c, flag;
{
  int    matchnum, wflag;

  mb_endl();
  wflag = (c == ' ');
  if (wflag){
    mb2_insert(1, ' ');
    mb_get_buffer(buf, nbuf);
    if (complete(buf, flag) == COMPLT_NO_MATCH){
      mb2_erasec(1);
    } else {
      mb_appenddiff(buf);
      return 0;
    }
  }
  mb_get_buffer(buf, nbuf);
  matchnum = complete(buf, flag);
  mb_appenddiff(buf);
  if (wflag){
    if (matchnum == COMPLT_AMBIGUOUS
	|| matchnum == COMPLT_NOT_UNIQUE)
      complete_list_names(buf, flag);
    else if (matchnum == COMPLT_NO_MATCH) {
      mb_insertcmplmsg(complete_message(matchnum));
      ttwait();
      mb_delcmplmsg();
    }
  } else {
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
message(char *fp,...)
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
  register char *fp;
  
  va_start(pvar);
  fp = va_arg(pvar, char *);
  ewprintf(fp);
  va_end(pvar);
  return 0;
}
#endif	/* SUPPORT_ANSI */

struct _Line {
  int  idx;
  int  lno;
  struct _Line *prev;
  struct _Line *next;
};

static int   _mb_ccol;
static int   _mb_crow;
static char *_mb_buf      = NULL;
static int  _mb_size      = 0;
static int  _mb_point     = 0;
static int  _mb_gapend    = 0;
static int  _mb_prompt    = 0;
static int  _mb_bufsize   = 0;
struct
  _Line  Line = {0, 0, NULL, NULL};
struct _Line  *CLine;

static int
mb_init(nbuf, fp, ap)
  int  nbuf;
  register char *fp;
  register va_list *ap;
{
  char  *prompt;
  struct _Line   *lp, *lp2;
  char  *sformat();

  prompt = sformat(fp, ap);
  if (_mb_buf == NULL){
    _mb_size = nbuf + strlen(prompt) + 10;
    if ((_mb_buf = malloc(_mb_size)) == NULL)
      return -1;
  }
  if (ttrow != nrow - 1){
    ttcolor(CTEXT);
    epresf = TRUE;
  }

  _mb_ccol = 0;
  _mb_crow = nrow-1;
  _mb_prompt    = 0;
  _mb_point     = 0;
  _mb_gapend    = _mb_size;
  _mb_bufsize   = 0;

  if (Line.next != NULL){
    lp = Line.next;
    while (lp != &Line){
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
  _mb_prompt = strlen(prompt);
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
  char *buf;
  int nbuf;
{
  register int  i, j;

  j = 0;
  for (i = _mb_prompt; i < _mb_point;){
    if (j >= nbuf-1){
#ifdef	KANJI
      if (ISKANJI(_mb_buf[i])) {
#ifdef	HOJO_KANJI
	if (i>=1 && ISHOJO(_mb_buf[i-1])) buf[j-2] = '\0';
	else
#endif
	buf[j-1] = '\0';
      } else
#endif	/* KANJI */
	buf[j] = '\0';
      return 0;
    }
    buf[j++] = _mb_buf[i++];
  }
  for (i = _mb_gapend; i < _mb_size;){
    if (j >= nbuf-1){
#ifdef	KANJI
	if (ISKANJI(_mb_buf[i])) {
#ifdef	HOJO_KANJI
	if (i>=1 && ISHOJO(_mb_buf[i-1])) buf[j-2] = '\0';
	else
#endif
	buf[j-1] = '\0';
    } else
#endif
	buf[j] = '\0';
      return 0;
    }
    buf[j++] = _mb_buf[i++];
  }
  buf[j] = '\0';
  return mb_bufsize();
}
static int
mb_bufsize()
{
  return _mb_bufsize-_mb_prompt;
}
static int
mb_pointchar()
{
  unsigned int  ch;

  ch = _mb_buf[_mb_point-1];
  return ch;
}
static int
mb_pointoverwrite(ch)
     unsigned int  ch;
{
  _mb_buf[_mb_point-1] = ch;
  return ch;
}
static int
mb_point()
{
  return _mb_point;
}
static int
mb_iseol()
{
  return (_mb_gapend == _mb_size);
}
static int
mb_isbol()
{
  return (_mb_point == _mb_prompt);
}
static int
mb_isword()
{
  return ISWORD(_mb_buf[_mb_gapend]);
}
static int
mb_gotochar(i)
  int  i;
{
  int  col, pt, ocol, opt;
  struct _Line *lp;

  if (i == _mb_point)
    return mb_point();
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
  return mb_point();
}
static int
mb2_gotochar(i)
  int  i;
{
  if (i != _mb_point)
    mb_move(i-_mb_point);
  return mb_point();
}
static int
mb_insert(n, c)
  int  n;
  char c;
{
  int  col, pt, ocol, opt;
#ifdef	KANJI
  static int  k1 = 0, nnn;
#ifdef	HOJO_KANJI
  static int  k2 = 0;
#endif
#endif
  struct _Line *lp;

#ifndef	KANJI
  if (n < 0)
    return -1;
  ocol = _mb_ccol;
  opt  = _mb_point;
  lp   = CLine;
  mb2_insert(n, c);
  mb_fixlines(ocol, lp, opt, &col, &pt);
  mb_refresh(col, pt);
#else	/* KANJI is TRUE */
  if (k1 == 0){
    if (n < 0)
      return -1;
    if (ISKANJI(c)){
      k1  = c;
      nnn = n;
    } else {
      ocol = _mb_ccol;
      opt  = _mb_point;
      lp   = CLine;
      mb2_insert(n, c);
      mb_fixlines(ocol, lp, opt, &col, &pt);
      mb_refresh(col, pt);
    }
#ifdef	HOJO_KANJI
  } else if (ISHOJO(k1) && k2 == 0) {
    if (ISKANJI(c)) {
      k2 = c;
      nnn = n;
    } else {
      ocol = _mb_ccol;
      opt  = _mb_point;
      lp   = CLine;
      mb2_insert(n, c);
      mb_fixlines(ocol, lp, opt, &col, &pt);
      mb_refresh(col, pt);
    }
#endif	/* HOJO_KANJI */
  } else {
    ocol = _mb_ccol;
    opt  = _mb_point;
    lp   = CLine;
    while (nnn-- > 0){
      mb2_insert(1, k1);
#ifdef	HOJO_KANJI
      if (k2 != 0) mb2_insert(1, k2);
#endif
      mb2_insert(1, c);
    }
    mb_fixlines(ocol, lp, opt, &col, &pt);
    mb_refresh(col, pt);
    k1 = 0;
#ifdef	HOJO_KANJI
    k2 = 0;
#endif
  }
#endif /* NOT KANJI */
  return 0;
}
static int
mb2_insert(n, c)
  int  n;
  char c;
{
  if (n < 0)
    return -1;
  while (n-- > 0)
    mb_putchar(c);
  return mb_point();
}
static int
mb_insertstr(s)
  char *s;
{
  int  col, pt, ocol, opt;
  struct _Line *lp;

  if (*s == '\0')
    return 0;
  ocol = _mb_ccol;
  opt  = _mb_point;
  lp   = CLine;
  while (*s != '\0')
    mb_putchar(*(s++));
  if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
    mb_refresh(ocol, opt);
  else
    mb_refresh(col, pt);
  return mb_point();
}

static _mb_cmpl_msg_len   = 0;

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
  pt = mb_point();
  while (*s != '\0')
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
  return mb_point();
}
static int
mb_appenddiff(buf)
  char *buf;
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
  return mb_point();
}
static int
mb2_appenddiff(buf)
  char *buf;
{
  int p2;
  char *p1;
  
  for (p1 = buf, p2 = _mb_prompt; p2 < _mb_point; p1++, p2++)
    ;
  while (*p1 != '\0')
    mb_putchar(*(p1++));
  return mb_point();
}
static int
mb_delc(n)
  int  n;
{
  int  col, pt;

  if (n < 0)
    return -1;
  if (mb2_delc(n) < 0)
    return -1;
  mb_fixlines(_mb_ccol, CLine, _mb_point, &col, &pt);
  mb_refresh(col, pt);
  return mb_point();
}
static int
mb2_delc(n)
  int  n;
{
  int  v, m;

  while (n-- > 0){
    if (_mb_gapend == _mb_size)
      break;
    chsize(&_mb_buf[_mb_gapend], &v, &m);
    _mb_gapend  += m;
    _mb_bufsize -= m;
  }
  return mb_point();
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
  return mb_point();
}
static int
mb2_erasec(n)
  int  n;
{
  if (n < 0)
    return -1;
  while (n-- > 0){
    if (_mb_point <= _mb_prompt)
      return -1;
    if (mb2_backwc(1) < 0)
      return -1;
    if (mb2_delc(1) < 0)
      return -1;
  }
  return mb_point();
}
static int
mb_killw(n)
  int   n;
{
  int  col, pt;
  struct _Line *lp;

  if (n < 0)
    return -1;
  lp = CLine;
  while (n-- > 0){
    if (!mb_iseol())
      mb2_forwc(1);
    while (!mb_iseol() && mb_isword())
      mb2_delc(1);
    if (!mb_isbol())
      mb2_backwc(1);
    while (!mb_isbol() && !mb_isword()){
      mb2_delc(1);
      mb2_backwc(1);
    }
    while (!mb_isbol() && mb_isword()){
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
  return mb_point();
}
static int
mb_delw(n)
  int n;
{
  int  v, m, ocol, opt, col, pt, i;
  struct _Line *lp;

  if (n <= 0)
    return mb_point();

  ocol = _mb_ccol;
  opt  = _mb_point;
  lp = CLine;
  while (n-- > 0){
    if (_mb_gapend == _mb_size)
      goto End;
    if (!mb_isword()){
      while (!mb_iseol() && !mb_isword()){
	chsize(&_mb_buf[_mb_gapend], &v, &m);
	for (i = 0; i < m; i++)
	  if (kinsert(_mb_buf[_mb_gapend+i], KFORW) < 0)
	    break;
	_mb_gapend  += m;
	_mb_bufsize -= m;
	if (_mb_gapend == _mb_size)
	  goto End;
      }
    }
    while (!mb_iseol() && mb_isword()){
      chsize(&_mb_buf[_mb_gapend], &v, &m);
      for (i = 0; i < m; i++)
	if (kinsert(_mb_buf[_mb_gapend+i], KFORW) < 0)
	  break;
      _mb_gapend  += m;
      _mb_bufsize -= m;
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
  return mb_point();
}
static int
mb_forwc(n)
  int  n;
{
  int  ocol, opt, col, pt;
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
  return mb_point();
}
static int
mb_backwc(n)
  int  n;
{
  int  col, pt;
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
  return mb_point();
}
static int
mb2_forwc(n)
  int  n;
{
  int  v, m;

  if (_mb_gapend == _mb_size)
    return mb_point();
  while ((n-- > 0) && (_mb_gapend < _mb_size)){
    chsize(&_mb_buf[_mb_gapend], &v, &m);
    mb_move(m);
  }
  return mb_point();
}
static int
mb2_backwc(n)
  int  n;
{
  int  m, v;

  if (_mb_point == _mb_prompt)
    return mb_point();
  while ((n-- > 0) && (_mb_point > _mb_prompt)){
    chsize2(&_mb_buf[_mb_point-1], &v, &m);
    mb_move(-m);
  }
  return mb_point();
}
static int
mb_forww(n)
  int n;
{
  int  ocol, opt, col, pt;
  struct _Line  *lp;

  if (n < 0)
    return mb_backww(-n);
  ocol = _mb_ccol;
  opt  = _mb_point;
  lp   = CLine;
  while (n-- > 0){
    /**
    if (!mb_iseol())
      mb2_forwc(1);
      **/
    if (!mb_isword()){
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
  return mb_point();
}
static int
mb_backww(n)
  int  n;
{
  int  ocol, opt, col, pt;
  struct _Line  *lp;

  if (n < 0)
    return mb_forww(-n);
  ocol = _mb_ccol;
  opt  = _mb_point;
  lp   = CLine;
  while (n-- > 0){
    if (!mb_isbol())
      mb2_backwc(1);
    if (!mb_isword()){
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
  return mb_point();
}
static int
mb_begl()
{
  return mb_gotochar(_mb_prompt);
}
static int
mb_endl()
{
  return mb_gotochar(_mb_bufsize);
}
static int  
mb_upw(n)
     int n;
{
  int  ocol, opt, col, pt;
  struct _Line  *lp;

  if (n <= 0)
    return mb_point();

  ocol = _mb_ccol;
  opt  = _mb_point;
  lp   = CLine;
  while (n-- > 0){
    if (!mb_isword()){
      while (!mb_iseol() && !mb_isword())
	mb2_forwc(1);
    }
    while (!mb_iseol() && mb_isword()){
      mb2_forwc(1);
      if (ISLOWER(mb_pointchar()))
	mb_pointoverwrite(TOUPPER(mb_pointchar()));
    }
  }
  if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
    mb_refresh(ocol, opt);
  else
    mb_redisplay();
  return mb_point();
}
static int  
mb_downw(n)
     int n;
{
  int  ocol, opt, col, pt;
  struct _Line  *lp;

  if (n <= 0)
    return mb_point();

  ocol = _mb_ccol;
  opt  = _mb_point;
  lp   = CLine;
  while (n-- > 0){
    if (!mb_isword()){
      while (!mb_iseol() && !mb_isword())
	mb2_forwc(1);
    }
    while (!mb_iseol() && mb_isword()){
      mb2_forwc(1);
      if (ISUPPER(mb_pointchar()))
	mb_pointoverwrite(TOLOWER(mb_pointchar()));
    }
  }
  if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
    mb_refresh(ocol, opt);
  else
    mb_redisplay();
  return mb_point();
}

static int
mb_cancel()
{
  struct _Line *lp;

  lp = CLine;
  mb2_cancel();
  if (lp == CLine){
    ttmove(_mb_crow, _mb_ccol);
    tteeol();
  } else
    mb_redisplay();
  return mb_point();
}
static int
mb2_cancel()
{
  int  col, pt;

  _mb_point   = _mb_prompt;
  _mb_gapend  = _mb_size;
  _mb_bufsize = _mb_prompt;
  mb_fixlines(0, Line.next, 0, &col, &pt);
  return mb_point();
}
static int
mb_kill()
{
  int col, pt;

  while (_mb_gapend < _mb_size){
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
  return mb_point();
}
static int
mb_yank(n)
  int  n;
{
  int  col, pt, ocol, opt, i;
  int  ch;
  struct _Line *lp;

  if (n < 0)
    return -1;
  ocol = _mb_ccol;
  opt  = _mb_point;
  lp   = CLine;
#ifdef CLIPBOARD
  receive_clipboard();
#endif
  while (n-- > 0){
    i = 0;
    while ((ch=kremove(i++)) >= 0){
      mb_putchar(ch);
    }
  }
  mb_fixlines(ocol, lp, opt, &col, &pt);
  if (lp == CLine)
    mb_refresh(ocol, opt);
  else
    mb_redisplay();
  return mb_point();
}
static int
mb_trans(n)
  int n;
{
  int  ocol, opt, col, pt, v1, m1, v2, m2, i;
  struct _Line *lp;
  char s[2];

  if (n > 0){
    ocol = 0;
    opt  = _mb_point;
    lp   =  CLine;
    while (n-- > 0){
      if (CLine == Line.next){
	lp   = CLine;
	opt  = 0;
      } else {
	lp   = CLine->prev;
	opt  = lp->idx;
      }
      if (mb_isbol())
	break;
      if (mb_iseol()){
	mb2_backwc(1);
	if (mb_isbol()){
	  mb2_forwc(1);
	  break;
	}
      }
      chsize2(&_mb_buf[_mb_point-1], &v1, &m1);
      for (i = m1-1; i >= 0; i--)
	s[i] = _mb_buf[--_mb_point];
      chsize(&_mb_buf[_mb_gapend], &v2, &m2);
      for (i = 0; i < m2; i++)
	_mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
      for (i = 0; i < m1; i++)
	_mb_buf[--_mb_gapend] = s[m1-i-1];

      if (mb_iseol())
	break;
      mb2_forwc(1);
    }
    if (mb_fixlines(ocol, lp, opt, &col, &pt) == 0)
      mb_refresh(ocol, opt);
    else 
      mb_redisplay();
  } else if (n < 0){
    while (n++ < 0){
      if (mb_isbol())
	break;
      if (mb_iseol()){
	mb2_backwc(1);
	if (mb_isbol()){
	  mb2_forwc(1);
	  break;
	}
      }
      chsize2(&_mb_buf[_mb_point-1], &v1, &m1);
      for (i = m1-1; i >= 0; i--)
	s[i] = _mb_buf[--_mb_point];
      chsize(&_mb_buf[_mb_gapend], &v2, &m2);
      for (i = 0; i < m2; i++)
	_mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
      for (i = 0; i < m1; i++)
	_mb_buf[--_mb_gapend] = s[m1-i-1];
      
      if (mb_isbol())
	break;
      mb2_backwc(1);
    }
    if (mb_fixlines(0, Line.next, 0, &col, &pt) == 0)
      mb_refresh(col, pt);
    else 
      mb_redisplay();
  }
  return mb_point();
}
static int
mb_matchparen(n, opar, cpar)
  int  n;
  char opar, cpar;
{
  int  i, dep, dep2, instr, oinstr, point, v, m;
  int  col, ocol, pt, opt, red, match, on;
  struct _Line  *clp, *dlp, *lp;

  if (n <= 0)
    return mb_point();

  instr = 0;
  for (i = _mb_prompt; i < _mb_point; i++){
    if (_mb_buf[i] == '"'){
      if (instr == 0)
	instr = 1;
      else 
	instr = 0;
    } else if (_mb_buf[i] == '\\')
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
  while (n-- > 0){
    clp = CLine;
    point = _mb_point;
    mb_putchar(cpar);
    mb_fixlines(ocol, lp, opt, &col, &pt);
    if (clp != CLine){
      red = 1;
      col = _mb_ccol-1;
    }
    clp = CLine;
    if (red == 0){
      mb_refresh(col, pt);
      dlp = clp;
    }
    dep  = 0;
    dep2 = 0;
    while (point >= _mb_prompt){
      if (instr == 0){
	if (_mb_buf[point] == '"'){
	  if ((point > _mb_prompt) && (_mb_buf[point-1] == '\\'))
	    point--;
	  else {
	    instr = 1;
	    dep2 = dep;
	  }
	} else if ((_mb_buf[point] == cpar) && (oinstr == 0)){
	  dep++;
	} else if ((_mb_buf[point] == opar) && (oinstr == 0)){
	  if (--dep == 0){
	    match++;
	    if (dlp != clp){
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
	if (_mb_buf[point] == '"'){
	  if ((point > _mb_prompt) && (_mb_buf[point-1] == '\\'))
	    point--;
	  else {
	    instr = 0;
	    dep = dep2;
	  }
	} else if ((_mb_buf[point] == cpar) && (oinstr == 1)){
	  dep++;
	} else if ((_mb_buf[point] == opar) && (oinstr == 1)){
	  if (--dep == 0){
	    match++;
	    if (dlp != clp){
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
      chsize2(&_mb_buf[point], &v, &m);
      if (point == clp->idx)
	clp = clp->prev; 
      point -= m;
    }
  }
  if (red == 1){
    mb_fixlines(ocol, lp, opt, NULL, NULL);
    mb_redisplay();
  } else 
    ttmove(_mb_crow, _mb_ccol);
  if (match != on)
    ttbeep();
  return mb_point();
}
static int 
mb_col(lp, pt)
  struct _Line *lp;
  int  pt;
{
  int  col, point, v, m;

  col = 0;
  for (point = lp->idx; point < pt; ){
    chsize(&_mb_buf[point], &v, &m);
    col   += v;
    point += m;
  }
  return col;
}
static int
mb_putchar(c)
  char  c;
{
  int  i, more;
  char *new_mb_buf;
  struct _Line *lp;

  if (_mb_point == (_mb_gapend-1)){
    more = _mb_size;
    if ((new_mb_buf = malloc(_mb_size+more)) == NULL)
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
  if (m > 0){
    while (m > 0){
      _mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
      --m;
    }
  } else {
    while (m < 0){
      _mb_buf[--_mb_gapend] = _mb_buf[--_mb_point];
      ++m;
    }
  }
}
static VOID
mb_movech(n)
  int n;
{
  int  v, m;

  if (n > 0){
    while (n > 0){
      chsize(&_mb_buf[_mb_point-1], &v, &m);
      while ((m--) > 0) 
	_mb_buf[_mb_point++] = _mb_buf[_mb_gapend++];
      --n;
    }
  } else {
    while (n < 0){
      chsize2(&_mb_buf[_mb_point-1], &v, &m);
      while ((m--) > 0) 
	_mb_buf[--_mb_gapend] = _mb_buf[--_mb_point];
      ++n;
    }
  }
}
static int
mb_fixlines(col, line, point, colp, ptp)
  int  col, point, *colp, *ptp;
  struct _Line *line;
{
  int    v, m, bp, ccol, opt, redraw, lno;
  struct _Line *lp0, *lp1;

  if (colp != NULL) *colp = col;
  if (ptp != NULL)  *ptp  = point; 
  ccol   = col;
  redraw = 0;
  bp     = 1;
  lno    = line->lno;
  for (;;){
    if (point == _mb_point){
      _mb_ccol = col;
      CLine    = line;
      point    = _mb_gapend;
      bp = 0;
    }
    if (point == _mb_size){
      /* ASSRT: line != &Line */
      goto End;
    }
    
    opt   = point;
    chsize(&_mb_buf[point], &v, &m);
    col   += v;
    ccol  += v;
    point += m;
    
    if (col >= (ncol-1)-1){
      if (line->next == &Line){
	if ((lp0 = (struct _Line*) malloc(sizeof (struct _Line))) == NULL)
	  return -1;
	line->next = lp0;
	lp0->next  = &Line;
	Line.prev  = lp0;
	lp0->prev  = line;
      } 
      line      = line->next;
      line->lno = ++lno;
      line->idx = opt;
      if (bp == 1){
	if (colp != NULL) *colp = 0;
	if (ptp != NULL)  *ptp  = line->idx;
	redraw = 1;
      }
      col  = v;
    }
  }

End:
  for (lp0 = line->next; lp0 != &Line; lp0 = lp1){
    lp1 = lp0->next;
    free(lp0);
  }
  line->next = &Line;
  Line.prev  = line;
  if ((_mb_ccol == (ncol-1)-2) && (_mb_gapend < _mb_size)){
    CLine = CLine->next;
    _mb_ccol = 0;
    if (colp != NULL) *colp = 0;
    if (ptp != NULL)  *ptp  = CLine->idx;
    redraw = 1;
  } else
  if ((_mb_ccol == 0) && (_mb_gapend == _mb_size)){
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
  int   col, idx;
{
  int  v, m, limit;

  limit = (ncol-1)-2;
  ttmove(_mb_crow, col);
  for ( ; idx < _mb_point; ){
    if (col >= limit)
      break;
    chsize(&_mb_buf[idx], &v, &m);
    while (m-- > 0)
      eputc(_mb_buf[idx++]);
    col += v;
  }
  for (idx = _mb_gapend; idx < _mb_size; ){
    if (col >= limit)
      break;
    chsize(&_mb_buf[idx], &v, &m);
    while (m-- > 0)
      eputc(_mb_buf[idx++]);
    col += v;
  }
  if (CLine->next != &Line){
    for ( ; col <= (ncol-1); col++)
      eputc('\\');
  } else 
    tteeol();
  ttmove(_mb_crow, _mb_ccol);
  mb_flush();
}
static VOID
mb_flush()
{
  ttflush();
}
#ifdef CANNA
static int
mb_henkan( c )
int c;
{
    int ilen;
    int i;
    static char kakutei[CANBUF];
    static int oldlength, oldrevPos;
    int  col, pt;
    struct _Line *lp;
    lp = CLine;

    ilen = jrKanjiString(0, c, kakutei, CANBUF, &ks);

    if (oldrevPos == 0)
	oldrevPos = mb_point();
    if (oldlength == 0)
	oldlength = mb_point();    

    if (ilen < 0)
	return FALSE;
    if (ks.info & KanjiModeInfo){
	jrKanjiControl(0,KC_QUERYMODE, mbMode);
    }
	
    if ( ilen > 0) {
	mb2_gotochar(oldlength);
	while (mb_point() > oldrevPos)
	    mb2_erasec(1);
	mb_fixlines(0, Line.next, 0, &col, &pt);
	if (lp == CLine)
	    mb_refresh(_mb_ccol, _mb_point); 
	else 
	    mb_redisplay();
	mb_insertstr(kakutei);
	oldlength = oldrevPos = 0;
    }
    if ( ks.length > 0) {
	mb2_gotochar(oldlength);
	while (mb_point() > oldrevPos)
	    mb2_erasec(1);
	mb_fixlines(0, Line.next, 0, &col, &pt);
	if (lp == CLine)
	    mb_refresh(_mb_ccol, _mb_point); 
	else 
	    mb_redisplay();
	oldrevPos = mb_point();
	mb_insert( 1, '|');
	mb_insertstr(ks.echoStr);
	mb_insert( 1, '|');
	oldlength = mb_point();
	mb_backwc(1);
    }
    else if ( ks.length == 0 && ilen == 0) {
	mb2_gotochar(oldlength);
	while (mb_point() > oldrevPos)
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

static char*
sformat(fp, ap)
  register char *fp;
  register va_list *ap;
{
  register char *s;
  register int  n;
  int   c, idx;
  char	kname[NKNAME];
  char	*keyname();
  char	*cp;

  n = ncol + 1;
  if ((s = malloc(n)) == NULL)
    return NULL;
  idx = 0;
  while ((c = *fp++) != '\0'){
    if (c != '%')
      idx = s_put_c(s, idx, n, c);
    else {
      c = *fp++;
      switch (c){
      case 'c':
	(VOID) keyname(kname, va_arg(*ap, int));
	idx = s_put_s(s, idx, n, kname);
	break;
      case 'k':
	cp = kname;
	for(c=0; c < key.k_count; c++) {
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
	idx = s_put_s(s, idx, n, va_arg(*ap, char *));
	break;
      case 'l':/* explicit longword */
	c = *fp++;
	switch(c) {
	case 'd':
	  idx = s_put_l(s, idx, n, (long)va_arg(*ap, long), 10);
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
  register char *p;
  register int idx, n;
  register int i;
  register int r;
{
  register int	q;
  
  if(i<0) {
    idx = s_put_c(p, idx, n, '-');
    i = -i;
  }
  if ((q=i/r) != 0)
    idx = s_put_i(p, idx, n, q, r);
  return s_put_c(p, idx, n, i%r+'0');
}

static int
s_put_l(p, idx, n, l, r)
  register char *p;
  register int  idx, n;
  register long l;
  register int  r;
{
  register long	q;
  
  if(l < 0) {
    idx = s_put_c(p, idx, n, '-');
    l = -l;
  }
  if ((q=l/r) != 0)
    idx = s_put_l(p, idx, n, q, r);
  return s_put_c(p, idx, n, (int)(l%r)+'0');
}

static int
s_put_s(p, idx, n,  s)
  register char *p, *s;
  register int idx, n;
{
  register int	c;

  while ((c = *s++) != '\0')
    idx = s_put_c(p, idx, n, c);
  return idx;
}

static int
s_put_c(p, idx, n, c)
  register char *p;
  register int  idx, n;
  register char c;
{
  epresf	= TRUE;

  if (idx < n) {
    if (ISCTRL(c)) {
      idx = s_put_c(p, idx, n, '^');
      c = CCHR(c);
    }
#ifdef	KANJI
#ifdef HANKANA
    {
      static int c1=0;
      
      if (ISKANJI(c)) {
	if (c1==0) c1=1;
	else c1=0;
      } else c1=0;
      if (ISHANKANA(c) && (c1 == 1))
	idx--;
    }
#endif  /* HANKANA */
    p[idx++] = c;
#else	/* NOT KANJI */
    p[idx++] = c;
#endif	/* KANJI */
  }
  return idx;
}

static VOID
chsize(s, visu, mem)
  register char *s;
  register int  *visu, *mem;
{
  if (ISCTRL(*s)){
    *visu = 2;
    *mem  = 1;
#ifdef	KANJI
  } else if (ISKANJI(*s)){
    *mem  = 2;
#ifdef	HOJO_KANJI
    if (ISHOJO(*s)) {
      *mem = 3;
      *visu = 2;
    } else
#endif	/* HOJO_KANJI */
#ifdef	HANKANA
    if (ISHANKANA(*s))
      *visu = 1;
    else 
#endif  /* HANKANA */
    *visu = 2;
#endif	/* KANJI */
  } else {
    *visu = 1;
    *mem  = 1;
  }
}

static VOID
chsize2(s, visu, mem)
  register char *s;
  register int  *visu, *mem;
{
  if (ISCTRL(*s)){
    *visu = 2;
    *mem  = 1;
#ifdef	KANJI
  } else if (ISKANJI(*s)){
    *mem  = 2;
#ifdef	HOJO_KANJI
    if (ISHOJO(*(s-2)) && ISKANJI(*(s-1))) {
      *mem = 3;
      *visu = 2;
    } else
#endif	/* HOJO_KANJI */
#ifdef	HANKANA
    if (ISHANKANA(*(s-1)))
      *visu = 1;
    else 
#endif  /* HANKANA */
    *visu = 2;
#endif	/* KANJI */
  } else {
    *visu = 1;
    *mem  = 1;
  }
}

static VOID
mb_hist_save(hist_buf, buf)
char *hist_buf[];
char *buf;
{
    int i;
    if (hist_buf[MB_NHISTS] != NULL)
	free(hist_buf[MB_NHISTS]);
    /* rotate history */
    for (i = MB_NHISTS; i > 1; i--)
	hist_buf[i] = hist_buf[i-1];
    /* and insert new history to head */
    hist_buf[1] = malloc(strlen(buf)+1);
    strcpy(hist_buf[1], buf);
}
#else   /* NOT MINIBUF_EDIT */
static int veread_del_char ();
static int veread_complete ();

static int
veread(fp, buf, nbuf, flag, ap)
    char    *fp;
    char    *buf;
    int    nbuf;
    int    flag;
    va_list *ap;
{
    int    cpos;
    int    c;

#ifdef	ADDFUNC	/* 91.01.16  by S.Yoshida */
    /* If an extra argument exists, use it.			*/
    /* This is a very easy way of not getting an argument from	*/
    /* the keyboard.	*/
    if(earg_exist)
      {
	strcpy(buf, earg_text);
	earg_exist = FALSE;
        edef_text = NULL;
	return (TRUE);
      }
#endif	/* ADDFUNC */
#ifndef NO_MACRO
    if(inmacro)
      {
        bcopy (maclcur->l_text, buf, maclcur->l_used);
	buf[maclcur->l_used] = '\0';
	maclcur = maclcur->l_fp;
#ifdef EXTD_DIR
        edef_text = NULL;
#endif
	return (TRUE);
      }
#endif
    if ((flag & EFNEW) || ttrow != nrow - 1)
      {
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
    } else 
#endif
    {
      buf[0] = '\0';
      cpos = 0;
    }
    for (;;)
      {
	ttflush();
        c = getkey(FALSE);
	tteeol();
        switch (c)
          {
          case ' ':
          case CCHR('I'):
            if (flag & EFAUTO)
	      {
#if 0    /* 90.12.10    Sawayanagi Yosirou */
		if (flag & EFFILE)
		  {
		    char    *p;

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
	    else if (cpos < nbuf - 1)
	      {
		buf[cpos++] = (char) c;
		buf[cpos] = '\0';
		eputc((char) c);
	      }
	    break;
	  case CCHR('J'):
	    c = CCHR('M');	/* and continue		*/
	  case CCHR('M'):	/* Return, done.	*/
	    if (flag & EFFUNC)
	      {
		int    matchnum;

		matchnum = complete (buf, flag);
		eputs (&buf[cpos]);
		tteeol();
		cpos = strlen (buf);
		if (matchnum != COMPLT_SOLE
			&& matchnum != COMPLT_NOT_UNIQUE)
		  {
		    /* complete() will be called again, but i don't mind it */
		    cpos = veread_complete (buf, cpos, (int)CCHR('I'), flag);
		    break;
		  }
	      }
	    complete_del_list ();
	    if (flag & EFCR)
	      {
		ttputc(CCHR('M'));
		ttflush ();
	      }
#ifndef NO_MACRO
	    if (macrodef)
	      {
		LINE *lp;

		if((lp = lalloc(cpos)) == NULL)
		  return FALSE;
		lp->l_fp = maclcur->l_fp;
		maclcur->l_fp = lp;
		lp->l_bp = maclcur;
		maclcur = lp;
		bcopy(buf, lp->l_text, cpos);
	      }
#endif
	    return ((buf[0] == '\0') ? FALSE : TRUE);
	  case CCHR('G'):		/* Bell, abort.		*/
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
				/* back up to first word character or beginning */
	    while ((cpos > 0) && !ISWORD(buf[cpos - 1]))
	      cpos = veread_del_char (buf, cpos);
	    while ((cpos > 0) && ISWORD(buf[cpos - 1]))
	      cpos = veread_del_char (buf, cpos);
	    break;
	  case CCHR('\\'):
	  case CCHR('Q'):	/* C-Q, quote next	*/
	    c = getkey(FALSE);	/* and continue		*/
          default:		/* All the rest.	*/
	    if (cpos < nbuf - 1)
	      {
		buf[cpos++] = (char) c;
		buf[cpos] = '\0';
		eputc((char) c);
	      }
	  }
      }
}

static int
veread_del_char (buf, cpos)
    char    *buf;
    int    cpos;
{
    if (cpos <= 0)
      return (0);
    ttputc('\b');
    ttputc(' ');
    ttputc('\b');
    --ttcol;
    --cpos;
    if (ISCTRL(buf[cpos]) != FALSE)
      {
	ttputc('\b');
	ttputc(' ');
	ttputc('\b');
	--ttcol;
      }
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    else if (ISKANJI(buf[cpos]))
      {
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
	if (!ISHANKANA(buf[--cpos])) {
	    ttputc('\b');
	    ttputc(' ');
	    ttputc('\b');
	    --ttcol;
	}
#else  /* not HANKANA */
	ttputc('\b');
	ttputc(' ');
	ttputc('\b');
	--ttcol;
	--cpos;
#endif  /* HANKANA */
      }
#endif	/* KANJI */
    buf[cpos] = '\0';
    return (cpos);
}

static int
veread_complete (buf, cpos, c, flag)
    char    *buf;
    int    cpos;
    int    c;
    int    flag;
{
    int    matchnum;
    int    i;
    int    wflag;
    int    cur_ttrow;
    int    cur_ttcol;

    wflag = (c == ' ');
    if (wflag)
      {
	buf[cpos] = ' ';
	buf[cpos + 1] = '\0';
	if (complete (buf, flag) == COMPLT_NO_MATCH)
	  buf[cpos] = '\0';
	else
	  {
	    buf[cpos + 1] = '\0';
	    eputs (&buf[cpos]);
	    tteeol();
	    ++cpos;
	    return (cpos);
	  }
      }
    matchnum = complete (buf, flag);
    if (buf[cpos] == '\0')
      {
	if (wflag)
	  {
	    if (matchnum == COMPLT_AMBIGUOUS
	        || matchnum == COMPLT_NOT_UNIQUE)
	      complete_list_names (buf, flag);
	    else if (matchnum == COMPLT_NO_MATCH)
	      {
		cur_ttrow = ttrow;
		cur_ttcol = ttcol;
		eputs (complete_message (matchnum));
		tteeol();
		ttmove (cur_ttrow, cur_ttcol);
	      }
	  }
	else
	  {
	    if (matchnum == COMPLT_AMBIGUOUS)
	      complete_list_names (buf, flag);
	    else
	      {
		cur_ttrow = ttrow;
		cur_ttcol = ttcol;
		eputs (complete_message (matchnum));
		tteeol();
		ttmove (cur_ttrow, cur_ttcol);
	      }
	  }
      }
    else
      {
	if (wflag)
	  {
	    for (i = cpos + 1; buf[i] != '\0'; i++)
	      {
	        if (i > cpos && ! ISWORD(buf[i - 1]))
	          {
		    buf[i] = '\0';
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
#else	/* NOT NEW_COMPLETE */
static veread(fp, buf, nbuf, flag, ap) char *fp; char *buf; va_list *ap; {
	register int	cpos;
	register int	i;
	register int	c;

#ifdef	ADDFUNC	/* 91.01.16  by S.Yoshida */
    /* If extra argument is exist, use it.			*/
    /* This is very easy way to not get argument from keyboard.	*/
    if(earg_exist)
      {
	strcpy(buf, earg_text);
	earg_exist = FALSE;
	return (TRUE);
      }
#endif	/* ADDFUNC */
#ifndef NO_MACRO
	if(inmacro) {
	    bcopy(maclcur->l_text, buf, maclcur->l_used);
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
	} else
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
			c = CCHR('M');		/* and continue		*/
		    case CCHR('M'):		/* Return, done.	*/
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
			if(macrodef) {
			    LINE *lp;

			    if((lp = lalloc(cpos)) == NULL) return FALSE;
			    lp->l_fp = maclcur->l_fp;
			    maclcur->l_fp = lp;
			    lp->l_bp = maclcur;
			    maclcur = lp;
			    bcopy(buf, lp->l_text, cpos);
			}
#endif
			goto done;

		    case CCHR('G'):		/* Bell, abort.		*/
		    case CCHR('['):
			eputc(CCHR('G'));
			(VOID) ctrlg(FFRAND, 0);
			ttflush();
			return ABORT;

		    case CCHR('H'):
		    case CCHR('?'):		/* Rubout, erase.	*/
			if (cpos != 0) {
				ttputc('\b');
				ttputc(' ');
				ttputc('\b');
				--ttcol;
				if (ISCTRL(buf[--cpos]) != FALSE) {
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
				}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
				else if (ISKANJI(buf[cpos])) {
#ifdef HANKANA  /* 92.11.21  by S.Sasaki  */
					if (!ISHANKANA(buf[--cpos])) {
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
					}

#else  /* HANKANA */
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
					--cpos;
#endif  /* HANKANA */
				}
#endif	/* KANJI */
				ttflush();
			}
			break;

		    case CCHR('X'):		/* C-X			*/
		    case CCHR('U'):		/* C-U, kill line.	*/
			while (cpos != 0) {
				ttputc('\b');
				ttputc(' ');
				ttputc('\b');
				--ttcol;
				if (ISCTRL(buf[--cpos]) != FALSE) {
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
				}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
				else if (ISKANJI(buf[cpos])) {
#ifdef HANKANA  /* 92.11.21  by S.Sasaki  */
					if (!ISHANKANA(buf[--cpos])) {
					    ttputc('\b');
					    ttputc(' ');
					    ttputc('\b');
					    --ttcol;
					}
#else  /* HANKANA */
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
					--cpos;
#endif  /* HANKANA */
				}
#endif	/* KANJI */
			}
			ttflush();
			break;

		    case CCHR('W'):		/* C-W, kill to beginning of */
						/* previous word	*/
			/* back up to first word character or beginning */
			while ((cpos > 0) && !ISWORD(buf[cpos - 1])) {
				ttputc('\b');
				ttputc(' ');
				ttputc('\b');
				--ttcol;
				if (ISCTRL(buf[--cpos]) != FALSE) {
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
				}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
				else if (ISKANJI(buf[cpos])) {
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
					--cpos;
				}
#endif	/* KANJI */
			}
			while ((cpos > 0) && ISWORD(buf[cpos - 1])) {
				ttputc('\b');
				ttputc(' ');
				ttputc('\b');
				--ttcol;
				if (ISCTRL(buf[--cpos]) != FALSE) {
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
				}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
				else if (ISKANJI(buf[cpos])) {
#ifdef HANKANA  /* 92.11.21  by S.Sasaki  */
					if (ISHANKANA(buf[--cpos])) {
					    ttputc('\b');
					    ttputc(' ');
					    ttputc('\b');
					    --ttcol;
					}
#else  /* not HANKANA */
					ttputc('\b');
					ttputc(' ');
					ttputc('\b');
					--ttcol;
					--cpos;
#endif  /* HANKANA */
				}
#endif	/* KANJI */
			}
			ttflush();
			break;

		    case CCHR('\\'):
		    case CCHR('Q'):		/* C-Q, quote next	*/
			c = getkey(FALSE);	/* and continue		*/
		    default:			/* All the rest.	*/
			if (cpos < nbuf-1) {
				buf[cpos++] = (char) c;
				eputc((char) c);
				ttflush();
			}
		}
	}
done:	return buf[0] != '\0';
}

/*
 * do completion on a list of objects.
 */
static int complt(flags, c, buf, cpos)
register char *buf;
register int cpos;
{
	register LIST	*lh, *lh2;
	int		i, nxtra;
	int		nhits, bxtra;
	int		wflag = FALSE;
	int		msglen, nshown;
	char		*msg;

	if ((flags&EFFUNC) != 0) {
	    buf[cpos] = '\0';
	    i = complete_function(buf, c);
	    if(i>0) {
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
	} else if ((flags&EFFILE)!=0) {
	    if (c != '\t' && c != ' ') panic("broken complt call: c");
	    buf[cpos] = '\0';
	    i = complete_filename(buf, cpos, c);
	    if(i>0) {
		eputs(&buf[cpos]);
		ttflush();
		return i;
	    }
	    switch(i) {
		case -3:
		    msg = " [Ambiguous]";
		    break;
		case -2:	/* no match */
#ifdef	HUMAN68K	/* 90.11.15    Sawayanagi Yosirou */
		    msg = " [No match]";
		    break;
#else	/* NOT HUMAN68K */
		    i=0;
#endif	/* HUMAN68K */
		case -1:	/* single match */
		case 0:
		    return i;
		default:
		    msg = " [Internal error]";
		    break;
	    }
#endif	/* NO_FILECOMP */
	} else {
	    if ((flags&EFBUF) != 0) lh = &(bheadp->b_list);
	    else panic("broken complt call: flags");

	    if (c == ' ') wflag = TRUE;
	    else if (c != '\t' && c != CCHR('M')) panic("broken complt call: c");

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
			if (lh->l_name[i] == '\0') nxtra = -1;
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
		if (nxtra < 0 && nhits > 1 && c == ' ') nxtra = 1;
		for (i = 0; i < nxtra; ++i) {
			buf[cpos] = lh2->l_name[cpos];
			eputc(buf[cpos++]);
		}
		ttflush();
		if (nxtra < 0 && c != CCHR('M')) return 0;
		return nxtra;
	    }
	}
	/* Set up backspaces, etc., being mindful of echo line limit */
	msglen = strlen(msg);
	nshown = (ttcol + msglen + 2 > ncol) ?
			ncol - ttcol - 2 : msglen;
	eputs(msg);
	ttcol -= (i = nshown);		/* update ttcol!		*/
	while (i--)			/* move back before msg		*/
		ttputc('\b');
	ttflush();			/* display to user		*/
	i = nshown;
	while (i--)			/* blank out	on next flush	*/
		eputc(' ');
	ttcol -= (i = nshown);		/* update ttcol on BS's		*/
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
getxtra(lp1, lp2, cpos, wflag) register LIST *lp1, *lp2; register int wflag; {
	register int	i;

	i = cpos;
	for (;;) {
		if (lp1->l_name[i] != lp2->l_name[i]) break;
		if (lp1->l_name[i] == '\0') break;
		++i;
		if (wflag && !ISWORD(lp1->l_name[i-1])) break;
	}
	return (i - cpos);
}
#endif	/* NEW_COMPLETE */

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
ewprintf(char *fp, ...)
{
    va_list	pvar;

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
#else	/* SUPPORT_ANSI */
VOID
ewprintf(va_alist)
va_dcl
{
	va_list pvar;
	register char *fp;

#ifndef NO_MACRO
	if(inmacro) return;
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
#endif	/* SUPPORT_ANSI */

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
register char *fp;
register va_list *ap;
{
	register int c;
	char	kname[NKNAME];
	char	*keyname();
	char	*cp;

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
				for(c=0; c < key.k_count; c++) {
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
	register int	q;

	if(i<0) {
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
	register long	q;

	if(l < 0) {
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
register char *s;
{
	register int	c;

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
register char c;
{
    epresf	= TRUE;
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
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#ifdef	SS_SUPPORT /* 92.11.21  by S.Sasaki */
		{
		    static int c1=0;

		    if (ISKANJI(c)) {
#ifdef	HOJO_KANJI
			if (ISHOJO(c)) {
			    c1 = 2;
			    ttcol--;
			} else
#endif
			if (c1==0) c1=1;
			else c1--;
		    } else c1=0;
#ifdef	HANKANA
		    if (ISHANKANA(c) && c1 == 1)
		        ttcol--;
#endif
		}
#endif  /* SS_SUPPORT */
		kttputc(c);
#else	/* NOT KANJI */
		ttputc(c);
#endif	/* KANJI */
		++ttcol;
	}
}

#ifndef	NEW_COMPLETE	/* 90.12.10    Sawayanagi Yosirou */
#ifndef NO_FILECOMP	/* 90.04.04  by K.Maeda */
complete_filename(buf, cpos, c)
char *buf;
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

	if (c == ' ') wflag = TRUE;
		else  wflag = FALSE;
	nxtra = HUGE; hit = files;

	for(try=files, i=0; i<nhits; i++) {
		if (try[cpos] == '\0') nxtra = -1;
		else {
			bxtra = cpos;
			for(;;) {
				if (try[bxtra] != hit[bxtra]) break;
				if (try[bxtra] == '\0') break;
				bxtra++;
				if (wflag && !ISWORD(try[bxtra-1])) break;
			}
			if (hit[bxtra] == '\0')
				hit = try;
			bxtra -= cpos;
			if (bxtra < nxtra) nxtra = bxtra;
		}
		try += strlen(try)+1;
	}
	if (nhits > 1 && nxtra == 0) {
		res = -3;	/* ambiguous */
		goto end;
	}
	if (nxtra < 0 && nhits > 1 && c == ' ') nxtra = 1;
	if (nxtra >= 0) {
		for (i=cpos; i<cpos+nxtra; i++)
			buf[i] = hit[i];
		buf[i] = '\0';
		res = nxtra;
	} else { /* sole completion */
		for (i=cpos, j=strlen(files); i<j;) {
			buf[i] = files[i];
			i++;
			if (wflag && !ISWORD(buf[i-1]))
				break;
		}
		buf[i] = '\0';
		res = i-cpos;
	}
end:
	free(files);
	return res;
}
#endif	/* NO_FILECOMP */
#endif	/* NEW_COMPLETE */
