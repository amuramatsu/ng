/* $Id: KgMenus.h,v 1.1 2000/06/27 01:48:00 amura Exp $ */
/*********************************************/
/*                                           */
/*       Designer (C) Ian OConnor 1994       */
/*                                           */
/*      Designer Produced C header file      */
/*                                           */
/*            For Kg 4.2                     */
/*                                           */
/*********************************************/

/*
 * $Log: KgMenus.h,v $
 * Revision 1.1  2000/06/27 01:48:00  amura
 * Initial revision
 *
 */

#define Win0FirstID 0
#define Win0_Gad0  0 /* Top    */
#define Win0_Gad1  1 /* Up     */
#define Win0_Gad2  2 /* Redraw */
#define Win0_Gad3  3 /* |<     */
#define Win0_Gad4  4 /*  <     */
#define Win0_Gad5  5 /*  >     */
#define Win0_Gad6  6 /*  >|    */
#define Win0_Gad7  7 /* Down   */
#define Win0_Gad8  8 /* Bottom */
#define Win0_Gad9  9  /* PgUp   */
#define Win0_Gad10 10 /* PgDown */
#define Win0_Gad11 11 /* >>     */
#define Win0_Gad12 12 /* <<     */
#define Win0_Gad13 13 /* Show   */
#define Win0_Gad14 14 /* Goto:  */
#define Win0_Gad15 15 /* SetFont  */
#define Win0_Gad16 16 /* SplitWindow  */
#define Win0_Gad17 17 /* NextWindow  */
#define Win0_Gad18 18 /* CloseWindow  */
#define Win0_Gad19 19 /* ToggleBORDER   */
#define Win0_Gad20 20 /* Done   */

extern struct TextAttr topaz800;
extern struct Gadget *Win0Gadgets[21];
extern struct Gadget *Win0GList;
extern struct Window *Win0;
extern APTR Win0VisualInfo;
extern UWORD Win0GadgetTypes[];
extern struct NewGadget Win0NewGadgets[];

extern int OpenWindowWin0( void );
extern void CloseWindowWin0( void );

