/* $Id: skg.c,v 1.9 2001/05/08 18:38:34 amura Exp $ */
/* - For Kg Ver.4.1.0 -                                     */
/* Simple Kanji Generator functions for MicroGnuEmacs(Kg)   */
/* for AMIGA series with ANS,DaiGokai(above ver.0.40).      */
/*   original written by Hirokazu Konish                    */
/*   rewrite for All ports and some patched by amura        */

/*
 * $Log: skg.c,v $
 * Revision 1.9  2001/05/08 18:38:34  amura
 * Make dictionary index when SKG start, for speed up dictionary searching
 *
 * Revision 1.8  2001/05/04 19:15:31  amura
 * correct handling of this/lastflag
 *
 * Revision 1.7  2001/05/04 17:52:02  amura
 * Make skg_init() and skg_finish(), and rewrite some routine for secure issue
 *
 * Revision 1.6  2001/04/30 14:59:49  amura
 * rewrite to speed up and bugfix
 *
 * Revision 1.5  2000/12/14 18:06:24  amura
 * filename length become flexible
 *
 * Revision 1.4  2000/11/16 14:31:13  amura
 * fix some typos which cause compile error when using
 * strict ANSI-C compiler (ex ACK, gcc-1.x)
 *
 * Revision 1.3  2000/10/02 16:13:46  amura
 * ignore mouse event in minibuffer editing
 *
 * Revision 1.2  2000/06/27 01:59:42  amura
 * small bugfix
 *
 * Revision 1.1.1.1  2000/06/27 01:47:56  amura
 * import to CVS
 *
 */

#include "config.h"               /* 93.10.25  by H.Konishi */ 
#ifdef INCLUDE_SKG
#include "def.h"
#include "key.h"
#ifndef NO_MACRO
#include "macro.h"
#endif
#ifdef	UNDO
#include "undo.h"
#endif

#ifdef AMIGA_FAST_FILE
# undef FILE
#endif
#ifdef MOUSE
extern int allow_mouse_event;
#endif

#define DIC_BUFFER_SIZE 512
#define KEY_BUFFER_SIZE 80

#define	KANA_MAXLEN	((3*2)+1)
#define	ROMAN_MAXLEN	5

#define	DIC_INDEX_SIZE	10
#define	MAX_TARGET	5

/* Key String for Searcing Dictionary */
typedef struct {   
    unsigned char hiragana[KANA_MAXLEN];
    int  length;
    int  flg;
} DICKEYSTR;
static DICKEYSTR dickeystr[KEY_BUFFER_SIZE];

/* Roman -> Kana convertion table */
typedef struct {
    unsigned char roman[ROMAN_MAXLEN];
    unsigned char kana[KANA_MAXLEN];
    char _dummy[8 - (ROMAN_MAXLEN+KANA_MAXLEN)%8];	/* for dword align */
} ROMANTBL;
static ROMANTBL *romantbl;
static ROMANTBL *romantbl_end;
int romantbl_size;

/* Dictionary file (opening while SKG active for speed reason) */
static FILE *dicfile;

/* Dictionary Index */
typedef struct {
    unsigned char key[2];
    long position;
} DICINDEX;
static DICINDEX dicindex[DIC_INDEX_SIZE];
static DICINDEX *end_of_dicindex;

typedef unsigned char WORDLIST[DIC_BUFFER_SIZE];

/* Kanji-Charactors (EUC code only!) */
#define HIRA_SMALL_TSU	"\xA4\xC3"
#define KATA_SMALL_TSU	"\xA5\xC3"
#define HIRA_A		"\xA4\xA2"

#define H_MODE		0	/* Hiragana MODE */
#define A_MODE		1	/* Ank MODE,or NOT-Convert MODE  */

#define SKG_CONV	0
#define SKG_NOCONV	1
#define SKG_QUIT	2

#define SKG_INSERT	0	/* Must not same value with SKG_QUIT */

#define	DEFAULT_ROMANNAME	"SKG-ROMAN"
#define DEFAULT_DICNAME		"SKG-JISYO"

/* SKG running flag */
static int in_skg = FALSE;

char *romanname = NULL;
char *dicname   = NULL;

extern int isetmark();
static int skg_init();
static VOID skg_finish();
static int skg_input_string();
static VOID skg_text_insert();
static int skg_convert_string();
static VOID skg_display_prompt();
static VOID search_dictionary();
static VOID target_display();
static VOID setup_keystring();
static int compare_keystring();

