/* $Id: fepctrl.c,v 1.1 2000/06/27 01:47:58 amura Exp $ */
/*
 * fepctrl.c 1.5 1992/04/08.  Public Domain.
 *
 * General purpose Japanese FEP control routines for MS-DOS.
 * Written by Junn Ohta (ohta@src.ricoh.co.jp, msa02563)
 *
 *	int fep_init(void)
 *		checks FEP and turn it off, returns FEP type.
 *	void fep_term(void)
 *		restore the status of FEP saved by fep_init().
 *	void fep_on(void)
 *		restore the status of FEP saved by fep_off().
 *	void fep_off(void)
 *		save the status of FEP and turn it off.
 *	void fep_force_on(void)
 *		turn FEP on by its default "on" status.
 *	void fep_force_off(void)
 *		don't save the status of FEP and turn it off.
 *	int fep_raw_init(void)
 *		same as fep_init(), but doesn't turn the FEP off.
 *	void fep_raw_term(void)
 *		same as fep_term(), but doesn't restore the FEP status.
 *	int fep_get_mode(void)
 *		return the current status of FEP (0 = off).
 *
 * Compiles under:
 *	Turbo C 1.5/2.0, Turbo C++ 1.0, Borland C++ 2.0/3.0,
 *	Microsoft C 5.1, Microsoft C 6.0, Quick C 2.0,
 *	Lattice C 4.1 (need negative stack frame, except huge model),
 *	LSI C-86 3.2/3.3 Sampler (small model only).
 */

/*
 * $Log: fepctrl.c,v $
 * Revision 1.1  2000/06/27 01:47:58  amura
 * Initial revision
 *
 */

#define	NG
#ifdef	NG
#include "config.h"
#endif

#ifdef	FEPCTRL

static char fepctrl_version[] = "FEPCTRL V1.5 04/08/1992";

/*
 * define appropriate directives shown below
 */
#define DO_MSKANJI	/* support MS-KANJI application interface */
#ifndef PC9801	/* 91.05.21  by Junn Ohta */
#define DO_GENERIC	/* support platforms other than PC-9801 */
#endif
/* #define DO_USERDEV	/* recognize user supplied device names */

#include <dos.h>
#include "fepctrl.h"

/*
 * default "on" status of FEP (used only in fep_force_on())
 */
static unsigned char fepon[NFEPS] = {
/* FEP_NONE	*/  0,
/* FEP_PC98A	*/  1,	/* 0=off, 1=on */
/* FEP_PC98B	*/  1,	/* 0=off, 1=on */
/* FEP_PC98C	*/  1,	/* 0=off, 1=on */
/* FEP_MSKANJI	*/  2,	/* 0=off, 1=system, 2=echo */
/* FEP_VJE	*/  2,	/* 0=off, 1=system, 2=echo */
/* FEP_ATOK6	*/  1,	/* 0=off, 1=roma, 2=kana, 3=alpha, 4=sym, 5=code */
/* FEP_ATOK7	*/  1,	/* 0=off, 1=on */
/* FEP_MTTK86	*/  1,  /* 0=off, 1=on */
/* FEP_MTTK2	*/  1,  /* 0=off, 1=on */
/* FEP_KATANA	*/  1,  /* 0=off, 1=echo, 2=system */
/* FEP_FIXER	*/  1,  /* 0=off, 1=echo, 2=system */
/* FEP_EGB2	*/  2,  /* 0=off, 1=system, 2=echo */
/* FEP_EGB3	*/  2,  /* 0=off, 1=system, 2=echo */
/* FEP_WXP	*/  1,  /* 0=off, 1=echo, 2=system */
/* FEP_WX2	*/  1,	/* 0=off, 1=echo, 2=system */
/* FEP_MGR2	*/  1,  /* 0=off, 1=on */
/* FEP_JJ	*/  1,	/* 0=off, 1=on */
/* FEP_NEC	*/  1,  /* 0=off, 1=on */
/* FEP_DFJ	*/  1,  /* 0=off, 1=on */
/* FEP_DANGO	*/  1,	/* 0=off, 1=kana, 3=alpha */
/* FEP_OTEMOTO	*/  2,	/* 0=off, 1=system, 2=echo */
/* FEP_OMAC	*/  1,	/* 0=off, 1=on */
/* FEP_AJIP1	*/  1,	/* 0=off, 1=on */
/* FEP_JOKER3	*/  1,	/* 0=off, 1=on */
/* FEP_KAZE	*/  1,	/* 0=off, 1=on */
/* FEP_OAK   */  0xd1,	/* 0r010100=off, 1r010001=on (r=romaji) */
/* FEP_MKK	*/  2,	/* 0=off, 1=system, 2=echo */
/* FEP_B16   */  0x80,	/* 00000000=off, 10000000=on */
/* FEP_RICOH */  0x80,	/* 00000000=off, 10000000=on */
/* FEP_WXPJ	*/  1,	/* 0=off, 1=echo, 2=system */
/* FEP_MIJ	*/  1,	/* 0=off, 1=on */
/* FEP_FEPEX	*/  1,	/* 0=off, 1=on */
/* FEP_AT6AX	*/  1,	/* 0=off, 1=roma, 2=kana, 3=alpha, 4=sym, 5=code */
/* FEP_AT6IBM	*/  1,	/* 0=off, 1=roma, 2=kana, 3=alpha, 4=sym, 5=code */
/* FEP_AT7IBM	*/  1,	/* 0=off, 1=on */
/* FEP_AT7DOSV*/ 0x80,	/* 00000000=off, 10000000=on */
/* FEP_IAS   */  0xc5 	/* 0rxxx000=off, 1rxxx101=on */
};

static int fep = FEP_NONE;
static int oldmode = 0;
static int keepmode = 0;

static void near __PASCAL fep_open(void);
static void near __PASCAL fep_close(void);
static int near __PASCAL fep_find(void);
static int near __PASCAL fep_mode(int);
static int near __PASCAL check_dev(char *);
static int near __PASCAL check_vec(int, unsigned, unsigned, char *);
static int near __PASCAL check_atok7(int);
static int near __PASCAL msknj_find(char *);
static int near __PASCAL msknj_get_mode(void);
static int near __PASCAL msknj_set_mode(int);

int __CDECL
fep_init(void)
{
	if (fep == FEP_NONE)
		fep = fep_find();
	fep_open();
	oldmode = keepmode = fep_mode(0);
	return fep;
}

void __CDECL
fep_term(void)
{
	fep_mode(oldmode);
	fep_close();
}

void __CDECL
fep_on(void)
{
	fep_mode(keepmode);
}

void __CDECL
fep_off(void)
{
	keepmode = fep_mode(0);
}

void __CDECL
fep_force_on(void)
{
	fep_mode(fepon[fep]);
}

