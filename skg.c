/* $Id: skg.c,v 1.2 2000/06/27 01:59:42 amura Exp $ */
/* - For Kg Ver.4.1.0 -                                     */
/* Simple Kanji Generator functions for MicroGnuEmacs(Kg)   */
/* for AMIGA series with ANS,DaiGokai(above ver.0.40).      */
/*   original written by Hirokazu Konish                    */
/*   rewrite for All ports and some patched by amura        */

/*
 * $Log: skg.c,v $
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

#define DIC_BUFFER_SIZE 512
#define KEY_BUFFER_SIZE 80

				/* Dictionary List Buffer */
static char dictionary_list[DIC_BUFFER_SIZE];

                      /* Key String for Searcing Dictionary */
static struct DICKEYSTR {   
    char hiragana[5]; 
    int  length;
    int  flg;
} dickeystr[KEY_BUFFER_SIZE];   

#define H_MODE 0   /* Hiragana MODE */
#define A_MODE 1   /* Ank MODE,or NOT-Convert MODE  */

char romanname[NFILEN] = "SKG-ROMAN";
char dicname[NFILEN]   = "SKG-JISYO";

extern int isetmark();
static int skg_check_exist_file();
static int skg_input_string();
static VOID skg_text_insert();
static int skg_convert_string();
static VOID skg_display_prompt();
static VOID search_dictionary();
static VOID target_display();
static VOID setup_keystring();
static int compare_keystring();

/* 
 *  Primitive functions for Strings 
 *
 */

/* Clear Key Buffer String */
static VOID
clear_string( cstr )
 char *cstr;
{
    int i;

    for (i=0; i < KEY_BUFFER_SIZE ; i++) cstr[i] = '\0';

}

/* Clear Dictinary Buffer String */
static VOID
clear_list( clist )
  char *clist;
{
    int i;
    
    for (i=0; i < DIC_BUFFER_SIZE ; i++) clist[i] = '\0';

}

/* Comparing Two Strings */
static int compare_string( s1 , s2 )
 char *s1,*s2;
{
    while( *s2 != '\0' )
    {
	if ( *s1 != *s2 ) return FALSE;
	s1++;s2++;
    }
    return TRUE;
}

/* Scanf for Dictionary File, with First Character Comparing  */
/*                                                            */
/*  Return Code is,which SKIP(=10), EOF(=?), TRUE(=?)         */
/*  dflg is Searching State,0 is Before Matching First Entry, */
/*  1 is Under Matching, 2 is Over Matching ( when Searching  */
/*  is complete.return EOF).                                  */ 
/*  k1,k2 is First Character in Key String. In not matching,  */
/*  skip entry.ZENKAKU-Code consists of 2 bytes.              */

#define SKIP  10