#define skip_entry(f)		do {	\
    register int c;			\
    while((c=fgetc(f)) != '\n');	\
} while(/*CONSTCOND*/0)

/* 
 *  Primitive functions for Strings 
 */

/* Clear Key Buffer String */
#define clear_string(str)	(str[0] = '\0')
/* Clear Dictinary Buffer String */
#define clear_list(str)		(str[0] = '\0')

static long
get_size(fp)
    FILE *fp;
{
    long size;
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    return size;
}

/* Comparing Two Strings */
static int
compare_string(s1, s2)
    register char *s1, *s2;
{
    while (*s2)
	if (*s1++ != *s2++)
	    return FALSE;
    return TRUE;
}

/* Append char to string */
static char *
strcat_c(s, c)
    char *s;
    int c;
{
    register char *p = s;
    while (*p)
	p++;
    *p++ = (char)c;
    *p = '\0';
    return s;
}

static int
make_dicindex(fp)
    FILE *fp;
{
    int i;
    int k1, k2, old_k1, old_k2;
    long diff, pos;

    diff = get_size(fp) / DIC_INDEX_SIZE;

    for (i=1; i<DIC_INDEX_SIZE; i++)
    {
	if (fseek(fp, diff*i, SEEK_SET) != 0)
	    return FALSE;
	skip_entry(fp);
	if ((k1=fgetc(fp)) == EOF || k2 == '\n')
	    break;
	if ((k2=fgetc(fp)) == EOF || k2 == '\n')
	    break;
	do
	{
	    old_k1 = k1; old_k2 = k2;
	    skip_entry(fp);
	    pos = ftell(fp);
	    if ((k1=fgetc(fp)) == EOF || k2 == '\n')
		goto end;
	    if ((k2=fgetc(fp)) == EOF || k2 == '\n')
		goto end;
	} while (k1 == old_k1 && k2 == old_k2);
	
	dicindex[i].key[0] = (unsigned char)k1;
	dicindex[i].key[1] = (unsigned char)k2;
	dicindex[i].position = pos;
    }
end:
    end_of_dicindex = &dicindex[i-1];
    
    /* This is sentinel */
    dicindex[0].key[0] = dicindex[0].key[1] = '\0';
    dicindex[0].position = 0;
    
    return TRUE;
}

static void
wind_dicindex(first_key, second_key, dicfile)
    register int first_key, second_key;
    FILE *dicfile;
{
    register DICINDEX *ptr = end_of_dicindex;
    while (ptr->key[0] > first_key || ptr->key[1] > second_key)
	ptr--;
    fseek(dicfile, ptr->position, SEEK_SET);
}

/* Scanf for Dictionary File, with First Character Comparing  */
/*                                                            */
/*  Return Code is, which EOF(=?), or 0			      */
/*  is complete. return EOF).                                 */ 
/*  k1,k2 is First Character in Key String. In not matching,  */
/*  skip entry.ZENKAKU-Code consists of 2 bytes.              */
static int
get_scanf(k1, k2, f, key, kanji)
    int k1, k2;
    FILE *f;
    register char *key, *kanji;
{
    register int rbuf;
    register char *p;
    int got_space = FALSE;
    
  next_entry:   
    /* Checking First Character */
    p = key;
    if ((rbuf=fgetc(f)) == EOF)
	return FALSE;
    
    /* If Not Matching Character k1, Skip This Entry */
    if (rbuf != (unsigned char)k1) 
    {
	/* this Entry is too big, finish searching dictionary */
	if (rbuf > (unsigned char)k1)
	    return FALSE;
	skip_entry(f);
	goto next_entry;
    }

    /* Stock read character to s1[] */
    *p++ = (char)rbuf;
    
    /* Checking Second Character */
    if (k2 != 0x00)
    {
	/* Case of Key String is Not 1 char */
	if ((rbuf=fgetc(f)) == EOF)
	    return FALSE;
	
	/* If Next Character is SPACE, Skip Comparing to k2 */
	if (rbuf == ' ')
	    got_space = TRUE;
	else
	{
	    if (rbuf != (unsigned char)k2) 
	    {
		/* this Entry is too big, finish searching dictionary */
		if (rbuf > (unsigned char)k2)
		    return FALSE;
		
		/* If Not Matching Character k2, Skip This Entry */
		skip_entry(f);
		goto next_entry;
	    }
	    *p++ = (char)rbuf;
	}
    }
    
    /*   Next to get Dictionary entry.       */
    while (!got_space)
    {
	/* get keystring, Until EOF,or SPACE */
	if ((rbuf=fgetc(f)) == EOF)
	    return EOF;
	
	if (rbuf == ' ')
	    got_space = TRUE;
	else 
	    *p++ = (char)rbuf;
    }
    /* Terminate key */
    *p = '\0';
    
    /* Next Get Dictionary Entry */
    while (TRUE)
    {
	if ((rbuf=fgetc(f)) == EOF)
	    return EOF;
	
	if (rbuf == '\n')
	    break; 
	else 
	    *kanji++ = (char)rbuf;
    }
    *kanji = '\0';
    return TRUE;
}

