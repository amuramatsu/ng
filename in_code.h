/* $Id: in_code.h,v 1.1.2.3 2005/04/07 14:27:28 amura Exp $ */
/*
 * Some special charactors of buffer internal code
 */

#ifndef __IN_CODE_H__
#define __IN_CODE_H__

typedef unsigned short NG_WCHAR_t;
#define NG_WCODE(n)	((NG_WCHAR_t)(n))

/*
 * 0x00--0x127 of Buffer internal code MUST map to ASCII
 */ 
#define NG_WSPACE	NG_WCODE(0x20)
#define NG_WTAB		NG_WCODE(0x08)
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