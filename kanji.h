#ifndef	__KANJI_H__
#define __KANJI_H__

#define	issjis1st(c)	(((c) >= 0x81 && (c) <= 0x9f) || \
			 ((c) >= 0xe0 && (c) <= 0xfc))
#define	iseuc1st(c)	((c) >= 0xa1 && (c) <= 0xfe)

#ifdef HANKANA
#define iskana(c)	((c) >= 0xa0 && (c) < 0xe0)
#endif

/* 90.07.25  Change to inline routine  by S.Yoshida (from) */
#define	jtoe(c1, c2)	do {c1 |= 0x80; c2 |= 0x80;} while (/*CONSTCOND*/0)
#define	etoj(c1, c2)	do {c1 &= 0x7f; c2 &= 0x7f;} while (/*CONSTCOND*/0)
#define	stoe(c1, c2)	do {			\
	if ((c1) >= 0xe0)			\
		(c1) -=  0x40;			\
	if ((c2) >= 0x9f) {			\
		(c1) = ((c1) - 0x88)*2 + 0xb0;	\
		(c2) += 0x02;			\
	} else {				\
		if ((c2) >= 0x7f)		\
			(c2) -= 0x01;		\
		(c1) = ((c1) - 0x89)*2 + 0xb1;	\
		(c2) = (c2) + 0x61;		\
	}					\
} while (/*CONSTCOND*/0)
#define	etos(c1, c2)	do {			\
	(c1) &= 0x7f;				\
	(c2) &= 0x7f;				\
	if ((c1) >= 0x5f)			\
		c1 += 0x80;			\
	if (((c1) % 2) == 0) {			\
		(c1) = ((c1) - 0x30)/2 + 0x88;	\
		(c2) += 0x7e;			\
	} else {				\
		if ((c2) >= 0x60)		\
			(c2) += 0x01;		\
		(c1) = ((c1) - 0x31)/2 + 0x89;	\
		(c2) += 0x1f;			\
	}					\
	(c1) &= 0xff;				\
	(c2) &= 0xff;				\
} while (0/*CONSTCOND*/)

#ifdef	UNICODE
#define isutf1byte(c)	(!(c)&0x80)
#define isutf2byte(c)	(!(c)&0x20)

#define u8tou2(a, b, c) do {					\
	if (c) {						\
		(a) = (((a)<<4) & 0xF0) | (((b)>>2) & 0x0F);	\
		(b) = (((b)<<6) & 0xC0) | ((c) & 0x3F);		\
	}							\
	else if (b) {						\
		(b) = (((a)<<6) & 0xC0) | ((b) & 0x3F);		\
		(a) = ((a)>>2) & 0x07;				\
	}							\
	else {							\
		(b) = (a);					\
		(a) = 0;					\
	}							\
} while (0/*CONSTCOND*/)
#define u2tou8(a, b, c) do {					\
	if (a) {						\
		if ((a) <= 0x07) {				\
			(a) = 0xC0|((a)<<2)|(((b)>>6)&0x03);	\
			(b) = 0x80|((b)&0x3F);			\
		} else {					\
			(c) = 0x80|((b)&0x3F);			\
			(b) = 0x80|(((a)<<2)&0x3C)|(((b)>>6)&0x03);\
			(a) = 0xE0|(((a)>>4)&0x0F);		\
		}						\
	} else {						\
		(a) = (b);					\
		(b) = (c) = 0;					\
	}							\
} while (0/*CONSTCOND*/)

#define	utoe(a,b,c)	utoe_in(&(a), &(b), &(c))
extern VOID utoe_in pro((int *c0, int *c1, int *c2));

#define	etou(a,b,c)	etou_in(&(a), &(b), c)
extern VOID etou_in pro((int *c0, int *c1, int c2));

#endif	/* UNICODE */

#endif	/* __KANJI_H__ */
