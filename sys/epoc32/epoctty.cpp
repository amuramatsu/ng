/* $Id: epoctty.cpp,v 1.3 2001/11/28 21:45:12 amura Exp $ */
/*
 *		Epoc32 Tty support class (Tested only at Psion S5mx)
 */

#include "config.h"
#include "def.h"
#include <e32cons.h>
#include "epoctty.h"

EpocTty*
EpocTty::NewL(const TDesC &aTitle, const TSize &aSize)
{
    EpocTty* self = new EpocTty();
    self->color = CTEXT;
    self->colorChange = FALSE;
    self->Create(aTitle, aSize);
    self->ClearScreen();
    return self;
}

void
EpocTty::PutLine(int row, int column, unsigned char *s, unsigned char *t,
		 short color)
{
    char *buf, *ptr;
    extern int ncol; /* defined in ttyio.cpp */
#ifdef KANJI
    buf = ptr = new char[ncol*3+1];
    /* create charactor buffer */
    for (int i=0; i<ncol; i++) {
#ifdef HANKANA
	if (ISKANA(*t)) {
	    *ptr++ = *t++;
	    *ptr++ = *s++;
	}
	else
#endif
#ifdef HOJO_KANJI
	if (ISHOJO(*t)) {
	    *ptr++ = *t;
	    *ptr++ = *s++;
	    *ptr++ = *s++;
	    t += 2;
	}
	else
#endif
	if (ISKANJI(*s)) {
	    *ptr++ = *s++;
	    *ptr++ = *s++;
	    t += 2;
	}
	else {
	    *ptr++ = *s++;
	    t++;
	}
    }
    int len = bufetou8(buf, ptr-buf, ncol*3);
    buf[len] = '\0';
#else /* NOT KANJI */
    buf = ptr = new char[ncol+1];
    for (int i=0; i<ncol; i++)
	*ptr++ = *s++;
    *ptr = '\0';
#endif /* KANJI */
    SetPos(column, row);
    if (color == CTEXT)
	SetTextAttribute(ETextAttributeNormal);
    else
	SetTextAttribute(ETextAttributeInverse);
    colorChange = !(color == this->color);
    Write(_L(buf));
    delete[] buf;
}

void
EpocTty::Putch(int ch)
{
    TBuf<1> tmp;
    tmp[0] = ch & 0xFF;
    if (colorChange) {
	if (color == CTEXT)
	    SetTextAttribute(ETextAttributeNormal);
	else
	    SetTextAttribute(ETextAttributeInverse);
	colorChange = FALSE;
    }
    Write(tmp);
}

void
EpocTty::SetColor(short aColor)
{
    if (color != aColor) {
	color = aColor;
	colorChange = TRUE;
    }
}
