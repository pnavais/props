AC_PREREQ([2.69])
AC_INIT([props], [0.1.0], [pnavais@gmail.com], [], [https://github.com/pnavais/props])
AC_CONFIG_SRCDIR([src/props.cc])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_PROG_RANLIB

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)

# Check for C++11
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# GCC specific configuration (optional)
GCC_WARN_CXXFLAGS="-fdiagnostics-color=auto -Wall -Wextra -Wpedantic -Wfatal-errors -Wshadow -Wnon-virtual-dtor"
GCC_WARN_CXXFLAGS_EXTRA="-Wcast-align -Wcast-qual -Wformat -Wpointer-arith -Wwrite-strings"
GCC_DEBUG_CXXFLAGS="-g -O0 -fno-omit-frame-pointer -fno-inline"

# Check for GCC warning flags (optional)
AC_MSG_CHECKING([whether to enable the GCC warnings])
AC_ARG_ENABLE([gcc-warnings],
    [AS_HELP_STRING([--enable-gcc-warnings],
                    [turn on lots of GCC warnings (for developers)])],
    [case "$enableval" in
        yes|no) ;;
        *)      AC_MSG_ERROR([bad value ${enableval} for gcc-warnings option]) ;;
    esac
    gcc_warnings=$enableval],
    [gcc_warnings=no]
)
AC_MSG_RESULT([$gcc_warnings])
if test "$gcc_warnings" = yes; then
    AC_SUBST([WARN_CXXFLAGS],"$GCC_WARN_CXXFLAGS $GCC_WARN_CXXFLAGS_EXTRA")
fi

# Check for GCC debug flags (optional)
AC_MSG_CHECKING([whether to enable the GCC debug build])
AC_ARG_ENABLE([gcc-debug],
    [AS_HELP_STRING([--enable-gcc-debug],
                    [turn on GCC debug flags (for developers)])],
    [case "$enableval" in
        yes|no) ;;
        *)      AC_MSG_ERROR([bad value ${enableval} for gcc-debug option]) ;;
    esac
    gcc_debug=$enableval],
    [gcc_debug=no]
)
AC_MSG_RESULT([$gcc_debug])
if test "$gcc_debug" = yes; then
    AC_SUBST([CXXFLAGS],"$CXXFLAGS $GCC_DEBUG_CXXFLAGS")
    AC_DEFINE([DEBUG],[],[Debug Mode])
else
    AC_DEFINE([NDEBUG],[],[Release Mode])
fi


		
# Configure internal libraries
CWD=`readlink -f $srcdir`
PROPS_LIB=${CWD}/lib/props-def
MAIN_LIB_HEADER="-I${CWD}/include"
PROPS_LIB_HEADER="-I${PROPS_LIB}/include"
PROPS_LIB_FUNC="-L${PROPS_LIB} -lprops"
AC_SUBST(MAIN_LIB_HEADER)
AC_SUBST(PROPS_LIB_HEADER)
AC_SUBST(PROPS_LIB_FUNC)

# Checks for libraries.

# Pthreads.
AX_PTHREAD([AM_CONDITIONAL([HAVE_PTHREAD],[true])], [AM_CONDITIONAL([HAVE_PTHREAD],[false])])
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_LIBS)

# PCRE
AX_PATH_LIB_PCRE([], [ AC_MSG_ERROR([No pcre library found.  You can get it at http://www.pcre.org]) ])
AC_SUBST(PCRE_LIBS)
AC_SUBST(PCRE_CFLAGS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 lib/props-def/Makefile
                 src/Makefile
                 ])
AC_OUTPUT

AC_MSG_RESULT([
props $VERSION is now configured
Configure Information:

  C++ Compiler      : $CXX
    DEFS            : $DEFS
    CPPFLAGS        : $CPPFLAGS
    CXXFLAGS        : $CXXFLAGS
    WARN_CXXFLAGS   : $WARN_CXXFLAGS
    WARNING         : $gcc_warnings
    DEBUG           : $gcc_debug

  Linker            : $LD
    LDFLAGS         : $LDFLAGS
    LIBS            : $LIBS  
	
  Libraries
    pthread         : $ax_pthread_ok
    pcre            : $with_pcre

])
