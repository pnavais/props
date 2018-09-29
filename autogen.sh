#! /bin/sh 

# This script is used to automate the process of running some of the autotools
# against their input files (configure.in, Makefile.am) after _any_ of them have
# been updated.  The commands and parameters were taken based on a similar script
# found via google and seeing the same commands issued in GNU autotool tutorials.

# Peform some cleanup
#rm -fr autom4te.cache build-aux config.h config.h.in config.log config.status configure Makefile
#Makefile.am stamp-h* aclocal.m4

aclocal 
automake --add-missing --force-missing 
autoconf 
#autoreconf -ivf
