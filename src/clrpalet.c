/*
 * MODULE NAME: ClrPalet.C
 *
 * DESCRIPTION:
 *   Source code for ColorPalette object class.
 *
 * OBJECT CLASS: ColorPalette
 *
 * CLASS HIERARCHY:
 *
 *     SOMObject
 *       --- WPObject
 *             --- WPAbstract
 *                   --- WPPalette
 *                         ---  WPColorPalette
 *                                  ---  ColorPalette
 *
 * CONTENTS:
 *   - instance method overrides (wpPaintCell/wpEditCell/wpRedrawCell/
 *     wpSetup/wpSelectCell) and new methods (Set/QueryEditDlgHandle,
 *     QuerySampleShape)
 *   - metaclass overrides (wpclsInitData registers COLOR_WHEEL_CLASS on
 *     the WPS process; wpclsQueryIconData supplies our class icon;
 *     wpclsQueryStyle clears CLSSTYLE_NEVERTEMPLATE)
 *   - ordinary code: the non-modal ColorPickingDialog procedure and the
 *     module-handle helper
 *
 * Original work:
 *   (C) IBM Corporation 1992/1993 - Named Color Palette WPS sample.
 *   Ported from the SOM 1.0 precompiler layout to modern SOM IDL bindings
 *   plus Open Watcom; every deviation is tagged PORTING NOTE below and
 *   summarized in README.md.
 */

/*
 * PM/DOS/WPS header prelude.  The SOM 1.0 precompiler emitted these into
 * the generated .IH (see orig\CLRPALET.CSC 'passthru: C.ih'); sc does not,
 * so they are carried here instead - BEFORE any class bindings are included.
 */
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define INCL_DOSMODULEMGR
#define INCL_WINWORKPLACE
#define INCL_WPCLASS
#define INCL_WPFOLDER

#include <os2.h>

/* Strip runtime overhead out of the generated bindings: SOM_NoTest removes
 * the somTestCls class-validity check from each method call, and _RETAIL
 * disables the xxxMethodDebug() trace hooks.  Both must be defined before
 * including the .ih.
 */
#define SOM_NoTest 1        /* Disables somTestCls on method resolution */
#define _RETAIL             /* Disables xxxMethodDebug() functions */

#define ColorPalette_Class_Source
#define M_ColorPalette_Class_Source

#include "clrpalet.ih"      /* implementation header emitted from clrpalet.idl */
#include "clrpids.h"        /* resource/message IDs (was the C.ph passthru)    */
#include "pdsctls.h"        /* color wheel control definitions                 */
#include "clrsampl.h"       /* color sample control message definitions        */

#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

/*
 * Forward references (was the C.ih passthru in the original)
 */
MRESULT EXPENTRY ClrWheelWndProc(
    HWND   hWnd,
    ULONG  msg,
    MPARAM mp1,
    MPARAM mp2);

MRESULT EXPENTRY ColorPickingDialog(
    HWND   hwnd,
    ULONG  msg,
    MPARAM mp1,
    MPARAM mp2);

/*
 * Data structure used by ColorPickingDialog
 */
typedef struct _COLORPICKDATA
{
    ColorPalette    *Palette;         /* Palette object                    */
    PCELL           pCell;            /* Cell being edited                 */
    ULONG           ulRGBOriginal;    /* Original color of the cell        */
    BOOL            fSpinSet;         /* Spinbutton is setting the color   */
} COLORPICKDATA;

/*
 * Global variables (was the C.ih passthru in the original)
 */
HMODULE vhmodClrPalet = NULLHANDLE;

/*
 * FUNCTION: GetClrPaletModHandle
 *
 * DESCRIPTION:
 *   Return our own DLL module handle, acquiring it on first use.
 *
 * PORTING NOTE: the original captured hmodule in an IBMC-style
 * _DLL_InitTerm that called _CRT_init().  Open Watcom's OS/2 runtime
 * provides no _CRT_init symbol (and supplies its own default DLL
 * initialization), so the custom init was dropped and the module handle
 * is now queried by our LIBRARY name (see clrpalet.def) on demand.
 */
