/* $Id: i_line.h,v 1.1.2.2 2005/02/20 03:25:59 amura Exp $ */
/*
 * This file is the internal buffer definition of the NG
 * display editor.
 */

#ifndef __I_LINE_H__
#define __I_LINE_H__

typedef unsigned long LINE_NO_t;	/* line number type */
typedef short LINE_OFF_t;		/* line offset type */
typedef unsigned short NG_WCHAR_t;	/* char */

/*
 * All text is kept in circularly linked
 * lists of "LINE" structures. These begin at the
 * header line (which is the blank line beyond the
 * end of the buffer). This line is pointed to by
 * the "BUFFER". Each line contains a the number of
 * bytes in the line (the "used" size), the size
 * of the text array, and the text. The end of line
 * is not stored as a byte; it's implied. Future
 * additions will include update hints, and a
 * list of marks into the line.
 */
typedef struct LINE {
    struct LINE *l_fp;			/* Link to the next line	*/
    struct LINE *l_bp;			/* Link to the previous line	*/
    LINE_OFF_t l_size;			/* Allocated size		*/
    LINE_OFF_t l_used;			/* Used size			*/
#ifndef ZEROARRAY
    NG_WCHAR_t l_text[1];		/* A bunch of characters.	*/
#else
    NG_WCHAR_t l_text[];		/* A bunch of characters.	*/
#endif
} LINE;

/*
 * The rationale behind these macros is that you
 * could (with some editing, like changing the type of a line
 * link from a "LINE *" to a "REFLINE", and fixing the commands
 * like file reading that break the rules) change the actual
 * storage representation of lines to use something fancy on
 * machines with small address spaces.
 */
#define lforw(lp)	((lp)->l_fp)
#define lback(lp)	((lp)->l_bp)
#define lgetc(lp, n)	(CHARMASK((lp)->l_text[(n)]))
#define lputc(lp, n, c) ((lp)->l_text[(n)]=(c))
#define llength(lp)	((lp)->l_used)
#define ltext(lp)	((lp)->l_text)


/*
 * This structure holds the starting position
 * (as a line/offset pair) and the number of characters in a
 * region of a buffer. This makes passing the specification
 * of a region around a little bit easier.
 */
typedef struct	{
    struct LINE *r_linep;		/* Origin LINE address.		*/
    LINE_OFF_t r_offset;		/* Origin LINE offset.		*/
    RSIZE r_size;			/* Length in characters.	*/
} REGION;

#endif /* __I_LINE_H__ */