skginput(f, n)
{
    char istr[KEY_BUFFER_SIZE];
    char dstr[KEY_BUFFER_SIZE];
    int mode = H_MODE;
    int s;

#ifdef	READONLY			/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();		/* do only displaying warning.	*/
	return FALSE;
    }
#endif	/* READONLY */

    if (in_skg)
	return FALSE;
    /* If Not Exist files what need, Quit. */  
    if (skg_init() == FALSE)
	return FALSE; 
    in_skg = TRUE;

#ifdef	UNDO
    undoptr = NULL;
#endif
    clear_string(istr);		/* Clear Input Buffer. */ 
    
    while (TRUE)		/* Skg Loop */
    {
	s = skg_input_string(&mode, istr);
	if (s == SKG_QUIT)
	    break;          /* Exit This Loop */

	if (strlen(istr) == 0)   /* If Null String, Insert NewLine */
	{ 
	    isetmark();
	    lnewline(); 
	}
	if (s == SKG_NOCONV) 
	{      /* This means Inserting without Kanji Converting */
	    skg_text_insert(istr, mode);
	    ewprintf("Insert....");        
	    clear_string(istr);
	}
	else                       /* Insert with Kanji Converting */
	{
	    if (skg_convert_string(mode, istr, dstr) == SKG_INSERT)
	    {          /* A_MODE means without KANA-Converting */  
		skg_text_insert(dstr, A_MODE);
		ewprintf("Insert....");        
		clear_string(istr);
	    }
	    clear_string(dstr);
	}
    } /* Skg Loop END */

    ewprintf("Quit..."); 
    skg_finish();
    
    in_skg = FALSE;
    return TRUE;
}

static int
convert_to_hiragana(dstr, keystr, size)
    char *dstr, *keystr;
{
    register char *ptr = keystr;
    clear_string(dstr);

    while (*ptr)
    {
	register ROMANTBL *romanptr = romantbl;
	while (romanptr < romantbl_end)
	{
	    if (compare_string(ptr, romanptr->roman))
	    {
		strcat(dstr, romanptr->kana);
		ptr += strlen(romanptr->roman);
		break;
	    }
	    romanptr++;
	}
	
	if (romanptr == romantbl_end && *ptr)
	{
	    if (*ptr == *(ptr+1))	/* for SOKUON */
	    {
		if (ISUPPER(*ptr))
		    strcat(dstr, KATA_SMALL_TSU);
		else
		    strcat(dstr, HIRA_SMALL_TSU);
		ptr++;
	    }
	    else			/* cannot convert to KANA */
	    {
		strcat(dstr, ptr);
		return FALSE;
	    }
	}
    }   
    return TRUE;
}

