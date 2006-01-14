/* $Id: epoctty.cpp,v 1.3.2.1 2006/01/14 23:43:38 amura Exp $ */
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
EpocTty::PutLine(int row, const NG_WCHAR_t *s, short color)
{
    char *buf, *ptr;
    extern int ncol; /* defined in ttyio.cpp */

    buf = ptr = new char[ncol*3+1];
    eop = &buf[ncol*3];
    
    /* create charactor buffer */
    for (int i=0; i<ncol; i++) {
	if (ISASCII(*s))
	    *ptr++ = *s++ & 0x7F;
	else {
	    if ((s=terminal_lang->lm_get_display_code(*s, ptr, eop - ptr)) < 0)
		break;
	    ptr += s;
	}
    }
    *ptr = '\0';
    SetPos(0, row);
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
