# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

#
# These macros allow compilation of libwcl with MSVC++7.
# Credit for some of this should got to Systems In Motion as some of these
# were taken from their aclocal.m4 macro in the Coin-2 library
#

#
# WCL_AC_MSVC_VERSION
#-----------------------------------
# Check which version of Visual studio we are
# using. This is a simple check but helps work out
# if we are using MSVC 6 which is majorly broken
#
AC_DEFUN([WCL_AC_MSVC_VERSION], [
AC_MSG_CHECKING([Visual Studio C++ version])
AC_TRY_COMPILE(
[],
[long long number = 0;],
[wcl_ac_msvc_version=7],
[wcl_ac_msvc_version=6])
AC_MSG_RESULT($wcl_ac_msvc_version)
])

#
# WCL_AC_SPACE_IN_PATHS
#------------------------------------
# This checks to see if there is any paths in
# the src directory. This causes issues with cygwin
#
AC_DEFUN([WCL_AC_SPACE_IN_PATHS], [
AC_MSG_CHECKING([for spaces in src path])
wcl_ac_full_builddir=`pwd`
wcl_ac_full_srcdir=`cd $srcdir; pwd`
if test -z "`echo $wcl_ac_full_srcdir | tr -cd ' '`"; then 
AC_MSG_RESULT([none found])
 else
AC_MSG_ERROR([Detected space character in the path leading up to the libwcl source directory - this will probably cause random problems later. You must move the libwcl source directory to another location.])
fi
]) 

#
# WCL_AC_MSVC_SETUP
#-----------------------------------
# This attempts to setup configure to
# use the Microsoft Visual C++ compiler
#
AC_DEFUN([WCL_AC_MSVC_SETUP], [ 

    AC_REQUIRE([WCL_AC_SPACE_IN_PATHS])
   
    if test -z "$CC" -a -z "$CXX"; then
	wcl_ac_wrapmsvc=`cd $ac_aux_dir; pwd`/wrapmsvc.exe
	echo "$as_me:$LINENO: wcl_ac_wrapmsvc=$wcl_ac_wrapmsvc" >&AS_MESSAGE_LOG_FD
	AC_MSG_CHECKING([setup for wrapmsvc.exe])
	if $wcl_ac_wrapmsvc >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD; then
	   m4_ifdef([$0_VISITED], [AC_FATAL([Macro $0 invoked multiple times])])
		    m4_define([$0_VISITED], 1)
		    CC=$wcl_ac_wrapmsvc
		    CXX=$wcl_ac_wrapmsvc
		    CFLAGS="$CFLAGS /EHsc /DWIN32 /MLd"
		    CXXFLAGS="$CXXFLAGS /EHsc /DWIN32 /MLd"
		    export CC CXX CXXFLAGS
	    AC_MSG_RESULT([working])

	    wcl_ac_check_link=`type link`
	    AC_MSG_CHECKING([whether Cygwin's /usr/bin/link shadows MSVC link.exe])

	    case x"$wcl_ac_check_link" in
	    x"link is /usr/bin/link"* )
		    wcl_ac_link="`which cl.exe`"
		    wcl_ac_link=`dirname "$wcl_ac_link"`/link.exe
		    #AC_MSG_RESULT([yes, using $wcl_ac_link instead])
		    AC_MSG_RESULT([yes])
		    AC_MSG_ERROR([This will break the build, please rename /usr/bin/link.exe to something else then try again])
		    #LINK=$wcl_ac_link
		    #export LINK	
		    ;;
	    * )
		    AC_MSG_RESULT(no)
		    ;;
	    esac
	else
	    case $host in
	    *-cygwin)
		AC_MSG_RESULT([not working])
		AC_MSG_ERROR([Compiler indicated it host as cygwin (MSVC won't do that)])
		;;
	    *)
		AC_MSG_RESULT([not working (as expected)])
		AC_MSG_ERROR([This aint windows, MSVC only works on Windows!])
		;;
	    esac
	fi


	RANLIB=:
	export RANLIB

  fi
])
