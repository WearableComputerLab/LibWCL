# $Id$
#
# This is the main Makefile defined for libwcl
# it indicates what is built, what requires what, etc
#

#
# Environment
#
INCLUDES= -I@top_srcdir@/src/
libwcl_a_includedir=$(includedir)/wcl

#
# What to build
#

if BUILD_WITH_MSVC
lib_LIBRARIES=wcl.lib
else

lib_LIBRARIES=libwcl.a

endif

#
# Predefines, needed for the conditionals below
#
wcl_lib_SOURCES=
libwcl_a_SOURCES=
#nobase_wcl_lib_include_HEADERS=
nobase_libwcl_a_include_HEADERS=

LIBFLAGS=ws2_32.lib


# 
# Networking Support
# 
network_headers=
network_sources=

#if ENABLE_NETWORK
network_headers+=\
              network/Socket.h\
              network/TCPServer.h\
              network/TCPSocket.h \
              network/UDPPacket.h\
              network/UDPServer.h\
              network/UDPSocket.h

network_sources+=\
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

#nobase_wcl_lib_include_HEADERS+=$(network_headers)
wcl_lib_SOURCES+=$(network_sources)
nobase_libwcl_a_include_HEADERS+=$(network_headers)
libwcl_a_SOURCES+=$(network_sources)

#endif


# **************************************************************************
# Custom rule for linking a Visual C++ (MS Windows) library.

if BUILD_WITH_MSVC
wcl.lib: Makefile \
  $(wcl_lib_OBJECTS) \
  $(wcl_lib_DEPENDENCIES)
	@basename="wcl"; \
	rm -f "$$basename.dll" "$$basename.lib"; \
	echo "Linking $$basename.lib..."; \
	echo $(CXX) -lib wcl.lib $(LDFLAGS) $(LIBFLAGS) \
	  $(wcl_lib_OBJECTS) \
	  $(wcl_lib_LIBADD) $(LIBS); \
	$(CXX) -lib /OUT:wcl.lib $(LDFLAGS) $(LIBFLAGS)\
	  $(wcl_lib_OBJECTS) \
	  $(wcl_lib_LIBADD) $(LIBS); 


wcl.dll: Makefile \
  $(wcl_lib_OBJECTS) \
  $(wcl_lib_DEPENDENCIES)
	@basename="wcl"; \
	rm -f "$$basename.dll" "$$basename.lib"; \
	echo "Linking $$basename.lib..."; \
	echo $(CXX) -dll wcl.dll $(LDFLAGS) $(LIBFLAGS) \
	  $(wcl_lib_OBJECTS) \
	  $(wcl_lib_LIBADD) $(LIBS); \
	$(CXX) -dll /OUT:wcl.dll $(LDFLAGS) $(LIBFLAGS)\
	  $(wcl_lib_OBJECTS) \
	  $(wcl_lib_LIBADD) $(LIBS); 

install-exec-local: wcl.lib wcl.dll
	@basename="wcl"; \
	for ext in dll pdb; do \
	  file="$$basename.$$ext"; \
	  if test -f "$$file"; then \
	    $(mkinstalldirs) "$(DESTDIR)$(bindir)"; \
	    echo " $(INSTALL_PROGRAM) $$file $(DESTDIR)$(bindir)/$$file"; \
	    $(INSTALL_PROGRAM) "$$file" "$(DESTDIR)$(bindir)/$$file" || exit 1; \
	  fi \
	done
endif
