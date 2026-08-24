/************************************************************************/
/*     The following [enclosed] code is library code created by the     */
/*     authors.  This source code is  provided to you solely            */
/*     for the purpose of assisting you in the development of your      */
/*     applications.  The code is provided "AS IS", without             */
/*     warranty of any kind.  The authors shall not be liable           */
/*     for any damages arising out of your use of the library code,     */
/*     even if they have been advised of the possibility of such        */
/*     damages.                                                         */
/************************************************************************/


PALETTE INSTALLATION INSTRUCTIONS: PLEASE FOLLOW CAREFULLY
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1) Copy the CLRPALET dll into a place that is on your LIBPATH. Note that
   this should NOT be the '.\' directory, but rather a real named directory
   that appears in your LIBPATH such as C:\OS2\DLL. Bear in mind that the '.\'
   directory for the Workplace Shell process is the root of the boot drive...

2) Run INSTALL.CMD to install the color palette folder and create instances
   of ColorPalette and StarColorPalette. NOTE: it seems as though you have to
   run INSTALL.CMD from the directory where CLRPALET.DLL is currently located
   otherwise the second object class (StarColorPalette) won't register - I
   am assuming this may be some kind of REXX problem, or a bug in Workplace
   Shell.

=> A folder called 'Palette Folder' should appear on your desktop containing
   two palette objects that will all be opened for you by the install program.

3) Run TESTAPP.EXE. It will popup a dialog that contains two color sample
   controls that are interfaced directly with one of the color palettes
   created in step(2).

=> When you double click on the color sample control,
   one of the palettes created in step(2) will appear and you can change
   the color of the sample to any value currently in that palette of colors.

4) Note that you can even use the WinReplaceObjectClass api to make your
   color palette object classes replace the system provided color palette
   class.

5) Run UNINSTAL.CMD to uninstall the color palette folder and delete instances
   of ColorPalette and StarColorPalette.

=> The folder called 'Palette Folder' will be deleted from your desktop.

BUILDING THE SOURCE:
~~~~~~~~~~~~~~~~~~~~

1) If you plan to modify and build the DLL and executable, please use the
   OS/2 2.1 Toolkit, as you will get errors with the 2.0 Toolkit.  The
   makefile we include, clrpalet.mak, should work with either CSet/2 or
   C Set++ V2.X.


MAKING THE WORKPLACE SHELL EASY TO START AND STOP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1) Copy the MYSHELL.EXE program to your boot drive, for example to the C:\OS2
   directory.

2) Make a backup of your CONFIG.SYS file (just in case you mess up!).

3) Change your CONFIG.SYS file so that MYSHELL.EXE will be run instead of
   PMSHELL.EXE:

     PROTSHELL=C:\OS2\MYSHELL.EXE

4) Reboot your machine.

=> A "Workplace Shell" icon will appear in the bottom left hand corner of your
   screen. You will notice that its context menu provides options that let
   you start or stop the shell at any time: the program is merely using
   the system provided WinStartApp() and WinTerminateApp() api calls to
   start and stop the Workplace Shell executable - PMSHELL.EXE.

   MYSHELL.EXE is handy when you start developing object classes of your own
   and need to update your object class dlls ...


SYS_DLLs:
~~~~~~~~~

1) The SYS_DLL sample program is located in PROFILE.ZIP.  Please follow
   the instructions in the SYSDLL.TXT file which is included.