static HMODULE GetClrPaletModHandle(void)
{
    if (!vhmodClrPalet)
        DosQueryModuleHandle("CLRPALET", &vhmodClrPalet);
    return vhmodClrPalet;
}

/********************* INSTANCE METHODS SECTION *****************************\
 *        Do not put any code in this section unless it is really an        *
 *      object INSTANCE method: otherwise parent method resolution will     *
 *                  not work correctly for that method                      *
\****************************************************************************/
#undef SOM_CurrentClass
#define SOM_CurrentClass SOMInstance

/*
 * NEW METHOD: SetEditDlgHandle
 *
 * DESCRIPTION:
 *   Store the handle of the color picking dialog window.
 */
SOM_Scope BOOL SOMLINK clrp_SetEditDlgHandle(
    ColorPalette    *somSelf,
    HWND            hwndEditDlg)
{
    ColorPaletteData *somThis = ColorPaletteGetData(somSelf);

    _hwndEditDlg = hwndEditDlg;
    return TRUE;
}

/*
 * NEW METHOD: QueryEditDlgHandle
 *
 * DESCRIPTION:
 *   Retrieve the handle of the color picking dialog window. Return NULL
 *   if the dialog has not yet been created.
 */
SOM_Scope HWND SOMLINK clrp_QueryEditDlgHandle(
    ColorPalette    *somSelf)
{
    ColorPaletteData *somThis = ColorPaletteGetData(somSelf);

    return _hwndEditDlg;
}

/*
 * NEW METHOD: QuerySampleShape
 *
 * DESCRIPTION:
 *   Describes the shape to be drawn for each color swatch. The pcPoints
 *   variable must always be filled out, to indicate how many points are
 *   contained in the pPoints array. pPoints either contains an array of
 *   POINTL structures that this method should fill out or it can be NULL
 *   to indicate that cPoints is being queried.
 *
 *   The point coordinates are specified in percentage coordinates.
 */
SOM_Scope BOOL SOMLINK clrp_QuerySampleShape(
    ColorPalette    *somSelf,
    PPOINTL         pPoints,
    PULONG          pcPoints)
{
    #define DEFAULT_CPOINTS 5
    static POINTL ptlShape[DEFAULT_CPOINTS] = { {   0,   0 },
                                                { 100,   0 },
                                                { 100, 100 },
                                                {   0, 100 },
                                                {   0,   0 }
                                              };

    /* Return the default color swatch shape, a rectangle.
     */
    if (pPoints)
        memcpy(pPoints, ptlShape, DEFAULT_CPOINTS * sizeof(POINTL));
    if (pcPoints)
        *pcPoints = DEFAULT_CPOINTS;
    return TRUE;
}

/*
 * METHOD OVERRIDE: wpPaintCell
 *
 * DESCRIPTION:
 *   Override the wpPaintCell method so that we can display different shape
 *   color swatches in the palette view. The shape of the color swatch is as
 *   defined by the QuerySampleShape method.
 */
SOM_Scope VOID SOMLINK clrp_wpPaintCell(
    ColorPalette    *somSelf,
    PCELL           pCell,
    HPS             hps,
    PRECTL          prcl,
    BOOL            fHilite)
{
    PPAINTPOT   pPaintPot = (PVOID)pCell;
    POINTL      *pPoints;
    ULONG       cPoints, i;
    LONG        xOrigin, yOrigin, xWidth, yHeight;

    /* Guarantee that the presentation space is currently in RGB mode
     */
    GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, 0);

    /* Query the number of points in the palette shape to be drawn
     */
    _QuerySampleShape(somSelf, NULL, &cPoints);
    if (cPoints)
    {
        /* Allocate an array of points to hold the shape
         */
        pPoints = malloc(cPoints * sizeof(POINTL));
        if (pPoints)
        {
            /* Scale the array of points so that they fit in the cell
             * rectangle for this item
             */
            _QuerySampleShape(somSelf, pPoints, &cPoints);
            xOrigin = prcl->xLeft;
            yOrigin = prcl->yBottom;
            /* A PRECTL excludes its right/top edges, so subtract one more
             * pixel to keep the scaled shape inside the cell (the original
             * author left a "why the -1 ???" here).
             */
            xWidth  = prcl->xRight - xOrigin - 1;
            yHeight = prcl->yTop   - yOrigin;
            for (i = 0; i < cPoints; i++)
            {
                pPoints[i].x = (pPoints[i].x * xWidth / 100) + xOrigin;
                pPoints[i].y = (pPoints[i].y * yHeight / 100) + yOrigin;
            }

            /* Create a path that contains the shape to be drawn
             */
            GpiBeginPath(hps, 1);
            GpiMove(hps, pPoints);
            GpiPolyLine(hps, cPoints - 1, pPoints + 1);
            GpiEndPath(hps);

            /* Fill the shape in the appropriate color for this cell
             */
            GpiSetColor(hps, pPaintPot->ulRGB);
            GpiFillPath(hps, 1, FPATH_ALTERNATE);

            /* Free the point array
             */
            free(pPoints);
        }
    }
}

