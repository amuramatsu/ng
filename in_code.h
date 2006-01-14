/* $Id: in_code.h,v 1.1.2.13 2006/01/14 15:35:35 amura Exp $ */
/*
 * Some special charactors of buffer internal code
 */

#ifndef __IN_CODE_H__
#define __IN_CODE_H__

typedef unsigned short NG_WCHAR_t;
typedef unsigned int NG_WCHAR_ta;
#define NG_WCODE(n)	((NG_WCHAR_t)(n))
#define NG_WCHARLEN(s)	(sizeof(s)/sizeof(NG_WCHAR_t))
#define NG_WSTR_NULL	((NG_WCHAR_t *)"")

#if 0 /* if sizeof(NG_WCHAR) == 1 */

#define NG_WCHAR_STRLEN 2
#define NG_WCHAR_TO_STR(str, n)	\
    { (str)[0] = (n); (str)[1] = '\0'; }

#include <stdlib.h>
#include <string.h>
#define	wstrcmp(a,b)	strcmp((const char *)(a), (const char *)(b))
#define	wstrncmp(a,b,n)	strcmp((const char *)(a), (const char *)(b), (n))
#define	wstrncmpa(a,b,n) strcmp((const char *)(a), (const char *)(b), (n))
#define	wstrlen(s)	strlen((const char *)(s));
#define wstrcpy(d,s)	((NG_WCHAR_t *)strcpy((char *)(d), (const char *)(s)))
#define wstrcat(d,s)	((NG_WCHAR_t *)strcat((char *)(d), (const char *)(s)))
#define wstrlcpy(d,s,n)	strlcpy((char *)(d), (const char *)(s), (n))
#define wstrlcat(d,s,n)	strlcat((char *)(d), (const char *)(s), (n))
#define wstrlcpya(d,s,n) strlcpy((char *)(d), (const char *)(s), (n))
#define wstrlcata(d,s,n) strlcat((char *)(d), (const char *)(s), (n))
#define strlcpyw(d, s, n) strlcpy((char *)(d), (const char *)(s), (n))
#define strlcatw(d, s, n) strlcat((char *)(d), (const char *)(s), (n))
#define watoi(s)	atoi(s)
#define wsnprintf	snprintf

#define _NG_WSTR(s)	(s)

#else

#define NG_WCHAR_STRLEN 3
#define NG_WCHAR_TO_STR(str, n)	\
    { (str)[0] = (n)>>8; (str)[1] = (n)&0xff; (str)[2] = '\0'; }

int wstrcmp _PRO((const NG_WCHAR_t *, const NG_WCHAR_t *));
int wstrncmp _PRO((const NG_WCHAR_t *, const NG_WCHAR_t *, size_t));
int wstrncmpa _PRO((const NG_WCHAR_t *, const char *, size_t));
size_t wstrlen _PRO((const NG_WCHAR_t *));
NG_WCHAR_t *wstrcpy _PRO((NG_WCHAR_t *, const NG_WCHAR_t *));
NG_WCHAR_t *wstrcat _PRO((NG_WCHAR_t *, const NG_WCHAR_t *));
size_t wstrlcpy _PRO((NG_WCHAR_t *, const NG_WCHAR_t *, size_t));
size_t wstrlcat _PRO((NG_WCHAR_t *, const NG_WCHAR_t *, size_t));
size_t wstrlcpya _PRO((NG_WCHAR_t *, const char *, size_t));
size_t wstrlcata _PRO((NG_WCHAR_t *, const char *, size_t));
size_t strlcpyw _PRO((char *, const NG_WCHAR_t *, size_t));
size_t strlcatw _PRO((char *, const NG_WCHAR_t *, size_t));
int watoi _PRO((const NG_WCHAR_t *));
size_t wsnprintf _PRO((NG_WCHAR_t *, size_t, const char *, ...));

NG_WCHAR_t *_ng_wstr _PRO((const char *));
#define _NG_WSTR(s)	_ng_wstr(s)

#endif

/*
 * 0x00--0x127 of Buffer internal code MUST map to ASCII
 */ 
#define NG_EOS		NG_WCODE(0x00)
#define NG_WSPACE	NG_WCODE(0x20)
#define NG_WTAB		NG_WCODE(0x09)
#define NG_WESC		NG_WCODE(0x1b)
#define NG_WBACKSL	NG_WCODE('\\')

