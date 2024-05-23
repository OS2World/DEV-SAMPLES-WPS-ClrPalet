/**************************************************************************\
 * NAME: TESTAPP.C
 *
 * DESCRIPTION:
 *	 Test application that illustrates the use of the color sample control.
\***************************************************************************/

#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINFRAMEMGR
#define INCL_WINPROGRAMLIST
#define INCL_WINSHELLDATA
#define INCL_WININPUT
#define INCL_WINSYS
#define INCL_WINWORKPLACE
#define INCL_WPCLASS
#define INCL_WPFOLDER
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#define	INCL_DOSEXCEPTIONS
#define INCL_DOSEXCEPTIONS
#define M_I86L

#include <stdio.h>
#include <string.h>
#include <os2.h>
#include "clrsampl.h"
#include "testapp.h"

/*----------------------------------------------------------------*/
/* Function: main                                                 */
/*                                                                */
/* Description:                                                   */
/*   Main line of program                                         */
/*----------------------------------------------------------------*/
int main(
	int 	argc,
	char 	*argv[] )
{
	HAB				hab;
	HMQ				hmq;
	QMSG   			qmsg;

	DosBeep(100,100);
	hab = WinInitialize( 0 );
	hmq = WinCreateMsgQueue( hab, 0 );

	/* Register the color sample control class
	 */
	RegisterColorSampleClass();
	
	/* Show our main dialog box...
	 */
	if (hab && hmq)
		WinDlgBox(HWND_DESKTOP,
                  HWND_DESKTOP,
                  TestAppDlgProc,
                  NULLHANDLE,
                  SAMPLE_DIALOG,
                  NULL);

	/* Cleanup our resources
	 */
	WinDestroyMsgQueue(hmq);
	WinTerminate(hab);
	return 0;
}
	
/*----------------------------------------------------------------*/
/* Function: TestAppDlgProc                                       */
/*                                                                */
/* Description:                                                   */
/*   Test application dialog procedure.                           */
/*----------------------------------------------------------------*/
MRESULT EXPENTRY TestAppDlgProc(
	HWND    hwnd,
	ULONG   msg,
	MPARAM  mp1,
	MPARAM  mp2)
{
	MRESULT	mr			= (MRESULT)0;
	USHORT	command;

	switch (msg)
	{
		/* Initialize our application dialog
		 */
		case WM_INITDLG:
 			/* Set the initial colors in our sample controls
			 */		
			WinSendDlgItemMsg(hwnd,DID_SAMPLE1,
							  CSM_SETRGBCOLOR,
							  MPFROMLONG(0x00FFFF),
							  0);
			WinSendDlgItemMsg(hwnd,DID_SAMPLE2,
							  CSM_SETRGBCOLOR,
							  MPFROMLONG(0xFFFF00),
							  0);
			break;		

		/* Process button pushes from our dialog
		 */
		case WM_COMMAND:
			command = SHORT1FROMMP(mp1);
	    	switch (command)
			{
				/* Allow DID_OK to go to WinDefDlgProc and dismiss our dialog
				 */
				case DID_OK:
					mr	= WinDefDlgProc(hwnd,msg,mp1,mp2);
					break;

				/* When the user presses the first "Edit color..." pushbutton
				 * we wish to tell color sample 1 to go into editing mode
				 */
				case DID_EDIT1:
					WinSendDlgItemMsg(hwnd,DID_SAMPLE1,
									  CSM_EDITRGBCOLOR,
									  0,
									  0);
					break;

				/* When the user presses the second "Edit color..." pushbutton
				 * we wish to tell color sample 2 to go into editing mode
				 */
				case DID_EDIT2:
					WinSendDlgItemMsg(hwnd,DID_SAMPLE2,
									  CSM_EDITRGBCOLOR,
									  0,
									  0);
					break;
			}
			break;

		default:
			mr	= WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return mr;
}

