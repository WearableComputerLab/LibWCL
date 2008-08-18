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
libwcl_win32_a_includedir=$(includedir)/wcl

#
# Predefines, needed for the conditionals below
#
LIBFLAGS=ws2_32.lib
CLEANFILES=

#
# Common Headers
# 
common_headers=\
	      debug.h\
		  CircularQueue\
		  Singleton
    

# 
# Networking Support
# 
network_headers=
network_sources=

if ENABLE_NETWORK
network_headers+=\
              network/Socket.h\
              network/TCPServer.h\
              network/TCPSocket.h \
              network/UDPPacket.h\
              network/UDPServer.h\
              network/UDPSocket.h

network_sources+=\
              network/Socket.cpp\
              network/TCPServer.cpp\
              network/TCPSocket.cpp\
              network/UDPServer.cpp\
              network/UDPSocket.cpp
endif


#
# Gesture Recognition
#
gestures_headers=
gestures_sources=
if ENABLE_GESTURES
gestures_headers+=\
			gestures/Gestures.h\
			gestures/Point.h

gestures_sources+=\
			gestures/Gestures.cpp\
			gestures/Point.cpp

endif


#
# Vicon MoCap
#
vicon_headers=
vicon_sources=

if ENABLE_VICON
vicon_headers+=\
			vicon/TrackedObject.h\
			vicon/ViconClient.h

vicon_sources+=\
			vicon/TrackedObject.cpp\
			vicon/ViconClient.cpp

endif


#
# 1394 Camera Support
#
video_1394_headers=
video_1394_sources=

if ENABLE_VIDEO_1394
video_1394_headers+=1394cam/IEEE1394Cam.h\
		    1394cam/AbstractIEEE1394Cam.h

video_1394_sources+=1394cam/AbstractIEEE1394Cam.cpp\
		    1394cam/IEEE1394Cam.cpp\
		    1394cam/debug.cpp
#
# Platform Specific Sources
#
if PLATFORM_OSX
video_1394_sources+=1394cam/osx/Camera.cpp\
		    1394cam/osx/Control.cpp\
		    1394cam/osx/FWController.cpp\
		    1394cam/osx/OSXIEEE1394Cam.cpp

endif

if PLATFORM_LINUX
video_1394_sources+=1394cam/linux/LinuxIEEE1394Cam.cpp\
		    1394cam/linux/firewire.cpp\
		    1394cam/linux/conversions.cpp

endif


endif

#
# What to build and what to install
#
if !ENABLE_MSVC
lib_LIBRARIES=libwcl.a
nobase_libwcl_a_include_HEADERS=$(common_headers) $(network_headers) $(video_1394_headers) $(gestures_headers) $(vicon_headers)
libwcl_a_SOURCES=$(network_sources) $(video_1394_sources) $(gestures_sources) $(vicon_sources)

else
lib_LIBRARIES=libwcl_win32.a
nobase_libwcl_win32_a_include_HEADERS=$(common_headers) $(network_headers) $(video_1394_headers) $(gestures_headers) $(vicon_headers)
libwcl_win32_a_SOURCES=$(network_sources) $(video_1394_sources) $(gestures_headers) $(vicon_sources)
noinst_DATA=wcl.lib
CLEANFILES+=wcl.lib

#	
# Custom rule for linking a Visual C++ (MS Windows) library.
#
wcl.lib: libwcl_win32_a
    copy libwcl_win32.a wcl.lib

libwcl_win32.a: Makefile\
  $(libwcl_win32_a_OBJECTS) \
  $(libwcl_win32_a_DEPENDENCIES)
	@basename="wcl"; \
	rm -f "$$basename.dll" "$$basename.lib"; \
	echo "Linking $$basename.lib..."; \
	echo $(CXX) -LIB /OUT:libwcl_win32.a $(LDFLAGS) $(LIBFLAGS) \
	  $(libwcl_win32_a_OBJECTS) \
	  $(libwcl_win32_a_LIBADD) $(LIBS); \
	$(CXX) -LIB /OUT:libwcl_win32.a $(LDFLAGS) $(LIBFLAGS)\
	  $(libwcl_win32_a_OBJECTS) \
	  $(libwcl_win32_a_LIBADD) $(LIBS); 

install-exec-local: wcl.lib
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
