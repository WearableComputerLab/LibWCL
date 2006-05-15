# 
# $Id$
#
# This configure build file indicates what libraries and includes
# exist on the system and generates the required code for to
# be able to correctly configure the libcommon library

AC_PREREQ(2.13)

# Initialise the system
AC_INIT(libwcl, 0.1, wcl@wcl.levels.unisa.edu.au)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(libwcl,0.1)

# Information
#AC_COPYRIGHT()
AC_REVISION($Id$)

#
# Configure options
#

# TCP/UDP Networking
AC_ARG_ENABLE(network, AC_HELP_STRING([--enable-network], [Enable basic TCP/UDP C++ Code]), , enable_network="no")
AM_CONDITIONAL(ENABLE_NETWORK, test "x$enable_network" = "xyes")

dnl AC_ARG_WITH(helix-libs, AC_HELP_STRING([--with-helix-libs], [some tesxt], helix_libs="$withval", helix_libs="no" )

# Test for Package Config
# We need this for a lot of packages
AC_PATH_PROG(PKG_CONFIG, pkg-config)
if test "x$PKG_CONFIG" = "x"; then
	AC_MSG_ERROR([You need to instal pkg-config])
fi

# Checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for header files
AC_CHECK_HEADERS([stdlib.h string.h])
			
#
# Check for libraries
#

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

# Files to create
AC_CONFIG_FILES([Makefile
                 src/Makefile
		 src/network/Makefile
		])


#
# Indicate what will be built into libwcl
#
echo "-----------------------------"
echo "libwcl Configuration Summary "
echo "-----------------------------"
echo "network support: $enable_network"
echo "-----------------------------"

# Build the configure file
AC_OUTPUT
