/* $Id: extern.h,v 1.2 2001/11/24 08:24:28 amura Exp $ */
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
 * $Log: extern.h,v $
 * Revision 1.2  2001/11/24 08:24:28  amura
 * Rewrite all sources (for msdos port)
 *
 * Revision 1.1  2001/11/23 19:55:24  amura
 * Rewrite all sources
 *
 */

/*
 * Externals.
 */
#ifdef __cplusplus
extern "C" {
#endif
extern int thisflag;
extern int lastflag;
extern int curgoal;
extern int epresf;
extern int sgarbf;
extern int mode;
extern WINDOW *curwp;
extern BUFFER *curbp;
extern WINDOW *wheadp;
extern BUFFER *bheadp;
extern char pat[];
extern BUFFER *bfind _PRO((char *, int));
extern WINDOW *popbuf _PRO((BUFFER *));
extern WINDOW *wpopup _PRO((void));
extern LINE *lalloc _PRO((int));
extern LINE *lallocx _PRO((int));
#ifdef SUPPORT_ANSI
extern VOID ewprintf(char *fp, ... );
#else
extern VOID ewprintf _PRO((va_alist));
#endif
extern VOID eerase _PRO((void));

/* define in ttyio.c */
extern int nrow;
extern int ncol;
extern int ttrow;
extern int ttcol;
extern int tceeol;
extern int tcinsl;
extern int tcdell;

extern char cinfo[];
extern char *keystrings[];
extern VOID update _PRO((void));
extern char *keyname _PRO((char *, int));
extern char *adjustname _PRO((char *));
extern VOID kdelete _PRO((void));
extern VOID lchange _PRO((int));

/*
   Should the 'ifdef' be applied for the following function declarations.
   For example, kttputc() should be declared only if the KANJI macro is
   defined.
 */

extern int forwchar _PRO((int, int));
extern int setmark _PRO((int, int));
extern int backline _PRO((int, int));
extern VOID warnreadonly _PRO((void));
extern int getgoal _PRO((LINE *));
extern int backpage _PRO((int, int));
extern int countlines _PRO((LINE *));
extern int colrow _PRO((LINE *, short, int *, int *));
extern short skipline _PRO((LINE *, int));
extern int nextwind _PRO((int, int));
#ifdef SUPPORT_ANSI
extern int ereply _PRO((char *, char *, int , ... ));
extern int eread _PRO((char *, char *, int, int, ...));
#else
extern int ereply _PRO((va_alist));
extern int eread _PRO((va_alist));
#endif
extern int getcolpos _PRO((void));
extern int showbuffer _PRO((BUFFER *, WINDOW *, int));
extern int bclear _PRO((BUFFER *));
extern int anycb _PRO((int));
extern int addline _PRO((BUFFER *, char *));
extern int eyorn _PRO((char *));
extern int eyesno _PRO((char *));
extern int buffsave _PRO((BUFFER *));
extern VOID ksetbufcode _PRO((BUFFER *));
extern int newline _PRO((int, int));
extern int selfinsert _PRO((int, int));
extern int cm_indent _PRO((int, int));
extern int cm_term _PRO((int, int));
extern int linsert _PRO((int, int));
extern int ldelete _PRO((RSIZE, int));
extern int lnewline _PRO((void));
extern int name_fent _PRO((char *, int));
extern int splitwind _PRO((int, int));
extern int delwind _PRO((int, int));
extern VOID eargset _PRO((char *));
extern int killbuffer _PRO((int, int));
extern int forwpage _PRO((int, int));
extern int d_undelbak _PRO((int, int));
extern int d_makename _PRO((LINE *, char *, int));
extern int readin _PRO((char *));
extern int fchkreadonly _PRO((char *));
#ifdef WIN32
extern int unlink _PRO((const char *));
extern int rmdir _PRO((const char *));
extern int rename _PRO((const char *, const char *));
#endif
extern int copy _PRO((char *, char *));
extern VOID kputc _PRO((int, FILE *, int));

/* define in tty.c */
extern VOID ttopen _PRO((void));
extern VOID ttinit _PRO((void));
extern VOID ttcolor _PRO((int));
extern VOID ttwindow _PRO((int, int));
extern VOID ttnowindow _PRO((void));
extern VOID tteeol _PRO((void));
extern VOID tttidy _PRO((void));
extern VOID ttflush _PRO((void));
extern VOID ttclose _PRO((void));
extern VOID tteeop _PRO((void));
extern VOID ttinsl _PRO((int, int, int));
extern VOID ttdell _PRO((int, int, int));
extern VOID ttputc _PRO((int));
extern VOID ttmove _PRO((int, int));
extern VOID ttresize _PRO((void));
extern VOID ttbeep _PRO((void));

/* define in ttyio.c */
extern int typeahead _PRO((void));
extern VOID panic _PRO((char *));
extern int ttgetc _PRO((void));
extern VOID ttungetc _PRO((int));
#ifdef FEPCTRL
extern VOID fepmode_off _PRO((void));
extern VOID fepmode_on _PRO((void));
#endif

#ifdef HANKANA
extern VOID putline _PRO((int, int, unsigned char *, unsigned char *, short));
#else
extern VOID putline _PRO((int, int, unsigned char *, short));
#endif
extern int vtputs _PRO((char *));
extern int kdispbufcode _PRO((BUFFER *));
extern int getkey _PRO((int));
extern VOID ungetkey _PRO((int));
extern int ctrlg _PRO((int, int));
extern int complete_del_list _PRO((void));
extern int complete_scroll_down _PRO((void));
extern int complete_scroll_up _PRO((void));
extern int complete_list_names _PRO((char *, int));
extern int kttputc _PRO((int));
extern int excline _PRO((char *));
extern int load _PRO((char *));
extern int ffropen _PRO((char *));
extern VOID ksetfincode _PRO((BUFFER *));
extern int ffgetline _PRO((char *, int, int *));
extern int kcodeconv _PRO((char *, int, BUFFER *, int));
extern int ffclose _PRO((void));
extern int insertfile _PRO((char *, char *));
extern int ffisdir _PRO((char *));
extern int dired _PRO((int, int));
extern int cmode _PRO((int, int));
extern int kcodecount _PRO((char *, int));
extern int ldelnewline _PRO((void));
extern int writeout _PRO((BUFFER *, char *));
extern int fgetfilemode _PRO((char *));
extern int fbackupfile _PRO((char *));
extern int ffwopen _PRO((char *));
extern int ffputbuf _PRO((BUFFER *));
extern int popbuftop _PRO((BUFFER *));
extern VOID upmodes _PRO((BUFFER *));
extern VOID vtputc _PRO((int));
extern int bufjtoe _PRO((char *, int));
extern int bufstoe _PRO((char *, int));
extern VOID bufetos _PRO((char *, int));
extern int charcategory _PRO((int, int));
extern VOID PutLine _PRO((int, unsigned char *, short));
extern VOID kgetkeyflush _PRO((void));
extern VOID kdselectcode _PRO((int));
extern VOID kfselectcode _PRO((FILE *, int));
extern VOID initcategory _PRO((int));
extern int ttwait _PRO((void));
extern int kgetkey _PRO((void));
extern int negative_argument _PRO((int, int));
extern int digit_argument _PRO((int, int));
extern int fillword _PRO((int, int));
extern int kcinsert _PRO((unsigned short *, unsigned short, int));
extern int kcdelete _PRO((unsigned short *, unsigned short, int));
extern int kinsert _PRO((int, int));
extern int kgrow _PRO((int));
extern int backchar _PRO((int, int));
extern int printversion _PRO((void));
extern int printoptions _PRO((void));
extern VOID dirinit _PRO((void));
extern VOID dirend _PRO((void));
extern int doin _PRO((void));
extern int gotoeop _PRO((int, int));
extern int inword _PRO((void));
extern int isbolkchar _PRO((int, int));
extern int iseolkchar _PRO((int, int));
extern int killregion _PRO((int, int));
extern int delwhite _PRO((int, int));
extern int backdel _PRO((int, int));
extern int receive_clipboard _PRO((void));
extern int kremove _PRO((int));
extern int re_readpattern _PRO((char *));
extern int re_forwsrch _PRO((void));
extern int re_backsrch _PRO((void));
extern int re_doreplace _PRO((RSIZE, char *, int));
extern int lreplace _PRO((RSIZE, char *, int));
extern int killmatches _PRO((int));
extern int countmatches _PRO((int));
extern int getregion _PRO((REGION *));
extern int setprefix _PRO((int, int));
extern int forwline _PRO((int, int));
extern int readpattern _PRO((char *));
extern int forwsrch _PRO((void));
extern int backsrch _PRO((void));
extern int isearch _PRO((int));
extern VOID vtsetsize _PRO((int, int));
extern int shrinkwind _PRO((int, int));
extern int forwword _PRO((int, int));
extern int incategory _PRO((void));
extern int iskanji _PRO((void));
#ifdef HOJO_KANJI
extern int ishojo _PRO((void));
#endif
extern int iskword _PRO((int, int));
extern int gotobol _PRO((int, int));
#ifdef EXTD_DIR
extern VOID ensurecwd _PRO((void));
extern VOID edefset _PRO((char *));
#endif
extern int rescan _PRO((int, int));

/* define in autosave.c */
#ifdef AUTOSAVE
extern VOID autosave_check _PRO((int));
extern VOID autosave_handler _PRO((void));
extern VOID autosave_name _PRO((char*, char*, int));
extern VOID del_autosave_file _PRO((char*));
extern VOID clean_autosave_file _PRO((void));
#endif

/* define in canna.c */
#ifdef CANNA
VOID canna_init _PRO((void));
VOID canna_end _PRO((void));
#endif

#ifdef __cplusplus
}
#endif