static int
get_scanf( k1, k2 , f , s1 , s2 , dflg )
 int k1,k2;
 FILE *f;
 char *s1,*s2;
 int *dflg;
{
    int flg=0,i;
    int rbuf;
    
    /* If Searching State is 2, Searching is complete. */
    if ( *dflg == 2 ) return EOF;

    /* Checking First Character */
    rbuf = fgetc( f );
    if ( rbuf==EOF )
	return EOF;
    else 
    {
	/* If Not Matching Character k1, Skip This Entry */
	if ( rbuf != (unsigned char)k1 ) 
	{
	    while( (rbuf=fgetc( f ))!=0x0a );
	    /* If Now Searching State is 1,Then to 2 */ 
	    if ( *dflg == 1 ) *dflg = 2;
	    return SKIP;
	}
    }

    /* Stock read character to s1[] */
    i=0;      
    s1[i]=(char )rbuf;i++;

    /* Checking Second Character */
    if (k2 != 0x00 )
    {
	/* Case of Key String is Not 1 char */
	rbuf=fgetc( f );
	if ( rbuf==EOF )
	    return EOF;
	else 
	{   
	    /* If Next Character is SPACE, Skip Comparing to k2 */
	    if ( rbuf == 0x20 )
		flg = 1;
	    else
	    {
		if ( rbuf != (unsigned char)k2 ) 
		{
		    /* If Not Matching Character k2, Skip This Entry */
		    while( (rbuf=fgetc( f ))!=0x0a );
		    if ( *dflg == 1 ) *dflg = 2;
		    return SKIP;
		}
		s1[i]=(char )rbuf;i++;
	    }
	}
    }

    /* Matching First Lattice,Get Following Strings Until SPACE-Code */
    /* If dflg is 0, change dflg to 1 */
    /* This means now Under Matching State */
    if ( *dflg == 0 ) *dflg = 1;  
    
    /* flg equal to 1 means that already got SPACE */
    /*   Code, Next to get Dictionary entry.       */
    while ( flg==0 )   
    {
	/* get keystring, Until EOF,or SPACE */
	if ( (rbuf=fgetc( f )) != EOF ) 
	{                   
	    if ( rbuf == 0x20 )
		flg=1;
	    else 
	    {
		s1[i]=(char )rbuf;i++;
	    }
	}
	else
	    flg = 2;
    }

 
    if (flg == 2 ) return EOF;

    /* Clear Rest Buffer Area with NULL */
    for ( ;i<KEY_BUFFER_SIZE ; i++ ) s1[i] = '\0';

    /* Next Get Dictionary Entry */
    flg = 0; i = 0;

    while ( flg==0 )
    {
	if ( (rbuf=fgetc( f )) != EOF ) 
	{
	    if ( rbuf == 0x0a )
		flg = 1; 
	    else 
	    {
		s2[i]=(char )rbuf;i++;
	    }
	}
	else
	    flg = 2;
    }
    
    for( ;i<DIC_BUFFER_SIZE ; i++ ) s2[i] = '\0';
    
    if (flg == 2 )
	return EOF;
    else 
	return TRUE;
}

skginput(f , n)
{
    static char istr[KEY_BUFFER_SIZE];
    static char dstr[KEY_BUFFER_SIZE];
    int mode = H_MODE, flg = 0;
    static int in_skg = FALSE;

#ifdef	READONLY			/* 91.01.05  by S.Yoshida */
    if (curbp->b_flag & BFRONLY) {	/* If this buffer is read-only, */
	warnreadonly();		/* do only displaying warning.	*/
	return FALSE;
    }
#endif	/* READONLY */

    /* If Not Exist files what need, Quit. */  
    if (skg_check_exist_file()==FALSE) return FALSE; 
    if (in_skg) return FALSE;

#ifdef	UNDO
    undoptr = NULL;
#endif
    clear_string( istr );                    /* Clear Input Buffer. */ 
    in_skg = TRUE;
    
    for(;;)              /* Skg Loop */
    {
	if ( skg_input_string(&flg,&mode,istr) == 1 )
	{                                     /* This means Quit */ 
	    break;          /* Exit This Loop */
	}
	if( strlen(istr) == 0 )   /* If Null String,Insert NewLine */
	{ 
	    isetmark();
	    lnewline(); 
	}
	
	if ( flg == 1 ) 
	{      /* This means Inserting without Kanji Converting */
	    skg_text_insert( istr , mode );
	    ewprintf("Insert....");        
	    clear_string( istr );
	    flg = 0;
	}
	else
	{                       /* Insert with Kanji Converting */
	    /* This Value 0 means Not Quit */
	    if (skg_convert_string(mode,istr,dstr) == 0)
	    {          /* A_MODE means without KANA-Converting */  
		skg_text_insert( dstr, A_MODE );
		ewprintf("Insert....");        
		clear_string( istr );
	    }
	    clear_string( dstr );
	}
    }  
    /* Skg Loop */

    ewprintf("Quit..."); 
    in_skg = FALSE;
    return TRUE;
}

