/* $Id: canna.c,v 1.1.2.1 2003/02/23 14:17:14 amura Exp $ */

/*
 * Canna Kana-Kanji Henkan server support routine.
 *       original code written by ENDO Yasuhiro 
 */

/*
 * $Log: canna.c,v $
 * Revision 1.1.2.1  2003/02/23 14:17:14  amura
 * move canna.c from sys/default to sys/unix
 *
 * Revision 1.2  2000/07/16 15:43:00  amura
 * undo support
 *
 * Revision 1.1.1.1  2000/06/27 01:48:02  amura
 * import to CVS
 *
 */

#include "config.h"
#ifdef CANNA

#include "def.h"
#include "key.h"
#ifndef NO_MACRO
#include "macro.h"
#endif
#ifdef	UNDO
#include "undo.h"
#endif

#include <canna/jrkanji.h>
#define CANBUF 1024

jrKanjiStatus ks;
jrKanjiStatusWithValue ksv;
char currentMode[CANBUF];
char origMode[CANBUF];
static char kakutei[CANBUF];
static int oldlength = 0;
static int oldrevPos = 0;

int canna_toggle()
{
	WINDOW *wp;
	curbp->b_flag ^= BFCANNA;
	wp = wheadp;                            /* Update mode lines.   */
    while (wp != NULL) {
		if (wp->w_bufp == curbp)
			wp->w_flag |= WFMODE;
		wp = wp->w_wndp;
    }

	return TRUE;

}

int henkan( c )
int c;
{
	int ilen;
	int i;

#ifdef  READONLY
	if (curbp->b_flag & BFRONLY) {
		warnreadonly();
		return TRUE;
	}
#endif

	ilen = jrKanjiString(0, c, kakutei, CANBUF, &ks);

	if (ilen < 0) {
		ewprintf("%s",jrKanjiError);
		return FALSE;
	}

	if(ks.info & KanjiModeInfo){
		WINDOW *wp;
		jrKanjiControl(0,KC_QUERYMODE, currentMode);
		wp = wheadp;                            /* Update mode lines.   */
		while (wp != NULL) {
			if (wp->w_bufp == curbp)
				wp->w_flag |= WFMODE;
			wp = wp->w_wndp;
		}
	}
	
	if ( ilen > 0) {
		curwp->w_doto = curwp->w_doto - oldrevPos;
		ldelete((RSIZE) oldlength, 0);
		for (i=0 ; kakutei[i] ; i++) {
#ifndef NO_MACRO
			if(macrodef && macrocount < MAXMACRO)
	    			macrocount++;
#endif
			key.k_chars[0] = kakutei[i];
			key.k_count = 1;
#ifdef	UNDO
			ublock_open(curbp);
#endif
			selfinsert(FFRAND,1);
#ifdef	UNDO	
			ublock_close(curbp);
#endif
			lastflag = thisflag;
		}
		oldlength = 0 ;
		oldrevPos = 0;
	}
	if ( ks.length > 0) {
		curwp->w_doto = curwp->w_doto - oldrevPos;
		ldelete((RSIZE) oldlength, 0);
		linsert( 1, '|');
		for (i=0 ; *(ks.echoStr + i) ; i++) {
			linsert(1,*(ks.echoStr + i));
		}
		linsert( 1, '|');
		oldlength = ks.length + 2 ;
		oldrevPos = ks.revPos + 1;
		curwp->w_doto = curwp->w_doto - oldlength + oldrevPos;
	}
	else if ( ks.length == 0 && ilen == 0) {
		curwp->w_doto = curwp->w_doto - oldrevPos;
		ldelete((RSIZE) oldlength, 0);
		oldlength = 0;
		oldrevPos = 0;
	}

	if ( ks.info & KanjiGLineInfo ) {
		
		if ( ks.gline.length ) {
			char kouhobuf[CANBUF];
			char *kb=kouhobuf, *gl=ks.gline.line;

			for(i=0; i<ks.gline.revPos;i++) *kb++ = *gl++;
			if ( ks.gline.revLen ) *kb++ = '[';
			for(i=0; i<ks.gline.revLen;i++) *kb++ = *gl++;
			if ( ks.gline.revLen ) *kb++ = ']';
			while(*kb++ = *gl++) ;

			ewprintf("%s", kouhobuf);
		}
		else eerase();
	}
	
	return TRUE;
}

VOID canna_width()
{
	if (ncol >= 2)
		jrKanjiControl(0, KC_SETWIDTH, (char *) ncol-2);
	else
		jrKanjiControl(0, KC_SETWIDTH, (char *) 0);
}

VOID canna_init()
{
	ksv.ks=&ks; 
	ksv.buffer = kakutei;
	ksv.bytes_buffer = CANBUF;
	ksv.val = CANNA_MODE_HenkanMode;
	jrKanjiControl(0,KC_INITIALIZE, NULL);
	jrKanjiControl(0,KC_SETAPPNAME, "ng");
	jrKanjiControl(0,KC_SETBUNSETSUKUGIRI, (char *)&ksv);
	jrKanjiControl(0,KC_CHANGEMODE, (char *)&ksv);
	jrKanjiControl(0,KC_QUERYMODE, currentMode);
	strcpy(origMode, currentMode);
	canna_width();
}

VOID canna_end()
{
	jrKanjiControl(0, KC_KILL, (char *) &ksv);
	jrKanjiControl(0, KC_FINALIZE, 0);
}
#endif /* CANNA */
