/* $Id: epoctty.h,v 1.1 2001/09/30 15:59:12 amura Exp $ */
/*
 *		Epoc32 Tty support class header (Tested only at Psion S5mx)
 */

#include <e32cons.h>
#include <e32base.h>
#include <e32twin.h>

class EpocTty : public CConsoleTextWin {
private:
    short color;
    int colorChange;
    
public:
    static EpocTty* NewL(const TDesC &aTitle, const TSize &aSize);
    void PutLine(int row, int column,
		 unsigned char *s, unsigned char *t, short aColor);
    void Putch(int ch);
    void SetColor(short aColor);
};
