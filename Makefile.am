# $Id$
#

# 
# Indicate this is not a GNU project and that 
# missing files should be automatically created
AUTOMAKE_OPTIONS=foreign

# Instruct aclocal to check the cfg directory for
# extra macros
ACLOCAL_AMFLAGS=-I cfg

#
# Indicate which directories to process
#
SUBDIRS=src examples

#
# Package config information
#
pkgconfigdir = @PKGCONFIG_LIBDIR@/
pkgconfig_DATA=libwcl.pc

EXTRA_DIST=libwcl.pc