static
int convert_to_hiragana( keystr , dstr )
  char *keystr,*dstr;
{
    FILE *romanfile;
    
    char bufkey[5],bufstr[5];
    int flg=0,sflg,cpnt;
    bufkey[0] = bufstr[0] = '\0';

    romanfile = fopen(romanname , "r");

    clear_string( dstr );	    
    cpnt = 0;

    while (( cpnt< strlen(keystr) )&&(flg==0))
    {
	rewind( romanfile );
	sflg = FALSE;
	
	while (fscanf( romanfile ,"%s %s",bufkey, bufstr  ) != EOF )
	{ 
	    if( compare_string(&keystr[cpnt],bufkey ) )
	    {
		strcat ( dstr , bufstr );
		cpnt  += strlen( bufkey );
		sflg = TRUE;
		break;
	    }
	}
	
	if(sflg==FALSE) 
	{
	    if( cpnt < strlen(keystr) )
	    {
		if ( keystr[cpnt+1] == keystr[cpnt] )
		{
		    if ( ISUPPER( keystr[cpnt] ) != 0 )
			strcat( dstr , "ッ" );
		    else
			strcat( dstr , "っ" );
		    cpnt++;
		}
		else 
		{
		    strcat(dstr , &keystr[cpnt] );
		    cpnt += strlen( &keystr[cpnt] );
		    flg = 1;
		}                 
	    }
	}
    }   
    
    fclose( romanfile );   
    return flg;
}

static
int skg_check_exist_file()
{
    FILE *romanfile,*dicfile;

    dicfile = fopen(dicname , "r");
    
    if ( dicfile == NULL ) 
    { 
	ewprintf("Can't Open Dictionary File...");
	return FALSE;
    }
    fclose(dicfile);
  
    romanfile = fopen(romanname , "r");

    if ( romanfile == NULL ) 
    { 
	ewprintf("Can't Open Roman Table File...");
	return FALSE;
    }
    fclose(romanfile);
    
    return TRUE;
}

static VOID
skg_text_insert( insert_str , mode )
    char *insert_str;
    int mode;
{
    int c,i;
    static char bufstr[KEY_BUFFER_SIZE]; 

    clear_string ( bufstr );

    isetmark();
    if (mode == A_MODE )
	strcpy( bufstr , insert_str );
    else
	convert_to_hiragana( insert_str , bufstr );

    for (i = 0 ; c = bufstr[i],((c!='\0')&&(c!='\n')) ; i++)
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
#endif	UNDO
	lastflag = thisflag;
	if ( i+1 == KEY_BUFFER_SIZE ) break;
    }

    update();
}

static
int skg_input_string( flg, mode, istr )
 int  *flg;
 int  *mode; 
 char *istr;
{
    char insert_c[2];
    register int c;
    insert_c[0] = '\0';
 
    skg_display_prompt(istr, *mode ); /* Display SKG-Prompt on MiniBuffer */ 
    update();    /* Display Current Cursor */
    ttwait();   

    for ( ;; )
    { 
	switch( c = getkey(FALSE) )
	{ 
	    /* Ret-Key, Insert String without Kanji-Converting */
	  case CCHR('M'):
	    *flg = 1; return 0;  

	    /* C-g, Abort. If istr NULL,then quit skg-mode. */
	  case CCHR('G'):
	    if ( strlen( istr ) == 0 )
	    {
		clear_string( istr );
		return 1;
	    }            /* Else Clear istr. */
	    else
		clear_string( istr );
	    break;

	  case ' ':
	    if ( strlen(istr) == 0 )
	    {
		*flg = 1; strcpy (istr, " ");
		return 0;
	    }
	    else 
	    {
		*flg = 0; return 0; 
	    }
	    break; 

	  case CCHR('\\'):
	    *mode = 1 - *mode;
	    break;

	  case CCHR('H') :
	  case CCHR('?') :
	    if ( strlen(istr) == 0 )
		goto rawkey;
	    istr[strlen(istr)-1] = '\0';
	    break;

	  default :
	    if (!ISCTRL((char) c)) 
	    { 
		sprintf( insert_c , "%c",c ); 
		strcat( istr , insert_c );
	    }
	    else if (strlen(istr) == 0)
	    {
rawkey:
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
	    }
	    else
	    {
		ttbeep(); 
		ewprintf("Help: RET = Insert, SPC = Convert, ^G = Quit, ^\\ = Mode");
		ttwait();
	    }
	}
	skg_display_prompt(istr,*mode );
	if (strlen(istr) == 0)
	    update();
    }
    
    return 0;
}