/*
 * METHOD OVERRIDE: wpEditCell
 *
 * DESCRIPTION:
 *   Override the wpEditCell method in order to display our own dialog
 *   to perform color editing instead of the standard system provided color
 *   selection dialog.
 *
 * PORTING NOTE: WPPalette::wpEditCell returns BOOL in Toolkit 4.5
 * (wppalet.idl:38-39); the original body fell off the end without a
 * return value, which the old compiler tolerated. Returning TRUE here.
 */
SOM_Scope BOOL SOMLINK clrp_wpEditCell(
    ColorPalette    *somSelf,
    PCELL           pCell,
    HWND            hwndPal)
{
    HWND hwndDlg;

    /* If our dialog window isn't around at the moment we should load it.
     * Note that the editing dialog is NOT modal, so that the user can quickly
     * edit more than one color in the palette very quickly.
     */
    if (!_QueryEditDlgHandle(somSelf))
        WinLoadDlg(HWND_DESKTOP,
                   hwndPal,
                   ColorPickingDialog,
                   GetClrPaletModHandle(),
                   IDDLG_COLORPICKINGDIALOG,
                   somSelf);

    /* Tell the editing dialog which cell's value it should currently
     * be modifying, and then force it to the top of the Z-order and
     * show it.
     */
    hwndDlg = _QueryEditDlgHandle(somSelf);
    if (hwndDlg)
    {
        /* Pass the PAINTPOT cell that is to be edited on to the dialog...
         */
        WinSendMsg(hwndDlg,
                   MSG_EDITCELL,
                   MPFROMP(pCell),
                   0);
        WinSetWindowPos(hwndDlg,
                        HWND_TOP,
                        0, 0, 0, 0,
                        SWP_ACTIVATE | SWP_ZORDER | SWP_SHOW);
    }
    return TRUE;
}

/*
 * METHOD OVERRIDE: wpSelectCell
 *
 * DESCRIPTION:
 *   Override the wpSelectCell method in order that we can tell the
 *   color sample control that we are buddies with what its new color should
 *   be.
 *
 * PORTING NOTE (wpSelectCell): back in 1993 this was an undocumented
 * private method, so the sample installed its override at runtime with
 * _somOverrideSMethod() and called the parent through _somFindMethod().
 * Toolkit 4.5 declares wpSelectCell publicly in WPPalette's interface and
 * releaseorder (wppalet.idl:47-48), and WPColorPalette already overrides
 * it (wpclrpal.idl:59) - so this port declares a plain 'wpSelectCell:
 * override;' in clrpalet.idl and uses the normal parent_* resolution.
 * The method returns void in the modern bindings (the old code cast the
 * parent pointer to a BOOL-returning function).
 */
