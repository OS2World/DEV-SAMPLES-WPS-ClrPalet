/*****************************************************************************
 * MODULE NAME: ClrPalet.C
 *
 * DESCRIPTION:
 *	 Source code for ColorPalette object class.
\****************************************************************************/
/* OBJECT CLASS: ColorPalette
 *
 * CLASS HIERARCHY:
 *
 *     SOMObject
 *       ÀÄÄ WPObject
 *             ÀÄÄ WPAbstract
 *                   ÀÄÄ WPPalette
 *                         ÀÄÄ  WPColorPalette
 *                    		      ÀÄÄ  ColorPalette
 *
 * DESCRIPTION:
 *	 This object class can act as a stand-alone object class that creates
 *	 a palette of named colors that can be dragged and dropped much like the
 *	 system color palette. It can also be used as a replacement object class
 *	 for the WPColorPalette class - thus replacing the color palette object
 *	 in the System Setup folder and any other color palettes that the user
 *   may have already created.
 *
 *	 Each element of the named color palette contains a sample swatch of the
 *	 color and the name of that color. Editing a color in the named color
 *	 palette invokes the Color Wheel control.
 *
 *	 The purpose of this class is to illustrate how the system provided
 *	 WPPalette object class works, and how the programmer can create his own
 *   customized palette of icons, menus, or any other sort of attribute that
 *   can be applied using drag and drop.
 */
#define SOMLINK	_System
#define ColorPalette_Class_Source
#define M_ColorPalette_Class_Source
#include "clrpalet.ih"              /* implementation header emitted from ColorPalette.csc */
#include "clrsampl.h"

/* Er, hum. This is a non-documented method that we had to override to
 * allow the color sample control to work nicely...
 */
SOM_Scope BOOL SOMLINK clrp_wpSelectCell(
	ColorPalette 	*somSelf,
	HWND			hwndPalette,
	PCELL 			pCell);

typedef BOOL (SOMLINK FNSELECTCELL)(ColorPalette *,HWND,PCELL);

typedef FNSELECTCELL *PFNSELECTCELL;

