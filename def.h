/* $Id: def.h,v 1.15.2.2 2003/03/08 01:22:35 amura Exp $ */
/*
 * This file is the general header file for all parts
 * of the MicroEMACS display editor. It contains all of the
 * general definitions and macros. It also contains some
 * conditional compilation flags. All of the per-system and
 * per-terminal definitions are in special header files.
 * The most common reason to edit this file would be to zap
 * the definition of CVMVAS or BACKUP.
 */

/*
 * $Log: def.h,v $
 * Revision 1.15.2.2  2003/03/08 01:22:35  amura
 * NOTAB is always enabled
 *
 * Revision 1.15.2.1  2003/03/08 00:17:23  amura
 * fix query-replace bug, too
 *
 * Revision 1.15  2001/05/25 15:36:52  amura
 * now buffers have only one mark (before windows have one mark)
 *
 * Revision 1.14  2001/02/18 17:07:27  amura
 * append AUTOSAVE feature (but NOW not work)
 *
 * Revision 1.13  2001/02/11 15:40:24  amura
 * some function are changed to static for speed/size
 *
 * Revision 1.12  2001/02/01 16:30:39  amura
 * add vtsetsize() definition and change NFILEN size for amiga
 *
 * Revision 1.11  2001/01/17 18:33:14  amura
 * add prototype of ishojo() and some change for WIN32
 *
 * Revision 1.10  2001/01/05 14:07:00  amura
 * first implementation of Hojo Kanji support
 *
 * Revision 1.9  2000/12/27 16:56:00  amura
 * change d_makename() params for conservative reason, and bugfix in dires_()
 *
 * Revision 1.8  2000/12/14 18:12:13  amura
 * use alloca() and more memory secure
 *
 * Revision 1.7  2000/09/21 17:28:29  amura
 * replace macro _WIN32 to WIN32 for Cygwin
 *
 * Revision 1.6  2000/07/22 20:50:54  amura
 * redefine NFILEN macro
 *
 * Revision 1.5  2000/07/20 12:45:17  amura
 * support undo with auto-fill mode
 *
 * Revision 1.4  2000/06/27 01:49:42  amura
 * import to CVS
 *
 * Revision 1.3  2000/06/01 05:25:06  amura
 * Undo support
 *
 * Revision 1.2  2000/05/01  23:04:38  amura
 * undo test version
 *
 * Revision 1.1  1999/05/19  03:52:32  amura
 * Initial revision
 *
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include	<stdlib.h>
#include	<string.h>
#include	"sysdef.h"		/* Order is critical.		*/
#include	"ttydef.h"
#include	"chrdef.h"

/*
 * If your system and/or compiler does not support the "void" type
 * then define NO_VOID_TYPE in sysdef.h.  In the absence of some
 * other definition for VOID, the default in that case will be to
 * turn it into an int, which works with most compilers that don't
 * support void.  In the absence of any definition of VOID or
 * NO_VOID_TYPE, the default is to assume void is supported, which
 * should be the case for most modern C compilers.
 */

#ifdef NO_VOID_TYPE
#  undef VOID
#  define VOID int			/* Default for no void is int */
#  define VOIDptr char*
#else
#ifndef VOID
#  define VOID void			/* Just use normal void */
#  define VOIDptr void*
#endif /* VOID */
#endif /* NO_VOID_TYPE */

#ifdef	NO_MACRO
#ifndef NO_STARTUP
#define NO_STARTUP			/* NO_MACRO implies NO_STARTUP */
#endif
#endif

#ifdef SUPPORT_ANSI
#define pro(x) x
#else
#define pro(x) ()
#endif

typedef int (*PF) pro((int, int)); /* generaly useful type */

/*
 * Table sizes, etc.
 */
