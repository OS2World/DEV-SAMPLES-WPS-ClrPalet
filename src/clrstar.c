/******************************************************************************
 * MODULE NAME: ClrStar.C
 *
 * DESCRIPTION:
 *   Source code for StarColorPalette object class.
 *
 * OBJECT CLASS: StarColorPalette
 *
 * CLASS HIERARCHY:
 *
 *     SOMObject
 *       --- WPObject
 *             --- WPAbstract
 *                   --- WPPalette
 *                         ---  WPColorPalette
 *                                  ---  ColorPalette
 *                                           ---  StarColorPalette
 *
 * DESCRIPTION:
 *   Minimal illustration of a palette subclass: it only overrides
 *   QuerySampleShape so each color swatch is drawn as a star instead of a
 *   rectangle.
 *
 * Original work:
 *   (C) IBM Corporation 1992/1993 - Named Color Palette WPS sample.
 *   Ported from the SOM 1.0 precompiler layout to modern SOM IDL bindings
 *   plus Open Watcom; every deviation is tagged below and summarized in
 *   README.md.
 *
 * PORTING NOTES:
 *   - the override's first parameter is StarColorPalette*, not
 *     ColorPalette* as SOM 1.0 generated: modern sc types overrides with
 *     the IMPLEMENTING class, and the definition must match the generated
 *     prototype exactly
 *   - no metaclass exists for this class; sc emits no M_StarColorPalette*
 *     bindings either ("SOM didn't give us a metaclass", as the original
 *     DEF put it), so there is nothing metaclass-related to export
 */

/*
 * PM/DOS/WPS header prelude.  The SOM 1.0 precompiler emitted these into
 * the generated .IH (see orig\CLRSTAR.CSC 'passthru: C.ih'); sc does not,
 * so they are carried here instead - BEFORE any class bindings are included.
 */
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_WINWORKPLACE
#define INCL_WPCLASS
#define INCL_WPFOLDER

#include <os2.h>

/* Strip runtime overhead out of the generated bindings (must precede the
 * .ih include - see clrpalet.c for details).
 */
#define SOM_NoTest 1        /* Disables somTestCls on method resolution */
#define _RETAIL             /* Disables xxxMethodDebug() functions      */

#define StarColorPalette_Class_Source

#include "clrstar.ih"       /* implementation header emitted from clrstar.idl */
#include <string.h>

/********************* INSTANCE METHODS SECTION *****************************\
 *        Do not put any code in this section unless it is really an        *
 *      object INSTANCE method: otherwise parent method resolution will     *
 *                  not work correctly for that method                      *
\****************************************************************************/
#undef SOM_CurrentClass
#define SOM_CurrentClass SOMInstance

/*
 * METHOD OVERRIDE: QuerySampleShape
 *
 * DESCRIPTION:
 *   Override the QuerySampleShape method so that this subclass will have a
 *   different shaped color swatch displayed in its palette view.
 */
SOM_Scope BOOL SOMLINK clrs_QuerySampleShape(
    StarColorPalette *somSelf,
    PPOINTL          pPoints,
    PULONG           pcPoints)
{
    #define STAR_CPOINTS               11
    static POINTL ptlStar[STAR_CPOINTS] = { {  20,   0 },
                                             {  30,  40 },
                                             {   0,  60 },
                                             {  30,  60 },
                                             {  50, 100 },
                                             {  70,  60 },
                                             { 100,  60 },
                                             {  70,  40 },
                                             {  80,   0 },
                                             {  50,  30 },
                                             {  20,   0 }
                                           };

    /* Return the coordinate data for a star shape, instead of just
     * letting this method go through to our parent class.
     */
    if (pPoints)
        memcpy(pPoints, ptlStar, STAR_CPOINTS * sizeof(POINTL));
    if (pcPoints)
        *pcPoints = STAR_CPOINTS;
    return TRUE;
}
