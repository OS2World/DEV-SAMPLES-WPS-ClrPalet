/* genbind.cmd - REXX script. Run ONCE on the OS/2 side (ArcaOS Dev VM),
 * from the project directory, to produce the SOM bindings:
 *     h\clrpalet.ih h\clrpalet.h      from idl\clrpalet.idl
 *     h\clrstar.ih  h\clrstar.h       from idl\clrstar.idl
 *
 * LOGGING: summary -> release\genbind.log ; raw compiler output ->
 * release\sc_raw.log (stdout+stderr when sh.exe is available).
 *
 * NOTE: never call bare "sc" - Open Watcom ships its own SC.EXE which
 * shadows the SOM compiler on PATH (and which kLIBC sh cannot exec,
 * producing a misleading "Syntax error" from ash).  This script therefore
 * uses an explicit compiler path.
 */

/* ---------------- EDIT THIS BLOCK FOR YOUR MACHINE ---------------------
 * somBin : directory containing the SOM compiler SC.EXE
 * smIdl  : extra directories (semicolon-separated) holding the IDL interface
 *          files sc must find: somobj.idl, somcls.idl, somcm.idl ...
 *          and wpobject.idl, wpfsys.idl, wpfolder.idl, wppalet.idl,
 *          wpclrpal.idl ...  ('.' included automatically)
 * Leave a value '' to have the script try common locations. */
somBin = ''
smIdl  = ''
/* ----------------------------------------------------------------------- */

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

logfile = 'release\genbind.log'
sclog   = 'release\sc_raw.log'
idl.0   = 2
idl.1   = 'clrpalet'
idl.2   = 'clrstar'

call log '=== genbind start ' date() time() ' ==='
call log 'cwd       : ' directory()

/* ---- locate the SOM compiler ------------------------------------------ */
if somBin = '' then do
    candidates = 'C:\SOM\BIN D:\SOM\BIN E:\SOM\BIN' || ,
                 ' C:\IBMSOM\BIN D:\IBMSOM\BIN' || ,
                 ' C:\SOMOBJ\BIN C:\SOMOBJECTS\BIN' || ,
                 ' C:\TOOLKIT\SOM\BIN D:\TOOLKIT\SOM\BIN' || ,
                 ' C:\OS2TK45\SOM\BIN D:\OS2TK45\SOM\BIN'
    do i = 1 while somBin = '' & i <= words(candidates)
        d = word(candidates, i)
        if stream(d'\SC.EXE', 'C', 'QUERY EXISTS') <> '' then
            somBin = d
    end
end

if somBin <> '' then
    scexe = stream(somBin'\SC.EXE', 'C', 'QUERY EXISTS')
else do
    p = SysSearchPath('PATH', 'SC.EXE')
    if pos('\WATCOM\', translate(p)) > 0 | pos('/WATCOM/', translate(p)) > 0 then
        p = ''                       /* that is Watcom's sc, not ours */
    scexe = p
end

if scexe = '' then do
    call log 'ERROR     : SOM compiler SC.EXE not found.'
    call log '            Set somBin at the top of this script to the'
    call log '            directory holding the SOM compiler and rerun.'
    call finish 8
end
call log 'sc        : ' scexe

/* ---- SMINCLUDE -------------------------------------------------------- */
cur = VALUE('SMINCLUDE', , 'OS2ENVIRONMENT')
if cur <> '' then
    call log 'SMINCLUDE : (from environment) ' cur
if smIdl <> '' then do
    x = VALUE('SMINCLUDE', smIdl';.', 'OS2ENVIRONMENT')
    call log 'SMINCLUDE : (set by script)   ' smIdl';.'
end
else if cur = '' then
    call log 'WARNING   : SMINCLUDE still empty - sc will not find somobj.idl/wppalette.idl'

/* ---- scratch dir for sc ------------------------------------------------ */
x = VALUE('SMTMP', '.\release', 'OS2ENVIRONMENT')

call SysFileDelete sclog

/* Prefer running the compiler under sh.exe so BOTH stdout and stderr land
 * in the log (OS/2 CMD.EXE cannot redirect stderr).
 * -I idl lets sc resolve '#include <clrpalet.idl>' inside clrstar.idl. */
scSh = translate(scexe, '/', '\')          /* forward slashes for sh      */
shp  = SysSearchPath('PATH', 'SH.EXE')
do n = 1 to idl.0
    stem = idl.n

    /* remove previous outputs FIRST, so the existence check below really
     * proves THIS run succeeded (stale bindings must never masquerade as
     * fresh ones - that is how a failed sc still reported "OK").         */
    call SysFileDelete 'h\'||stem||'.ih'
    call SysFileDelete 'h\'||stem||'.h'
    call SysFileDelete 'h\'||stem||'.c'

    /* -d h : sc's "output directory for each emitted file" option, so the
     * .ih/.h/.c land DIRECTLY in h\ (a bare invocation drops them in the
     * cwd / input dir).  NOTE: sc has no -o option - that token seen in
     * somipc's internal command line is not usable here.                 */
    if shp <> '' then do
        call log 'shell     : sh.exe (stdout+stderr captured)'
        ADDRESS CMD "sh.exe -c '"""scSh""" -s ""ih;h;c"" -v -d h -I idl idl/" || stem || ".idl >> release/sc_raw.log 2>&1'"
    end
    else do
        if n = 1 then call SysFileDelete sclog
        call log 'shell     : cmd.exe only (stdout captured; stderr stays on console)'
        ADDRESS CMD '"'||scexe||'" -s "ih;h;c" -v -d h -I idl idl\'||stem||'.idl >> release\sc_raw.log'
    end
    scrc = rc
    call log 'sc rc     ['stem']: ' scrc

    /* keep the emitted template .c as reference for SOMInitModule etc.,  */
    /* but out of the build path (src\*.c are hand-ported)                */
    ADDRESS CMD 'if exist h\'||stem||'.c copy h\'||stem||'.c release\sc_'||stem||'_template.c >nul'
    ADDRESS CMD 'if exist h\'||stem||'.c del  h\'||stem||'.c'

    if stream('h\'||stem||'.ih', 'C', 'QUERY EXISTS') = '' | ,
       stream('h\'||stem||'.h',  'C', 'QUERY EXISTS') = '' then do
        call log 'ERROR     : sc produced no bindings for ' stem
        call log '            (rc='scrc'). Inspect release\sc_raw.log.'
        call finish 12
    end
end

ok = 1
do n = 1 to idl.0 while ok
    stem = idl.n
    if stream('h\'||stem||'.ih', 'C', 'QUERY EXISTS') = '' | ,
       stream('h\'||stem||'.h',  'C', 'QUERY EXISTS') = '' then
        ok = 0
end

if ok then do
    call log 'RESULT    : OK - all bindings written to h\.'
    call finish 0
end
else do
    call log 'RESULT    : FAILED - bindings missing; inspect ' sclog
    call finish 12
end

finish:
    call lineout logfile          /* flush/close the log */
    exit arg(1)

log:
    parse arg msg
    call lineout logfile, msg
    say msg
    return

halt:
    say 'genbind interrupted.'
    exit 4