#if defined(MSDOS)
#define NFILEN	80			/* Length, file name.		*/
#elif defined(HUMAN68K)
#define NFILEN	128			/* Length, file name.		*/
#elif defined(AMIGA)
#define NFILEN	256			/* Length, file name.		*/
#else /* Maybe Win32 or UN*X */
#define NFILEN	1024			/* Length, file name.		*/
#endif
#define NBUFN	32			/* Length, buffer name.		*/
#define NLINE	256			/* Length, line.		*/
#define NINPUT	32			/* Length, small minibuf input	*/
#define PBMODES 8			/* modes per buffer		*/
#define NKBDM	256			/* Length, keyboard macro.	*/
#define NPAT	80			/* Length, pattern.		*/
#define HUGE	1000			/* A rather large number.	*/
#define NSRCH	128			/* Undoable search commands.	*/
#define NXNAME	64			/* Length, extended command.	*/
#define NKNAME	20			/* Length, key names		*/
#define UNDOSIZE 10			/* Undo buffer size		*/
/*
 * Universal.
 */
#define FALSE	0			/* False, no, bad, etc.		*/
#define TRUE	1			/* True, yes, good, etc.	*/
#define ABORT	2			/* Death, ^G, abort, etc.	*/

#define KPROMPT 2			/* keyboard prompt		*/

/*
 * These flag bits keep track of
 * some aspects of the last command. The CFCPCN
 * flag controls goal column setting. The CFKILL
 * flag controls the clearing versus appending
 * of data in the kill buffer.
 */
#define CFCPCN	0x0001			/* Last command was C-P, C-N	*/
#define CFKILL	0x0002			/* Last command was a kill	*/
#define CFINS	0x0004			/* Last command was self-insert */
#define CFINS2	0x0008			/* Last command was insert	*/

/*
 * File I/O.
 */
#define FIOSUC	0			/* Success.			*/
#define FIOFNF	1			/* File not found.		*/
#define FIOEOF	2			/* End of file.			*/
#define FIOERR	3			/* Error.			*/
#define FIOLONG 4			/* long line partially read	*/

/*
 * Directory I/O.
 */
#define DIOSUC	0			/* Success.			*/
#define DIOEOF	1			/* End of file.			*/
#define DIOERR	2			/* Error.			*/

/*
 * Display colors.
 */
#define CNONE	0			/* Unknown color.		*/
#define CTEXT	1			/* Text color.			*/
#define CMODE	2			/* Mode line color.		*/

/* Flags for keyboard involked functions */

#define FFUNIV		1		/* universal argument		*/
#define FFNEGARG	2		/* negitive only argument	*/
#define FFOTHARG	4		/* other argument		*/
#define FFARG		7		/* any argument			*/
#define FFRAND		8		/* Called by other function	*/

/*
 * Flags for "eread".
 */
#define EFFUNC	0x0001			/* Autocomplete functions.	*/
#define EFBUF	0x0002			/* Autocomplete buffers.	*/
#define EFFILE	0x0004			/* " files (maybe someday)	*/
#define EFAUTO	0x0007			/* Some autocompleteion on	*/
#define EFNEW	0x0008			/* New prompt.			*/
#define EFCR	0x0010			/* Echo CR at end; last read.	*/

/*
 * Flags for "ldelete"/"kinsert"
 */

#define KNONE	0
#define KFORW	1
#define KBACK	2
#ifdef KANJI
#define KNOKANJI	0x80
#define KFLAGS(n)	((n) & 0x0f)
#endif

/*
 * All text is kept in circularly linked
 * lists of "LINE" structures. These begin at the
 * header line (which is the blank line beyond the
 * end of the buffer). This line is pointed to by
 * the "BUFFER". Each line contains a the number of
 * bytes in the line (the "used" size), the size
 * of the text array, and the text. The end of line
 * is not stored as a byte; it's implied. Future
 * additions will include update hints, and a
 * list of marks into the line.
 */
typedef struct	LINE {
	struct	LINE *l_fp;		/* Link to the next line	*/
	struct	LINE *l_bp;		/* Link to the previous line	*/
	short	l_size;			/* Allocated size		*/
	short	l_used;			/* Used size			*/
#ifndef ZEROARRAY
	char	l_text[1];		/* A bunch of characters.	*/
#else
	char	l_text[];		/* A bunch of characters.	*/
#endif
}	LINE;