static
char *next_target( target ,dstr )
char *target, *dstr;
{
    int i;
    
    target++;
    if((*target == '\0')||(*target == '\n')) target = dictionary_list + 1;
    for(i=0 ; *target!='/' ; target++,i++ ) dstr[i]= *target;
    dstr[i]='\0';
    
    return target;
}

char *prev_target( target ,dstr )
 char *target,*dstr;
{
    int i;
    
    if( target == dictionary_list) 
	target = dictionary_list + strlen( dictionary_list ) - 1;
    for( target-- ; *target != '/' ; target-- );

    if( target == dictionary_list) 
	target = dictionary_list + strlen( dictionary_list ) - 1;
    for( target-- ; *target != '/' ; target-- );

    target++;
    for(i=0 ; *target!='/' ; target++,i++ ) dstr[i]= *target;
    dstr[i]='\0';

    return target;
}

static int
skg_convert_string(mode , istr , dstr )
  int  mode;
  char *istr,*dstr;
{
    static char rstr[KEY_BUFFER_SIZE];
    char *target;
    int c;
 
    search_dictionary( mode , istr , dictionary_list ,rstr );

    clear_string( dstr ); 
    target = dictionary_list;
    
    target = next_target( target , dstr );
    target_display(dstr , rstr );

    for( ;; )
    {
	switch ( c = getkey( FALSE ) )
	{
	  case  CCHR('G'):
	  case  CCHR('H'):
	  case  CCHR('?'):
	    (VOID )ctrlg( FFRAND , 0 ); 
	    return 1; 

	  case  CCHR('M'):
	    strcat( dstr , rstr );
	    return 0;

 	  case  CCHR('N'):
	  case        ' ':
	    target = next_target( target, dstr );
	    target_display(dstr , rstr );
	    break;

 	  case  CCHR('P'):
	    target = prev_target( target, dstr );
	    target_display(dstr , rstr );
	    break;

	  case  CCHR('L'):
	    update();
	    ttwait();
	    break;

	  default:
	    if (!ISCTRL(c)) {
		ungetkey(c);
		strcat( dstr , rstr );
		return 0;
	    }
	    ttbeep();
	    ewprintf("Help: RET=Insert, SPC ^N = Next, ^P = Prev, ^G = Quit, ^L = Location");
	    ttwait();
	    break;
	}
    }
    return 1;
}

static VOID
target_display(dstr , rstr )
     char *dstr;
     char *rstr;
{
    ewprintf("Skg[--]?|%s|%s",dstr,rstr );
}

static VOID
skg_display_prompt( istr , mode )
  char *istr; 
  int mode;
{
    static char bufstr[KEY_BUFFER_SIZE];
    
    if ( mode == A_MODE )
    {
	ewprintf("Skg[*a]:%s",istr );
    }      
    else
    {
	convert_to_hiragana( istr , bufstr );  
	ewprintf("Skg[あ]:%s",bufstr );
    }
}

static char *
dic_next_target( tlist, target ,dstr )
     char *tlist;
     char *target;
     char *dstr;
{
    int i;
    
    target++;
    if((*target == '\0')||(*target == '\n')) return NULL;
    for(i=0 ; *target!='/' ; target++,i++ ) dstr[i]= *target;
    dstr[i]='\0';

    return target;
}

static char *
makedic( tlist, trest , rlength , result )
 char *tlist, *trest;
 int rlength;
 char *result;
{
    char *p;
    static char tmpstr[KEY_BUFFER_SIZE];

    clear_string( tmpstr );
    clear_list( result );

    p = tlist;
    p = dic_next_target( tlist, p , tmpstr );

    for(  ; p != NULL ; p = dic_next_target( tlist, p , tmpstr ) )
    {
	strcat ( tmpstr, trest );
	if ( strlen( result) + strlen (tmpstr) + 1 < DIC_BUFFER_SIZE )
	{
	    strncat ( result , tmpstr , strlen( tmpstr ) - rlength );
	    strcat ( result , "/");
	    clear_string( tmpstr );
	}
	else break;
    }
    
    return ( result );
    
} 

static struct {
    char list[DIC_BUFFER_SIZE];
    char rest[KEY_BUFFER_SIZE];
} ctarget[5];

