#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(VidoTest, [0.1], marnermr@cs.unisa.edu.au)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([./])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

#
# Checking for libwcl
#
PKG_CHECK_MODULES(WCL, libwcl)
PKGCONFIG_OTHERLIBS="$PKGCONFIG_OTHERLIBS $WCL_LIBS"
PKGCONFIG_OTHERINCLUDES="$PKGCONFIG_OTHERINCLUDES $WCL_CFLAGS"

PKGCONFIG_OTHERLIBS="$PKGCONFIG_OTHERLIBS -lGL -lGLU -lglut"

# Checks for header files.

AC_SUBST(PKGCONFIG_OTHERLIBS)
AC_SUBST(PKGCONFIG_OTHERINCLUDES)

AC_PROG_RANLIB
AC_OUTPUT
