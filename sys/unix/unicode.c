/* $Id: unicode.c,v 1.1 2001/08/17 19:15:08 amura Exp $ */
/*
 *		Unicode handling routines for POSIX iconv
 *		These are only used when UNICODE is #defined.
 *
 *		Coded by MURAMATSU Atsushi
 */

#include "config.h"
#include "def.h"

#ifdef	UNICODE
#include "kanji.h"
#include <iconv.h>
#include <errno.h>

#ifndef	EUC_CODENAME
#define	EUC_CODENAME	"EUC-JP"
#endif

#ifndef	UCS2_CODENAME
#define	UCS2_CODENAME	"UCS-2"
#endif

#ifndef	UTF8_CODENAME
#define	UTF8_CODENAME	"UTF-8"
#endif

#define	UTF8_KANJIONLY

static iconv_t iconv_u2_e = (iconv_t)-1;
static iconv_t iconv_e_u2 = (iconv_t)-1;
static iconv_t iconv_u8_e = (iconv_t)-1;
static iconv_t iconv_e_u8 = (iconv_t)-1;

VOID
utoe_in(c0, c1, c2)
     int *c0;
     int *c1;
     int *c2;
{
    char input[2], output[3];
    const char *p_in = input;
    char *p_out = output;
    size_t in_len = 2, out_len = 3;
    size_t res;
    
    input[0] = *c0 & 0xFF;
    input[1] = *c1 & 0xFF;
    bzero(output, sizeof(output));
    
    if (iconv_u2_e == (iconv_t)-1) {
	iconv_u2_e = iconv_open(EUC_CODENAME, UCS2_CODENAME);
	if (iconv_u2_e == (iconv_t)-1) {
	    *c0 = *c1 = *c2 = 0;
	    return;
	}
	iconv(iconv_u2_e, NULL, NULL, NULL, NULL);
    }

    res = iconv(iconv_u2_e, &p_in, &in_len, &p_out, &out_len);
    if (res < 0) {
	*c0 = *c1 = *c2 = 0;
	return;
    }
    *c0 = output[0] & 0xFF;
    *c1 = output[1] & 0xFF;
    *c2 = output[2] & 0xFF;
}

VOID
etou_in(c0, c1, c2)
     int *c0;
     int *c1;
     int c2;
{
    char input[3], output[10];
    const char *p_in = input;
    char *p_out = output;
    size_t in_len = 2, out_len = sizeof(output);
    size_t res;

    input[0] = *c0;
    input[1] = *c1;
    if (c2) {
	input[2] = c2;
	in_len = 3;
    }
    bzero(output, sizeof(output));

    if (iconv_e_u2 == (iconv_t)-1) {
	iconv_e_u2 = iconv_open(UCS2_CODENAME, EUC_CODENAME);
	if (iconv_e_u2 == (iconv_t)-1) {
	    *c0 = *c1 = 0;
	    return;
	}
	iconv(iconv_e_u2, NULL, NULL, NULL, NULL);
    }
    res = iconv(iconv_e_u2, &p_in, &in_len, &p_out, &out_len);
    if (res < 0) {
	ewprintf("Error code %d", errno);
	ttwait();
	*c0 = *c1 = 0;
	return;
    }
    *c0 = output[0] & 0xFF;
    *c1 = output[1] & 0xFF;
}

int
bufu2toe(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
    char *output;
    size_t in_len = len, out_len = buflen;
    const char *in_p;
    char *out_p;
    size_t res;

    if (iconv_u2_e == (iconv_t)-1) {
	iconv_u2_e = iconv_open(EUC_CODENAME, UCS2_CODENAME);
	if (iconv_u2_e == (iconv_t)-1) {
	    return 0;
	}
	iconv(iconv_u2_e, NULL, NULL, NULL, NULL);
    }
    in_p = p;
    out_p = output = alloca(buflen);
    res = iconv(iconv_u2_e, &in_p, &in_len, &out_p, &out_len);
    if (res < 0)
	return 0;
    bcopy(output, p, res);
    return res;
}

int
bufu8toe(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
    char *output;
    size_t in_len = len, out_len = buflen;
    const char *in_p;
    char *out_p;
    size_t res;

    if (iconv_u8_e == (iconv_t)-1) {
	iconv_u8_e = iconv_open(EUC_CODENAME, UTF8_CODENAME);
	if (iconv_u8_e == (iconv_t)-1) {
	    return 0;
	}
	iconv(iconv_u8_e, NULL, NULL, NULL, NULL);
    }
    in_p = p;
    out_p = output = alloca(buflen);
    res = iconv(iconv_u8_e, &in_p, &in_len, &out_p, &out_len);
    if (res < 0)
	return 0;
    bcopy(output, p, res);
    return res;
}

int
bufetou2(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
    char *output;
    size_t in_len = len, out_len = buflen;
    const char *in_p;
    char *out_p;
    size_t res;

    if (iconv_e_u2 == (iconv_t)-1) {
	iconv_e_u2 = iconv_open(UCS2_CODENAME, EUC_CODENAME);
	if (iconv_e_u2 == (iconv_t)-1) {
	    return 0;
	}
	iconv(iconv_e_u2, NULL, NULL, NULL, NULL);
    }
    in_p = p;
    out_p = output = alloca(buflen);
    res = iconv(iconv_e_u2, &in_p, &in_len, &out_p, &out_len);
    if (res < 0)
	return 0;
    bcopy(output, p, res);
    return res;
}

int
bufetou8(p, len, buflen)
    char *p;
    int len;
    int buflen;
{
    char *output;
    size_t in_len = len, out_len = buflen;
    const char *in_p;
    char *out_p;
    size_t res;

    if (iconv_e_u8 == (iconv_t)-1) {
	iconv_e_u8 = iconv_open(UTF8_CODENAME, EUC_CODENAME);
	if (iconv_e_u8 == (iconv_t)-1) {
	    return 0;
	}
	iconv(iconv_e_u8, NULL, NULL, NULL, NULL);
    }
    in_p = p;
    out_p = output = alloca(buflen);
    res = iconv(iconv_e_u8, &in_p, &in_len, &out_p, &out_len);
    if (res < 0)
	return 0;
    bcopy(output, p, res);
    return res;
}

VOID
iconv_all_close()
{
    if (iconv_u2_e != (iconv_t)-1) {
	iconv_close(iconv_u2_e);
    }
    if (iconv_e_u2 != (iconv_t)-1) {
	iconv_close(iconv_e_u2);
    }
    if (iconv_u8_e != (iconv_t)-1) {
	iconv_close(iconv_u8_e);
    }
    if (iconv_e_u8 != (iconv_t)-1) {
	iconv_close(iconv_e_u8);
    }
    iconv_u2_e = iconv_e_u2 = iconv_u8_e = iconv_e_u8 = (iconv_t)-1;
}
#endif /* UNICODE */
