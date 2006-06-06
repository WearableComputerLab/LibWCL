# WCL_PLATFORM_SETUP
# -----------------------------------------
# Run platform specific macros 
#
AC_DEFUN([WCL_PLATFORM_SETUP],[


   case $host_os in
    darwin* ) 
	WCL_PLATFORM_MACOSX()
	;;
    linux*)
	WCL_PLATFORM_LINUX()
        ;;
    *)
        ;;
    esac

    # Define a Makefile.am conditional that we are on mac
    AM_CONDITIONAL(PLATFORM_OSX,test "x$platform_osx" = "xyes")
    AM_CONDITIONAL(PLATFORM_LINUX,test "x$platform_linux" = "xyes")
])

#
# WCL_AC_PLATFORM_MACOSX
#-----------------------------------
# Define specific features needed for MacOSX
#
AC_DEFUN([WCL_PLATFORM_MACOSX], [

# Setup platform specific compile flags
CFLAGS="$CFLAGS -DMACOSX"
CXXFLAGS="$CXXFLAGS -DMACOSX"
export CFLAGS CXXFLAGS

# Setup Objective C Compiler
_AM_DEPENDENCIES(OBJC)

OBJC="${CC}"
OBJCFLAGS="${CFLAGS}"
AC_SUBST(OBJCCOMPILER)
AC_SUBST(OBJC)
AC_SUBST(OBJCFLAGS)

# Define that which platform we are on
platform_osx=yes

])


#
# WCL_AC_PLATFORM_LINUX
#-----------------------------------
# Define specific features needed for Linux
#
AC_DEFUN([WCL_PLATFORM_LINUX], [

# Setup platform specific compile flags
CFLAGS="$CFLAGS -DLINUX"
CXXFLAGS="$CXXFLAGS -DLINUX"
export CFLAGS CXXFLAGS

# Define that which platform we are on
platform_liinux=yes

])