static int
skg_init()
{
    int i,c;
    FILE *romanfile;
    ROMANTBL *romanptr;
    char bufstr[KANA_MAXLEN], bufkey[ROMAN_MAXLEN];

    /* Open dictionary file */
    dicfile = fopen((dicname!=NULL) ? dicname : DEFAULT_DICNAME , "rb");
    if (dicfile == NULL) 
    { 
	ewprintf("Can't Open Dictionary File...");
	return FALSE;
    }
    ewprintf("Initialize Dictionary Index...");
    if (make_dicindex(dicfile) == FALSE)
    { 
	ewprintf("Can't Make Dictionary Index...");
	fclose(dicfile);
	return FALSE;
    }

    /* Loading Roman->Kana table */
    romanfile = fopen((romanname!=NULL) ? romanname : DEFAULT_ROMANNAME, "r");
    if (romanfile == NULL) 
    { 
	fclose(dicfile);
	ewprintf("Can't Open Roman Table File...");
	return FALSE;
    }
    
    /* counting entrys */
    i = 0;
    while ((c=fgetc(romanfile)) != EOF)
	if (c == '\n')
	    i++;

    /* make Roman->Kana table */
    romanptr = romantbl = (ROMANTBL*)malloc(sizeof(ROMANTBL)*i);
    rewind(romanfile);
    romantbl_size = 0;
    while (romanptr<(romantbl+i) &&
	   fscanf(romanfile, "%s %s", bufkey, bufstr) != EOF)
    {
	if (bufkey[0]=='\0' || bufstr[0]=='\0')
	    continue;
	strncpy(romanptr->roman, bufkey, ROMAN_MAXLEN);
	romanptr->roman[ROMAN_MAXLEN-1] = '\0';
	strncpy(romanptr->kana,  bufstr, KANA_MAXLEN);
	romanptr->kana[KANA_MAXLEN-1]   = '\0';
	romanptr++;
	romantbl_size++;
	clear_string(bufkey);
	clear_string(bufstr);
    }
    romantbl_end = romantbl + romantbl_size;
    fclose(romanfile);
    
    return TRUE;
}

static VOID
skg_finish()
{
    fclose(dicfile);
    free(romantbl);
    romantbl_size = 0;
}

static VOID
skg_text_insert(insert_str, mode)
    char *insert_str;
    int mode;
{
    register int c;
    static char bufstr[KEY_BUFFER_SIZE]; 
    register char *p = bufstr;
    clear_string(bufstr);

    isetmark();
    if (mode == A_MODE)
	strncpy(bufstr, insert_str, sizeof(bufstr));
    else
	convert_to_hiragana(bufstr, insert_str, sizeof(bufstr));
    bufstr[sizeof(bufstr)-1] = '\0';

    while ((c=*p++)!='\0' && c!='\n')
    {
#ifndef NO_MACRO
	if(macrodef && macrocount < MAXMACRO)
	    macrocount++;
#endif	
	key.k_chars[0] = c;
	key.k_count = 1;
#ifdef	UNDO
	ublock_open(curbp);
#endif
	selfinsert(FFRAND, 1);
#ifdef	UNDO
	ublock_close(curbp);
#endif
	/* Flag reset */
	lastflag = thisflag;
	thisflag = 0;
    }

    update();
}

static int
skg_input_string(mode, istr)
    int  *mode; 
    register char *istr;
{
    register int c;
 
    skg_display_prompt(istr, *mode); /* Display SKG-Prompt on MiniBuffer */
    update();    /* Display Current Cursor */
    ttwait();   

    while (TRUE)
    {
#ifdef	MOUSE
	allow_mouse_event = TRUE;
#endif
	c = getkey(FALSE); 
#ifdef	MOUSE
	allow_mouse_event = FALSE;
#endif
	switch(c)
	{ 
	    /* Ret-Key, Insert String without Kanji-Converting */
	  case CCHR('M'):
	    return SKG_NOCONV;

	    /* C-g, Abort. If istr NULL,then quit skg-mode. */
	  case CCHR('G'):
	    if (strlen(istr) == 0)
	    {
		clear_string(istr);
		return SKG_QUIT;
	    }            /* Else Clear istr. */
	    else
		clear_string(istr);
	    break;

	  case ' ':
	    if (strlen(istr) == 0)
	    {
		strcpy (istr, " ");
		return SKG_NOCONV;
	    }
	    return SKG_CONV;

	  case CCHR('\\'):
	    *mode = (*mode == A_MODE) ? H_MODE : A_MODE;
	    break;

	  case CCHR('H') :
	  case CCHR('?') :
	    if (strlen(istr) != 0)
	    {
		istr[strlen(istr)-1] = '\0';
		break;
	    }
	    /*FALLTHRU*/

	  default :
	    if (c==(c&0xFF) && !ISCTRL((char)c)) 
		strcat_c(istr, c);
	    else if (strlen(istr) == 0)
	    {
		ungetkey(c);
		if (doin() != TRUE)
		{
		    ttbeep();
#ifdef  KANJI
		    kgetkeyflush();
#endif
#ifndef NO_MACRO
		    macrodef = FALSE;
#endif
		    ttwait();
		}
		/* Flag reset */
		lastflag = thisflag;
		thisflag = 0;
	    }
	    else
	    {
		ttbeep(); 
		ewprintf("Help: RET = Insert, SPC = Convert,"
			 " ^G = Quit, ^\\ = Mode");
		ttwait();
	    }
	}
	skg_display_prompt(istr, *mode);
	if (strlen(istr) == 0)
	    update();
    }
    
    return SKG_NOCONV;	/* this is dummy! */
}