void __CDECL
fep_force_off(void)
{
	fep_mode(0);
}

int __CDECL
fep_raw_init(void)
{
	if (fep == FEP_NONE)
		fep = fep_find();
	fep_open();
	return fep;
}

void __CDECL
fep_raw_term(void)
{
	fep_close();
}

int __CDECL
fep_get_mode(void)
{
	return fep_mode(-1);
}

/*--------------------------------------------------------------------*/
#ifdef TEST
/*
 * mail routines for the batch utility
 *
 * Usage: fepctrl [command]
 * command:
 *	help   - print this message
 *	test   - ordinary FEP testing
 *	name   - print installed FEP name
 *	on     - turn FEP on
 *	off    - turn FEP off
 *	flip   - flip FEP status
 *	status - return FEP status
 *	check [fepname ...]
 *		- check existence of specified FEP
 * fepname:
 *	as follows...
 */

static char *fepname[NFEPS] = {
/* FEP_NONE	*/  "(none)",
/* FEP_PC98A	*/  "PC9801A",
/* FEP_PC98B	*/  "PC9801B",
/* FEP_PC98C	*/  "PC9801C",
/* FEP_MSKANJI	*/  "MSKANJI",
/* FEP_VJE	*/  "VJE",
/* FEP_ATOK6	*/  "ATOK6",
/* FEP_ATOK7	*/  "ATOK7",
/* FEP_MTTK86	*/  "MTTK86",
/* FEP_MTTK2	*/  "MTTK2",
/* FEP_KATANA	*/  "KATANA",
/* FEP_FIXER	*/  "FIXER",
/* FEP_EGB2	*/  "EGB2",
/* FEP_EGB3	*/  "EGB3",
/* FEP_WXP	*/  "WXP",
/* FEP_WX2	*/  "WX2",
/* FEP_MGR2	*/  "MGR2",
/* FEP_JJ	*/  "JJ",
/* FEP_NEC	*/  "NEC",
/* FEP_DFJ	*/  "DFJ",
/* FEP_DANGO	*/  "DANGO",
/* FEP_OTEMOTO	*/  "OTEMOTO",
/* FEP_OMAC	*/  "OMAC",
/* FEP_AJIP1	*/  "AJIP1",
/* FEP_JOKER3	*/  "JOKER3",
/* FEP_KAZE	*/  "KAZE",
/* FEP_OAK	*/  "OAK",
/* FEP_MKK	*/  "MKK",
/* FEP_B16	*/  "B16",
/* FEP_RICOH	*/  "RICOH",
/* FEP_WXPJ	*/  "WXPJ",
/* FEP_MIJ	*/  "MIJ",
/* FEP_FEPEX	*/  "FEPEX",
/* FEP_AT6AX	*/  "AT6AX",
/* FEP_AT6IBM	*/  "AT6IBM",
/* FEP_AT7IBM	*/  "AT7IBM",
/* FEP_AT7DOSV	*/  "AT7DOSV",
/* FEP_IAS	*/  "IBMIAS"
};

#define	getch()		(bdos(8, 0, 0) & 0xff)
#define	putch(c)	bdos(2, (int)(c), 0)

static void near __PASCAL
putstr(char *s)
{
	while (*s)
		putch(*s++);
}

static void near __PASCAL
echoline(void)
{
	int c;
	putstr(fep_get_mode()? " on: ": "off: ");
	while ((c = getch()) != '\r' && c != '\n')
		putch(c);
	putstr("\r\n");
}

void __CDECL
main(int ac, char **av)
{
	int fep, ret;

	ac--, av++;
	if (ac == 0)
		*av = "help";
	if (!strcmp(*av, "test")) {
		putstr("fep = ");
		putstr(fepname[fep_init()]);
		putstr("\r\n");
		putstr("enter 4 lines of text\r\n");
		fep_force_on();
		echoline();
		fep_off();
		echoline();
		fep_on();
		echoline();
		fep_force_off();
		echoline();
		fep_term();
		exit(0);
	}
	ret = 0;
	fep = fep_raw_init();
	if (!strcmp(*av, "name")) {
		putstr(fepname[fep]);
		putstr("\r\n");
	} else if (!strcmp(*av, "on")) {
		if (!fep_get_mode())
			fep_force_on();
	} else if (!strcmp(*av, "off")) {
		if (fep_get_mode())
			fep_force_off();
	} else if (!strcmp(*av, "flip")) {
		if (fep_get_mode())
			fep_force_off();
		else
			fep_force_on();
	} else if (!strcmp(*av, "status")) {
		ret = fep_get_mode()? 1: 0;
	} else if (!strcmp(*av, "check")) {
		ac--, av++;
		if (ac == 0)
			ret = fep;
		while (ac > 0) {
			if (!strcmp(fepname[fep], *av)) {
				ret = fep;
				break;
			}
			ac--, av++;
		}
	} else {
		putstr(fepctrl_version);
		putstr(" - batch utility\r\n");
		putstr("Usage: fepctrl [command]\r\n"
			"command:\r\n"
			"\thelp   - print this message\r\n"
			"\ttest   - ordinary FEP testing\r\n"
			"\tname   - print installed FEP name\r\n"
			"\ton     - turn FEP on\r\n"
			"\toff    - turn FEP off\r\n"
			"\tflip   - flip FEP status\r\n"
			"\tstatus - return FEP status\r\n"
			"\tcheck [fepname ...]\r\n"
			"\t       - check existence of specified FEP\r\n"
			"fepname:\r\n\t");
		for (fep = 1; fep < NFEPS; fep++) {
			if (fep > 1)
				putstr(", ");
			if ((fep % 8) == 0)
				putstr("\r\n\t");
			putstr(fepname[fep]);
		}
		putstr("\r\n");
	}
	fep_raw_term();
	exit(ret);
}

#endif /* TEST */
/*--------------------------------------------------------------------*/

#ifdef MSC_INLINE
#undef MSC_INLINE
#endif
#if defined(_MSC_VER) && _MSC_VER >= 600 || defined(_QC)
#define	MSC_INLINE
#endif

typedef struct {
	unsigned char	ftype;		/* fep type */
	unsigned char	inum;		/* dointr number */
	char		*dname;		/* device name */
} FEPDEV;

typedef struct {
	unsigned char	ftype;		/* fep type */
	unsigned char	inum;		/* dointr number */
	unsigned char	offset;		/* offset of id string */
	unsigned char	relative;	/* offset is relative? */
	char		*idstr;		/* id string */
} FEPINT;

#ifdef DO_USERDEV
/*
 * device names supplied by user for FEP residence check
 */