/********************* INSTANCE METHODS SECTION *****************************\
 *        Do not put any code in this section unless it is really an		*
 *      object INSTANCE method: otherwise parent method resolution will		*
 *			       not work correctly for that method						*
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
	ColorPalette 	*somSelf,
	HWND			hwndEditDlg)
{
	ColorPaletteData *somThis	= ColorPaletteGetData(somSelf);

	_hwndEditDlg	= hwndEditDlg;
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
	ColorPalette	*somSelf)
{
	ColorPaletteData *somThis	= ColorPaletteGetData(somSelf);

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
SOM_Scope BOOL   SOMLINK clrp_QuerySampleShape(
	ColorPalette *somSelf,
	PPOINTL 	 pPoints,
	PULONG 		 pcPoints)
{
	#define 		DEFAULT_CPOINTS				5
	ColorPaletteData *somThis				  = ColorPaletteGetData(somSelf);
	static POINTL	ptlShape[DEFAULT_CPOINTS] = { {   0,   0 },
												  { 100,   0 },
												  { 100, 100 },
												  {   0, 100 },
												  {   0,   0 }
												};

	/* Return the default color swatch shape, a rectangle.
	 */
	if (pPoints)
		memcpy(pPoints,ptlShape,DEFAULT_CPOINTS*sizeof(POINTL));
	if (pcPoints)
		*pcPoints	= DEFAULT_CPOINTS;
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
	ColorPalette 	*somSelf,
	PCELL			pCell,
	HPS 			hps,
	PRECTL 			prcl,
	BOOL			fHilite)
{	
	PPAINTPOT	pPaintPot	= (PVOID)pCell;
	POINTL		*pPoints;
	ULONG		cPoints, i;
	LONG		xOrigin, yOrigin, xWidth, yHeight;

	/* Guarantee that the presentation space is currently in RGB mode
	 */
    GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,0 );

	/* Query the number of points in the palette shape to be drawn
	 */
	_QuerySampleShape(somSelf,NULL,&cPoints);
	if (cPoints)
	{
		/* Allocate an array of points to hold the shape
		 */
		pPoints	= malloc(cPoints * sizeof(POINTL));
		if (pPoints)
		{
			/* Scale the array of points so that they fit in the cell
			 * rectangle for this item
			 */
			_QuerySampleShape(somSelf,pPoints,&cPoints);
			xOrigin		= prcl->xLeft;
			yOrigin		= prcl->yBottom;
			xWidth		= prcl->xRight - xOrigin - 1; /* why the -1 ??? */
			yHeight		= prcl->yTop   - yOrigin;
			for (i	= 0; i < cPoints; i++)
			{
				pPoints[i].x	= (pPoints[i].x * xWidth / 100) + xOrigin;
				pPoints[i].y	= (pPoints[i].y * yHeight / 100) + yOrigin;
			}

			/* Create a path that contains the shape to be drawn
			 */
			GpiBeginPath(hps,1);
			GpiMove(hps,pPoints);
			GpiPolyLine(hps,cPoints - 1,pPoints + 1);
			GpiEndPath(hps);

			/* Fill the shape in the appropriate color for this cell
			 */
			GpiSetColor(hps,pPaintPot->ulRGB);
			GpiFillPath(hps,1,FPATH_ALTERNATE);

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
 */
SOM_Scope BOOL SOMLINK clrp_wpEditCell(
	ColorPalette 	*somSelf,
	PCELL 			pCell,
	HWND 			hwndPal)
{
	HWND		hwndDlg;

	/* If our dialog window isn't around at the moment we should load it.
	 * Note that the editing dialog is NOT modal, so that the user can quickly
	 * edit more than one color in the palette very quickly.
	 */
	if (!_QueryEditDlgHandle(somSelf))
		WinLoadDlg(HWND_DESKTOP,
				   hwndPal,
				   ColorPickingDialog,
				   vhmodClrPalet,
				   IDDLG_COLORPICKINGDIALOG,
				   somSelf);

	/* Tell the editing dialog which cell's value it should currently
	 * be modifying, and then force it to the top of the Z-order and
	 * show it.
	 */
	hwndDlg		=	_QueryEditDlgHandle(somSelf);
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
}

/*
 * (NON-DOCUMENTED) METHOD OVERRIDE: wpSelectCell
 *
 * DESCRIPTION:
 *   Override the wpSelectCell method in order that we can tell the
 *   color sample control that we are buddies with what its new color should
 *   be. Alright, so it is naughty to override a private method - but this one
 *   should have been public anyway, it is perfectly harmless !
 */
SOM_Scope BOOL SOMLINK clrp_wpSelectCell(
	ColorPalette 	*somSelf,
	HWND			hwndPalette,
	PCELL 			pCell)
{
	ColorPaletteData *somThis  				= ColorPaletteGetData(somSelf);
	PFNSELECTCELL	 parent_wpSelectCell	= NULL;
	BOOL			 rc						= FALSE;
	SOMClass	  	 *Class, *ParentClass;

	/* If we are currently storing the handle of a color sample control,
	 * tell it about the newly selected color ...
	 */
	if (_hwndColorSample)
		WinSendMsg(_hwndColorSample,
				   CSM_SETRGBCOLOR,
				   MPFROMLONG(((PPAINTPOT)pCell)->ulRGB),
				   0);

	/* Remember who the seleceted cell currently is
	 */
	_pSelectedCell	= pCell;

	/* Call our parent method indirectly, since this isn't a public method!
	 */
	Class	= _somGetClass(somSelf);
	if (Class)
	{
		ParentClass	= _somGetParent(_ColorPalette);
		if (ParentClass)
		{
			_somFindMethod(ParentClass,
						   SOM_IdFromString("wpSelectCell"),
						   (somMethodProc **)&parent_wpSelectCell);
			if (parent_wpSelectCell)				
				rc	= (*parent_wpSelectCell)(somSelf,hwndPalette,pCell);
		}
	}
	return	rc;
}

/*
 * METHOD OVERRIDE: wpRedrawCell
 *
 * DESCRIPTION:
 *   Override the wpRedrawCell method in order that we can tell the
 *   color sample control that opened us what its new color should be.
 */
SOM_Scope BOOL SOMLINK clrp_wpRedrawCell(
	ColorPalette 	*somSelf,
	PCELL 			pCell)
{
	ColorPaletteData *somThis  				= ColorPaletteGetData(somSelf);

	/* If we are currently storing the handle of a color sample control,
	 * tell it about the newly selected color ...
	 */
	if (_hwndColorSample && (pCell == _pSelectedCell))
		WinSendMsg(_hwndColorSample,
				   CSM_SETRGBCOLOR,
				   MPFROMLONG( ((PPAINTPOT)pCell)->ulRGB ),
				   0);
	return parent_wpRedrawCell(somSelf,pCell);
}

/*
 * METHOD OVERRIDE: wpSetup
 *
 * DESCRIPTION:
 *   Look for our special setup string parameter "CTRLHDL=xxxxxxxx" coming
 *   through to us from a color sample control window.
 */
SOM_Scope BOOL SOMLINK clrp_wpSetup(
	ColorPalette	*somSelf,
	PSZ 			pszSetupString)
{
	ColorPaletteData *somThis  				= ColorPaletteGetData(somSelf);
	CHAR			 szHandle[20];
	ULONG			 cbszHandle;
	HAB				 hab;
	HWND			 hwndColorSample;
	HWND			 hwndDlg;

	/* Scan the setup string for the "CTRLHDL=xxxxxxxx" parameter so that
	 * we can extract the window handle of the color sample control that
	 * wishes to communicate with us.
	 */
	cbszHandle	= sizeof(szHandle);
	hab			= WinQueryAnchorBlock(HWND_DESKTOP);
	if (  _wpScanSetupString(somSelf,
		                     pszSetupString,
		                     "CTRLHDL",
			                 szHandle,
			                 &cbszHandle)
	   && cbszHandle
	   )
	{
		/* Only accept the specified handle value for us to communicate
		 * with if it is a valid window handle.
		 */
		sscanf(szHandle,"%lx",&hwndColorSample);
		if (hwndColorSample && WinIsWindow(hab,hwndColorSample))
		{
			_hwndColorSample	= hwndColorSample;

			/* Destroy the color picking dialog if it is visible
			 */
			hwndDlg	= _QueryEditDlgHandle(somSelf);
			if (hwndDlg)
				WinDestroyWindow(hwndDlg);
		}
	}

	/* Call our parent method so that all other setup parameters are
	 * processed correctly
	 */
	return parent_wpSetup(somSelf,pszSetupString);
}

/*********************** CLASS METHODS SECTION ******************************\
 *        Do not put any code in this section unless it is really an		*
 *       object CLASS method: otherwise parent method resolution will		*
 *		              not work correctly for that method                     *
\****************************************************************************/
#undef SOM_CurrentClass
#define SOM_CurrentClass SOMMeta

/*
 * METACLASS METHOD OVERRIDE: wpclsInitData
 *
 * DESCRIPTION:
 *   Override this method in order to perform one-time initialization when
 *   an object class is instantiated.
 */
SOM_Scope void SOMLINK clrpM_wpclsInitData(
	M_ColorPalette *somSelf)
{
	/* Register the color wheel control on the Workplace Shell process. We
	 * only have to do this once, so using the metaclass initialization
	 * method is an ideal place to do it ....
	 */
	WinRegisterClass(NULLHANDLE,
					 COLOR_WHEEL_CLASS,
				     ClrWheelWndProc,
					 CS_SYNCPAINT | CS_SIZEREDRAW,
					 USER_RESERVED
					);

	/* Override the wpSelectCell private method
	 */
	_somOverrideSMethod(somSelf,
						SOM_IdFromString("wpSelectCell"),
						(somMethodProc *)clrp_wpSelectCell);

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
	M_ColorPalette *somSelf,
	PICONINFO		pIconInfo)
{
	/* Tell the system that we have our own special default class icon
	 * stored as a resource within our dll module.
	 */
	if (pIconInfo)
	{
		pIconInfo->cb		= sizeof(ICONINFO);
		pIconInfo->fFormat	= ICON_RESOURCE;
		pIconInfo->hmod		= vhmodClrPalet; /* Module handle of our dll */
		pIconInfo->resid	= ID_CLRPALET;	 /* Icon resource id */
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
	M_ColorPalette *somSelf)
{
	ULONG	ulStyle;

	/* Tell the system that we don't want to have a template created
	 * in the templates folder automatically. We don't really need one
	 * because we are a replacement for the existing color palette class.
	 */
	ulStyle	= parent_wpclsQueryStyle(somSelf);

	return ulStyle & ~CLSSTYLE_NEVERTEMPLATE;
}

/**************************** ORDINARY CODE *********************************\
 *           Put any code here that isn't a method of the object class		*
\****************************************************************************/
#undef SOM_CurrentClass

/*
 * DIALOG PROCEDURE: ColorPickingDialog
 *
 * DESCRIPTION:
 *   This code manages the color picking dialog, allowing the user to modify
 *   the color value of a given cell within the color palette.
 */
MRESULT EXPENTRY ColorPickingDialog(
	HWND	hwnd,
	ULONG	msg,
	MPARAM	mp1,
	MPARAM	mp2)
{
	COLORPICKDATA	*pPickData	= WinQueryWindowPtr(hwnd,QWL_USER);
	MRESULT			mr			= 0;
	RGB2			*pRGB, rgb;
    CLRWHLCDATA		cwcd;
	ULONG			Value;

	switch (msg)
	{
		/* Initialize our dialog
		 */
		case WM_INITDLG:
			/* Allocate a structure to store our palette object pointer
			 * and the cell that is currently being edited.
			 */
			pPickData	= malloc(sizeof(COLORPICKDATA));
			if (pPickData)
			{
				/* Store away the object pointer to the palette that we
				 * are operating on.
				 */
				WinSetWindowPtr(hwnd,QWL_USER,pPickData);
				pPickData->Palette		= PVOIDFROMMP(mp2);
				pPickData->pCell		= NULL;
				pPickData->fSpinSet		= TRUE;

				/* Set the ranges on each of our spinbuttons
				 */
				WinSendDlgItemMsg(hwnd,DID_SPINR,		
								  SPBM_SETLIMITS,
								  MPFROMLONG(255),
								  MPFROMLONG(0));
				WinSendDlgItemMsg(hwnd,DID_SPINB,		
								  SPBM_SETLIMITS,
								  MPFROMLONG(255),
								  MPFROMLONG(0));
				WinSendDlgItemMsg(hwnd,DID_SPING,		
								  SPBM_SETLIMITS,
								  MPFROMLONG(255),
								  MPFROMLONG(0));
				pPickData->fSpinSet		= FALSE;
			}

			/* Store our handle away in the palette object so that it
			 * knows that we are loaded.
			 */
			_SetEditDlgHandle(pPickData->Palette,hwnd);
			break;

		/* Cleanup when our dialog is destroyed
		 */
		case WM_DESTROY:
			mr	= WinDefDlgProc(hwnd,msg,mp1,mp2);

			/* Let the palette object know that this dialog is no longer
			 * loaded and then free up our data structure
			 */
			_SetEditDlgHandle(pPickData->Palette,NULLHANDLE);
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
						WinSendDlgItemMsg(hwnd,DID_SPINR,
							  			  SPBM_QUERYVALUE,
										  MPFROMP(&Value),
										  0);
						rgb.bRed		= Value;
						WinSendDlgItemMsg(hwnd,DID_SPING,
							  			  SPBM_QUERYVALUE,
										  MPFROMP(&Value),
										  0);
						rgb.bGreen		= Value;
						WinSendDlgItemMsg(hwnd,DID_SPINB,
							  			  SPBM_QUERYVALUE,
										  MPFROMP(&Value),
										  0);
						rgb.bBlue		= Value;
						rgb.fcOptions	= 0;
						((PPAINTPOT)pPickData->pCell)->ulRGB
							= *((PULONG)&rgb);

						/* Redraw the cell and reset the color wheel
						 */
						_wpRedrawCell(pPickData->Palette,						
									  pPickData->pCell);
						WinSendDlgItemMsg(hwnd,DID_COLORWHEEL,
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
			switch(SHORT1FROMMP(mp1))
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
						WinSendDlgItemMsg(hwnd,DID_COLORWHEEL,
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
				pPickData->pCell	 = PVOIDFROMMP(mp1);
				if (pPickData->pCell)
				{
					pPickData->ulRGBOriginal = ((PPAINTPOT)mp1)->ulRGB;
					WinSendDlgItemMsg(hwnd,DID_COLORWHEEL,
									  CWM_SETRGBCLR,
									  MPFROMP(&((PPAINTPOT)mp1)->ulRGB),
									  0);
					WinSendMsg(hwnd,
							   MSG_SETRGBSPINBTNS,
							   MPFROMP(&((PPAINTPOT)mp1)->ulRGB),
							   0);
				}
			}
			break;

		/* Set the current values in the R,G and B spinbuttons
		 */
		case MSG_SETRGBSPINBTNS:
			if (pPickData)
			{
				pPickData->fSpinSet	= TRUE;
				pRGB				= PVOIDFROMMP(mp1);
				WinSendDlgItemMsg(hwnd,DID_SPINR,		
								  SPBM_SETCURRENTVALUE,
								  MPFROMSHORT(pRGB->bRed),
								  0);
				WinSendDlgItemMsg(hwnd,DID_SPINB,		
								  SPBM_SETCURRENTVALUE,
								  MPFROMSHORT(pRGB->bBlue),
								  0);
				WinSendDlgItemMsg(hwnd,DID_SPING,		
								  SPBM_SETCURRENTVALUE,
								  MPFROMSHORT(pRGB->bGreen),
								  0);
				pPickData->fSpinSet	= FALSE;
			}
			break;

		default:
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return mr;
}

/*
 * FUNCTION: _DLL_InitTerm
 *
 * DESCRIPTION:
 *   This function merely stores away our module handle when we get loaded
 *   so that we can use it later for loading icons, dialogs and so on from
 *   the resources within our dll.
 */
ULONG APIENTRY _DLL_InitTerm (
	ULONG hmodule,	
	ULONG ulFlag)		
{
	switch (ulFlag)
	{
		case 0:	/* A zero flag means called at DLL initialization */
			if (_CRT_init () == -1)
				return (ULONG)0;
			vhmodClrPalet	= hmodule; /* Store our mod handle in a global */
			break;
	}
	return 1;
}