/*
 * The rationale behind these macros is that you
 * could (with some editing, like changing the type of a line
 * link from a "LINE *" to a "REFLINE", and fixing the commands
 * like file reading that break the rules) change the actual
 * storage representation of lines to use something fancy on
 * machines with small address spaces.
 */
#define lforw(lp)	((lp)->l_fp)
#define lback(lp)	((lp)->l_bp)
#define lgetc(lp, n)	(CHARMASK((lp)->l_text[(n)]))
#define lputc(lp, n, c) ((lp)->l_text[(n)]=(c))
#define llength(lp)	((lp)->l_used)
#define ltext(lp)	((lp)->l_text)

/*
 * All repeated structures are kept as linked lists of structures.
 * All of these start with a LIST structure (except lines, which
 * have their own abstraction). This will allow for
 * later conversion to generic list manipulation routines should
 * I decide to do that. it does mean that there are four extra
 * bytes per window. I feel that this is an acceptable price,
 * considering that there are usually only one or two windows.
 */
typedef struct LIST {
	union {
		struct WINDOW	*l_wp;
		struct BUFFER	*x_bp;	/* l_bp is used by LINE */
		struct LIST	*l_nxt;
	} l_p;
	char	*l_name;
} LIST;
/*
 * Usual hack - to keep from uglifying the code with lotsa
 * references through the union, we #define something for it.
 */
#define l_next	l_p.l_nxt

/*
 * There is a window structure allocated for
 * every active display window. The windows are kept in a
 * big list, in top to bottom screen order, with the listhead at
 * "wheadp". Each window contains its own value of dot.
 * The flag field contains some bits that are set by commands
 * to guide redisplay; although this is a bit of a compromise in
 * terms of decoupling, the full blown redisplay is just too
 * expensive to run for every input character.
 */
typedef struct	WINDOW {
	LIST	w_list;			/* List header		       */
	struct	BUFFER *w_bufp;		/* Buffer displayed in window	*/
	struct	LINE *w_linep;		/* Top line in the window	*/
	struct	LINE *w_dotp;		/* Line containing "."		*/
	short	w_lines;		/* Top line displayed line number */
	short	w_doto;			/* Byte offset for "."		*/
	char	w_toprow;		/* Origin 0 top row of window	*/
	char	w_ntrows;		/* # of rows of text in window	*/
	char	w_force;		/* If NZ, forcing row.		*/
	char	w_flag;			/* Flags.			*/
	char	w_dotlines;		/* line containing "." on tty lines */
}	WINDOW;
#define w_wndp	w_list.l_p.l_wp
#define w_name	w_list.l_name

/*
 * Window flags are set by command processors to
 * tell the display system what has happened to the buffer
 * mapped by the window. Setting "WFHARD" is always a safe thing
 * to do, but it may do more work than is necessary. Always try
 * to set the simplest action that achieves the required update.
 * Because commands set bits in the "w_flag", update will see
 * all change flags, and do the most general one.
 */
#define WFFORCE 0x01			/* Force reframe.		*/
#define WFMOVE	0x02			/* Movement from line to line.	*/
#define WFEDIT	0x04			/* Editing within a line.	*/
#define WFHARD	0x08			/* Better to a full display.	*/
#define WFMODE	0x10			/* Update mode line.		*/

/*
 * Text is kept in buffers. A buffer header, described
 * below, exists for every buffer in the system. The buffers are
 * kept in a big list, so that commands that search for a buffer by
 * name can find the buffer header. There is a safe store for the
 * dot and mark in the header, but this is only valid if the buffer
 * is not being displayed (that is, if "b_nwnd" is 0). The text for
 * the buffer is kept in a circularly linked list of lines, with
 * a pointer to the header line in "b_linep".
 */