static FEPDEV fepudev[] = {
	{ FEP_PC98A,	0,    "FP$PC98A" },
	{ FEP_PC98B,	0,    "FP$PC98B" },
	{ FEP_PC98B,	0,    "FP$DUMMY" }, /* backward compatibility */
	{ FEP_PC98B,	0,    "FP$PC98"  }, /* backward compatibility */
	{ FEP_PC98C,	0,    "FP$PC98C" },
	{ FEP_VJE,	0,    "FP$VJE"   },
	{ FEP_ATOK6,	0x6f, "FP$ATOK6" }, /* ATOK6, DANGO */
	{ FEP_MTTK86,	0,    "FP$MTTK"  },
	{ FEP_KATANA,	0,    "FP$KTN"   },
	{ FEP_EGB2,	0,    "FP$EGB2"  },
	{ FEP_EGB3,	0,    "FP$EGB3"  },
	{ FEP_WXP,	0,    "FP$WXP"   }, /* WX, WXS, WXP 1.03a */
	{ FEP_WX2,	0x70, "FP$WX2"   }, /* WX2, WXP 1.04c */
	{ FEP_MGR2,	0,    "FP$MGR2"  },
	{ FEP_JJ,	0,    "FP$JJ"    },
	{ FEP_NEC,	0,    "FP$NEC"   },
	{ FEP_DFJ,	0,    "FP$DFJ"   },
	{ FEP_OMAC,	0,    "FP$OMAC"  },
	{ FEP_AJIP1,	0,    "FP$AJIP1" },
	{ FEP_JOKER3,	0xf1, "FP$JOKER" },
	{ FEP_KAZE,	0,    "FP$KAZE"  },
#ifdef DO_GENERIC
	{ FEP_OAK,	0,    "FP$OAK"   },
#endif
	{ FEP_NONE, }
};
#endif /* DO_USERDEV */

/*
 * real ID string placed in the memory
 */
static FEPINT fepint[] = {
	{ FEP_MTTK86,	0x70, 0x12, 0, "MTTK86"   },
	{ FEP_MTTK2,	0x70, 0x12, 0, "MTTK"     },
	{ FEP_EGB2,	0x70, 0x28, 0, "ERGOA"    },
	{ FEP_EGB3,	0x70, 0x28, 0, "ERGO"     },
	{ FEP_WXP,	0x70, 0x22, 0, "WX      " },
	{ FEP_WX2,	0x70, 0x22, 0, "WX2     " },
	{ FEP_DFJ,	0x6f, 0x02, 0, "DFJ"      },
	{ FEP_JJ,	0x70, 0x0a, 0, "JJ1_V"    },
	{ FEP_ATOK6,	0x6f, 0x03, 1, "ATOK5020" },
	{ FEP_ATOK6,	0x6f, 0x1d, 0, "ATOK5020" }, /* backward compat. */
	{ FEP_KATANA,	0x4b, 0x0b, 0, "KTN"	  },
	{ FEP_DANGO,	0x6f, 0x03, 1, "ATOKI"    },
	{ FEP_OMAC,	0x09, 0x12, 0, "Taikei"   },
	{ FEP_JOKER3,	0xf1, 0x12, 0, "JOKER3"   },
/*	{ FEP_AJIP1,	0xf3, 0x0a, 0, "AJ1V01PC" }, */
/*	{ FEP_VJE,	0x70, 0x12, 0, "VJE-"     }, */
/*	{ FEP_MGR2,	0x70, 0x0a, 0, "#MGR#REX" }, */
/*	{ FEP_NEC,	0xdc, 0x0a, 0, "$AID#NEC" }, */
/*	{ FEP_NEC,	0xdc, 0x0a, 0, "$ATC#NEC" }, */
#ifdef DO_GENERIC
	{ FEP_OAK,	0xed, 0x12, 0, "OAK000"   }, /* use 0xec to dointr */
	{ FEP_B16,	0x16, 0x02, 1, "K,YUGEV"  },
/*	{ FEP_RICOH,	0x16, 0x0a, 0, "$NIHONGO" }, */
	{ FEP_WXPJ,	0x6c, 0x22, 0, "WX      " },
	{ FEP_MIJ,	0x6f, 0x12, 0, "MTTK"     }, /* NOT TESTED */
/*	{ FEP_FEPEX,	0x83, 0x28, 0, "ERGO"	  }, */
	{ FEP_AT6AX,	0x65, 0x0d, 1, "ATOK6020" },
	{ FEP_AT6IBM,	0x60, 0x0a, 0, "ATOK6I%%" },
	{ FEP_AT6IBM,	0x60, 0x45, 1, "7TO6"	  }, /* ATOK7 + 7TO6 */
#endif
	{ FEP_NONE, }
};

/*
 * real device names built in by FEPs
 */
static FEPDEV fepdev[] = {
	{ FEP_VJE,	0,    "AS$VJE"   },
	{ FEP_ATOK6,	0x6f, "ATOKF%%%" },
	{ FEP_MGR2,	0,    "#MGR#REX" },
	{ FEP_AJIP1,	0,    "AJ1V01PC" },
	{ FEP_NEC,	0,    "$AID#NEC" }, /* NEC AI Henkan on DOS3.3C */
	{ FEP_NEC,	0,    "$AIC#NEC" }, /* NEC AI Henkan */
	{ FEP_NEC,	0,    "$ATC#NEC" }, /* NEC Chikuji Kana-Kanji */
#ifdef DO_GENERIC
	{ FEP_RICOH,	0,    "$NIHONGO" }, /* RICOH Nihongo Nyuryoku */
	{ FEP_FEPEX,	0,    "(ERGOSFT" }, /* FEPEX (on IF800EX/RX) */
	{ FEP_AT7DOSV,	0,    "IAEAT7%%" }, /* ATOK7 on IBM DOS/V $IAS */
	{ FEP_IAS,	0,    "$IBMAIAS" }, /* FEP uses IBM DOS/V $IAS */
#endif
	{ FEP_NONE, }
};

/*
 * FIXER ioctl data packet
 */
typedef struct {
	char	name[8];
	int	version;
	void	(far __CDECL *v3func)(void);	
	int	v3vec;
	void	(far __CDECL *v4func)(void);
	int	v4vec;
} FIXIDP;

#if defined(LSI_C)
static void (far *fixer_entry)(int, int, int, int);
static void (far *atok7_entry)(int, int near *);
#elif defined(LATTICE)
static void (far __CDECL *fixer_entry)(void);
static unsigned atok7_seg, atok7_off;
#else /* Turbo C, Microsoft C, Quick C */
static void (far __CDECL *fixer_entry)(void);
static void (far __CDECL *atok7_entry)(void);
#endif
static int api_vec = -1;	/* API interrupt vector */

