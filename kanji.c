/* $Id: kanji.c,v 1.19.2.1 2006/01/11 14:47:34 amura Exp $ */
/*
 *		Kanji handling routines.
 *		These are only used when KANJI is #defined.
 *
 *		Coded by Shigeki Yoshida (shige@csk.CO.JP)
 */
/* 90.01.29	Created by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */

#ifdef KANJI	/* 90.01.29  by S.Yoshida */
#include "def.h"
#include "kinit.h"
#include "kanji.h"

int global_kfio 	= KFIO;		/* default-kanji-fileio-code	*/
int global_kexpect	= KEXPECT;	/* kanji-expected-code		*/
int global_kinput 	= KINPUT;	/* kanji-input-code		*/
int global_kdisplay	= KDISPLAY;	/* kanji-display-code		*/
static int to_k_fio	= TO_KFIO;	/* to-kanji-fileio		*/
static int to_a_fio	= TO_AFIO;	/* to-ascii-fileio		*/
static int to_k_display	= TO_KDISPLAY;	/* to-kanji-display		*/
static int to_a_display	= TO_ADISPLAY;	/* to-ascii-display		*/
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
static int to_kana_fio	= TO_KANAFIO;	   /* to-kana-fileio		*/
static int to_kana_display = TO_KANADISPLAY;  /* to-kana-display	*/
#endif /* HANKANA */
static int local_kfin;			/* Buffer local file input code. */

#ifdef USE_UNICODE
static char symbol_c[] = {'N', 'S', 'J', 'E', 'U', 'W', '-', 'T'};
static char *symbol_s[] = {"No-conversion", "Shift-JIS", "JIS", "EUC",
			   "UTF-8", "UCS-2", "NIL", "T"};
					/* Symbol chars & strings to	*/
					/* display a KANJI code info to	*/
					/* the user.  This order depend	*/
					/* on the values of KANJI code	*/
					/* macro defined at def.h.	*/

static char *kcodename_u[] = {"NOCONV", "SHIFT-JIS", "JIS", "EUC",
			      "UTF-8", "UCS-2", "NIL", "T"};
static char *kcodename_l[] = {"noconv", "shift-jis", "jis", "euc",
			      "utf-8", "ucs-2", "nil", "t"};
#define	NKCODENAME	8
					/* The strings of KANJI code	*/
					/* name and the numner of KANJI	*/
					/* code to determine what KANJI	*/
					/* code the user want to use.	*/
					/* This order depend on	the	*/
					/* values of KANJI code macro	*/
					/* defined at def.h.		*/
#else /* not USE_UNICODE */
static char symbol_c[] = {'N', 'S', 'J', 'E', '-', 'T'};
static char *symbol_s[] = {"No-conversion", "Shift-JIS", "JIS", "EUC", "NIL", "T"};
					/* Symbol chars & strings to	*/
					/* display a KANJI code info to	*/
					/* the user.  This order depend	*/
					/* on the values of KANJI code	*/
					/* macro defined at def.h.	*/

static char *kcodename_u[] = {"NOCONV", "SHIFT-JIS", "JIS", "EUC", "NIL", "T"};
static char *kcodename_l[] = {"noconv", "shift-jis", "jis", "euc", "nil", "t"};
#define	NKCODENAME	6
					/* The strings of KANJI code	*/
					/* name and the numner of KANJI	*/
					/* code to determine what KANJI	*/
					/* code the user want to use.	*/
					/* This order depend on	the	*/
					/* values of KANJI code macro	*/
					/* defined at def.h.		*/
#endif /* USE_UNICODE */

#define	ESC	CCHR('[')		/* Escape char.			*/

/* 90.07.25  Change to inline routine  by S.Yoshida (to) */

#define	CAT_NONE	0		/* Category: No category.	*/
#define	CAT_ASCII	1		/* Category: ASCII code char.	*/
#define	CAT_KIGOU	2		/* Category: JIS KIGOU char.	*/
#define	CAT_EISUUJI	3		/* Category: JIS EI-SUUJI char.	*/
#define	CAT_HIRAGANA	4		/* Category: HIRAGANA char.	*/
#define	CAT_KATAKANA	5		/* Category: KATAKANA char.	*/
#define	CAT_GREEK	6		/* Category: JIS GREEK char.	*/
#define	CAT_RUSSIAN	7		/* Category: JIS RUSSIAN char.	*/
#define	CAT_KANJI	8		/* Category: KANJI char.	*/

/* prototype for static routines */
static int kcodenumber _PRO((int *code, int num));
static int kanalastchar _PRO((int *lastch));
static int klastchar _PRO((int *lastch, int kselect));
static int kcodecheck _PRO((char *p, int len));
static int bufjtoe_c _PRO((char *j, int len));
int bufstoe_c _PRO((char *p, int len));

#ifdef USE_UNICODE
extern int bufu2toe _PRO((char *p, int len, int buflen));
extern int bufu8toe _PRO((char *p, int len, int buflen));
extern int bufetou2 _PRO((char *p, int len, int buflen));
extern int bufetou8 _PRO((char *p, int len, int buflen));
static int bufu2toe_c _PRO((char *p, int len));
static int bufu8toe_c _PRO((char *p, int len));
#endif /* USE_UNICODE */


