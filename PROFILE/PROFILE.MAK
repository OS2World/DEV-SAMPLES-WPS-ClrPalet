
# MAKE file for profile.mak

CC=Icc

# Non-Debug Compile flags
CFLAGS=-G3 -O+ -Rn -W3 -C

# Debug Compile flags
# CFLAGS=-G3 -Ti+ -O- -Rn -W3 -C

# Non-Debug Link flags
LFLAGS=

# Debug Link flags
# LFLAGS=/DE

profile.Exe: profile.obj
  Link386 $(LFLAGS) @profile.Lnk;

profile.obj: profile.c
 $(CC) $(CFLAGS) -Fo profile.obj profile.C