#define GET_IOC_DATA	0x4400	/* Get IOCTL Data		*/
#define	RCV_IOC_CHAR	0x4402	/* Receive IOCTL Character	*/
#define	GET_CBRK	0x3300	/* Get CBREAK (CTRL-C Check)	*/
#define	SET_CBRK	0x3301	/* Set CBREAK (CTRL-C Check)	*/
#define	GET_VECTOR	0x35	/* Get Interrupt Vector		*/
#define	OPEN_HANDLE	0x3d	/* Open Handle			*/
#define	CLOSE_HANDLE	0x3e	/* Close Handle			*/
#define DEVICE		0x80	/* handle is not a file		*/
#define	CFLAG		0x0001	/* iAPX86 carry flag bit	*/

#if defined(__TURBOC__) || defined(LSI_C)
#define	MK_FAR(seg,off)	MK_FP(seg, off)
#define	FAR_SEG(addr)	FP_SEG(addr)
#define	FAR_OFF(addr)	FP_OFF(addr)
#define	FAR_OFF2(addr)	FP_OFF(addr)
#elif defined(LATTICE)
#define	MK_FAR(seg,off) (void far *)(((unsigned long)(seg)<<4)+(unsigned)(off))
#define	FAR_SEG(addr)	FP_SEG((void far *)(addr))
#define	FAR_OFF(addr)	FP_OFF((void far *)(addr))
#define	FAR_OFF2(addr)	((unsigned)(addr))	/* avoid normalization */
#else /* Microsoft C, Quick C */
#define	MK_FAR(seg,off)	(void far *)((unsigned long)(seg)<<16|(unsigned)(off))
#define	FAR_SEG(addr)	((unsigned)((unsigned long)(addr) >> 16))
#define	FAR_OFF(addr)	((unsigned)(addr))
#define	FAR_OFF2(addr)	((unsigned)(addr))
#endif

#ifdef LATTICE
static union REGS regs;
static union REGSS xregs;
static struct SREGS sregs;
/*
 * we must use int86s()/intdoss(), since Lattice's int86x()/intdosx()
 * don't return the modified values of segment registers
 */
#define	Int86x(inum, xp1, xp2, dummy)	int86s(inum, xp1, xp2)
#define	Intdosx(xp1, xp2, dummy)	intdoss(xp1, xp2)
#define	Regs				xregs
#define	Sregs				xregs.x
#else /* Turbo C, Microsoft C, Quick C, LSI C */
static union REGS regs;
static struct SREGS sregs;
#define	Int86x(inum, rp1, rp2, sp)	int86x(inum, rp1, rp2, sp)
#define	Intdosx(rp1, rp2, sp)		intdosx(rp1, rp2, sp)
#define	Regs				regs
#define	Sregs				sregs
#endif

static unsigned char *p;
static unsigned char far *fp;

#ifndef __TURBOC__
#if defined(LATTICE)
/*
 * fake function to mimic disable() and enable()
 */
static unsigned char far disable_fake[] = { 0xfa, 0xcb }; /* cli, retf */
static unsigned char far enable_fake[]  = { 0xfb, 0xcb }; /* sti, retf */
static void (far *disable)(void) = (void (far *)(void))disable_fake;
static void (far *enable)(void)  = (void (far *)(void))enable_fake;
#elif defined(LSI_C)
#define	disable()	di()
#define	enable()	ei()
#else /* Microsoft C, Quick C */
#define	disable()	_disable()
#define	enable()	_enable()
#endif
#endif /* !__TURBOC__ */

#ifdef __cplusplus
extern "C" {
extern int __CDECL strncmp(char *, char *, unsigned int);
extern void * __CDECL memset(void *, int, unsigned int);
}
#else
extern int __CDECL strncmp(char *, char *, unsigned int);
extern void * __CDECL memset(void *, int, unsigned int);
#endif

static int near __PASCAL
fep_find(void)
{
	FEPDEV *fv;
	FEPINT *fi;
	FIXIDP fixer;

#ifdef DO_USERDEV
	/*
	 * detect fep type by its USER SUPPLIED device name
	 */
	for (fv = fepudev; fv->ftype != FEP_NONE; fv++) {
		if (check_dev(fv->dname)) {
			api_vec = fv->inum;
			return fv->ftype;
		}
	}
#endif

	/*
	 * loaded into bank memory by MELCO's FEP loader (PC-9801 only)
	 */
	if (check_dev("FPLD$10")) {
		if (check_dev("AS$VJE"))
			return FEP_VJE;		/* don't use MS-KANJI API */
		if (check_vec(0x70, 0x12, 0, "MTTK"))
			return FEP_MTTK86;	/* don't use INT 70H I/F */
		if (check_dev("$AID#NEC"))
			return FEP_NEC;		/* don't use MS-KANJI API */
	}

#ifdef DO_MSKANJI
	/*
	 * MS-KANJI FEP and its variants
	 */
	if (msknj_find("MS$KANJI"))
		return FEP_MSKANJI;
	if (msknj_find("$KANJI"))
		return FEP_OTEMOTO;
#endif

	/*
	 * detect fep type by its id string
	 */
	for (fi = fepint; fi->ftype != FEP_NONE; fi++) {
		if (check_vec(fi->inum, fi->offset, fi->relative, fi->idstr)) {
			api_vec = fi->inum;
			return fi->ftype;
		}
	}

	/*
	 * detect fep type by its device name
	 */
	for (fv = fepdev; fv->ftype != FEP_NONE; fv++) {
		if (check_dev(fv->dname)) {
			api_vec = fv->inum;
			return fv->ftype;
		}
	}

	/*
	 * special case: ATOK7 function call
	 */
	if (check_atok7(0x6f))
		return FEP_ATOK7;
#ifdef DO_GENERIC
	if (check_atok7(0x60) && check_dev("ATOK7I%%"))
		/* PS/55 DOS 4.0J only; avoid to use ATOK7 API on DOS/V */
		return FEP_AT7IBM;
#endif

	/*
	 * special case: FIXER3/4 application interface
	 */
	regs.x.ax = GET_IOC_DATA;
	regs.x.bx = 0;	/* stdin */
	intdos(&regs, &regs);
	if (regs.x.dx & DEVICE) {
		regs.x.ax = RCV_IOC_CHAR;
		regs.x.bx = 0;	/* stdin */
		regs.x.cx = sizeof(fixer);
		regs.x.dx = FAR_OFF((void far *)&fixer);
		sregs.ds = FAR_SEG((void far *)&fixer);
		intdosx(&regs, &regs, &sregs);
		if (regs.x.ax >= 16 && !strncmp(fixer.name, "FIXER   ", 8)) {
			if (fixer.v3vec != -1) {
				api_vec = fixer.v3vec;
				return FEP_FIXER;
			}
			if (!check_dev("FPLD$10") &&
			    !check_dev("$BNK#DRV") &&
			    fixer.v3func != (void far *)0) {
				fixer_entry = fixer.v3func;
				return FEP_FIXER;
			}
			if (fixer.version < 0x0400)
				return FEP_PC98B;
		}
	}

	/*
	 * special case: Katana/ACE software dointr interface
	 */
	if (check_vec(0x48, 0x0a, 0, "CON     ") &&
	    check_vec(0x49, 0x0a, 0, "CON     ") &&
	    check_vec(0x4a, 0x0a, 0, "CON     ")) {
		/*
		 * This may not be a good way to check Katana/ACE.
		 * However, console status report (sending "<ESC>@"
		 * returns "$") is not reliable here, because we
		 * may have characters in the keyboard buffer.
		 */
		return FEP_KATANA;
	}

	/*
	 * special case: Panacom MKK software dointr interface
	 */
	if (check_vec(0xc0, 0x0a, 0, "CON     ") &&
	    check_vec(0xc3, 0x0a, 0, "CON     "))
		return FEP_MKK;

	return FEP_NONE;
}