/*
 * 0xFF00-- specials
 */
#define NG_SPECIAL_CODE(c)	(((c)&0xFF00) == 0xFF00)

#define NG_W_INPUTCONT	NG_WCODE(0xFF00)
#define NG_WFILLER	NG_WCODE(0xFF01)

#define NG_W_PF01	NG_WCODE(0xFF10)
#define NG_W_PF02	NG_WCODE(0xFF11)
#define NG_W_PF03	NG_WCODE(0xFF12)
#define NG_W_PF04	NG_WCODE(0xFF13)
#define NG_W_PF05	NG_WCODE(0xFF14)
#define NG_W_PF06	NG_WCODE(0xFF15)
#define NG_W_PF07	NG_WCODE(0xFF16)
#define NG_W_PF08	NG_WCODE(0xFF17)
#define NG_W_PF09	NG_WCODE(0xFF18)
#define NG_W_PF10	NG_WCODE(0xFF19)
#define NG_W_PF11	NG_WCODE(0xFF1A)
#define NG_W_PF12	NG_WCODE(0xFF1B)
#define NG_W_PF13	NG_WCODE(0xFF1C)
#define NG_W_PF14	NG_WCODE(0xFF1D)
#define NG_W_PF15	NG_WCODE(0xFF1E)
#define NG_W_PF16	NG_WCODE(0xFF1F)
#define NG_W_PF17	NG_WCODE(0xFF20)
#define NG_W_PF18	NG_WCODE(0xFF21)
#define NG_W_PF19	NG_WCODE(0xFF22)
#define NG_W_PF20	NG_WCODE(0xFF23)

#define NG_W_UP		NG_WCODE(0xFF40)
#define NG_W_DOWN	NG_WCODE(0xFF41)
#define NG_W_LEFT	NG_WCODE(0xFF42)
#define NG_W_RIGHT	NG_WCODE(0xFF43)
#define NG_W_HOME	NG_WCODE(0xFF44)
#define NG_W_END	NG_WCODE(0xFF45)
#define NG_W_PGUP	NG_WCODE(0xFF46)
#define NG_W_PGDN	NG_WCODE(0xFF47)
#define NG_W_HELP	NG_WCODE(0xFF48)
#define NG_W_CLEAR	NG_WCODE(0xFF49)
#define NG_W_PAUSE	NG_WCODE(0xFF4A)

#define NG_W_SUP	NG_WCODE(0xFF50)
#define NG_W_SDOWN	NG_WCODE(0xFF51)
#define NG_W_SLEFT	NG_WCODE(0xFF52)
#define NG_W_SRIGHT	NG_WCODE(0xFF53)
#define NG_W_SHOME	NG_WCODE(0xFF54)
#define NG_W_SEND	NG_WCODE(0xFF55)
#define NG_W_SPGUP	NG_WCODE(0xFF56)
#define NG_W_SPGDN	NG_WCODE(0xFF57)
#define NG_W_SHELP	NG_WCODE(0xFF58)
#define NG_W_SCLEAR	NG_WCODE(0xFF59)
#define NG_W_SPAUSE	NG_WCODE(0xFF5A)

/*
 * SPECIAL CHARACTORS TRAILING next code
 */
#define NG_HAS_NEXTCODE(c)	(((c)&0xFF80) == 0xFF80)

#define NG_WW_MOUSE_L		NG_WCODE(0xFF80)
#define NG_WW_MOUSE_M		NG_WCODE(0xFF81)
#define NG_WW_MOUSE_R		NG_WCODE(0xFF82)
#define NG_WW_MOUSE_SHIFT_L	NG_WCODE(0xFF83)
#define NG_WW_MOUSE_SHIFT_M	NG_WCODE(0xFF84)
#define NG_WW_MOUSE_SHIFT_R	NG_WCODE(0xFF85)
#define NG_WW_MOUSE_CTRL_L	NG_WCODE(0xFF86)
#define NG_WW_MOUSE_CTRL_M	NG_WCODE(0xFF87)
#define NG_WW_MOUSE_CTRL_R	NG_WCODE(0xFF88)
/* trailing code: COLUMN*256 + LINE */

#define NG_WW_MENU_M	NG_WCODE(0xFFC0)
#define NG_WW_MENU_E	NG_WCODE(0xFFC1)
/* trailing code: menusublevel*256 + menuno */

#endif /* __IN_CODE_H__ */
