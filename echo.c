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

/* $Id: echo.c,v 1.1 2000/03/10 21:32:52 amura Exp $ */

/* $Log: echo.c,v $
/* Revision 1.1  2000/03/10 21:32:52  amura
/* Initial revision
/*
 */

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
#else
VOID		ewprintf();
#endif
static VOID	eformat();
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
#endif	/* ADDFUNC */

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
		s = ereply("Please answer yes or no.  %s? (yes or no) ",
			   buf, sizeof(buf), sp);
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
#ifdef ADDFUNC
	edef_text = (char *)0;
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
#ifdef ADDFUNC
	edef_text = (char *)0;
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

#ifdef ADDFUNC
    if (edef_text) {
      strcpy(buf, edef_text);
      edef_text = (char *)0;
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
	if ( (buf[--cpos] & 0xff) != 0x8e ) {
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
					if ( (buf[--cpos] & 0xff) != 0x8e ) {
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
					if ( (buf[--cpos] & 0xff) != 0x8e ) {
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
					if ( (buf[--cpos] & 0xff) == 0x8e ) {
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
	if (ttcol+2 < ncol) {
		if (ISCTRL(c)) {
			eputc('^');
			c = CCHR(c);
		}
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
		{
		    static int c1=0;

		    if (ISKANJI(c)) {
			if (c1==0) c1=1;
			else c1=0;
		    } else c1=0;
		}
#endif  /* HANKANA */
		ttcol += kttputc(c);
#else	/* NOT KANJI */
		ttputc(c);
		++ttcol;
#endif	/* KANJI */
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