static int near __PASCAL
open_dev(char *dname)
{
	static char dbuf[15] = "@:";
	char *p = dbuf + 2;

	while ((*p++ = *dname++) != 0)
		/* void */;
	regs.h.ah = OPEN_HANDLE;
	regs.h.al = 0;
	regs.x.dx = FAR_OFF((void far *)dbuf);
	sregs.ds = FAR_SEG((void far *)dbuf);
#if defined(LATTICE)
	if (intdosx(&regs, &regs, &sregs) & CFLAG)
		return -1;
#elif defined(LSI_C)
	intdosx(&regs, &regs, &sregs);
	if (regs.x.flags & CFLAG)
		return -1;
#else /* Turbo C, Microsoft C, Quick C */
	intdosx(&regs, &regs, &sregs);
	if (regs.x.cflag)
		return -1;
#endif
	return regs.x.ax;
}

static void near __PASCAL
close_dev(int fd)
{
	regs.h.ah = CLOSE_HANDLE;
	regs.x.bx = fd;
	intdos(&regs, &regs);
}

static int near __PASCAL
check_dev(char *dname)
{
	int fd, dc;

	if ((fd = open_dev(dname)) < 0)
		return 0;

	regs.x.ax = GET_IOC_DATA;
	regs.x.bx = fd;
	intdos(&regs, &regs);
	dc = regs.x.dx;
	close_dev(fd);
	return (dc & DEVICE)? 1: 0;
}

static int near __PASCAL
check_vec(int inum, unsigned offset, unsigned relative, char *idstr)
{
	/*
	 * get dointr vector
	 */
	Regs.h.ah = GET_VECTOR;
	Regs.h.al = (unsigned char)inum;
	Intdosx(&Regs, &Regs, &Sregs);
	/*
	 * check id string
	 */
	if (relative)
		offset += Regs.x.bx;
	fp = (unsigned char far *)MK_FAR(Sregs.es, offset);
	for (p = (unsigned char *)idstr; *p; p++)
		if (*p != *fp++)
			return 0;
	return 1;
}

static int near __PASCAL
check_atok7(int inum)
{
	Regs.h.ah = GET_VECTOR;
	Regs.h.al = (unsigned char)inum;
	Intdosx(&Regs, &Regs, &Sregs);
	fp = (unsigned char far *)MK_FAR(Sregs.es, Regs.x.bx + 0x30);
	for (p = (unsigned char *)"ATOK7"; *p; p++)
		if (*p != *fp++)
			return 0;
#ifdef LATTICE
	atok7_seg = Sregs.es;
	atok7_off = Regs.x.bx + 0x20;
#else
	atok7_entry = (void (far __CDECL *)(void))
				MK_FAR(Sregs.es, Regs.x.bx+0x20);
#endif
	return 1;
}

#if !defined(__TURBOC__) && !defined(LSI_C) && !defined(MSC_INLINE)
/*
 * fake function to call fixer_entry() and atok7_entry()
 *		for Microsoft C 5.1 and Lattice C
 */
static unsigned char far fixer_fake[] = {
	0x55,			/* push  bp				*/
	0x8b, 0xec,		/* mov   bp, sp				*/
	0x8b, 0x46, 0x0a,	/* mov   ax, word ptr [bp+10]	;func	*/
	0x8a, 0x56, 0x0c,	/* mov   dl, byte ptr [bp+12]	;val	*/
	0xff, 0x5e, 0x06,	/* call  dword ptr [bp+6]	;addr	*/
	0x33, 0xc0,		/* xor   ax, ax				*/
	0x8a, 0xc2,		/* mov   al, dl				*/
	0x8b, 0xe5,		/* mov   sp, bp				*/
	0x5d,			/* pop   bp				*/
	0xcb			/* retf					*/
};
static unsigned char far atok7_fake[] = {
	0x55,			/* push  bp				*/
	0x8b, 0xec,		/* mov   bp, sp				*/
	0x8b, 0x46, 0x0e,	/* mov   ax, word ptr [bp+14]	;pseg	*/
	0x8e, 0xc0,		/* mov   es, ax				*/
	0x8b, 0x5e, 0x0c,	/* mov   bx, word ptr [bp+12]	;poff	*/
	0x8b, 0x46, 0x0a,	/* mov   ax, word ptr [bp+10]	;func	*/
	0xff, 0x5e, 0x06,	/* call  dword ptr [bp+6]	;addr	*/
	0x8b, 0xe5,		/* mov   sp, bp				*/
	0x5d,			/* pop   bp				*/
	0xcb			/* retf					*/
};
#ifdef LATTICE
static int (far __CDECL *fixer_dummy)(void (far *)(void), int, int) 
	= (int (far __CDECL *)())fixer_fake;
static void (far __CDECL *atok7_dummy)(unsigned,unsigned,int,unsigned,unsigned)
	= (void (far __CDECL *)())atok7_fake;
#endif
#endif /* Microsoft C 5.1, Lattice C */

static int near __PASCAL
call_fixer(int func, int val)
{
	if (api_vec != -1) {
		regs.x.ax = func;
		regs.h.dl = (unsigned char)val;
		int86(api_vec, &regs, &regs);
		return regs.h.dl;
	}
#if defined(__TURBOC__)
	_AX = func;
	_DL = (unsigned char)val;
	fixer_entry();
	return (int)_DL;
#elif defined(LSI_C)
	fixer_entry(func, 0, 0, val);  /* ax = func, dx = val */
	_asm_c("\n\txor\tax,ax");
	_asm_c("\n\tmov\tal,dl");
	/* return ax */
#elif defined(MSC_INLINE)
	_asm {
		mov	ax,func
		mov	dl,val
	}
	fixer_entry();
	_asm {
		xor	ax,ax
		mov	al,dl
	}
	/* return ax */
#elif defined(LATTICE)
	return fixer_dummy(fixer_entry, func, val);
#else /* Microsoft C 5.1 */
	return ((int (far __CDECL *)(void (far __CDECL *)(void), int, int))
					fixer_fake)(fixer_entry, func, val);
#endif
}

