# $Id$
#

# 
# Indicate this is not a GNU project and that 
# missing files should be automatically created
AUTOMAKE_OPTIONS=foreign subdir-objects

# Instruct aclocal to check the cfg directory for
# extra macros
ACLOCAL_AMFLAGS=-I cfg

LDFLAGS=@PKGCONFIG_OTHERLIBS@
CXXFLAGS=@PKGCONFIG_OTHERINCLUDES@

#
# Indicate which directories to process
#
SUBDIRS=src examples tests tests/gtest

#
# Package config information
#
pkgconfigdir = @PKGCONFIG_LIBDIR@/
pkgconfig_DATA=libwcl.pc

EXTRA_DIST=libwcl.pc
