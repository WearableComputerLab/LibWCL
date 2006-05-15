# $ClearChain: projects/clearchain/src/Makefile.am,v 1.4 2006/01/25 04:52:37 benjsc Exp $
#
OPTIONS=

if ENABLE_NETWORK
    OPTIONS+=network
endif 

SUBDIRS= $(OPTIONS) .

INCLUDES= -I@top_srcdir@/src/

lib_LIBRARIES=libwcl.a
#inst_LIBRARIES=libwcl.a

libwcl_a_SOURCES=
libwcl_a_LIBADD= network/libwcl_network.a