static int near __PASCAL
call_atok7(int func, int val)
{
#if defined(MSC_INLINE)
	int pseg, poff;
#endif
	int packet[32];

	*packet = val;
#if defined(__TURBOC__)
	_ES = FAR_SEG((void far *)packet);
	_BX = FAR_OFF((void far *)packet);
	_AX = func;
	atok7_entry();
#elif defined(LSI_C)
	_asm_c("\n\tpush\tss");
	_asm_c("\n\tpop\tes");
	atok7_entry(func, packet);  /* ax = func, es:bx = packet */
#elif defined(MSC_INLINE)
	pseg = FAR_SEG((void far *)packet);
	poff = FAR_OFF((void far *)packet);
	_asm {
		mov	ax,pseg
		mov	es,ax
		mov	bx,poff
		mov	ax,func
	}
	atok7_entry();
#elif defined(LATTICE)
	atok7_dummy(atok7_off, atok7_seg, func,
		FAR_OFF((void far *)packet), FAR_SEG((void far *)packet));
#else /* Microsoft C 5.1 */
	((void (far __CDECL *)(void (far __CDECL *)(void), int, int far *))
			atok7_fake)(atok7_entry, func, (int far *)packet);
#endif
	return *packet;
}

/*
 * PC-9801 CRT/KBD BIOS, VRAM
 */
#define	KBDBIOS		((unsigned char far *)MK_FAR(0x0000, 0x0502))
#define	CRTWORK		((unsigned char far *)MK_FAR(0x0000, 0x053d))
#define	CRTLINES	((unsigned char far *)MK_FAR(0x0000, 0x0713))
#define	VRAM		((unsigned char far *)MK_FAR(0xa000, 0x0000))
#define	KBDMAX		16		/* max # of keys in KBDBUF */
#define	CTRLXFER	0xb500		/* CTRL-XFER key scan code */

struct kbdbios_t {
	unsigned short		buf[KBDMAX];
	unsigned char near	*tbl;
	unsigned short near	*head;
	unsigned short near	*tail;
	unsigned char		count;
} far *kp = (struct kbdbios_t far *)KBDBIOS;

/*
 * Look into CRT BIOS work area and check FEP status
 */
static int near __PASCAL
pc98_crtmode(void)
{
	return (*CRTWORK == 2)? 1: 0;
}

#ifdef DO_USERDEV
/*
 * Look into VRAM bottom right corner and check FEP status (for FEP_KAZE)
 */
static int near __PASCAL
pc98_vrammode(void)
{
	return ((VRAM[*CRTLINES? 0x0f9d: 0x0c7d] & 0x7f) > '!')? 1: 0;
}
#endif

/*
 * Set key code directly into PC-9801 BIOS key code buffer
 */
static void near __PASCAL
pc98_setkey(unsigned short keycode)
{
	disable();
	*(unsigned short far *)(MK_FAR(0x0000, kp->tail)) = keycode;
	kp->count++;
	kp->tail++;
	if (kp->tail >= (unsigned short near *)FAR_OFF2(&kp->buf[KBDMAX]))
		kp->tail = (unsigned short near *)FAR_OFF2(kp->buf);
	enable();
}

static void near __PASCAL
pc98_flushkey(unsigned short keycode)
{
	while (bdos(6, 0xff, 0) & 0xff) /* void */;
	pc98_setkey(keycode);
	while (bdos(6, 0xff, 0) & 0xff) /* void */;
}

static void near __PASCAL
pc98_pushkey(unsigned short keycode)
{
	/*
	 * raise dummy keyboard interrupt after feeding CTRL-XFER.
	 * i know it is a brute force attack, but it works!
	 */
	pc98_setkey(keycode);
	int86(0x09, &regs, &regs);
	while (bdos(6, 0xff, 0) & 0xff) /* void */;
}

static int near __PASCAL
set_cbrk(int newcbrk)
{
	int curcbrk;

	/*
	 * get current cbrk value
	 */
	regs.x.ax = GET_CBRK;
	intdos(&regs, &regs);
	curcbrk = regs.h.dl;
	/*
	 * set new cbrk value
	 */
	regs.x.ax = SET_CBRK;
	regs.h.dl = newcbrk;
	intdos(&regs, &regs);
	return curcbrk;
}

static int near __PASCAL
dointr(int vec, int ah, int al)
{
	regs.h.ah = ah;
	regs.h.al = al;
	int86(vec, &regs, &regs);
	return regs.h.al;
}

/*
 * NEC extended function call (int DCh)
 */
static void near __PASCAL
donec(int cl)
{
	regs.x.ax = 0;
	regs.h.cl = cl;
	int86(0xdc, &regs, &regs);
}

static void near __PASCAL
fep_open(void)
{
	if (fep == FEP_NEC)
		donec(224);
}

static void near __PASCAL
fep_close(void)
{
	if (fep == FEP_NEC)
		donec(225);
}