SOM_Scope VOID SOMLINK clrp_wpSelectCell(
    ColorPalette    *somSelf,
    HWND            hwndPalette,
    PCELL           pCell)
{
    ColorPaletteData *somThis = ColorPaletteGetData(somSelf);

    /* If we are currently storing the handle of a color sample control,
     * tell it about the newly selected color ...
     */
    if (_hwndColorSample)
        WinSendMsg(_hwndColorSample,
                   CSM_SETRGBCOLOR,
                   MPFROMLONG(((PPAINTPOT)pCell)->ulRGB),
                   0);

    /* Remember who the selected cell currently is
     */
    _pSelectedCell = pCell;

    /* Parent processing (normal resolution - see note above)
     */
    parent_wpSelectCell(somSelf, hwndPalette, pCell);
}

/*
 * METHOD OVERRIDE: wpRedrawCell
 *
 * DESCRIPTION:
 *   Override the wpRedrawCell method in order that we can tell the
 *   color sample control that opened us what its new color should be.
 */
SOM_Scope BOOL SOMLINK clrp_wpRedrawCell(
    ColorPalette    *somSelf,
    PCELL           pCell)
{
    ColorPaletteData *somThis = ColorPaletteGetData(somSelf);

    /* If we are currently storing the handle of a color sample control,
     * tell it about the newly selected color ...
     */
    if (_hwndColorSample && (pCell == _pSelectedCell))
        WinSendMsg(_hwndColorSample,
                   CSM_SETRGBCOLOR,
                   MPFROMLONG(((PPAINTPOT)pCell)->ulRGB),
                   0);
    return parent_wpRedrawCell(somSelf, pCell);
}

/*
 * METHOD OVERRIDE: wpSetup
 *
 * DESCRIPTION:
 *   Look for our special setup string parameter "CTRLHDL=xxxxxxxx" coming
 *   through to us from a color sample control window.
 */
SOM_Scope BOOL SOMLINK clrp_wpSetup(
    ColorPalette    *somSelf,
    PSZ             pszSetupString)
{
    ColorPaletteData *somThis = ColorPaletteGetData(somSelf);
    CHAR    szHandle[20];
    ULONG   cbszHandle;
    HAB     hab;
    HWND    hwndColorSample;
    HWND    hwndDlg;

    /* Scan the setup string for the "CTRLHDL=xxxxxxxx" parameter so that
     * we can extract the window handle of the color sample control that
     * wishes to communicate with us.
     */
    cbszHandle = sizeof(szHandle);
    hab        = WinQueryAnchorBlock(HWND_DESKTOP);
    if (_wpScanSetupString(somSelf,
                           pszSetupString,
                           "CTRLHDL",
                           szHandle,
                           &cbszHandle)
        && cbszHandle)
    {
        /* Only accept the specified handle value for us to communicate
         * with if it is a valid window handle.
         */
        sscanf(szHandle, "%lx", &hwndColorSample);
        if (hwndColorSample && WinIsWindow(hab, hwndColorSample))
        {
            _hwndColorSample = hwndColorSample;

            /* Destroy the color picking dialog if it is visible
             */
            hwndDlg = _QueryEditDlgHandle(somSelf);
            if (hwndDlg)
                WinDestroyWindow(hwndDlg);
        }
    }

    /* Call our parent method so that all other setup parameters are
     * processed correctly
     */
    return parent_wpSetup(somSelf, pszSetupString);
}

/*********************** CLASS METHODS SECTION ******************************\
 *        Do not put any code in this section unless it is really an        *
 *       object CLASS method: otherwise parent method resolution will       *
 *                       not work correctly for that method                 *
\****************************************************************************/
#undef SOM_CurrentClass
#define SOM_CurrentClass SOMMeta

/*
 * METACLASS METHOD OVERRIDE: wpclsInitData
 *
 * DESCRIPTION:
 *   Override this method in order to perform one-time initialization when
 *   an object class is instantiated.  The WPS calls it exactly once per
 *   class per process, which makes it the ideal place to register the
 *   custom control our edit dialog depends on.
 */
