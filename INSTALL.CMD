/* REXX installation program for ColorPalette and StarColorPalette classes */
call RxFuncAdd 'SysCreateObject', 'RexxUtil', 'SysCreateObject'
call RxFuncAdd 'SysRegisterObjectClass', 'RexxUtil', 'SysRegisterObjectClass'

/* Register the object classes */
rc	= SysRegisterObjectClass('ColorPalette','CLRPALET');
if rc = 0 then
	say 'Registration of ColorPalette class failed.';
rc	= SysRegisterObjectClass('StarColorPalette','CLRPALET');
if rc = 0 then
	say 'Registration of StarColorPalette class failed.';

/* Create an instance of each class, open in palette view */

rc	= SysCreateObject('WPFolder',,
					  'Palette Folder',,
					  '<WP_DESKTOP>',,
					  'OPEN=ICON;ICONPOS=55,50;OBJECTID=<PALETFLDR>',,
					  'update');
if rc = 0 then
	say 'Creation of palette folder failed.';
rc	= SysCreateObject('ColorPalette',,
					  'Color Palette',,
					  '<PALETFLDR>',,
					  'OPEN=DEFAULT;OBJECTID=<CLRPALET>',,
					  'replace');
if rc = 0 then
	say 'Creation of color palette failed.';
rc	= SysCreateObject('StarColorPalette',,
					  'Star^Color Palette',,
					  '<PALETFLDR>',,
					  'OPEN=DEFAULT;OBJECTID=<STARCLRPALET>',,
					  'replace');
if rc = 0 then
	say 'Creation of star color palette failed.';

if rc = 1 then
	say 'Successful completion';


