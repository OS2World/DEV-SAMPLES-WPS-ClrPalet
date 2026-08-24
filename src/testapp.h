/***************************************************************************\
 * NAME: TESTAPP.H
 *
 * DESCRIPTION:
 *	 Test application that shows a sample use of the color sample control.
\***************************************************************************/

#define DebugBox(title, text) WinMessageBox(HWND_DESKTOP,HWND_DESKTOP, (PSZ) text , (PSZ) title, 20, MB_OK)

/* Function definitions
 */
MRESULT EXPENTRY TestAppDlgProc(
	HWND 	hwnd,
	ULONG	message,
	MPARAM	mp1,
	MPARAM	mp2 );

int main(
	int 	argc,
	char 	*argv[] );

#define SAMPLE_DIALOG			100
#define	DID_EDIT1				901
#define DID_EDIT2				902
#define DID_SAMPLE1				903
#define DID_SAMPLE2				904
#define DID_BOGUS  				0xFFFF

