/* $Id: epoctty.h,v 1.1.2.1 2006/01/14 23:43:38 amura Exp $ */
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
    void PutLine(int row, const NG_WCHAR_t *s, short aColor);
    void Putch(int ch);
    void SetColor(short aColor);
};
