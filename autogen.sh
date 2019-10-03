#! /bin/sh 

# This script is used to automate the process of running some of the autotools
# against their input files (configure.in, Makefile.am) after _any_ of them have
# been updated.  The commands and parameters were taken based on a similar script
# found via google and seeing the same commands issued in GNU autotool tutorials.

# Peform some cleanup
#rm -fr autom4te.cache build-aux config.h config.h.in config.log config.status configure Makefile
#Makefile.am stamp-h* aclocal.m4

AC_SEARCH_OPTS=""

# For those of us with pkg-config and other tools in /usr/local
PATH=$PATH:/usr/local/bin

# This is to make life easier for people who installed pkg-config in /usr/local
# but have autoconf/make/etc in /usr/. AKA most mac users
if [ -d "/usr/local/share/aclocal" ]
then
    AC_SEARCH_OPTS="-I /usr/local/share/aclocal"
fi

aclocal 
autoconf 
autoheader
automake --add-missing --force-missing 
#autoreconf -ivf
