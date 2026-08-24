/* register.cmd - register the ColorPalette and StarColorPalette classes and
 * create a palette folder on the desktop containing one instance of each.
 * Run on the ArcaOS Dev VM from the project directory (after a successful
 * wmake); the DLL is referenced by full path so no LIBPATH edit is needed.
 *
 * REXX, using the RexxUtil functions that map straight onto
 * WinRegisterObjectClass / WinCreateObject.  Mirrors orig\INSTALL.CMD,
 * except that it registers against our built DLL path instead of the
 * bare module name 'CLRPALET'.
 */

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

dll = directory()'\release\clrpalet.dll'
if stream(dll, 'C', 'QUERY EXISTS') = '' then do
    say 'ERROR: ' dll 'not found - run wmake first.'
    exit 1
end
say 'DLL: ' dll

/* Register both classes against our DLL (INSTALL.CMD used 'CLRPALET').
 */
rc = SysRegisterObjectClass('ColorPalette',      dll)
if rc = 0 then
    say 'Registration of ColorPalette class failed.'
rc2 = SysRegisterObjectClass('StarColorPalette', dll)
if rc2 = 0 then
    say 'Registration of StarColorPalette class failed.'
if rc <> 1 | rc2 <> 1 then do
    say 'Registration failed - see C:\POPUPLOG.OS2'
    exit 1
end

/* Create an instance of each class inside a desktop palette folder
 * (same objects and OBJECTIDs as INSTALL.CMD).
 */
rc = SysCreateObject('WPFolder',            ,
                     'Palette Folder',      ,
                     '<WP_DESKTOP>',        ,
                     'OPEN=ICON;ICONPOS=55,50;OBJECTID=<PALETFLDR>', ,
                     'update')
if rc = 0 then
    say 'Creation of palette folder failed.'

rc = SysCreateObject('ColorPalette',        ,
                     'Color Palette',       ,
                     '<PALETFLDR>',         ,
                     'OPEN=DEFAULT;OBJECTID=<CLRPALET>',  ,
                     'replace')
if rc = 0 then
    say 'Creation of color palette failed.'

rc = SysCreateObject('StarColorPalette',    ,
                     'Star^Color Palette',  ,
                     '<PALETFLDR>',         ,
                     'OPEN=DEFAULT;OBJECTID=<STARCLRPALET>', ,
                     'replace')
if rc = 0 then
    say 'Creation of star color palette failed.'

if rc = 1 then do
    say 'Successful completion'
    say '(Open "Palette Folder" on the desktop; double-click a swatch to'
    say ' open the color picking dialog.)'
end
exit 0
