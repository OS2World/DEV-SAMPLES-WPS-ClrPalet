/******************************************************************************
 * MODULE NAME: ClrSampl.H
 *
 * DESCRIPTION:
 *	 Include file for COLOR_SAMPLE_CLASS window class
\****************************************************************************/

/* ----- Window class name and registration function ----- */

#define COLOR_SAMPLE_CLASS "ColorSampleControlClass"

BOOL EXPENTRY RegisterColorSampleClass(VOID);

/* ----- New messages supported by this control ----- */

#define CSM_SETRGBCOLOR			WM_USER	+ 0x100	
/* ULONG mp1:  RGB color value to be displayed
 * VOID  mp2:  not used
 * RETURNS:	   TRUE if the color was accepted, FALSE otherwise.
 */

#define CSM_QUERYRGBCOLOR		WM_USER	+ 0x101
/* ULONG mp1:  not used
 * VOID  mp2:  not used
 * RETURNS:	   RGB color value currently being displayed.
 */

#define CSM_EDITRGBCOLOR		WM_USER	+ 0x102
/* ULONG mp1:  not used
 * VOID  mp2:  not used
 * RETURNS:	   Cause this color sample control to go into editing mode.
 */

/* ----- Private functions used by our window class ----- */

MRESULT EXPENTRY ClrSampleWndProc(
	HWND	hwnd,
	ULONG	msg,
	MPARAM	mp1,
	MPARAM	mp2);

VOID MyDrawBorder(
	HPS		hps,
	RECTL	*prcl,
	ULONG	TopLeftColor,
	ULONG	BottomRightColor,
	LONG	cxBorder,
	LONG	cyBorder);