static VOID
clear_ctarget()
{
    int i;

    for( i = 0 ; i<5 ; i++ ) 
    {
	clear_list( ctarget[i].list );
	clear_string( ctarget[i].rest );
    }

}

static VOID
search_dictionary( mode, istr, dlist, rstr )
 int mode;
 char *istr, *dlist, *rstr;
{
    static char tmpstr [KEY_BUFFER_SIZE];
    static char tmprstr[KEY_BUFFER_SIZE];
    static char tmplist[DIC_BUFFER_SIZE];

    int first_key,second_key;
    
    FILE *dicfile;

    int match_strings = 0,
	target_number = 0,
	scan_flg,
	dflg = 0;

    dicfile = fopen(dicname , "rb");

    ewprintf("Now Scanning Dictionary..."); 

    clear_list( dlist );
    clear_string( rstr );
    clear_ctarget();

    setup_keystring( mode,istr );
    first_key = (unsigned int )dickeystr[0].hiragana[0]; 
    second_key = (unsigned int )dickeystr[0].hiragana[1]; 

    while( (scan_flg = 
	    get_scanf( first_key ,second_key, dicfile ,
		      tmpstr , tmplist , &dflg )
	    ) != EOF ) 
    {
	if ( scan_flg == TRUE )
	{	
	    if(compare_keystring(istr , tmpstr,tmprstr,mode))
	    {
		match_strings++;
		ewprintf("(Matching %d targets)",match_strings); 
		
		strcpy( dlist ,tmplist ); 
		strcpy( rstr  ,tmprstr ); 
		if ( target_number < 5 ) 
		{
		    strcpy ( ctarget[target_number].list, tmplist );
		    strcpy ( ctarget[target_number].rest, tmprstr );
		    target_number++;
		}
	    }
	}
    }
    if ( strlen( dlist ) == 0 ) 
    {
	if ( mode == H_MODE) 
	{
	    convert_to_hiragana( istr , tmpstr );
	    strcpy( dlist , "/");
	    strcat( dlist ,tmpstr );
	    strcat(dlist,"/");
	    clear_string( rstr );
	}
	else
	{
	    strcpy( dlist , "/"); strcat( dlist ,istr ); strcat(dlist,"/");
	    clear_string( rstr );
	}
    }
    else
    {
	if ( target_number > 1 )
	{
	    int i;
	    
	    if ( target_number < match_strings )  i = 4;
	    else  i = target_number - 2;          
	    
	    for (  ; i >= 0 ; i-- )
	    {
		ewprintf( "Concatenate..." );

		makedic( ctarget[i].list, ctarget[i].rest , 
			strlen ( rstr ) , tmplist ); 
		if( strlen( dlist ) + strlen( tmplist ) < DIC_BUFFER_SIZE )
		    strcat( dlist, tmplist );
		else
		    break;
	    }
	}
    }
    
    fclose( dicfile );    
    return;
}

