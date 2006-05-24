# $Id$
#
# This is the main Makefile defined for libwcl
# it indicates what is built, what requires what, etc
#

# Environment
INCLUDES= -I@top_srcdir@/src/
libwcl_a_includedir=$(includedir)/wcl

# What to build
lib_LIBRARIES=libwcl.a

#  Predefines
#AM_CXXFLAGS=
#AM_LDFLAGS=
nobase_libwcl_a_include_HEADERS=
libwcl_a_SOURCES=

# 
# Networking Support
# 
if ENABLE_NETWORK
nobase_libwcl_a_include_HEADERS+=\
              network/Socket.h\
              network/TCPServer.h\
              network/TCPSocket.h \
              network/UDPPacket.h\
              network/UDPServer.h\
              network/UDPSocket.h

libwcl_a_SOURCES+=\
              network/Socket.h\
              network/Socket.cpp\
              network/Network.h\
              network/TCPServer.cpp\
              network/TCPServer.h\
              network/TCPSocket.cpp\
              network/TCPSocket.h\
              network/UDPPacket.h\
              network/UDPServer.cpp\
              network/UDPServer.h\
              network/UDPSocket.cpp\
              network/UDPSocket.h
endif