static char *
next_target(dictionary_list, target, dstr)
    WORDLIST dictionary_list;
    register char *target, *dstr;
{
    if (target == NULL)
	target = dictionary_list + 1;
    else {
	target++;
	if (*target=='\0' || *target == '\n' || *target == '\r')
	    target = dictionary_list + 1;
    }
    while (*target != '/')
	*dstr++ = *target++;
    *dstr = '\0';
    return target;
}

static char *
prev_target(dictionary_list, target, dstr)
    WORDLIST dictionary_list;
    register char *target, *dstr;
{
    if (target == (char *)dictionary_list) 
	target = dictionary_list + strlen(dictionary_list) - 1;
    for (target-- ; *target != '/' ; target--);

    if (target == (char *)dictionary_list) 
	target = dictionary_list + strlen(dictionary_list) - 1;
    for (target-- ; *target != '/' ; target--);
    target++;

    while (*target != '/')
	*dstr++ = *target++;
    *dstr = '\0';

    return target;
}

static int
skg_convert_string(mode, istr, dstr)
  int  mode;
  char *istr,*dstr;
{
    WORDLIST dictionary_list;
    static char rstr[KEY_BUFFER_SIZE];
    register char *target;
    int c;
 
    search_dictionary(mode, istr, dictionary_list, rstr);
    clear_string(dstr); 
    target = next_target(dictionary_list, NULL, dstr);
    target_display(dstr, rstr);
    
    for (;;)
    {
	switch (c = getkey(FALSE))
	{
	  case  CCHR('G'):
	  case  CCHR('H'):
	  case  CCHR('?'):
	    (VOID)ctrlg(FFRAND, 0);
	    return SKG_QUIT; 

	  case  CCHR('M'):
	    strcat(dstr , rstr);
	    return SKG_INSERT;

 	  case  CCHR('N'):
	  case  ' ':
	    target = next_target(dictionary_list, target, dstr);
	    target_display(dstr, rstr);
	    break;

 	  case  CCHR('P'):
	    target = prev_target(dictionary_list, target, dstr);
	    target_display(dstr, rstr);
	    break;

	  case  CCHR('L'):
	    update();
	    ttwait();
	    break;

	  default:
	    if (!ISCTRL(c)) {
		ungetkey(c);
		strcat(dstr, rstr);
		return SKG_INSERT;
	    }
	    ttbeep();
	    ewprintf("Help: RET=Insert, SPC ^N = Next, "
		     "^P = Prev, ^G = Quit, ^L = Location");
	    ttwait();
	    break;
	}
    }
    return SKG_QUIT;
}

static VOID
target_display(dstr, rstr)
    char *dstr,*rstr;
{
    ewprintf("Skg[--]?|%s|%s", dstr, rstr);
}

static VOID
skg_display_prompt(istr, mode)
    char *istr; 
    int mode;
{
    static char bufstr[KEY_BUFFER_SIZE];
    
    if (mode == A_MODE)
	ewprintf("Skg[*a]:%s", istr);
    else
    {
	convert_to_hiragana(bufstr, istr, sizeof(bufstr));
	ewprintf("Skg[" HIRA_A "]:%s", bufstr);
    }
}

static char *
dic_next_target(target, dstr)
    char *target;
    char *dstr;
{
    target++;
    if (*target == '\0' || *target == '\n')
	return NULL;
    while (*target != '\0' && *target != '/')
	*dstr++ = *target++;
    *dstr = '\0';

    return target;
}

/*XXX*/
static char *
makedic(tlist, trest, rlength, result)
    char *tlist, *trest;
    int rlength;
    char *result;
{
    register char *p;
    static char tmpstr[KEY_BUFFER_SIZE];

    clear_string(tmpstr);
    clear_list(result);

    for (p=dic_next_target(tlist, tmpstr); p != NULL;
	 p=dic_next_target(p, tmpstr))
    {
	strcat(tmpstr, trest);
	if (strlen(result)+strlen(tmpstr)+1 > DIC_BUFFER_SIZE)
	    break;

	strncat(result, tmpstr, strlen(tmpstr)-rlength);
	strcat(result , "/");
	clear_string(tmpstr);

    }
    
    return result;
} 

