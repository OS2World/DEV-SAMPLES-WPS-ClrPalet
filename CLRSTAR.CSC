/****************************************************************************\
 * MODULE NAME: ClrPalet.CSC
 *
 * DESCRIPTION:
 *   Object class definition file for StarColorPalette object class.
\****************************************************************************/
## Include the class definition for the parent class
##
include <clrpalet.sc>

## Define the object class name and various attributes used by the SOM
## compiler in generating skeletons for the new methods and overrides that
## will be needed to implement this object class.
##
class: StarColorPalette,
       external stem   = clrs,
       local,
       external prefix = clrs_,
       classprefix     = clrsM_,
       major version   = 1,
       minor version   = 2;
--
-- OBJECT CLASS: StarColorPalette
--
-- CLASS HIERARCHY:
--
--     SOMObject
--       юдд WPObject
--             юдд WPAbstract
--                   юдд WPPalette
--                         юдд  WPColorPalette
--                    		      юдд  ColorPalette
--                    				      юдд  StarColorPalette
--
-- DESCRIPTION:
--   Illustrate a Workplace Shell palette subclass that only overrides
--   one method
--

## Specify the parent class name
##
parent: ColorPalette;

passthru: C.ih;

#define SOM_NoTest 1        /* Disables somTestCls on method resolution */
#define _RETAIL             /* Disables xxxMethodDebug() functions */

/* Include the standard PM header files
 */
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_WINWORKPLACE
#define INCL_WPCLASS
#define INCL_WPFOLDER
#include <os2.h>
#include <string.h>

endpassthru;   /* .ih */

methods:

## Specify methods being overridden
override QuerySampleShape;

