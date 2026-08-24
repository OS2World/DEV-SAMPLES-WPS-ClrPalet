#****************************************************************************
# Makefile.wat - Open Watcom build for the ColorPalette / StarColorPalette
#                Workplace Shell classes (OS/2 32-bit LX DLL) plus the
#                color-sample-control test application (PM EXE).
#
# Project layout:
#   idl\    clrpalet.idl, clrstar.idl (interface definitions)
#   h\      headers: clrpids.h, pdsctls.h, clrsampl.h +
#           sc-generated clrpalet.ih/.h, clrstar.ih/.h
#   src\    implementations, resource scripts, module definitions
#   orig\   pristine copy of DEV-SAMPLES-WPS-ClrPalet
#   release build output: .obj/.res/.dll/.exe/.map
#
# Runs identically natively on OS/2 (ArcaOS Dev VM) or cross-hosted on
# Windows - same tools, same flags.
#
# PREREQUISITES (details and rationale in README.md):
#   1. Open Watcom installed; WATCOM environment variable set.
#   2. h\clrpalet.ih/.h and h\clrstar.ih/.h produced ONCE by the SOM
#      compiler - run genbind.cmd on the OS/2 side.  This makefile never
#      invokes sc: the real implementation lives in src\*.c and must NOT
#      be overwritten by sc's emitted templates.
#   3. SOMINC points at a directory containing som.h, somobj.h, ...;
#      WPSINC at one containing os2.h, pmwin.h, wpobject.h, ...
#   4. SOMLIB: import library for SOM.DLL - shipped with the toolkit.
#      Parent WPS classes come from wpconfig.dll; src\wpconfig.def +
#      IMPLIB produce release\wpconfig.lib for those (see README.md).
#****************************************************************************

WATCOM  = $(%WATCOM)

# ---- adjust these paths/libraries for your machine ------------------------
# Defaults match the ArcaOS Dev VM (toolkit at C:\os2tk45 - same tree that
# SMINCLUDE uses for genbind.cmd).  Override without editing:
#     wmake -f Makefile.wat SOMINC=D:\path\som\include WPSINC=...
# SOMINC : som.h, somobj.h, somcdev.h, ...
SOMINC  = C:\os2tk45\som\include
# WPSINC : os2.h, pmwin.h, wpobject.h, wpfsys.idl headers, ...
WPSINC  = C:\os2tk45\h
# import library for SOM.DLL - shipped with the toolkit
SOMLIB  = C:\os2tk45\som\lib\somtk.lib
# ---------------------------------------------------------------------------

HDIR    = h
SRC     = src
OUT     = release

CC      = wcc386
LINK    = wlink
RC      = wrc
IMPLIB  = implib

# Calling-convention note: SOMLINK stays EMPTY under Watcom (somltype.h has
# no __WATCOMC__ case) - do not -d-define it (E1100 macro conflict).  Linkage
# is instead guaranteed by #pragma linkage(..., system) emitted into the
# generated bindings and used throughout the toolkit headers (sombtype.h:35,
# wpobject.h:2096, ...), which matches IBM's own header design.
#
# -wcd=1177: with -wx, IBM's sombtype.h(41) ("typedef somMethodProc SOMDLINK
# *somMethodPtr;") raises W1177 "Modifier repeated in declaration" under
# Watcom - IBM never built these headers with OW.  The toolkit tree is
# read-only ground truth, so this single message number is silenced instead;
# every other warning still surfaces.
CFLAGS  = -bt=os2 -zq -wx -wcd=1177 -d1 &
          -I$(HDIR) -I$(SOMINC) -I$(WPSINC)

# DLL object modules: the two SOM classes plus the color wheel control.
DLLOBJ  = $(OUT)\clrpalet.obj $(OUT)\clrstar.obj $(OUT)\clrwheel.obj

# Export set - mirrors src\clrpalet.def exactly.  NOTE: sc emits no
# M_StarColorPalette bindings (no explicit metaclass in clrstar.idl) -
# the 1993 DEF comment "SOM didn't give us a metaclass" still holds.
EXPS    = EXP ColorPaletteClassData EXP ColorPaletteCClassData &
          EXP ColorPaletteNewClass &
          EXP M_ColorPaletteClassData EXP M_ColorPaletteCClassData &
          EXP M_ColorPaletteNewClass &
          EXP StarColorPaletteClassData EXP StarColorPaletteCClassData &
          EXP StarColorPaletteNewClass &
          EXP SOMInitModule