/*
 * COMMAND: change-default-fileio-code
 * This is a command processor for changing the value of the global
 * file I/O KANJI code.  When this function is called, the value is
 * changed rotaly. (NIL -> NOCONV -> SJIS -> JIS -> EUC)
 */
/*ARGSUSED*/
int
k_rot_fio(f, n)
int f, n;
{
    switch (global_kfio) {
    case NIL:
	global_kfio = NOCONV;
	break;
    case NOCONV:
	global_kfio = SJIS;
	break;
    case SJIS:
	global_kfio = JIS;
	break;
    case JIS:
	global_kfio = EUC;
	break;
    case EUC:
#ifdef	USE_UNICODE
	global_kfio = UTF8;
	break;
    case UTF8:
	global_kfio = UCS2;
	break;
    case UCS2:
#endif
	global_kfio = NIL;
	break;
    }
    return TRUE;
}

/*
 * COMMAND: set-default-fileio-code
 * This is a command processor for setting the value of the global
 * file I/O KANJI code.
 */
/*ARGSUSED*/
int
k_set_fio(f, n)
int f, n;
{
    register int s;
    
    if (f & FFARG) {
	if (n >= 0 && n < NKCODENAME - 1) {
	    global_kfio = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = kcodenumber(&n, NKCODENAME - 1)) == TRUE) {
	global_kfio = n;
    }
    return (s);
}

/*
 * COMMAND: change-fileio-code
 * This is a command processor for changing the value of the buffer
 * local file I/O KANJI code.  When this function is called, the value
 * is changed rotaly. (NIL -> NOCONV -> SJIS -> JIS -> EUC)
 */
/*ARGSUSED*/
int
k_rot_buffio(f, n)
int f, n;
{
    switch (curbp->b_kfio) {
    case NIL:
	curbp->b_kfio = NOCONV;
	break;
    case NOCONV:
	curbp->b_kfio = SJIS;
	break;
    case SJIS:
	curbp->b_kfio = JIS;
	break;
    case JIS:
	curbp->b_kfio = EUC;
	break;
    case EUC:
#ifdef	USE_UNICODE
	curbp->b_kfio = UTF8;
	break;
    case UTF8:
	curbp->b_kfio = UCS2;
	break;
    case UCS2:
#endif
	curbp->b_kfio = NIL;
	break;
    }
    upmodes(curbp);			/* Only update cur-buf's modeline. */
    return TRUE;
}

/*
 * COMMAND: set-kanji-fileio-code
 * This is a command processor for setting the value of the buffer
 * local file I/O KANJI code.
 */
/*ARGSUSED*/
int
k_set_buffio(f, n)
int f, n;
{
    register int s;
    
    if (f & FFARG) {
	if (n > 0 && n < NKCODENAME - 1) {
	    curbp->b_kfio = n;
	    upmodes(curbp);	/* Only update cur-buf's modeline. */
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = kcodenumber(&n, NKCODENAME - 1)) == TRUE) {
	curbp->b_kfio = n;
	upmodes(curbp);		/* Only update cur-buf's modeline. */
    }
    return (s);
}

/*
 * COMMAND: set-kanji-expected-code
 * This is a command processor for setting the value of the global
 * expected file input KANJI code.
 */
/*ARGSUSED*/
int
k_set_expect(f, n)
int f, n;
{
    register int s;
    
    if (f & FFARG) {
	if (n > 0 && n < NKCODENAME) {
	    global_kexpect = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = kcodenumber(&n, NKCODENAME)) == TRUE) {
	global_kexpect = n;
    }
    return (s);
}

/*
 * COMMAND: change-input-code
 * This is a command processor for changing the value of the global
 * keyboard input KANJI code.  When this function is called, the value
 * is changed rotaly. (NOCONV -> SJIS -> JIS -> EUC)
 */
/*ARGSUSED*/
int
k_rot_input(f, n)
int f, n;
{
    switch (global_kinput) {
    case NOCONV:
	global_kinput = SJIS;
	break;
    case SJIS:
	global_kinput = JIS;
	break;
    case JIS:
	global_kinput = EUC;
	break;
    case EUC:
#ifdef	USE_UNICODE
	global_kinput = UTF8;
	break;
    case UTF8:
	global_kinput = UCS2;
	break;
    case UCS2:
#endif
	global_kinput = NOCONV;
	break;
    }
    upmodes(NULL);			/* Update each win's modeline. */
    return TRUE;
}

/*
 * COMMAND: set-kanji-input-code
 * This is a command processor for setting the value of the global
 * keyboard input KANJI code.
 */
/*ARGSUSED*/
int
k_set_input(f, n)
int f, n;
{
    register int s;
    
    if (f & FFARG) {
	if (n > 0 && n < NKCODENAME - 1) {
	    global_kinput = n;
	    upmodes(NULL);	/* Update each win's modeline. */
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = kcodenumber(&n, NKCODENAME - 1)) == TRUE) {
	global_kinput = n;
	upmodes(NULL);		/* Update each win's modeline. */
    }
    return (s);
}

