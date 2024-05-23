/******************************************************************************
 * MODULE NAME: ClrSampl.C
 *
 * DESCRIPTION:
 *	 Source code for COLOR_SAMPLE_CLASS window class
\****************************************************************************/
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#define INCL_WINWORKPLACE
#define INCL_WPFOLDER
#include <os2.h>
#include "clrsampl.h"

static CLASSINFO	vclsiStatic;

/*
 * FUNCTION: RegisterColorSampleClass
 *
 * DESCRIPTION:
 *   Register the color sample control window class name on this process.
 *   Note that this function must be called before any instance of the
 *   COLOR_SAMPLE_CLASS are used in your application's dialogs.
 */
BOOL EXPENTRY RegisterColorSampleClass()
{
	BOOL		rc;

	/* Register the color sample control class when requested to do so.
	 * Because we are incredibly lazy, we are using the WC_STATIC window
	 * procedure to perform most of our standard message processing - but
	 * why reinvent the wheel ?
	 */
	rc	= WinQueryClassInfo(NULLHANDLE,
							WC_STATIC,
							&vclsiStatic);
	if (rc)
	{
		vclsiStatic.flClassStyle	&= ~CS_PUBLIC;
		WinRegisterClass(NULLHANDLE,
						 COLOR_SAMPLE_CLASS,
						 ClrSampleWndProc,
						 vclsiStatic.flClassStyle | SS_FGNDFRAME,
						 vclsiStatic.cbWindowData
						);
	}
	return rc;
}

/*
 * WINDOW PROCEDURE: ClrSampleWndProc
 *
 * DESCRIPTION:
 *   Window procedure for the color sample window class.
 */
MRESULT EXPENTRY ClrSampleWndProc(
	HWND	hwnd,
	ULONG	msg,
	MPARAM	mp1,
	MPARAM	mp2)
{
	MRESULT		mr					= 0;
	CHAR		szSetup[CCHMAXPATH];
	HPS			hps;
	HOBJECT		hObject;
	ULONG		ulRGB;
	LONG  		cxBorder, cyBorder;
	RECTL		rclPaint;

	switch (msg)
	{
		/* Repaint this window as a square color swatch with a 3D border
		 */
		case WM_PAINT:
			hps 	= WinBeginPaint(hwnd, NULLHANDLE, &rclPaint);
			ulRGB	= WinQueryWindowULong(hwnd,QWL_USER);
			if (hps)
			{
				cxBorder	= WinQuerySysValue(HWND_DESKTOP,SV_CXBORDER),
				cyBorder	= WinQuerySysValue(HWND_DESKTOP,SV_CYBORDER);
				WinQueryWindowRect(hwnd,&rclPaint);
				GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,NULL);
				MyDrawBorder(hps,
						     &rclPaint,
						     SYSCLR_BUTTONDARK,
						     SYSCLR_BUTTONLIGHT,
						     cxBorder,
						     cyBorder);
				WinInflateRect(NULLHANDLE,&rclPaint,-cxBorder,-cyBorder);
				MyDrawBorder(hps,
						     &rclPaint,
						     SYSCLR_BUTTONLIGHT,
						     SYSCLR_BUTTONDARK,
						     WinQuerySysValue(HWND_DESKTOP,SV_CXBORDER),
						     WinQuerySysValue(HWND_DESKTOP,SV_CYBORDER));
				WinInflateRect(NULLHANDLE,&rclPaint,-cxBorder,-cyBorder);
				WinFillRect(hps,&rclPaint,ulRGB);
				WinEndPaint(hps);
			}
			break;
			
		/* Use the QWL_USER window word to store the current RGB color value
		 */
		case CSM_SETRGBCOLOR:
			WinSetWindowULong(hwnd,QWL_USER,LONGFROMMP(mp1));		
			WinInvalidateRect(hwnd,NULL,FALSE);
			mr		= (MRESULT)TRUE;
			break;

		/* Return the current RGB color value from our QWL_USER window word.
		 */
		case CSM_QUERYRGBCOLOR:
			mr		= (MRESULT)WinQueryWindowULong(hwnd,QWL_USER);
			break;

		/* If the user double clicks on us, communicate with our color
		 * palette object !
		 */
		case CSM_EDITRGBCOLOR:
		case WM_OPEN:
			sprintf(szSetup,"OPEN=DEFAULT;CTRLHDL=%lx",(ULONG)hwnd);

			/* Set the setup string through to our installed object
			 */
			hObject = WinQueryObject("<CLRPALET>");
			if (hObject)
				WinSetObjectData(hObject,szSetup);
			break;		

		default:
			mr	= (*vclsiStatic.pfnWindowProc)(hwnd,msg,mp1,mp2);
			break;
	}
	return mr;
}

/*
 * FUNCTION: MyDrawBorder
 *
 * DESCRIPTION:
 *   Draw a two color border of system defined thickness. WinDrawBorder does
 *   this functionality too, but you have to use undocumented flags to get
 *   the chiseled border effect !
 */
VOID MyDrawBorder(
	HPS		hps,
	RECTL	*prcl,
	ULONG	TopLeftColor,
	ULONG	BottomRightColor,
	LONG	cxBorder,
	LONG	cyBorder)
{
	RECTL	rcl;
	ULONG	ulSave;

	/* Draw the horizontals
	 */
	WinCopyRect(NULLHANDLE,&rcl,prcl);
	ulSave		= rcl.yBottom;
	rcl.yBottom	= rcl.yTop 	  - cyBorder;
	WinFillRect(hps,&rcl,TopLeftColor);
	rcl.yBottom	= ulSave;
	rcl.yTop	= rcl.yBottom + cyBorder;
	WinFillRect(hps,&rcl,BottomRightColor);

	/* Draw the verticals
	 */
	WinCopyRect(NULLHANDLE,&rcl,prcl);
	ulSave		= rcl.xRight;
	rcl.xRight	= rcl.xLeft  + cxBorder;
	WinFillRect(hps,&rcl,TopLeftColor);
	rcl.xRight 	= ulSave;
	rcl.xLeft	= rcl.xRight - cxBorder;
	WinFillRect(hps,&rcl,BottomRightColor);
}