# Parent-class symbols referenced by the sc-generated class-construction
# code all live in wpconfig.dll (wppalet.idl:86, wpclrpal.idl:34);
# somtk.lib only covers SOM.DLL itself.  An import library is generated
# from src\wpconfig.def (see the WPSLIB rule below).
WPSDEF  = $(SRC)\wpconfig.def
WPSLIB  = $(OUT)\wpconfig.lib

LFLAGS  = SYSTEM OS2V2_DLL NAME $(OUT)\clrpalet.dll &
          OP MAP=$(OUT)\clrpalet.map &
          LIBF $(SOMLIB),$(WPSLIB) $(EXPS)

# Test application - plain PM program, no SOM linkage at all.
XFLAGS  = SYSTEM OS2V2_PM NAME $(OUT)\testapp.exe &
          OP MAP=$(OUT)\testapp.map OP STACK=32768

all : $(OUT)\clrpalet.dll $(OUT)\testapp.exe

$(OUT)\clrpalet.obj : $(SRC)\clrpalet.c idl\clrpalet.idl &
                      $(HDIR)\clrpalet.ih $(HDIR)\clrpalet.h &
                      $(HDIR)\clrpids.h $(HDIR)\pdsctls.h $(HDIR)\clrsampl.h
    $(CC) -bd $(CFLAGS) $(SRC)\clrpalet.c -fo=$@

$(OUT)\clrstar.obj : $(SRC)\clrstar.c idl\clrstar.idl &
                     $(HDIR)\clrstar.ih $(HDIR)\clrstar.h &
                     $(HDIR)\clrpalet.h
    $(CC) -bd $(CFLAGS) $(SRC)\clrstar.c -fo=$@

$(OUT)\clrwheel.obj : $(SRC)\clrwheel.c $(HDIR)\pdsctls.h
    $(CC) -bd $(CFLAGS) $(SRC)\clrwheel.c -fo=$@

# wrc -r always writes <name>.res next to the source; relocate afterwards.
$(OUT)\clrpalet.res : $(SRC)\clrpalet.rc $(HDIR)\clrpids.h $(HDIR)\pdsctls.h &
                      $(SRC)\clrpalet.ico
    $(RC) -r -i=$(HDIR) -i=$(WPSINC) $(SRC)\clrpalet.rc
    copy $(SRC)\clrpalet.res $(OUT)
    del $(SRC)\clrpalet.res

$(OUT)\wpconfig.lib : $(WPSDEF)
    $(IMPLIB) $@ $?

$(OUT)\clrpalet.dll : $(DLLOBJ) $(OUT)\clrpalet.res $(SRC)\clrpalet.def $(WPSLIB)
    $(LINK) $(LFLAGS) FIL $(OUT)\clrpalet.obj,$(OUT)\clrstar.obj,$(OUT)\clrwheel.obj
    $(RC) $(OUT)\clrpalet.res $(OUT)\clrpalet.dll
# No MAPSYM step: IBM mapsym rejects Watcom's map format ("Unexpected eof"),
# and .sym files are optional debug aids.

$(OUT)\testapp.obj : $(SRC)\testapp.c $(SRC)\testapp.h &
                     $(HDIR)\clrsampl.h
    $(CC) $(CFLAGS) $(SRC)\testapp.c -fo=$@

$(OUT)\clrsampl.obj : $(SRC)\clrsampl.c $(HDIR)\clrsampl.h
    $(CC) $(CFLAGS) $(SRC)\clrsampl.c -fo=$@

$(OUT)\testapp.exe : $(OUT)\testapp.obj $(OUT)\clrsampl.obj
    $(LINK) $(XFLAGS) FIL $(OUT)\testapp.obj,$(OUT)\clrsampl.obj

bindings : .SYMBOLIC
    @echo Run genbind.cmd on the OS/2 side once; this makefile expects
    @echo h\clrpalet.ih/.h and h\clrstar.ih/.h to already exist.

clean : .SYMBOLIC
    -del $(OUT)\*.obj
    -del $(OUT)\*.res
    -del $(OUT)\*.dll
    -del $(OUT)\*.exe
    -del $(OUT)\*.map
# wcc386 drops <name>.err files into the current directory on diagnostics
    -del *.err