SOM_Scope void SOMLINK clrpM_wpclsInitData(
    M_ColorPalette  *somSelf)
{
    /* Register the color wheel control on the Workplace Shell process.
     * CS_SYNCPAINT | CS_SIZEREDRAW give it standard static-control paint
     * behaviour, and USER_RESERVED (see pdsctls.h) reserves the window
     * words the control keeps its CLRWHEEL data behind.
     */
    WinRegisterClass(NULLHANDLE,
                     COLOR_WHEEL_CLASS,
                     ClrWheelWndProc,
                     CS_SYNCPAINT | CS_SIZEREDRAW,
                     USER_RESERVED);

    /* PORTING NOTE: the original also installed the wpSelectCell override
     * here via _somOverrideSMethod(); that is now declared in the IDL -
     * see the wpSelectCell override above.
     */
    parent_wpclsInitData(somSelf);
}

/*
 * METACLASS METHOD OVERRIDE: wpclsQueryIconData
 *
 * DESCRIPTION:
 *   Override this method in order to provide a unique icon for the
 *   ColorPalette object class.
 */
SOM_Scope ULONG SOMLINK clrpM_wpclsQueryIconData(
    M_ColorPalette  *somSelf,
    PICONINFO       pIconInfo)
{
    /* Tell the system that we have our own special default class icon
     * stored as a resource within our dll module.
     */
    if (pIconInfo)
    {
        pIconInfo->cb     = sizeof(ICONINFO);
        pIconInfo->fFormat = ICON_RESOURCE;
        pIconInfo->hmod   = GetClrPaletModHandle();  /* our dll module     */
        pIconInfo->resid  = ID_CLRPALET;             /* icon resource id   */
    }
    return sizeof(ICONINFO);
}

/*
 * METACLASS METHOD OVERRIDE: wpclsQueryStyle
 *
 * DESCRIPTION:
 *   Override this method to modify class styles for the ColorPalette object
 *   class.
 */
SOM_Scope ULONG SOMLINK clrpM_wpclsQueryStyle(
    M_ColorPalette  *somSelf)
{
    ULONG ulStyle;

    /* Tell the system that we don't want to have a template created
     * in the templates folder automatically. We don't really need one
     * because we are a replacement for the existing color palette class.
     */
    ulStyle = parent_wpclsQueryStyle(somSelf);

    return ulStyle & ~CLSSTYLE_NEVERTEMPLATE;
}

/**************************** ORDINARY CODE *********************************\
 *           Put any code here that isn't a method of the object class      *
\****************************************************************************/
#undef SOM_CurrentClass

/*
 * DIALOG PROCEDURE: ColorPickingDialog
 *
 * DESCRIPTION:
 *   This code manages the color picking dialog, allowing the user to modify
 *   the color value of a given cell within the palette.  The dialog is
 *   modeless and one instance per palette object at a time: the palette
 *   object keeps the dialog handle (Set/QueryEditDlgHandle) and the dialog
 *   keeps its COLORPICKDATA pointer in QWL_USER.
 *
 *   MSG_EDITCELL and MSG_SETRGBSPINBTNS are this module's private messages
 *   (WM_USER-based, defined in clrpids.h).
 */