static VOID
setup_keystring( mode , keystr )
  int mode;
  char *keystr;
{
    FILE *romanfile;

    char bufkey[5],bufstr[5];
    int  cpnt,flg,sflg, i,j;
    bufkey[0] = bufstr[0] = '\0';
    
    for ( i=0; i< KEY_BUFFER_SIZE ;i++ )
    {
	for( j=0 ; j<3 ;j++ ) dickeystr[i].hiragana[j]='\0';
	dickeystr[i].length = 0;
    }

    if ( mode == A_MODE )
    {
	for( i = 0; i < strlen( keystr ); i++ )
	{
	    sprintf( bufkey,"%c", keystr[i] );
	    strcat ( dickeystr[i].hiragana , bufkey );
	    dickeystr[i].length = 1;
	    dickeystr[i].flg = 1;
	}
	return;
    }

    romanfile = fopen(romanname , "r");

    cpnt = 0;
    i = 0;
    flg = 0;
    
    while ( (cpnt< strlen(keystr))&&(flg == 0))
    {
	rewind( romanfile );
	sflg = FALSE;

	while (fscanf( romanfile ,"%s %s",bufkey, bufstr  ) != EOF )
	{ 
	    /*
	     if( (char *)strstr(&keystr[cpnt],bufkey ) == (char *)(&keystr[cpnt]) )
	     */
	    if( compare_string(&keystr[cpnt],bufkey ) )
	    {
		strcat ( dickeystr[i].hiragana , bufstr );
		dickeystr[i].length = strlen( bufkey );
		dickeystr[i].flg = 1;
		cpnt  += strlen( bufkey );
		sflg = TRUE; i++; 
		break;
	    }
	}
	
	if(sflg==FALSE) 
	{
	    if( cpnt < strlen(keystr) )
	    {
		if ( keystr[cpnt+1] == keystr[cpnt] )
		{
		    if ( ISUPPER ( keystr[cpnt] ) != 0 )
			strcat( dickeystr[i].hiragana , "ッ" );
		    else
			strcat( dickeystr[i].hiragana , "っ" );
		    dickeystr[i].length = 1;
		    dickeystr[i].flg = 1;
		    cpnt++; i++;
		}
		else 
		{
		    flg = 1;
		}                 
  	     }
	}
    }    
    
    if (flg == 1 )
    {
	for ( ; cpnt < strlen(keystr) ; cpnt++,i++ )
	{
	    sprintf( bufstr,"%c",keystr[cpnt]);
	    strcat( dickeystr[i].hiragana, bufstr );
	    dickeystr[i].length = 1;
	    dickeystr[i].flg = 0;
	}
    }
    
    fclose( romanfile );   
}

static char *
convert_to_n_hiragana( keystr , dstr ,length)
  char *keystr,*dstr;
  int length;
{
    char bufkey[5],bufstr[5];
    int flg=0,cpnt,kpnt,clen,cmode,klen;
    bufkey[0] = bufstr[0] = '\0';

    clear_string( dstr );	    
    cpnt = 0; kpnt = 0;
    
    clen  = length / 2;
    cmode = length % 2;

    for ( kpnt = 0,klen = 0 ; ((flg==0)&&(klen < clen )) ; )
    {
	if ( dickeystr[kpnt].flg == 1) 
	{ 
	    strcat( dstr, dickeystr[kpnt].hiragana );
	    cpnt += dickeystr[kpnt].length;
	    klen += ( strlen( dickeystr[kpnt].hiragana ) / 2 ); 
	    kpnt++;
	}
	else 
	    flg = 1;
    }

    if ( flg == 1 ) 
    {
	strcat( dstr, &keystr[cpnt]);
	cpnt += strlen( &keystr[cpnt] );
    }
    else 
    {
	if((cmode==1)&&(keystr[cpnt]!='\0')) 
	{
	    sprintf( bufstr,"%c",keystr[cpnt] );strcat( dstr , bufstr );
	}
    }
    
    return (&keystr[cpnt]);

}

static int
compare_keystring(  istr, tmpstr,tmprstr,mode )
  char *istr,*tmpstr,*tmprstr; 
  int mode;
{
    int icpnt;
    /* char bufkey[5],bufstr[8]; */
    static char cstr[KEY_BUFFER_SIZE];
    
    clear_string( tmprstr );
    
    if (mode == A_MODE )
    {
	/*
	 if((char *)strstr( &istr[0] , tmpstr ) == (char *)(&istr[0]) )
	 */
	if(compare_string( &istr[0] , tmpstr ) )
	{
	    icpnt = strlen(tmpstr);
	    strcpy(tmprstr,&istr[icpnt]);
	    return TRUE;
	}
	else return FALSE;
    }
    else
    {
	istr = convert_to_n_hiragana( istr , cstr , strlen( tmpstr ));
	if( compare_string( &cstr[0] , tmpstr ) )
	{
	    convert_to_hiragana( istr,tmprstr);
	    return TRUE;
	}
	else return FALSE;
    }
    
}

skg_set_romanname(f, n)
{
    char file[NFILEN];
    int s;

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

    strcpy(romanname, file);
    return TRUE;
};


skg_set_dicname(f, n)
{
    char file[NFILEN];
    int s;

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

    strcpy(dicname, file);
    return TRUE;
}

#endif