/*
 * COMMAND: change-display-code
 * This is a command processor for changing the value of the global
 * display KANJI code.  When this function is called, the value is
 * changed rotaly. (NOCONV -> SJIS -> JIS -> EUC)
 */
/*ARGSUSED*/
int
k_rot_display(f, n)
int f, n;
{
    switch (global_kdisplay) {
    case NOCONV:
	global_kdisplay = SJIS;
	break;
    case SJIS:
	global_kdisplay = JIS;
	break;
    case JIS:
	global_kdisplay = EUC;
	break;
    case EUC:
#ifdef	USE_UNICODE
	global_kdisplay = UTF8;
	break;
    case UTF8:
	global_kdisplay = UCS2;
	break;
    case UCS2:
#endif
	global_kdisplay = NOCONV;
	break;
    }
    sgarbf = TRUE;			/* Must update full screen. */
    return TRUE;
}

/*
 * COMMAND: set-kanji-display-code
 * This is a command processor for setting the value of the global
 * display KANJI code.
 */
/*ARGSUSED*/
int
k_set_display(f, n)
int f, n;
{
    register int s;

    if (f & FFARG) {
	if (n > 0 && n < NKCODENAME - 1) {
	    global_kdisplay = n;
	    sgarbf = TRUE;	/* Must update full screen. */
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = kcodenumber(&n, NKCODENAME - 1)) == TRUE) {
	global_kdisplay = n;
	sgarbf = TRUE;		/* Must update full screen. */
    }
    return (s);
}

/*
 * COMMAND: list-kanji-code
 * Display a list of kanji-code related valiables in the *Kanji Codes*
 * buffer.
 */
/*ARGSUSED*/
int
k_list_code(f, n)
int f, n;
{
    register BUFFER *bp;
    register WINDOW *wp;
    char line[80];

    if ((bp = bfind("*Kanji Codes*", TRUE)) == NULL)
	return FALSE;
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
    bp->b_flag &= ~(BFCHG | BFACHG);	/* Blow away old.	*/
#else
    bp->b_flag &= ~BFCHG;		/* Blow away old.	*/
#endif	/* AUTOSAVE	*/
    if (bclear(bp) != TRUE)
	return FALSE;
    
    strcpy(line, "* List of kanji-code related variables *");
    if (addline(bp, line) == FALSE)
	return FALSE;
    strcpy(line, "Global variables:");
    if (addline(bp, line) == FALSE)
	return FALSE;
    sprintf(line, "\tdefault-kanji-fileio-code : %s", symbol_s[global_kfio]);
    if (addline(bp, line) == FALSE)
	return FALSE;
    sprintf(line, "\tkanji-expected-code : %s", symbol_s[global_kexpect]);
    if (addline(bp, line) == FALSE)
	return FALSE;
    sprintf(line, "\tkanji-display-code : %s", symbol_s[global_kdisplay]);
    if (addline(bp, line) == FALSE)
	return FALSE;
    sprintf(line, "\tkanji-input-code : %s", symbol_s[global_kinput]);
    if (addline(bp, line) == FALSE)
	return FALSE;
    if (curbp->b_kfio == JIS || global_kfio == JIS || global_kdisplay == JIS) {
	sprintf(line, "\tto-kanji-fileio : %d (%c)", to_k_fio, to_k_fio);
	if (addline(bp, line) == FALSE)
	    return FALSE;
	sprintf(line, "\tto-ascii-fileio : %d (%c)", to_a_fio, to_a_fio);
	if (addline(bp, line) == FALSE)
	    return FALSE;
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	sprintf(line, "\tto-kana-fileio : %d (%c)", to_kana_fio, to_kana_fio);
	if (addline(bp, line) == FALSE)
	    return FALSE;
#endif  /* HANKANA */
	sprintf(line, "\tto-kanji-display : %d (%c)",
		to_k_display, to_k_display);
	if (addline(bp, line) == FALSE)
	    return FALSE;
	sprintf(line, "\tto-ascii-display : %d (%c)",
		to_a_display, to_a_display);
	if (addline(bp, line) == FALSE)
	    return FALSE;
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	sprintf(line, "\tto-kana-display : %d (%c)",
		to_kana_display, to_kana_display);
	if (addline(bp, line) == FALSE)
	    return FALSE;
#endif  /* HANKANA */
    }
    strcpy(line, "Buffer local variable:");
    if (addline(bp, line) == FALSE)
	return FALSE;
    sprintf(line, "\tkanji-fileio-code : %s", symbol_s[(int)curbp->b_kfio]);
    if (addline(bp, line) == FALSE)
	return FALSE;

    if ((wp = popbuf(bp)) == NULL)
	return FALSE;
    bp->b_dotp = lforw(bp->b_linep); /* put dot at beginning of buffer */
    bp->b_doto = 0;
    wp->w_dotp = bp->b_dotp;	/* fix up if window already on screen */
    wp->w_doto = bp->b_doto;
    return TRUE;
}

/*
 * COMMAND: list-kanji-code-briefly
 * Display a list of values of kanji-code related valiables in the
 * mini buffer.
 */