MRESULT EXPENTRY ColorPickingDialog(
    HWND   hwnd,
    ULONG  msg,
    MPARAM mp1,
    MPARAM mp2)
{
    COLORPICKDATA *pPickData = WinQueryWindowPtr(hwnd, QWL_USER);
    MRESULT       mr         = 0;
    RGB2          *pRGB, rgb;
    ULONG         Value;

    switch (msg)
    {
        /* Initialize our dialog
         */
        case WM_INITDLG:
            /* Allocate a structure to store our palette object pointer
             * and the cell that is currently being edited.  mp2 carries
             * the ColorPalette somSelf handed to WinLoadDlg().
             */
            pPickData = malloc(sizeof(COLORPICKDATA));
            if (pPickData)
            {
                /* Store away the object pointer to the palette that we
                 * are operating on.
                 */
                WinSetWindowPtr(hwnd, QWL_USER, pPickData);
                pPickData->Palette   = PVOIDFROMMP(mp2);
                pPickData->pCell     = NULL;
                /* fSpinSet suppresses SPBN_CHANGE handling while WE are
                 * pushing values into the spinbuttons, otherwise the
                 * programmatic updates would feed back through WM_CONTROL
                 * as if the user had spun them.
                 */
                pPickData->fSpinSet  = TRUE;

                /* Set the ranges on each of our spinbuttons
                 */
                WinSendDlgItemMsg(hwnd, DID_SPINR,
                                  SPBM_SETLIMITS,
                                  MPFROMLONG(255),
                                  MPFROMLONG(0));
                WinSendDlgItemMsg(hwnd, DID_SPINB,
                                  SPBM_SETLIMITS,
                                  MPFROMLONG(255),
                                  MPFROMLONG(0));
                WinSendDlgItemMsg(hwnd, DID_SPING,
                                  SPBM_SETLIMITS,
                                  MPFROMLONG(255),
                                  MPFROMLONG(0));
                pPickData->fSpinSet = FALSE;
            }

            /* Store our handle away in the palette object so that it
             * knows that we are loaded.  (Assumes the malloc above
             * succeeded - same as the original.)
             */
            _SetEditDlgHandle(pPickData->Palette, hwnd);
            break;

        /* Cleanup when our dialog is destroyed
         */
        case WM_DESTROY:
            mr = WinDefDlgProc(hwnd, msg, mp1, mp2);

            /* Let the palette object know that this dialog is no longer
             * loaded and then free up our data structure
             */
            _SetEditDlgHandle(pPickData->Palette, NULLHANDLE);
            if (pPickData)
                free(pPickData);
            break;

        /* Change the color of the color swatch in the palette view when
         * the user picks a new color from either the spin buttons or
         * the color wheel control.
         */
        case WM_CONTROL:
            switch (SHORT2FROMMP(mp1))
            {
                case CWN_RGBCLRSELECTED:
                    if (pPickData && pPickData->pCell)
                    {
                        /* The new color to be used is passed to us in MP2
                         */
                        ((PPAINTPOT)pPickData->pCell)->ulRGB
                            = LONGFROMMP(mp2);

                        /* Redraw the cell and reset the spin buttons
                         */
                        _wpRedrawCell(pPickData->Palette,
                                      pPickData->pCell);
                        WinSendMsg(hwnd,
                                   MSG_SETRGBSPINBTNS,
                                   MPFROMP(&((PPAINTPOT)pPickData->pCell)->ulRGB),
                                   0);
                    }
                    break;

                case SPBN_CHANGE:
                    if (pPickData && !pPickData->fSpinSet && pPickData->pCell)
                    {
                        /* Figure out the new color to be used by reading
                         * the values of each spin button in turn...
                         */
                        WinSendDlgItemMsg(hwnd, DID_SPINR,
                                          SPBM_QUERYVALUE,
                                          MPFROMP(&Value),
                                          0);
                        rgb.bRed  = Value;
                        WinSendDlgItemMsg(hwnd, DID_SPING,
                                          SPBM_QUERYVALUE,
                                          MPFROMP(&Value),
                                          0);
                        rgb.bGreen = Value;
                        WinSendDlgItemMsg(hwnd, DID_SPINB,
                                          SPBM_QUERYVALUE,
                                          MPFROMP(&Value),
                                          0);
                        rgb.bBlue    = Value;
                        rgb.fcOptions = 0;
                        /* Overlay trick: on little-endian, an RGB2's first
                         * three bytes (bRed/bGreen/bBlue) are exactly the
                         * low three bytes of a ULONG RGB value.
                         */
                        ((PPAINTPOT)pPickData->pCell)->ulRGB
                            = *((PULONG)&rgb);

                        /* Redraw the cell and reset the color wheel
                         */
                        _wpRedrawCell(pPickData->Palette,
                                      pPickData->pCell);
                        WinSendDlgItemMsg(hwnd, DID_COLORWHEEL,
                                          CWM_SETRGBCLR,
                                          MPFROMP(&((PPAINTPOT)pPickData->pCell)->ulRGB),
                                          0);
                    }
                    break;
            }
            break;

        /* Handle the UNDO and HELP pushbuttons
         */
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                case DID_UNDO:
                    /* Set the color of the color swatch being edited back
                     * to its original value, and reset the position of
                     * the crosshairs.
                     */
                    if (pPickData && pPickData->pCell)
                    {
                        ((PPAINTPOT)pPickData->pCell)->ulRGB
                            = pPickData->ulRGBOriginal;
                        _wpRedrawCell(pPickData->Palette,
                                      pPickData->pCell);
                        WinSendDlgItemMsg(hwnd, DID_COLORWHEEL,
                                          CWM_SETRGBCLR,
                                          MPFROMP(&pPickData->ulRGBOriginal),
                                          0);
                        WinSendMsg(hwnd,
                                   MSG_SETRGBSPINBTNS,
                                   MPFROMP(&pPickData->ulRGBOriginal),
                                   0);
                    }
                    break;
            }
            break;

        /* Adjust the color crosshairs when the user asks to edit a
         * different cell.
         */
        case MSG_EDITCELL:
            if (pPickData)
            {
                /* Store away the new cell that is being edited and the
                 * original color that was contained in this paintpot.
                 */
                pPickData->pCell = PVOIDFROMMP(mp1);
                if (pPickData->pCell)
                {
                    pPickData->ulRGBOriginal = ((PPAINTPOT)pPickData->pCell)->ulRGB;
                    WinSendDlgItemMsg(hwnd, DID_COLORWHEEL,
                                      CWM_SETRGBCLR,
                                      MPFROMP(&((PPAINTPOT)pPickData->pCell)->ulRGB),
                                      0);
                    WinSendMsg(hwnd,
                               MSG_SETRGBSPINBTNS,
                               MPFROMP(&((PPAINTPOT)pPickData->pCell)->ulRGB),
                               0);
                }
            }
            break;

        /* Set the current values in the R,G and B spinbuttons.  mp1
         * points at a ULONG RGB value; reading it as RGB2* relies on the
         * same little-endian byte overlay as the SPBN_CHANGE case above.
         */
        case MSG_SETRGBSPINBTNS:
            if (pPickData)
            {
                pPickData->fSpinSet = TRUE;
                pRGB                = PVOIDFROMMP(mp1);
                WinSendDlgItemMsg(hwnd, DID_SPINR,
                                  SPBM_SETCURRENTVALUE,
                                  MPFROMSHORT(pRGB->bRed),
                                  0);
                WinSendDlgItemMsg(hwnd, DID_SPINB,
                                  SPBM_SETCURRENTVALUE,
                                  MPFROMSHORT(pRGB->bBlue),
                                  0);
                WinSendDlgItemMsg(hwnd, DID_SPING,
                                  SPBM_SETCURRENTVALUE,
                                  MPFROMSHORT(pRGB->bGreen),
                                  0);
                pPickData->fSpinSet = FALSE;
            }
            break;

        default:
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }
    return mr;
}

