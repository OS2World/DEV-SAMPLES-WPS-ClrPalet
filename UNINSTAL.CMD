/* REXX installation program for ColorPalette and StarColorPalette classes */
call RxFuncAdd 'SysDestroyObject', 'RexxUtil', 'SysDestroyObject'
call RxFuncAdd 'SysRegisterObjectClass', 'RexxUtil', 'SysRegisterObjectClass'

/* Create an instance of each class, open in palette view */
/*if rc != 0 then*/
/*do*/
	rc	= SysDestroyObject('<CLRPALET>');
	rc	= SysDestroyObject('<STARCLRPALET>');
	rc	= SysDestroyObject('<PALETFLDR>');
/*end*/
if rc = 1 then
	say 'Successful completion';