/*ARGSUSED*/
int
k_show_code(f, n)
int f, n;
{
    char buf[80];
    char *bp;

#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
    sprintf(buf, "global:[FID=%c%c%c] local:[F=%c] expected:[E=%c]",
#else  /* not HANKANA */
    sprintf(buf, "global: [FID=%c%c%c]  local: [F=%c]  expected: [E=%c]",
#endif  /* HANKANA */
	    symbol_c[global_kfio],
	    symbol_c[global_kinput],
	    symbol_c[global_kdisplay],
	    symbol_c[(int)curbp->b_kfio],
	    symbol_c[global_kexpect]);
    if (curbp->b_kfio == JIS ||
	global_kfio == JIS || global_kdisplay == JIS) {
	bp = buf + strlen(buf);
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	sprintf(bp, " K:[FD=%c%c] A:[FD=%c%c] KANA:[FD=%c%c]",
		to_k_fio, to_k_display, to_a_fio, to_a_display,
		to_kana_fio, to_kana_display);
#else  /* not HANKANA */
	sprintf(bp, "  Ksel: [FD=%c%c]  Asel: [FD=%c%c]",
		to_k_fio, to_k_display, to_a_fio, to_a_display);
#endif  /* HANKANA */
    }

    ewprintf(buf);
    return TRUE;
}

/*
 * COMMAND: set-to-kanji-fileio
 * This is a command processor for setting the value of the global
 * KANJI select char for a file.
 */
/*ARGSUSED*/
int
k_set_tokfio(f, n)
int f, n;
{
    int s;

    if (f & FFARG) {
	if (n == '@' || n == 'B') {
	    to_k_fio = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = klastchar(&n, TRUE)) == TRUE) {
	to_k_fio = n;
    }
    return (s);
}

/*
 * COMMAND: set-to-ascii-fileio
 * This is a command processor for setting the value of the global
 * ASCII select char for a file.
 */
/*ARGSUSED*/
int
k_set_toafio(f, n)
int f, n;
{
    register int s;

    if (f & FFARG) {
	if (n == 'B' || n == 'J' || n == 'H') {
	    to_a_fio = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = klastchar(&n, FALSE)) == TRUE) {
	to_a_fio = n;
    }
    return (s);
}

#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
/*
 * COMMAND: set-to-kana-fileio
 * This is a command processor for setting the value of the global
 * KANA select char for a file.
 */
/*ARGSUSED*/
int
k_set_tokanafio(f, n)
int f, n;
{
    register int s;
    
    if (f & FFARG) {
	if (n == '7' || n == '8' || n == 'I') {
	    to_kana_fio = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = kanalastchar(&n)) == TRUE) {
	to_kana_fio = n;
    }
    return (s);
}

/*
 * COMMAND: set-to-kana-display
 * This is a command processor for setting the value of the global
 * KANA select char for a display.
 */
/*ARGSUSED*/
int
k_set_tokanadisplay(f, n)
int f, n;
{
    int s;
    
    if (f & FFARG) {
	if (n == '7' || n == '8' || n == 'I') {
	    to_kana_fio = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = kanalastchar(&n)) == TRUE) {
	to_kana_display = n;
    }
    return (s);
}
#endif  /* HANKANA */

/*
 * COMMAND: set-to-kanji-display
 * This is a command processor for setting the value of the global
 * KANJI select char for a display.
 */
/*ARGSUSED*/
int
k_set_tokdisplay(f, n)
int f, n;
{
    int s;
    
    if (f & FFARG) {
	if (n == '@' || n == 'B') {
	    to_k_fio = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = klastchar(&n, TRUE)) == TRUE) {
	to_k_display = n;
    }
    return (s);
}

/*
 * COMMAND: set-to-ascii-display
 * This is a command processor for setting the value of the global
 * ASCII select char for a display.
 */
/*ARGSUSED*/
int
k_set_toadisplay(f, n)
int f, n;
{
    int s;
    
    if (f & FFARG) {
	if (n == 'B' || n == 'J' || n == 'H') {
	    to_a_fio = n;
	    s = TRUE;
	}
	else {
	    s = FALSE;
	}
    }
    else if ((s = klastchar(&n, FALSE)) == TRUE) {
	to_a_display = n;
    }
    return (s);
}

/*
 * Select target KANJI code and return its inner number.
 */
static int
kcodenumber(code, num)
int *code;
int num;
{
    register int s;
    register int i;
    register int n;
    char kcode[NINPUT];
    
    for (n = 0; n < 2; n++) {
	if ((s = ereply("Kanji Code System : ", kcode, sizeof(kcode)))
	    != TRUE) {
	    return (s);
	}
	if (ISDIGIT(kcode[0])) {
	    if ((i = atoi(kcode)) >= 0 && i < num) {
		*code = i;
		return TRUE;
	    }
	}
	else {
	    for (i = 0; i < num; i++) {
		if (strcmp(kcodename_u[i], kcode) == 0 ||
		    strcmp(kcodename_l[i], kcode) == 0) {
		    *code = i;
		    return TRUE;
		}
	    }
	}
    }
    return FALSE;
}

#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
/*
 * Input last char of the KANA code select escape sequence,
 * and return its value.
 */
static int
kanalastchar(lastch)
int *lastch;
{
    register int s;
    register int i;
    register char *p = "KANA Select Char [78I] : ";
    register int n;
    char lchar[NINPUT];
    
    for (n = 0; n < 2; n++) {
	if ((s = ereply(p, lchar, sizeof(lchar))) != TRUE) {
	    return (s);
	}
	if (ISDIGIT(lchar[0])) {
	    i = atoi(lchar);
	    if (i == 7)
		i = '7';
	    else if (i == 8)
		i = '8';
	}
	else {
	    i = lchar[0];
	}
	if (i == '7' || i == '8' || i == 'I') {
	    *lastch = i;
	    return TRUE;
	}
    }
    return FALSE;
}
#endif  /* HANKANA */

/*
 * Input last char of the KANJI/ASCII code select escape sequence,
 * and return its value.  KANJI select char is input when kselect
 * is TRUE, othewise ASCII select char is input.
 */
static int
klastchar(lastch, kselect)
int *lastch;
int kselect;
{
    register int s;
    register int i;
    register char *p = kselect ? "KANJI Select Char [@B] : " :
				 "ASCII Select Char [BHJ] : ";
    register int n;
    char lchar[NINPUT];
    
    for (n = 0; n < 2; n++) {
	if ((s = ereply(p, lchar, sizeof(lchar))) != TRUE) {
	    return (s);
	}
	if (ISDIGIT(lchar[0])) {
	    i = atoi(lchar);
	}
	else {
	    i = lchar[0];
	}
	if ((kselect && (i == '@' || i == 'B')) ||
	    (!kselect && (i == 'B' || i == 'J' || i == 'H'))) {
	    *lastch = i;
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * Set the buffer local file I/O code when buffer is created.
 */
VOID
ksetbufcode(bp)
BUFFER	*bp;
{
    bp->b_kfio = global_kfio;
}

/*
 * Show file I/O, input, display KANJI code into mode line.
 */
int
kdispbufcode(bp)
register BUFFER	*bp;
{
    register int n = 0;
    
    /* 90.12.28  Change display style like as Nemacs 3.3. by S.Yoshida */
    vtputc(symbol_c[(int)bp->b_kfio]);	n++;
    vtputc(symbol_c[global_kinput]);	n++;
    vtputc(symbol_c[global_kdisplay]);	n++;
    vtputc(':');			n++;
    return (n);
}

#ifdef WIN32
extern int ttputkc _PRO((int, int));
#else
#define ttputkc(a, b)		(ttputc(a),ttputc(b))
#endif

static int kdselected = SELROMA;
#ifdef	HANKANA
static int kanadselected = FALSE;
#endif

/*
 * Check current JIS code condition, and output KANJI/ASCII code
 * select escape sequence to the display if nessesary.
 */
VOID
kdselectcode(next_is_k)
register int next_is_k;		/* Is next code KANJI ? */
{
    if (global_kdisplay != JIS)
	return;

    /* Now KANJI && next is ASCII	*/
    if (kdselected==SELKANJI && !next_is_k) {
	ttputc(ESC);		/* Select ASCII code.		*/
	ttputc('(');
	ttputc(to_a_display);
	kdselected = SELROMA;
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
    }
    else if (kanadselected && !next_is_k) { /* Now KANJI && next is ASCII */
	kanadselected = FALSE;	   /* Select ASCII code.	*/
	if (to_kana_display == '7')
	    ttputc(0x0f);
	if (kdselected != SELROMA) {
	    ttputc(ESC);
	    ttputc('(');
	    ttputc(to_a_display);
	}
    }
    else if (kanadselected && next_is_k) { 
	kanadselected = FALSE;
	if (to_kana_display == '7')
	    ttputc(0x0f);
	ttputc(ESC);
	ttputc('$');
	ttputc(to_k_display);
	kdselected = TRUE;
#endif  /* HANKANA */
    }
    else if (!kdselected && next_is_k) { /* Now ASCII && next is KANJI */
	ttputc(ESC);		/* Select KANJI code.		*/
	ttputc('$');
	ttputc(to_k_display);
	kdselected = TRUE;
    }
}

static int kfselected = SELROMA;
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
static int kanafselected = FALSE;
#endif  /* HANKANA */

/*
 * Set buffer local file input code to read new file.
 */
VOID
ksetfincode(bp)
register BUFFER	*bp;
{
    if ((local_kfin = global_kexpect) == _T_) {
	local_kfin = NIL;
    }
    else if (local_kfin == NIL) {
	if (bp != NULL)
	    local_kfin = bp->b_kfio;
    }
}

/*
 * Convert KANJI code form a file code (JIS/Shift-JIS/EUC) to
 * a buffer code (EUC) in the text line.
 * When file KANJI code is not decided, we check and determine it
 * to see a text line.
 */
int
kcodeconv(buf, len, bp, buflen)
register char *buf;
register int len;
register BUFFER	*bp;
int buflen;
{
    if (local_kfin == NIL) {	/* Not decided.	*/
	local_kfin = kcodecheck(buf, len);
    }
    if (local_kfin != NIL) {	/* Must convert. */
	switch (local_kfin) {
	case JIS:
	    len = bufjtoe(buf, len);
	    break;
	case SJIS:
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	    len = bufstoe(buf, len);
#else	/* Not HANKANA */
	    bufstoe(buf, len);
#endif  /* HANKANA */
	    break;
	/* case EUC: */   /* EUC need not convert. */
#ifdef	USE_UNICODE
	case UTF8:
	    len = bufu8toe(buf, len, buflen);
	    break;
	case UCS2:
	    len = bufu2toe(buf, len, buflen);
	    break;
#endif	/* USE_UNICODE */
	}
	if (bp != NULL && bp->b_kfio == NIL) { /* set buffer local code. */
	    bp->b_kfio = local_kfin;
	}
    }
    return(len);
}

#if defined(SS_SUPPORT) || defined(USE_UNICODE)
/*
 * Count after converting KANJI code form a file code (JIS/Shift-JIS/EUC) to
 * a buffer code (EUC) in the text line.
 * When file KANJI code is not decided, we check and determine it
 * to see a text line.
 */
int
kcodecount(buf, len)
register char *buf;
register int len;
{
    if (local_kfin == NIL) {	/* Not decided.	*/
	local_kfin = kcodecheck(buf, len);
    }
    if (local_kfin != NIL) {	/* Must convert. */
	switch (local_kfin) {
	case JIS:
	    len = bufjtoe_c(buf, len);
	    break;
	case SJIS:
	    len = bufstoe_c(buf, len);
	    break;
#ifdef USE_UNICODE
	case UTF8:
	    len = bufu8toe_c(buf, len);
	    break;
	case UCS2:
	    len = bufu2toe_c(buf, len);
	    break;
#endif
	}
    }
    return(len);
}
#endif  /* SS_SUPPORT || USE_UNICODE */

/*
 * Check and determine what kind of KANJI code exists.
 * If no KANJI code exists, NIL (Not determine/No conversion) is
 * returned. If both EUC and Shift-JIS code is possible,
 * we think it is EUC.
 * This routine is refered to Nemacs's kanji.c.
 */
#ifdef  SS_SUPPORT /* 92.11.21  by S.Sasaki */
static int
kcodecheck(p, len)
char *p;
int len;
{
    register unsigned char c;
    register char *endp = p + len - 1;
    register int notjis = FALSE;
    
    while (p < endp) {
	c = *p++;
	if (c == ESC && ( *p == '$' || *p == '(' ) && !notjis) {
	    return (JIS);
	}
	else if (c >= 0x80) {
	    if (!iskana(c))
		notjis = TRUE;
	    if (c < 0xa0 && c != SS2 && c != SS3) {
		return (SJIS);
	    }
	    else if (c > 0xef) {
		return (EUC);
	    }
	    c = *p++;
	    if (c < 0xa0) {
		return (SJIS);
	    }
	    else if (c > 0xfc) {
		return (EUC);
	    }
	}
    }
    return (notjis ? EUC: NIL);
}
#else  /* not SS_SUPPORT */
static int
kcodecheck(p, len)
char *p;
int len;
{
    register unsigned char c;
    register char *endp = p + len - 1;
    register int notjis = FALSE;

    while (p < endp) {
	c = *p++;
	if (c == ESC && *p == '$' && !notjis) {
	    return (JIS);
	}
	else if (c >= 0x80) {
	    notjis = TRUE;
	    if (c < 0xa0) {
		return (SJIS);
	    }
	    else if (c < 0xe0 || c > 0xef) {
		return (EUC);
	    }
	    c = *p++;
	    if (c < 0xa1) {
		return (SJIS);
	    }
	    else if (c > 0xfc) {
		return (EUC);
	    }
	}
    }
    return (notjis ? EUC: NIL);
}
#endif  /* SS_SUPPORT */

/*
 * Convert KANJI code from JIS to EUC of the text in a buffer.
 * KANJI/ASCII selecting escape sequence is striped. So converted
 * EUC text must be shorter than sorce JIS code text.
 */
int
bufjtoe(j, len)
char *j;				/* JIS code text.	*/
int len;
{
    register int c1, c2;
    register char *e = j;		/* Converted EUC text. (same buffer) */
    register char *endj = j + len;
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
    register int kselected = SELROMA;
    char *cp, *eorg = j;
#else  /* not HANKANA */
    register int kselected = FALSE;
#endif  /* HANKANA */
    
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
    if (len == 0)
	return(0);
    if ((cp = alloca((unsigned)(len))) == NULL) {
	ewprintf("Could not allocate %d bytes", len);
	return(-1);
    }
    bcopy(j, cp, len);
    j = cp;
    endj = j + len;
#endif  /* HANKANA */
    
    while (j < endj) {
	c1 = *j++;
	if (c1 == ESC) {
	    if (*j == '$' && (j[1] == '@' || j[1] == 'B')) {
		kselected = SELKANJI;
		j += 2;
	    }
	    else if (*j == '(' &&
		     (j[1] == 'B' || j[1] == 'J' || j[1] == 'H')) {
		kselected = SELROMA;
		j += 2;
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	    }
	    else if (*j == '(' && j[1] == 'I') {
		kselected = SELKANA;
		j += 2;
#endif  /* HANKANA */
#ifdef	HOJO_KANJI
	    }
	    else if (*j == '$' && j[1] == '(' && j[2] == 'D') {
		kselected = SELHOJO;
		j += 3;
#endif
	    }
	    else
		*e++ = c1;
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	}
	else if (c1 == 0x0e) {
	    kselected = SELKANA;
	}
	else if (c1 == 0x0f) {
	    kselected = SELROMA;
#endif  /* HANKANA */
	}
	else if (kselected == SELKANJI) {
	    c2 = *j++;
	    jtoe(c1, c2);
	    *e++ = c1;
	    *e++ = c2;
#ifdef	HOJO_KANJI
	}
	else if (kselected == SELHOJO) {
	    *e++ = (char)SS3;
	    c2 = *j++;
	    jtoe(c1, c2);
	    *e++ = c1;
	    *e++ = c2;
#endif
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
	}
	else if (kselected == SELKANA) {
	    *e++ = (char)SS2;
	    *e++ = (char)(c1 | 0x80);
	}
	else if (iskana(c1 & 0xff)) {
	    *e++ = (char)SS2;
	    *e++ = (char)c1;
#endif  /* HANKANA */
	}
	else
	    *e++ = (char)c1;
    }
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
    return (e - eorg);
#else  /* not HANKANA */
    return (len + e - j);
#endif  /* HANKANA */
}

/*
 * Convert KANJI code from Shift-JIS to EUC of the text in a buffer.
 */
#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
int
bufstoe(p, len)
char *p;
int len;
{
    register int c1, c2;
    register char *porg, *tmpstr, *cp, *endcp;

    if (len == 0)
	return(0);
    if ((tmpstr = alloca((unsigned)(len))) == NULL) {
	ewprintf("Could not allocate %d bytes", len);
	return(-1);
    }
    cp = tmpstr;
    bcopy(p, cp, len);
    endcp = cp + len;
    porg = p;
	
    while (cp < endcp) {
	c1 = *cp++ & 0xff;
	if (iskana(c1)) {
	    *p++ = (char)SS2;
	    *p++ = (char)c1;
	}
	else if (issjis1st(c1)) {
	    c2 = *cp++ & 0xff;
	    stoe(c1, c2);
	    *p++ = (char)c1;
	    *p++ = (char)c2;
	}
	else {
	    *p++ = (char)c1;
	}
    }
    return(p - porg);
}
#else  /* not HANKANA */
int
bufstoe(p, len)
char *p;
int  len;
{
    register int c1, c2;
    register char *endp = p + len;
    
    while (p < endp) {
	c1 = *p++ & 0xff;
	if (issjis1st(c1)) {
	    c2 = *p++ & 0xff;
	    stoe(c1, c2);
	    p[-2] = c1;
	    p[-1] = c2;
	}
    }
}
#endif /* HANKANA */

VOID
bufetos(p, len)
char *p;
int len;
{
    register int c1, c2;
    register char *endp = p + len;
    register char *q = p;

    while (p < endp) {
	c1 = *p++ & 0xff;
	if (ISKANJI(c1)) {
#ifdef	HANKANA
	    if (ISHANKANA(c1))
		*q++ = *p++;
	    else
#endif  /* HANKANA */
#ifdef	HOJO_KANJI
	    if (ISHOJO(c1)) {
		c1 = TOUFU1ST;
		c2 = TOUFU2ND;
		etos(c1,c2);
		*q++ = c1;
		*q++ = c2;
		p += 2;
	    }
	    else
#endif
	    {
		c2 = *p++ & 0xff;
		etos(c1,c2);
		*q++ = c1;
		*q++ = c2;
	    }
	}
	else
	    *q++ = c1;
    }
}

#ifdef  SS_SUPPORT /* 92.11.21  by S.Sasaki */
static int
bufjtoe_c(j, len)
char *j;				/* JIS code text.	*/
int len;
{
    register int c1;
    register int leng;
    register char *endj = j + len;
    register int kselected = SELROMA;
    
    leng = 0;
    while (j < endj) {
	c1 = *j++;
	if (c1 == ESC) {
	    if (*j == '$' && (j[1] == '@' || j[1] == 'B')) {
		kselected = SELKANJI;
		j += 2;
	    }
	    else if (*j == '(' &&
		     (j[1] == 'B' || j[1] == 'J' || j[1] == 'H')) {
		kselected = SELROMA;
		j += 2;
#ifdef	HANKANA
	    }
	    else if (*j == '(' && j[1] == 'I') {
		kselected = SELKANA;
		j += 2;
#endif
#ifdef	HOJO_KANJI
	    }
	    else if (*j == '$' && j[1] == '(' && j[2] == 'D') {
		kselected = SELHOJO;
		j += 3;
#endif
	    }
	    else
		leng++;
	}
	else if (c1 == 0x0e)
	    kselected = SELKANA;
	else if (c1 == 0x0f)
	    kselected = SELROMA;
	else if (kselected == SELKANJI) {
	    j++;
	    leng += 2;
#ifdef	HOJO_KANJI
	}
	else if (kselected == SELHOJO) {
	    j++;
	    leng += 3;
#endif
#ifdef	HANKANA
	}
	else if (kselected == SELKANA) {
	    leng += 2;
	}
	else if (iskana(c1 & 0xff)) {
	    leng += 2;
#endif
	}
	else {
	    leng++;
	}
    }
    return (leng);
}

int
bufstoe_c(p, len)
char *p;
int len;
{
    register int c1;
    register char *endp;

    endp = p + len;
    
    len=0;
    while (p < endp) {
	c1 = *p++ & 0xff;
#ifdef	HANKANA
	if (iskana(c1)) {
	    len += 2;
	}
	else
#endif
	if (issjis1st(c1)) {
	    p++;
	    len += 2;
	}
	else {
	    len++;
	}
    }
    return(len);
}
#endif /* SS_SUPPORT */

#ifdef	USE_UNICODE
int
bufu2toe_c(p, len)
register char *p;
int len;
{
    register char *end_p = p + len;
    int c1, c2, c3;

    len = 0;
    while (p < end_p) {
	c1 = *p++;
	c2 = *p++;
	if (c1 == 0 && c2 <= 0x7F)
	    len++;
	else {
	    utoe(c1, c2, c3);
	    if (c3)
		len += 3;
	    else if (c2)
		len += 2;
	    else if (c1)
		len++;
	}
    }
    return len;
}

int
bufu8toe_c(p, len)
register char *p;
int len;
{
    register char *end_p = p + len;
    int c1, c2, c3;

    len = 0;
    while (p < end_p) {
	if (isutf1byte(*p)) {
	    len++;
	    p++;
	}
	else {
	    if (isutf2byte(*p)) {
		c1 = *p++;
		c2 = *p++;
		u8tou2(c1, c2, 0);
	    }
	    else {
		c1 = *p++;
		c2 = *p++;
		c3 = *p++;
		u8tou2(c1, c2, c3);
	    }
	    utoe(c1, c2, c3);
	    if (c3)
		len += 3;
	    else if (c2)
		len += 2;
	    else if (c1)
		len++;
	}
    }
    return len;
}
#endif	/* USE_UNICODE */

/*
 * Is current position char KANJI ?
 */
int
iskanji()
{
    return(ISKANJI(lgetc(curwp->w_dotp, curwp->w_doto)));
}

#ifdef	HOJO_KANJI
/*
 * Is current position char Hojo KANJI ?
 */
int
ishojo()
{
    return(ISHOJO(lgetc(curwp->w_dotp, curwp->w_doto)));
}
#endif

/*
 * Is this KANJI word char ?
 * This routine return TRUE when c1 is ASCII char,
 * but it doesn't happen now.
 */
int
iskword(c1, c2)
register int c1;
register int c2;
{
    return(charcategory(c1, c2) != CAT_NONE);
}

/*
 * Get char category of current position char.
 */
int
getcategory()
{
    return(charcategory(lgetc(curwp->w_dotp, curwp->w_doto),
			lgetc(curwp->w_dotp, curwp->w_doto + 1)));
}

int cur_cat;			/* Current char category.	*/
int cur_dir;			/* Current search direction.	*/

/*
 * Set char category of search start position char,
 * and set search direction.
 */
VOID
initcategory(dir)
int dir;
{
    cur_cat = getcategory();
    cur_dir = dir;
}

/*
 * Is it in a same category ?
 * return TRUE if category of current position char is same as
 * start position char's one (now in a word), or it is HIRAGANA
 * added to the end of that word.
 */
int
incategory()
{
    register int cat;
    
    if (curwp->w_doto == llength(curwp->w_dotp))
	return FALSE;
    cat = getcategory();
    if (cur_dir == 1) {		/* Direction is forward. */
	if (cat == CAT_HIRAGANA) { /* Now start added HIRAGANA part. */
	    cur_cat = cat;
	}
    }
    else {			/* Direcrion is backward. */
	if (cur_cat == CAT_HIRAGANA && cat != cur_cat) {
	    /* Now end added HIRAGANA part and start word part. */
	    cur_cat = cat;
	}
    }
    return(cat == cur_cat);
}

/*
 * Return char category of one char.
 */
int
charcategory(c1, c2)
register int c1;
register int c2;
{
    switch (c1) {
    case 0xa1:
	if (c2 >= 0xb8 && c2 <= 0xbb) {
	    return(CAT_KANJI);
	}
	else if (c2 >= 0xb3 && c2 <= 0xb6) {
	    return(CAT_HIRAGANA);
	}
	else if (c2 == 0xbc) {
	    return(CAT_KATAKANA);
	}
	/* else it is KIGOU. */
    case 0xa2:
    case 0xa8:
	return(CAT_KIGOU);
	/* Not reached */
    case 0xa3:
	return(CAT_EISUUJI);
	/* Not reached */
    case 0xa4:
	return(CAT_HIRAGANA);
	/* Not reached */
    case 0xa5:
	return(CAT_KATAKANA);
	/* Not reached */
    case 0xa6:
	return(CAT_GREEK);
	/* Not reached */
    case 0xa7:
	return(CAT_RUSSIAN);
	/* Not reached */
    default:
	if (c1 <= 0x7f) {
	    return(CAT_ASCII);
	}
	else if (c1 >= 0xb0) {
	    return(CAT_KANJI);
	}
	return(CAT_NONE);
	/* Not reached */
    }
}
#endif	/* KANJI */