/*
 * FUNCTION: SOMInitModule
 *
 * DESCRIPTION:
 *   Class-library initialization entry point; the SOM kernel runs this
 *   export when the class DLL is loaded (prototype per IBM's animal
 *   sample).  It creates the class objects this DLL provides: the
 *   metaclasses first, then the instance classes (each NewClass body
 *   statically references its parents).  StarColorPalette has no explicit
 *   metaclass methods, but modern sc still emits its implicit M_ bindings.
 */
#pragma linkage(SOMInitModule, system)
void SOMLINK SOMInitModule(integer4 majorVersion, integer4 minorVersion);

/* StarColorPaletteNewClass is prototyped in h\clrstar.ih, which clrpalet.c
 * does not include (it belongs to the sibling class); declare it here so
 * the linker-visible call below is checked. */
#pragma linkage(StarColorPaletteNewClass, system)
SOMClass * SOMLINK StarColorPaletteNewClass(integer4 somtmajorVersion,
                                            integer4 somtminorVersion);

void SOMLINK SOMInitModule(integer4 majorVersion, integer4 minorVersion)
{
    M_ColorPaletteNewClass(majorVersion, minorVersion);
    ColorPaletteNewClass(majorVersion, minorVersion);
    StarColorPaletteNewClass(majorVersion, minorVersion);
}
