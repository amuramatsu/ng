/* $Id: fepctrl.h,v 1.2 2001/11/23 11:56:49 amura Exp $ */
/*
 * fepctrl.h 1.5 1992/04/08.  Public Domain.
 *
 * General purpose Japanese FEP control routines for MS-DOS.
 * Written by Junn Ohta (ohta@src.ricoh.co.jp, msa02563)
 *
 * Compiles under:
 *	Turbo C 1.5/2.0, Turbo C++ 1.0, Borland C++ 2.0/3.0,
 *	Microsoft C 5.1, Microsoft C 6.0, Quick C 2.0,
 *	Lattic C 4.1 (need negative stack frame, except huge model),
 *	LSI C-86 3.2/3.3 Sampler (small model only).
 */

/*
 * $Log: fepctrl.h,v $
 * Revision 1.2  2001/11/23 11:56:49  amura
 * Rewrite all sources
 *
 * Revision 1.1.1.1  2000/06/27 01:47:58  amura
 * import to CVS
 *
 */

#ifndef __FEPCTRL_H
#define __FEPCTRL_H

/*
 * Holy Oracle for MS-DOS C compilers
 */
#if defined(__TURBOC__)
#  if __STDC__
#    define _Cdecl
#    define _Pascal
#  else
#    define _Cdecl  cdecl
#    define _Pascal pascal
#  endif
#  define __CDECL  _Cdecl
#  define __PASCAL _Pascal
#elif defined(LSI_C)
#  define __CDECL
#  define __PASCAL
#elif defined(LATTICE)
#  define __CDECL
#  define __PASCAL pascal
#elif defined(_MSC_VER) && _MSC_VER >= 600
#  define __CDECL  _cdecl
#  define __PASCAL _fastcall
#else /* Microsoft C 5.1, Quick C */
#  ifndef NO_EXT_KEYS
#    define _CDECL  cdecl
#    define _PASCAL pascal
#  else
#    define _CDECL
#    define _PASCAL
#  endif
#  define __CDECL  _CDECL
#  define __PASCAL _PASCAL
#endif

/*
 * Japanese FEP type (returned by fep_init())
 */
enum __fep_t {
    FEP_NONE,
    FEP_PC98A,		/* FEP controlled via PC-9801 BIOS (type A) */
    FEP_PC98B,		/* FEP controlled via PC-9801 BIOS (type B) */
    FEP_PC98C,		/* FEP controlled via PC-9801 BIOS (type C) */
    FEP_MSKANJI,	/* MS-KANJI Application Interface */
    FEP_VJE,		/* VACS Japanese Entry System Alpha/Sigma/Beta */
    FEP_ATOK6,		/* JUSTSYSTEM's Automatic Transfer of Kana-Kanji 6 */
    FEP_ATOK7,		/* JUSTSYSTEM's Automatic Transfer of Kana-Kanji 7 */
    FEP_MTTK86,		/* K3's Matsutake (pinetree mushroom) 86 */
    FEP_MTTK2,		/* K3's Matsutake (pinetree mushroom) V2 */
    FEP_KATANA,		/* Something Good's Katana/ACE */
    FEP_FIXER,		/* Citysoft's FIXER3/FIXER4 */
    FEP_EGB2,		/* ERGOSOFT's EGBridge Ver.2 */
    FEP_EGB3,		/* ERGOSOFT's EGBridge Ver.3 */
    FEP_WXP,		/* A.I.Soft's WXP 1.03a, WX 1.0, WXS 1.0 */
    FEP_WX2,		/* A.I.Soft's WXII with /VZ (and WXP 1.04c) */
    FEP_MGR2,		/* REED REX's MGR2 */
    FEP_JJ,		/* REED REX's JJ */
    FEP_NEC,		/* NEC's Chikuji/AI Kana-Kanji Henkan */
    FEP_DFJ,		/* Digital Farm's DFJ Ver.1 */
    FEP_DANGO,		/* Suzuki Kyoiku Soft's DANGO Henkan */
    FEP_OTEMOTO,	/* Gengo Kogaku Kenkyujo's OTEMOTO (at hand) */
    FEP_OMAC,		/* Morrin's OMAC */
    FEP_AJIP1,		/* Acel's AJIP1 */
    FEP_JOKER3,		/* Micronics's Joker-III */
    FEP_KAZE,		/* Enzan-Hoshi-Gumi's Kaze / Arashi */
    FEP_OAK,		/* OASYS Kana-Kanji Henkan (Fujitsu FM-R series) */
    FEP_MKK,		/* Matsushita Kana-Kanji Henkan (Panacom series) */
    FEP_B16,		/* Renbunsetsu/Jidou Henkan (Hitachi B16 series) */
    FEP_RICOH,		/* Nihongo Nyuryoku System (RICOH Mr. Mytool MAGUS) */
    FEP_WXPJ,		/* A.I.Soft's WXP (Toshiba J3100) */
    FEP_MIJ,		/* MIJ (Toshiba J3100); NOT TESTED!! */
    FEP_FEPEX,		/* ERGOSOFT's FEPEX Ver.2 (Oki IF800) */
    FEP_AT6AX,		/* JUSTSYSTEM's ATOK6 (AX machines) */
    FEP_AT6IBM,		/* JUSTSYSTEM's ATOK6 (IBM PS/55 JDOS4) */
    FEP_AT7IBM,		/* JUSTSYSTEM's ATOK7 (IBM PS/55 JDOS4) */
    FEP_AT7DOSV,	/* JUSTSYSTEM's ATOK7 (IBM DOS/V) */
    FEP_IAS,		/* IBM DOS/V Input Assist Subsystem (SKK, MKK) */
    
    NFEPS		/* Number of FEPs defined */
};

#ifdef __cplusplus
extern "C" {
#endif
int  __CDECL fep_init(void);
void __CDECL fep_term(void);
void __CDECL fep_on(void);
void __CDECL fep_off(void);
void __CDECL fep_force_on(void);
void __CDECL fep_force_off(void);
int  __CDECL fep_raw_init(void);
void __CDECL fep_raw_term(void);
int  __CDECL fep_get_mode(void);
#ifdef __cplusplus
}
#endif

#endif /* __FEPCTRL_H */