static int near __PASCAL
fep_mode(int newmode)
{
	int curmode;

	switch (fep) {
#ifdef DO_MSKANJI
	case FEP_MSKANJI:
	case FEP_OTEMOTO:
		/* mode: 0 = off, 1 = system, 2 = echo */
		curmode = msknj_get_mode();
		if (newmode != -1 && newmode != curmode)
			msknj_set_mode(newmode);
		return curmode;
#endif
	case FEP_VJE:
	case FEP_EGB2:
	case FEP_EGB3:
		/* mode: 0 = off, 1 = system, 2 = echo */
		curmode = dointr(0x70, 0, 0);
		if (newmode != -1 && newmode != curmode) {
			dointr(0x70, 1, newmode);
			if (fep == FEP_EGB2)
				dointr(0x70, 12, 0); /* unlock mode key */
		}
		return curmode;
	case FEP_ATOK6:		/* api_vec = 0x6f */
	case FEP_AT6AX:		/* api_vec = 0x65 */
	case FEP_AT6IBM:	/* api_vec = 0x60 */
	case FEP_DANGO:		/* api_vec = 0x6f */
		/* mode: 0=off, 1=roma, 2=kana, 3=alpha, 4=sym, 5=code */
		curmode = dointr(api_vec, 102, 0);
		if (newmode != -1 && newmode != curmode)
			dointr(api_vec, (!newmode? 11: newmode), 0);
		return curmode;
	case FEP_ATOK7:
	case FEP_AT7IBM:
		/* mode: 0 = off, 1 = on */
		curmode = call_atok7(3, -1)? 1: 0;
		if (newmode != -1 && newmode != curmode)
			call_atok7(newmode? 1: 2, 0);
		return curmode;
	case FEP_MTTK2:		/* api_vec = 0x70 */
	case FEP_MIJ:		/* api_vec = 0x6f */
		/* mode: 0 = off, 1 = on */
		Regs.h.ah = 21;
		Int86x(api_vec, &Regs, &Regs, &Sregs);
		fp = (unsigned char far *)MK_FAR(Sregs.es, Regs.x.bx);
		curmode = fp[5];
		if (newmode != -1 && newmode != curmode)
			dointr(api_vec, 0, newmode);
		return curmode;
	case FEP_FIXER:
		/* mode: 0 = off, 1 = echo, 2 = system */
		curmode = call_fixer(0x0901, 0);
		if (newmode != -1 && newmode != curmode)
			call_fixer(0x0900, newmode);
		return curmode;
	case FEP_KATANA:
		/* mode: 0 = off, 1 = echo, 2 = system */
		curmode = dointr(0x48, 2, 0);
		if (newmode != -1 && newmode != curmode) {
			int old = set_cbrk(0);
			dointr(0x48, 3, newmode);
			set_cbrk(old);
		}
		return curmode;
	case FEP_WXP:
		/* mode: 0 = off, 1 = echo, 2 = system */
		curmode = dointr(0x70, 0x4c, 0);
		if (newmode == 0 && curmode > 0) {
			while (dointr(0x70, 0x4c, 0))
				pc98_flushkey(CTRLXFER);
		} else if (newmode > 0 && curmode == 0) {
			dointr(0x70, 0x5c, newmode);
			dointr(0x70, 0x52, 1);
		}
		return curmode;
	case FEP_WX2:		/* api_vec = 0x70 */
	case FEP_WXPJ:		/* api_vec = 0x6c */
		/* mode: 0 = off, 1 = echo, 2 = system */
		curmode = dointr(api_vec, 0x4c, 0);
		if (newmode != -1 && newmode != curmode)
			dointr(api_vec, 0, newmode);
		return curmode;
	case FEP_MGR2:
	case FEP_JJ:
		/* mode: 0 = off, 1 = on */
		regs.h.ah = 0;
		int86(0x70, &regs, &regs);
		curmode = (regs.h.dl & 0x01);
		if (newmode != -1 && newmode != curmode)
			dointr(0x46, 0, newmode);
		return curmode;
	case FEP_JOKER3:
		/* mode: 0 = off, 1 = on */
		curmode = dointr(0xf1, 0x8f, 0);
		if (newmode != -1 && newmode != curmode)
			dointr(0xf1, newmode? 0x86: 0x85, 0);
		return curmode;
	case FEP_NEC:
		/* mode: 0 = off, 1 = on */
		curmode = pc98_crtmode();
		if (newmode != -1 && newmode != curmode)
			donec(newmode? 239: 240);
		return curmode;
	case FEP_PC98A:
	case FEP_MTTK86:
	case FEP_DFJ:
		/* mode: 0 = off, 1 = on */
		curmode = pc98_crtmode();
		if (newmode != -1)
			while (pc98_crtmode() != newmode)
				pc98_flushkey(CTRLXFER);
		return curmode;
	case FEP_PC98B:
	case FEP_AJIP1:
		/* mode: 0 = off, 1 = on */
		curmode = pc98_crtmode();
		if (newmode != -1 && newmode != curmode)
			pc98_setkey(CTRLXFER);
		return curmode;
	case FEP_PC98C:
	case FEP_OMAC:
		/* mode: 0 = off, 1 = on */
		curmode = pc98_crtmode();
		if (newmode != -1 && newmode != curmode)
			pc98_pushkey(CTRLXFER);
		return curmode;
#ifdef DO_USERDEV
	case FEP_KAZE:
		/* mode: 0 = off, 1 = on */
		curmode = pc98_vrammode();
		if (newmode != -1 && newmode != curmode)
			pc98_flushkey(CTRLXFER);
		return curmode;
#endif
#ifdef DO_GENERIC
	case FEP_OAK:
		/*
		 * mode: 0r010100 = off, 1r010001 = on (r = romaji)
		 *   bit7: size: 0 = hankaku, 1 = zenkaku
		 *   bit6: romaji entry: 0 = off, 1 = on
		 *   bit5: always 0
		 *   bit4: mode change: 0 = disabled, 1 = enabled
		 *   bit3: always 0
		 * bit2-0: char type: 001 = hiragana, 010 = katakana,
		 *	   011 = uppercase, 100 = lowercase, 101 = al
		 */
#define	OAKOFF(mode)	((mode) == 0 || ((mode) & 0x84) == 0x04)
		curmode = dointr(0xec, 3, 0);
		if (newmode == -1)
			return OAKOFF(curmode)? 0: curmode;
		if (OAKOFF(newmode) && !OAKOFF(curmode))
			dointr(0xec, 2, (curmode & 0x50) | 0x04);
		else if (!OAKOFF(newmode) && OAKOFF(curmode))
			dointr(0xec, 2, newmode);
		return curmode;
	case FEP_MKK:
		/* mode: 0 = off, 1 = system, 2 = echo */
		curmode = dointr(0xc0, 1, 0);
		if (newmode != -1 && newmode != curmode)
			dointr(0xc0, 0, newmode);
		return curmode;
	case FEP_B16:
	case FEP_RICOH:
		/* mode: 00000000 = off, 10000000 = on */
		curmode = dointr(0x16, 8, 0) & 0x80;
		if (newmode != -1 && newmode != curmode)
			dointr(0x16, 7, newmode);
		return curmode;
	case FEP_FEPEX:
		/* mode: 0 = off, 1 = on */
		regs.h.cl = 0x0e;
		int86(0x83, &regs, &regs);
		curmode = regs.h.al;
		if (newmode != -1 && newmode != curmode) {
			regs.h.cl = 0x0b;
			regs.h.bl = newmode;
			int86(0x83, &regs, &regs);
		}
		return curmode;
	case FEP_AT7DOSV:
		/* mode: 00000000 = off, 10000000 = on */
		regs.x.ax = 0x1301;
		int86(0x16, &regs, &regs);
		curmode = regs.h.dl;
		if (newmode != -1 && newmode != curmode) {
			regs.x.ax = 0x1300;
			regs.h.dl = newmode;
			int86(0x16, &regs, &regs);
		}
		return curmode;
	case FEP_IAS:
		/*
		 * mode: 0rxxx000 = off, 1rxxx101 = on (r = romaji)
		 *   bit7: kanji: 0 = off, 1 = on
		 *   bit6: romaji: 0 = off, 1 = on
		 * bit5-3: reserved
		 * bit2-1: shift: 00=alpha, 01=kata, 10=hira, 11=reserved
		 *   bit0: size: 0 = hankaku, 1 = zenkaku
		 */
#define	IASOFF(mode)	((mode & 0x87) == 0x00)
		regs.x.ax = 0x1301;
		int86(0x16, &regs, &regs);
		curmode = regs.h.dl;
		if (newmode == -1)
			return IASOFF(curmode)? 0: curmode;
		if (IASOFF(newmode) != IASOFF(curmode)) {
			regs.x.ax = 0x1300;
			regs.h.dl = (newmode & 0x87) | (curmode & 0x78);
			int86(0x16, &regs, &regs);
		}
		return curmode;
#endif /* DO_GENERIC */
	default:
		return 0;
	}
}

