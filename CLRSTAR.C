/******************************************************************************
 * MODULE NAME: ClrStar.C
 *
 * DESCRIPTION:
 *	 Source code for StarColorPalette object class.
\****************************************************************************/
/* OBJECT CLASS: StarColorPalette
 *
 * CLASS HIERARCHY:
 *
 *     SOMObject
 *       ÀÄÄ WPObject
 *             ÀÄÄ WPAbstract
 *                   ÀÄÄ WPPalette
 *                         ÀÄÄ  WPColorPalette
 *                    		      ÀÄÄ  ColorPalette
 *                   		 		      ÀÄÄ  StarColorPalette
 *
 * DESCRIPTION:
 */
#define SOMLINK	_System
#define StarColorPalette_Class_Source
#include "clrstar.ih" 	  /* implementation header emitted from ClrStar.csc */

/********************* INSTANCE METHODS SECTION *****************************\
 *        Do not put any code in this section unless it is really an		*
 *      object INSTANCE method: otherwise parent method resolution will		*
 *			       not work correctly for that method						*
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
	ColorPalette	*somSelf,
	PPOINTL			pPoints,
	PULONG 			pcPoints)
{
/*	StarColorPaletteData *somThis = StarColorPaletteGetData(somSelf); */
	#define 		STAR_CPOINTS				11
	static POINTL	ptlStar[STAR_CPOINTS] = { {  20,   0 },
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
		memcpy(pPoints,ptlStar,STAR_CPOINTS * sizeof(POINTL));
	if (pcPoints)
		*pcPoints	= STAR_CPOINTS;
	return TRUE;
}