static struct {
    char list[DIC_BUFFER_SIZE];
    char rest[KEY_BUFFER_SIZE];
} ctarget[MAX_TARGET];

static VOID
clear_ctarget()
{
    register int i;

    for (i=0; i<MAX_TARGET; i++) 
    {
	clear_list(ctarget[i].list);
	clear_string(ctarget[i].rest);
    }

}

static VOID
search_dictionary(mode, istr, dlist, rstr)
    int mode;
    char *istr,*rstr;
    WORDLIST dlist;
{
    static char tmpstr [KEY_BUFFER_SIZE];
    static char tmprstr[KEY_BUFFER_SIZE];
    static char tmplist[DIC_BUFFER_SIZE];

    int first_key, second_key;
    register int match_strings = 0, target_number = 0;
    
    ewprintf("Now Scanning Dictionary..."); 

    clear_list(dlist);
    clear_string(rstr);
    clear_ctarget();

    setup_keystring(mode, istr);
    first_key  = dickeystr[0].hiragana[0]; 
    second_key = dickeystr[0].hiragana[1]; 

    /* rewind(dicfile); */
    wind_dicindex(first_key, second_key, dicfile);
    while (get_scanf(first_key, second_key, dicfile, tmpstr, tmplist))
    {	
	if (compare_keystring(istr, tmpstr, tmprstr, mode))
	{
	    match_strings++;
	    ewprintf("(Matching %d targets)", match_strings); 
	    
	    strcpy(dlist, tmplist); 
	    strcpy(rstr,  tmprstr); 
	    if (target_number < MAX_TARGET) 
	    {
		strcpy(ctarget[target_number].list, tmplist);
		strcpy(ctarget[target_number].rest, tmprstr);
		target_number++;
	    }
	}
    }

    if (strlen(dlist) == 0) 
    {
	if (mode == H_MODE) 
	{
	    convert_to_hiragana(tmpstr, istr, sizeof(tmpstr));
	    sprintf(dlist, "/%s/", tmpstr);
	    clear_string(rstr);
	}
	else
	{
	    sprintf(dlist, "/%s/", istr);
	    clear_string(rstr);
	}
    }
    else
    {
	if (target_number > 1)
	{
	    int i;
	    if (target_number < match_strings)
		i = MAX_TARGET - 1;
	    else
		i = target_number - 2;          
	    
	    for (; i>=0; i--)
	    {
		ewprintf( "Concatenate..." );

		makedic(ctarget[i].list, ctarget[i].rest, 
			strlen(rstr), tmplist); 
		if (strlen(dlist)+strlen(tmplist)+1 > DIC_BUFFER_SIZE)
		    break;
		strcat(dlist, tmplist);
	    }
	}
    }
    return;
}

static VOID
setup_keystring(mode, keystr)
    int mode;
    register char *keystr;
{
    register DICKEYSTR *dickeyptr = dickeystr;
    
    if (mode == A_MODE)
    {
	while (*keystr)
	{
	    dickeyptr->hiragana[0] = *keystr++;
	    dickeyptr->hiragana[1] = '\0';
	    dickeyptr->length = 1;
	    dickeyptr->flg = TRUE;
	    dickeyptr++;
	}
    }
    else
    {
	char *keystr_end = keystr+strlen(keystr);
	register char *ptr = keystr;
	
	while (ptr < keystr_end)
	{
	    register ROMANTBL *romanptr = romantbl;
	    while (romanptr < romantbl_end)
	    {
		if (compare_string(ptr, romanptr->roman))
		{
		    int len = strlen(romanptr->roman);
		    strcpy(dickeyptr->hiragana, romanptr->kana);
		    dickeyptr->length = len;
		    dickeyptr->flg    = TRUE;
		    dickeyptr++; 
		    ptr += len;
		    break;
		}
		romanptr++;
	    }
	    
	    if (romanptr == romantbl_end)
	    {
		if (ptr < keystr_end)
		{
		    if (*ptr != *(ptr+1))
			break;

		    if (ISUPPER(*ptr))
			strcpy(dickeyptr->hiragana , KATA_SMALL_TSU);
		    else
			strcpy(dickeyptr->hiragana , HIRA_SMALL_TSU);
		    dickeyptr->length = 1;
		    dickeyptr->flg = TRUE;
		    dickeyptr++;
		    ptr++;
		}
	    }
	}    
	while (ptr < keystr_end)
	{
	    dickeyptr->hiragana[0] = *ptr++;
	    dickeyptr->hiragana[1] = '\0';
	    dickeyptr->length = 1;
	    dickeyptr->flg = FALSE;
	    dickeyptr++;
	}
    }
    
    /* end mark */
    dickeyptr->hiragana[0] = '\0';
    dickeyptr->length = 0;
    dickeyptr->flg = FALSE;
}

