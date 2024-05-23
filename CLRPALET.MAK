#****************************************************************************
#  WorkPlace Shell Color Palette sample object class makefile
#****************************************************************************

# Dot directive definition area (usually just suffixes)
#
.SUFFIXES:
.SUFFIXES: .c .obj .dll .csc .sc .h .ih .ph .psc .rc .res

# Environment Setup for the component(s).
#
TOOLKIT = e:\toolkt21
SOMTEMP = .\somtemp
SCPATH  = $(TOOLKIT)\sc

!if [set SMINCLUDE=.;$(SCPATH);] || \
    [set SMTMP=$(SOMTEMP)] || \
    [set SMEMIT=ih;h;ph;psc;sc;c]
!endif

!if [cd $(SOMTEMP)]
!  if [md $(SOMTEMP)]
!    error error creating $(SOMTEMP) directory
!  endif
!else
!  if [cd ..]
!    error - Couldn't cd .. from $(SOMTEMP) directory
!  endif
!endif


# Set up the lists of libraries used and object modules to be built
#
CLRPALETOBJS    = clrpalet.obj clrstar.obj clrwheel.obj
CLRPALETLNKLIBS = os2386.lib dde4mbs.lib som.lib
CLRPALETLNKOPTS = /noi /map /nol /nod /exepack /packcode /packdata /align:16 /noe
TESTAPPOBJS     = clrsampl.obj testapp.obj
TESTAPPLNKLIBS  = os2386.lib dde4sbs.lib
TESTAPPLNKOPTS  = /NOP /NOI /NOP /M /NOL /NOD /PM:PM

# General rules for how to build things
#
.c.obj:
        $(CC) /Ge- /Ti+ /Ss+ -c $<

.csc.ih:
        sc -r $*.csc

# List of all the targets to be built
#
all: clrpalet.lib clrpalet.dll testapp.exe

# Build rules for CLRPALET.LIB
#
clrpalet.lib: clrpalet.def
        implib  clrpalet.lib clrpalet.def

# Build rules for CLRPALET.DLL
#
clrpalet.dll: $$(@B).def $(CLRPALETOBJS) clrpalet.res
         link386 $(CLRPALETLNKOPTS) $(CLRPALETOBJS),$@,,$(CLRPALETLNKLIBS),$*;
         rc $*.res $*.dll
         mapsym clrpalet.map

clrpalet.obj: $$(@B).c $$(@B).ih $$(@B).h  $$(@B).sc

clrpalet.ih: clrpalet.csc
        sc -r $*.csc

clrstar.obj: $$(@B).c $$(@B).ih $$(@B).h  $$(@B).sc

clrstar.ih: clrstar.csc
        sc -r $*.csc

clrpalet.res: $$(@B).rc $$(@B).ih
         rc -r $*.rc $*.res

# Build rules for TESTAPP.EXE
#
testapp.exe: $$(@B).def $(TESTAPPOBJS) testapp.res
         link386 $(TESTAPPLNKOPTS) $(TESTAPPOBJS),$@,,$(TESTAPPLNKLIBS),$*;
         rc $*.res $*.exe
         mapsym testapp.map

testapp.obj: testapp.c
   icc /Ss /Q /c /Ti /Gmdn $*.c

clrsampl.obj: clrsampl.c
   icc /Ss /Q /c /Ti /Gmdn $*.c

testapp.res: $$(@B).rc
         rc -r $*.rc $*.res