typedef struct	BUFFER {
	LIST	b_list;			/* buffer list pointer		*/
	struct	BUFFER *b_altb;		/* Link to alternate buffer	*/
	struct	LINE *b_dotp;		/* Link to "." LINE structure	*/
	struct	LINE *b_markp;		/* ditto for mark		*/
	struct	LINE *b_linep;		/* Link to the header LINE	*/
	struct	MAPS_S *b_modes[PBMODES]; /* buffer modes		*/
	short	b_doto;			/* Offset of "." in above LINE	*/
	short	b_marko;		/* ditto for the "mark"		*/
	short	b_nmodes;		/* number of non-fundamental modes */
	char	b_nwnd;			/* Count of windows on buffer	*/
	char	b_flag;			/* Flags			*/
	char	*b_fname;		/* File name			*/
#ifdef	EXTD_DIR
	char	*b_cwd;			/* Current working directory for
					   this buffer.  By Tillanosoft */
#endif
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	char	b_kfio;			/* Local KANJI file I/O code.	*/
#endif	/* KANJI */
#ifdef  VARIABLE_TAB
	char	b_tabwidth;		/* Local TAB width		*/
#endif  /* VARIABLE_TAB */
#ifdef	UNDO
	struct	UNDO_DATA *b_ustack[UNDOSIZE+1];
					/* Undo stack data		*/
	short	b_utop;			/* Undo stack top		*/
	short	b_ubottom;		/* Undo stack bottom		*/
	struct	UNDO_DATA **b_ulast;	/* Last edited undo data	*/
#endif	/* UNDO */
}	BUFFER;
#define b_bufp	b_list.l_p.x_bp
#define b_bname b_list.l_name

#define BFCHG	0x01			/* Changed.			*/
#define BFBAK	0x02			/* Need to make a backup.	*/
#define BFNOTAB 0x04			/* no tab mode			*/
#define BFOVERWRITE 0x08		/* overwrite mode		*/
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#define	BFAUTOFILL 0x10			/* autofill mode with KANJI.	*/

#define	NOCONV	0			/* No KANJI conversion.		*/
#define	SJIS	1			/* KANJI code is Shift-JIS.	*/
#define	JIS	2			/* KANJI code is JIS.		*/
#define	EUC	3			/* KANJI code is EUC.		*/
#define	NIL	4			/* Not decided.			*/
#define	T	5			/* Guess.			*/
#endif	/* KANJI */
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
#define	BFRONLY	0x20			/* Read only mode.		*/
#endif	/* READONLY */
#ifdef CANNA
#define BFCANNA 0x40
#endif
#ifdef	AUTOSAVE	/* 96.12.24 by M.Suzuki	*/
#define	BFACHG	0x80			/* Auto save after changed.	*/
#endif	/* AUTOSAVE */

#ifdef ADDFUNC
#define MG_RATIO_ALL -1 /* used at dotpos() to return value */
#define MG_RATIO_TOP -2 /* used at dotpos() to return value */
#define MG_RATIO_BOT -3 /* used at dotpos() to return value */
#endif

/*
 * This structure holds the starting position
 * (as a line/offset pair) and the number of characters in a
 * region of a buffer. This makes passing the specification
 * of a region around a little bit easier.
 */
typedef struct	{
	struct	LINE *r_linep;		/* Origin LINE address.		*/
	short	r_offset;		/* Origin LINE offset.		*/
	RSIZE	r_size;			/* Length in characters.	*/
}	REGION;

/*
 * Externals.
 */