#ifdef DO_MSKANJI
/*
 * MS-KANJI Application Interface access routines
 */

struct Kkname {
	short		wLevel;			/* API support level */
	char		rgchName[8];		/* name of system */
	char		rgchVersion[4];		/* version # of system */
	char		rgchRevision[4];	/* revision # of system */
	char		Reserved[14];		/* reserved */
};
struct Dataparm {
	short		wType;			/* display switch */
	short		wScan;			/* key scan code */
	short		wAscii;			/* ascii code */
	short		wShift;			/* shift key status */
	short		wExShift;		/* extended shift key status */
	short		cchResult;		/* size of confirmed string */
	char far	*lpchResult;		/* ptr to confirmed string */
	short		cchMode;		/* size of mode display str */
	char far	*lpchMode;		/* ptr to mode display str */
	char far	*lpattrMode;		/* ptr to attribute of it */
	short		cchSystem;		/* size of system string */
	char far	*lpchSystem;		/* ptr to system string */
	char far	*lpattrSystem;		/* ptr to attribute of it */
	short		ccBuf;			/* size of unconfirmed str */
	char far	*lpchBuf;		/* ptr to unconfirmed str */
	char far	*lpattrBuf;		/* ptr to attribute of it */
	short		cchBufCursor;		/* cursor position in it */
	char		Reserved[34];		/* reserved */
};
struct Funcparm {
	short		wFunc;			/* function # */
	short		wMode;			/* reference or set */
	struct Kkname far *lpKkname;		/* translation system */
	struct Dataparm far *lpDataparm;	/* data parameter */
	char		Reserved[4];		/* reserved */
};

/*
 * function # for wFunc
 */
#define	KKAsk	1
#define	KKOpen	2
#define KKClose	3
#define	KKInOut	4
#define	KKMode	5

/*
 * mode bit for wMode
 */
#define	KM_GET	0x0000
#define	KM_SET	0x8000
#define	KM_OFF	0x0001
#define	KM_ON	0x0002
#define	KM_SYS	0x0004
#define	KM_ECHO	0x0008

#if defined(LSI_C)
static int (far *KKfunc)(void);
#elif defined(__TURBOC__)
static int (far pascal *KKfunc)(struct Funcparm far *);
#else /* Microsoft C, Quick C, Lattice C */
static int (far pascal *KKfunc)(int *, struct Funcparm far *);
#endif
static int near __PASCAL msknj_kkfunc(struct Funcparm far *);
static struct Funcparm Funcbuf;
static struct Kkname Kkbuf;

static int near __PASCAL
msknj_find(char *dname)
{
	int fd, n;

	if (!check_dev(dname))
		return 0;

	/*
	 * get entry address of KKfunc()
	 */
	fd = open_dev(dname);
	regs.x.ax = RCV_IOC_CHAR;
	regs.x.bx = fd;
	regs.x.cx = 4;
	regs.x.dx = FAR_OFF((void far *)&KKfunc);
	sregs.ds = FAR_SEG((void far *)&KKfunc);
	intdosx(&regs, &regs, &sregs);
	n = regs.x.ax;
	close_dev(fd);
	if (n != 4)		/* ioctl failed */
		return 0;

	/*
	 * check API support level
	 */
	memset(&Funcbuf, 0, sizeof(Funcbuf));
	Funcbuf.wFunc = KKAsk;
	Funcbuf.lpKkname = (struct Kkname far *)&Kkbuf;
	if (msknj_kkfunc((struct Funcparm far *)&Funcbuf) < 0)
		return 0;
	if (Kkbuf.wLevel < 1)	/* level too low */
		return 0;

	/*
	 * we don't use MS-KANJI API upon ATOK7 with MSATOK,
	 * because SoftVision's MSATOK doesn't satisfy the
	 * specification of KKMode function.  MSKN 2.0 (later
	 * version of MSATOK; identified as "MSKN    ") is O.K.
	 */
	if (!strncmp(Kkbuf.rgchName, "MSKNATOK", 8))
		return 0;

	return 1;
}

static int near __PASCAL
msknj_get_mode(void)
{
	memset(&Funcbuf, 0, sizeof(Funcbuf));
	Funcbuf.wFunc = KKMode;
	Funcbuf.wMode = KM_GET;
	msknj_kkfunc((struct Funcparm far *)&Funcbuf);
	return (Funcbuf.wMode & KM_OFF)? 0: (Funcbuf.wMode & KM_SYS)? 1: 2;
}

static int near __PASCAL
msknj_set_mode(int mode)
{
	memset(&Funcbuf, 0, sizeof(Funcbuf));
	Funcbuf.wFunc = KKMode;
	Funcbuf.wMode = KM_SET;
	Funcbuf.wMode |= (mode == 0)? KM_OFF: KM_ON;
	Funcbuf.wMode |= (mode == 1)? KM_SYS: KM_ECHO;
	msknj_kkfunc((struct Funcparm far *)&Funcbuf);
	return 0;
}

static int near __PASCAL
msknj_kkfunc(struct Funcparm far *bufp)
{
#if defined(__TURBOC__)
	static int sp, ret;
	sp = _SP;
	ret = KKfunc((struct Funcparm far *)bufp);
	_SP = sp;
	return ret;
#elif defined(LSI_C)
	static int savesp, ret;
	savesp = _asm_i("\n\tmov\tax,sp");
	/*
	 * pass the parameter to KKfunc() thru stack
	 *	mov   ax, [bufp].seg
	 *	mov   bx, [bufp].off
	 *	push  bx
	 *	push  ax
	 */
	_asm_c("\n\tpush\tbx\n\tpush\tax", (struct Funcparm far *)bufp);
	ret = KKfunc();
	_asm_c("\n\tmov\tsp,ax", savesp);
	return ret;
#else /* Microsoft C, Quick C, Lattice C */
	int magic;
	/*
	 * magic cookie (hinted by Oh!No!):
	 * adjust resulting stack pointer for both C and pascal calls.
	 * valid under all memory models of Microsoft C and Lattice C.
	 */
	return KKfunc(&magic, (struct Funcparm far *)bufp);
#endif
}
#endif /* DO_MSKANJI */

#endif /* FEPCTRL */
