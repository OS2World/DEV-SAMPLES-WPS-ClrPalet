/****************************************************************************\
 * MODULE NAME: ClrPalet.CSC
 *
 * DESCRIPTION:
 *   Object class definition file for ColorPalette object class.
\****************************************************************************/
### Note: The header preceding this comment will be emitted in all files.
###    Any comments to go public should be preceeded with '--'
###    Any comments to remain private should be preceeded with '#'

## Include the class definition for the parent class
##
include <wpclrpal.sc>

## Define the object class name and various attributes used by the SOM
## compiler in generating skeletons for the new methods and overrides that
## will be needed to implement this object class.
##
class: ColorPalette,
       external stem   = clrp,
       local,
       external prefix = clrp_,
       classprefix     = clrpM_,
       major version   = 1,
       minor version   = 2;
--
-- OBJECT CLASS: ColorPalette
--
-- CLASS HIERARCHY:
--
--     SOMObject
--       ÀÄÄ WPObject
--             ÀÄÄ WPAbstract
--                   ÀÄÄ WPPalette
--                         ÀÄÄ  WPColorPalette
--                    		      ÀÄÄ  ColorPalette
--
-- DESCRIPTION:
--	 This object class can act as a stand-alone object class that creates
--	 a palette of named colors that can be dragged and dropped much like the
--	 system color palette. It can also be used as a replacement object class
--	 for the WPColorPalette class - thus replacing the color palette object
--	 in the System Setup folder and any other color palettes that the user
--   may have already created.
--
--	 Each element of the named color palette contains a sample swatch of the
--	 color and the name of that color. Editing a color in the named color
--	 palette invokes the Color Wheel control.
--
--	 The purpose of this class is to illustrate how the system provided
--	 WPPalette object class works, and how the programmer can create his own
--   customized palette of icons, menus, or any other sort of attribute that
--   can be applied using drag and drop.
--

## Specify the parent class name
##
parent: WPColorPalette;

## Specify the release order of methods. To ensure binary compatability,
## always add new methods at the END of the release order and never remove
## any entries from the release order of a ship-level object class.
##
release order:
		SetEditDlgHandle,
		QueryEditDlgHandle,
		QuerySampleShape;

## Passthru PUBLIC definitions to the .h file
##
passthru: C.h, after;

endpassthru;   /* C.h */

## Passthru PRIVATE definitions to the .ph file: definitions that class
## client code will not need, but may be required in another internal C
## file or the resource file.
##
passthru: C.ph;

#define ID_CLRPALET					1
#define IDDLG_COLORPICKINGDIALOG	10
#define DID_COLORWHEEL				801
#define DID_UNDO					802
#define DID_HELP					803
#define DID_BOGUS					804
#define DID_SPINR					805
#define DID_SPING					806
#define DID_SPINB					807
#define COLOR_WHEEL_CLASS			"ColorWheelControlClass"

#define MSG_EDITCELL		WM_USER
#define MSG_SETRGBSPINBTNS	WM_USER	+ 1

endpassthru;   /* C.ph */

## Passthru IMPLEMENTATION definitions to the .ih file: definitions and
## includes that are only required in the C source code.
##
passthru: C.ih;

/* These two definitions make our compiled size smaller: otherwise SOM will
 * add a lot of debug code in on our behalf whenever we call a method
 * and around each method call itself.
 */
#define SOM_NoTest 1        /* Disables somTestCls on method resolution */
#define _RETAIL             /* Disables xxxMethodDebug() functions */

/* Include standard PM header files
 */
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_WINWORKPLACE
#define INCL_WPCLASS
#define INCL_WPFOLDER
#include <os2.h>
#include "clrpalet.ph"

/* Include C runtime library functions that we might need
 */
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

/* Include the colour wheel control header file and define its window
 * procedure and class name
 */
#include "pdsctls.h"

MRESULT EXPENTRY ClrWheelWndProc(
	HWND   hWnd,
	ULONG  msg,
	MPARAM mp1,
	MPARAM mp2);

/* Dialog procedure that allows the user to change the color of an
 * element within the palette
 */
MRESULT EXPENTRY ColorPickingDialog(
	HWND	hwnd,
	ULONG	msg,
	MPARAM	mp1,
	MPARAM	mp2);

/* Data structure used by ColorPickingDialog
 */
typedef struct _COLORPICKDATA
{
	ColorPalette	*Palette;		/* Palette object */
	PCELL			pCell;			/* Cell being edited */
	ULONG			ulRGBOriginal;	/* Original color of the cell */
	BOOL			fSpinSet;		/* Spinbutton is setting the color */
} COLORPICKDATA;

/* Global variables
 */
HMODULE vhmodClrPalet		= NULLHANDLE;

endpassthru;   /* .ih */

## Define instance data
##
data:
   HWND		hwndEditDlg;		/* Editing dialog handle */
   HWND		hwndColorSample;	/* Sample control that we are talking with */
   PCELL	pSelectedCell;		/* Currently selected cell */

## Specify the methods and method overrides needed to implement this
## object class. The SOM compiler will generate skeletons for each of
## these methods in our source code.
##
methods:

## Define instance methods

BOOL	SetEditDlgHandle( HWND	hwndEditDlg );
--
-- NEW METHOD: SetEditDlgHandle
--
-- DESCRIPTION:
--   Store the handle of the color picking dialog window.
--

HWND	QueryEditDlgHandle( );
--
-- NEW METHOD: QueryEditDlgHandle
--
-- DESCRIPTION:
--   Retrieve the handle of the color picking dialog window. Return NULL
--   if the dialog has not yet been created.
--

BOOL	QuerySampleShape( PPOINTL	pPoints, PULONG pcPoints );
--
-- NEW METHOD: QuerySampleShape
--
-- DESCRIPTION:
--   Describes the shape to be drawn for each color swatch. The pcPoints
--   variable must always be filled out, to indicate how many points are
--   contained in the pPoints array. pPoints either contains an array of
--   POINTL structures that this method should fill out or it can be NULL
--   to indicate that cPoints is being queried.
--
--   The point coordinates are specified in percentage coordinates.
--

## Specify instance methods being overridden

## Define class methods

## Specify class methods being overridden

override wpPaintCell;

override wpEditCell;

override wpRedrawCell;

override wpSetup;

override wpclsInitData, class;

override wpclsQueryIconData, class;

override wpclsQueryStyle, class;