static char *
convert_to_n_hiragana(dstr, keystr, length)
    char *dstr;
    register char *keystr;
    int length;
{
    register int cpnt,clen,klen;
    int cmode;
    register DICKEYSTR *dickeyptr;

    clear_string(dstr);	    

    cpnt = 0; klen = 0;
    clen  = length / 2;
    cmode = length % 2;

    dickeyptr = dickeystr;
    while (klen < clen)
    {
	if (!dickeyptr->flg)
	    break;
	strcat(dstr, dickeyptr->hiragana);
	cpnt += dickeyptr->length;
	klen += strlen(dickeyptr->hiragana) / 2; 
	dickeyptr++;
    }

    if (klen < clen)
    {
	strcat(dstr, &keystr[cpnt]);
	cpnt += strlen(&keystr[cpnt]);
    }
    else 
    {
	if (cmode && keystr[cpnt]!='\0') 
	    strcat_c(dstr, keystr[cpnt]);
    }
    
    return (&keystr[cpnt]);
}

static int
compare_keystring(istr, tmpstr, tmprstr, mode)
    char *istr, *tmpstr, *tmprstr; 
    int mode;
{
    char cstr[KEY_BUFFER_SIZE];
    clear_string(tmprstr);
    
    if (mode == A_MODE)
    {
	if (compare_string(istr, tmpstr))
	{
	    strcpy(tmprstr, &istr[strlen(tmpstr)]);
	    return TRUE;
	}
	else
	    return FALSE;
    }
    else
    {
	istr = convert_to_n_hiragana(cstr, istr, strlen(tmpstr));
	if (compare_string(cstr, tmpstr))
	{
	    convert_to_hiragana(tmprstr, istr, sizeof(tmprstr));
	    return TRUE;
	}
	else
	    return FALSE;
    }
}

skg_set_romanname(f, n)
{
    char file[NFILEN];
    int s;

    if (in_skg) {
	ewprintf("Cannot change Dictionary while SKG is running");
	return FALSE;
    }
#ifdef	EXTD_DIR
    ensurecwd();
    edefset(curbp->b_cwd);
#endif
#ifndef	NO_FILECOMP
    if ((s = eread("SKG roman-kana dictionary: ", file, NFILEN, EFNEW|EFFILE|EFCR)) != TRUE)
#else
    if ((s = ereply("SKG roman-kana dictionary : ", file, NFILEN)) != TRUE)
#endif
	return s;

    if (romanname)
	free(romanname);
    if ((romanname=malloc(strlen(file)+1)) == NULL) {
	ewprintf("Cannot allocate %d bytes", strlen(file)+1);
	return FALSE;
    }
    strcpy(romanname, file);
    return TRUE;
}


skg_set_dicname(f, n)
{
    char file[NFILEN];
    int s;

    if (in_skg) {
	ewprintf("Cannot change Dictionary while SKG is running");
	return FALSE;
    }
#ifdef	EXTD_DIR
    ensurecwd();
    edefset(curbp->b_cwd);
#endif
#ifndef	NO_FILECOMP
    if ((s = eread("SKG kana-kanji dictionary: ", file, NFILEN, EFNEW|EFFILE|EFCR)) != TRUE)
#else
    if ((s = ereply("SKG kana-kanji dictionary : ", file, NFILEN)) != TRUE)
#endif
	return s;
   
    if (dicname)
	free(dicname);
    if ((dicname=malloc(strlen(file)+1)) == NULL) {
	ewprintf("Cannot allocate %d bytes", strlen(file)+1);
	return FALSE;
    }
    strcpy(dicname, file);
    return TRUE;
}

#endif
