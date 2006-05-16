# $Id$
#
# This is the main Makefile defined for libwcl
# it indicates what is built, what requires what, etc
#

# What to build
INCLUDES= -I@top_srcdir@/src/
lib_LIBRARIES=libwcl.a

#  Predefines
AM_CXXFLAGS=
AM_LDFLAGS=
libwcl_a_SOURCES=
nobase_include_HEADERS=

# 
# Networking Support
# 
if ENABLE_NETWORK
nobase_include_HEADERS+=\
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
