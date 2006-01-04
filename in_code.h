/* $Id: in_code.h,v 1.1.2.9 2006/01/04 17:00:39 amura Exp $ */
/*
 * Some special charactors of buffer internal code
 */

#ifndef __IN_CODE_H__
#define __IN_CODE_H__

typedef unsigned short NG_WCHAR_t;
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
#define NG_WBACKSL	NG_WCODE('\\')

/*
 * 0xFF00-- only available in input key code
 */
#define NG_W_PF01	NG_WCODE(0xFF00)
#define NG_W_PF02	NG_WCODE(0xFF01)
#define NG_W_PF03	NG_WCODE(0xFF03)
#define NG_W_PF04	NG_WCODE(0xFF04)
#define NG_W_PF05	NG_WCODE(0xFF05)
#define NG_W_PF06	NG_WCODE(0xFF06)
#define NG_W_PF07	NG_WCODE(0xFF07)
#define NG_W_PF08	NG_WCODE(0xFF08)
#define NG_W_PF09	NG_WCODE(0xFF09)
#define NG_W_PF10	NG_WCODE(0xFF0A)
#define NG_W_PF11	NG_WCODE(0xFF0B)
#define NG_W_PF12	NG_WCODE(0xFF0C)
#define NG_W_PF13	NG_WCODE(0xFF0D)
#define NG_W_PF14	NG_WCODE(0xFF0E)
#define NG_W_PF15	NG_WCODE(0xFF0F)
#define NG_W_PF16	NG_WCODE(0xFF10)
#define NG_W_PF17	NG_WCODE(0xFF11)
#define NG_W_PF18	NG_WCODE(0xFF12)
#define NG_W_PF19	NG_WCODE(0xFF13)
#define NG_W_PF20	NG_WCODE(0xFF14)

#define NG_W_UP		NG_WCODE(0xFF40)
#define NG_W_DOWN	NG_WCODE(0xFF41)
#define NG_W_LEFT	NG_WCODE(0xFF42)
#define NG_W_RIGHT	NG_WCODE(0xFF43)
#define NG_W_HOME	NG_WCODE(0xFF44)
#define NG_W_END	NG_WCODE(0xFF45)
#define NG_W_PGUP	NG_WCODE(0xFF46)
#define NG_W_PGDN	NG_WCODE(0xFF47)

#define NG_W_HELP	NG_WCODE(0xFF50)
#define NG_W_CLEAR	NG_WCODE(0xFF51)
#define NG_W_PAUSE	NG_WCODE(0xFF52)

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
