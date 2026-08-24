/* deregister.cmd - remove the objects created by register.cmd and
 * deregister both classes.  Mirrors orig\UNINSTAL.CMD, plus the
 * SysDeregisterObjectClass calls it forgot.
 */

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

rc = SysDestroyObject('<CLRPALET>')
say 'Destroy <CLRPALET>      rc =' rc
rc = SysDestroyObject('<STARCLRPALET>')
say 'Destroy <STARCLRPALET>  rc =' rc
rc = SysDestroyObject('<PALETFLDR>')
say 'Destroy <PALETFLDR>     rc =' rc

rc = SysDeregisterObjectClass('ColorPalette')
say 'Deregister ColorPalette     rc =' rc
rc = SysDeregisterObjectClass('StarColorPalette')
say 'Deregister StarColorPalette rc =' rc

/* WPS keeps class data cached per DLL path: if you rebuilt the DLL,
 * reboot (or at least close the WPS) before registering a new build.
 */
say 'Done. A WPS restart is recommended after deregistering.'
exit 0
