/* $Id */
/*
 *		Epoc32 Tty support class (Tested only at Psion S5mx)
 */

#include "config.h"
#include "def.h"
#include <e32cons.h>
#include "epoctty.h"

extern CTrapCleanup* cleanup;

EpocTty*
EpocTty::NewL(const TDesC &aTitle, const TSize &aSize)
{
    EpocTty* self = new EpocTty();
    self->color = CTEXT;
    self->colorChange = FALSE;
    self->Create(aTitle, aSize);
    return self;
}

void
EpocTty::PutLine(int row, int column, unsigned char *s, unsigned char *t,
		 short color)
{
}

void
EpocTty::Putch(int ch)
{
    TBuf<2> tmp;
    tmp[0] = ch & 0xFF;
    tmp[1] = 0;
    if (colorChange) {
	colorChange = FALSE;
    }
    this->Printf(tmp);
}

void
EpocTty::SetColor(short aColor)
{
    if (color != aColor) {
	color = aColor;
	colorChange = TRUE;
    }
}