extern	int	thisflag;
extern	int	lastflag;
extern	int	curgoal;
extern	int	epresf;
extern	int	sgarbf;
extern	int	mode;
extern	WINDOW	*curwp;
extern	BUFFER	*curbp;
extern	WINDOW	*wheadp;
extern	BUFFER	*bheadp;
extern	char	pat[];
extern	BUFFER	*bfind();
extern	WINDOW	*popbuf();
extern	WINDOW	*wpopup();
extern	LINE	*lalloc();
extern	LINE	*lallocx();
#ifdef SUPPORT_ANSI
extern VOID ewprintf pro((char *fp, ... ));
#else
extern VOID ewprintf pro((va_alist));
#endif
extern VOID eerase();
extern	int	nrow;
extern	int	ncol;
extern	int	ttrow;
extern	int	ttcol;
extern	int	tceeol;
extern	int	tcinsl;
extern	int	tcdell;
extern	char	cinfo[];
extern	char	*keystrings[];
extern	VOID	update();
extern	char	*keyname();
extern	char	*adjustname();
extern	VOID	kdelete();
extern	VOID	lchange();

/*
   Should the 'ifdef' be applied for the following function declarations.
   For example, kttputc() should be declared only if the KANJI macro is
   defined.
 */

extern int forwchar pro((int, int));
extern int setmark pro((int, int));
extern int backline pro((int, int));
extern VOID warnreadonly pro((void));
extern int getgoal pro((LINE *));
extern int backpage pro((int, int));
extern int countlines pro((LINE	*));
extern int colrow pro((LINE *, short, int *, int *));
extern short skipline pro((LINE *, int));
extern int nextwind pro((int, int));
#ifdef SUPPORT_ANSI
extern int ereply pro((char *, char *, int , ... ));
extern int eread pro((char *, char *, int, int, ...));
#else
extern int ereply pro((va_alist));
extern int eread pro((va_alist));
#endif
extern int getcolpos pro((void));
extern int showbuffer pro((BUFFER *, WINDOW *, int));
extern int bclear pro((BUFFER *));
extern int anycb pro((int));
extern int addline pro((BUFFER *, char *));
extern int eyorn pro((char *));
extern int eyesno pro((char *));
extern int buffsave pro((BUFFER *));
extern VOID ksetbufcode pro((BUFFER *));
extern int newline pro((int, int));
extern int selfinsert pro((int, int));
extern int cm_indent pro((int, int));
extern int cm_term pro((int, int));
extern int linsert pro((int, int));
extern int ldelete pro((RSIZE, int));
extern int lnewline pro((void));
extern int panic pro((char *));
extern int name_fent pro((char *, int));
extern int splitwind pro((int, int));
extern int delwind pro((int, int));
extern int ttmove pro((int, int));
extern VOID eargset pro((char *));
extern int killbuffer pro((int, int));
extern int forwpage pro((int, int));
extern int d_undelbak pro((int, int));
extern int d_makename pro((LINE *, char *, int));
extern int readin pro((char *));
extern int fchkreadonly pro((char *));
#ifdef WIN32
extern int unlink pro((const char *));
extern int rmdir pro((const char *));
extern int rename pro((const char *, const char *));
#endif
extern int copy pro((char *, char *));
extern int ttopen pro((void));
extern int ttinit pro((void));
extern int ttcolor pro((int));
extern int ttnowindow pro((void));
extern int tteeol pro((void));
extern int tttidy pro((void));
extern int ttflush pro((void));
extern int ttclose pro((void));
extern int typeahead pro((void));
extern int tteeop pro((void));
#ifdef HANKANA
extern VOID putline pro((int, int, unsigned char *, unsigned char *, short));
#else
extern VOID putline pro((int, int, unsigned char *, short));
#endif
extern int vtputs pro((char *));
extern int kdispbufcode pro((BUFFER *));
extern int ttinsl pro((int, int, int));
extern int ttdell pro((int, int, int));
extern int fepmode_off pro((void));
extern int getkey pro((int));
extern VOID ungetkey pro((int));
extern int ctrlg pro((int, int));
extern int complete_del_list pro((void));
extern int complete_scroll_down pro((void));
extern int complete_scroll_up pro((void));
extern int complete_list_names pro((char *, int));
extern int ttputc pro((int));
extern int kttputc pro((int));
extern int excline pro((char *));
extern int load pro((char *));
extern int ffropen pro((char *));
extern VOID ksetfincode pro((BUFFER *));
extern int ffgetline pro((char *, int, int *));
extern int kcodeconv pro((char *, int, BUFFER *));
extern int ffclose pro((void));
extern int insertfile pro((char *, char *));
extern int ffisdir pro((char *));
extern int dired pro((int, int));
extern int cmode pro((int, int));
extern int kcodecount pro((char *, int));
extern int ldelnewline pro((void));
extern int writeout pro((BUFFER *, char *));
extern int fgetfilemode pro((char *));
extern int fbackupfile pro((char *));
extern int ffwopen pro((char *));
extern int ffputbuf pro((BUFFER *));
extern int popbuftop pro((BUFFER *));
extern VOID upmodes pro((BUFFER *));
extern VOID vtputc pro((int));
extern int ttgetc pro((void));
extern int ttungetc pro((int));
extern int bufjtoe pro((char *, int));
extern int bufstoe pro((char *, int));
extern VOID bufetos pro((char *, int));
extern int charcategory pro((int, int));
extern VOID PutLine pro((int, unsigned char *, short));
extern VOID kgetkeyflush pro((void));
extern VOID kdselectcode pro((int));
extern VOID kfselectcode pro((FILE *, int));
extern VOID initcategory pro((int));
extern int ttwait pro((void));
extern int kgetkey pro((void));
extern int fepmode_on pro((void));
extern int negative_argument pro((int, int));
extern int digit_argument pro((int, int));
extern int fillword pro((int, int));
extern int kcinsert pro((unsigned short *, unsigned short, int));
extern int kcdelete pro((unsigned short *, unsigned short, int));
extern int kinsert pro((int, int));
extern int kgrow pro((int));
extern int backchar pro((int, int));
extern int printversion pro((void));
extern VOID Exit pro((int));
extern int printoptions pro((void));
extern VOID dirinit pro((void));
extern VOID dirend pro((void));
extern int doin pro((void));
extern int ttbeep pro((void));
extern int gotoeop pro((int, int));
extern int inword pro((void));
extern int isbolkchar pro((int, int));
extern int iseolkchar pro((int, int));
extern int killregion pro((int, int));
extern int delwhite pro((int, int));
extern int backdel pro((int, int));
extern int receive_clipboard pro((void));
extern int kremove pro((int));
extern int re_readpattern pro((char *));
extern int re_forwsrch pro((void));
extern int re_backsrch pro((void));
extern int re_doreplace pro((RSIZE, char *, int));
extern int lreplace pro((RSIZE, char *, int));
extern int killmatches pro((int));
extern int countmatches pro((int));
extern int getregion pro((REGION *));
extern int setprefix pro((int, int));
extern int forwline pro((int, int));
extern int readpattern pro((char *));
extern int forwsrch pro((void));
extern int backsrch pro((void));
extern int isearch pro((int));
extern VOID vtsetsize pro((int, int));
extern int ttresize pro((void));
extern int shrinkwind pro((int, int));
extern int forwword pro((int, int));
extern int incategory pro((void));
extern int iskanji pro((void));
#ifdef	HOJO_KANJI
extern int ishojo pro((void));
#endif
extern int iskword pro((int, int));
extern int gotobol pro((int, int));
#ifdef	EXTD_DIR
extern VOID ensurecwd pro((void));
extern VOID edefset pro((char *));
#endif
extern int rescan pro((int, int));
#ifdef	AUTOSAVE
extern VOID autosave_check pro((int));
extern VOID autosave_handler pro((void));
extern VOID autosave_name pro((char*, char*, int));
extern VOID del_autosave_file pro((char*));
extern VOID clean_autosave_file pro((void));
#endif

/*
 * Standard I/O.
 */
#ifndef SUPPORT_ANSI
extern VOIDptr malloc();
extern VOIDptr realloc();
#endif

#ifdef	CANNA
VOID canna_init();
VOID canna_end();
#endif
