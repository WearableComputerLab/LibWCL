# $Id$
#
# This is the main Makefile defined for libwcl
# it indicates what is built, what requires what, etc
#

#
# Environment
#
INCLUDES= -I@top_srcdir@/src/
libwcl_la_includedir=$(includedir)/wcl
AM_CPPFLAGS=
AM_LDFLAGS=
AM_YFLAGS=-d

#
# Predefines, needed for the conditionals below
#
CLEANFILES=
BUILT_SOURCES=

#
# Common Headers
# 
common_headers=\
	      debug.h\
		  CircularQueue\
		  Singleton

parser_headers=
parser_sources=
    

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
			gestures/Gestures.h

gestures_sources+=\
			gestures/Gestures.cpp

endif

#
# Geometry stuff 
#
geometry_headers=
geometry_sources=
if ENABLE_GEOMETRY
geometry_headers+=\
			geometry/BoundingBox.h \
			geometry/Face.h \
			geometry/GeometryLoader.h \
			geometry/Line.h \
			geometry/LineSegment.h \
			geometry/Plane.h \
			geometry/PolygonObject.h \
			geometry/Vertex.h

geometry_sources+=\
			geometry/BoundingBox.cpp \
			geometry/Face.cpp \
			geometry/GeometryLoader.cpp \
			geometry/Line.cpp \
			geometry/LineSegment.cpp \
			geometry/Plane.cpp \
			geometry/PolygonObject.cpp \
			geometry/Vertex.cpp

endif

#
# Parsers
#
if ENABLE_PARSERS
parser_sources+=	parser/OBJFormatGrammar.yy \
			parser/OBJFormatScanner.ll \
			parser/OBJParser.cpp \
			parser/Parser.cpp

parser_headers+=	parser/OBJParser.h \
			parser/Parser.h

BUILT_SOURCES+=		OBJFormatGrammar.cc \
		       	OBJFormatGrammar.h \
			OBJFormatScanner.cc
CLEANFILES+=OBJFormatGrammar.h\
	   OBJFormatGrammar.cc\
	   OBJFormatScanner.cc
endif

#
# Vicon MoCap
#
vicon_headers=
vicon_sources=

if ENABLE_VICON
vicon_headers+=\
			tracking/Polhemus.h \
			tracking/PolhemusTrackedObject.h \
			tracking/TrackedObject.h \
			tracking/Tracker.h \
			tracking/ViconTrackedObject.h \
			tracking/ViconClient.h \
			tracking/VirtualTrackedObject.h \
			tracking/VirtualTracker.h

vicon_sources+=\
			tracking/Polhemus.cpp \
			tracking/PolhemusTrackedObject.cpp \
			tracking/ViconTrackedObject.cpp \
			tracking/ViconClient.cpp \
			tracking/VirtualTrackedObject.cpp \
			tracking/VirtualTracker.cpp

endif

#
# 1394 Camera Support
#
video_1394_headers=
video_1394_sources=

if ENABLE_VIDEO_1394
video_1394_headers+=1394cam/DC1394.h

video_1394_sources+=1394cam/DC1394.cpp
endif

#
# UVC Camera Support
#
video_uvc_headers=
video_uvc_sources=

if ENABLE_VIDEO_UVC
video_uvc_headers+=uvccam/UVCCamera.h

video_uvc_sources+=uvccam/UVCCamera.cpp
endif

#
# Math Libraries
# 
maths_headers= \
		  maths/Matrix.h\
		  maths/Quaternion.h\
	      maths/SMatrix.h\
	      maths/Vector.h

maths_sources=maths/Matrix.cpp\
	      maths/Quaternion.cpp\
	      maths/SMatrix.cpp\
	      maths/Vector.cpp


rawports_headers=
rawports_sources=
if ENABLE_RAWPORTS
#
# Raw ports
#
rawports_headers+=parallel/Parallel.h\
		 serial/Serial.h

rawports_sources+=parallel/Parallel.cpp\
		 serial/Serial.cpp
endif

x11_headers=
x11_sources=

if ENABLE_X11
#
# X11 Specific 
#
x11_headers+=display/XDisplay.h
x11_sources+=display/XDisplay.cpp
AM_CPPFLAGS+=@X11_CFLAGS@
AM_LDFLAGS+=@X11_LIBS@
endif



#
# What to build and what to install
#
lib_LTLIBRARIES=libwcl.la
nobase_libwcl_la_include_HEADERS=$(common_headers)\
			        $(network_headers)\
				$(video_1394_headers)\
				$(video_uvc_headers)\
				$(gestures_headers)\
				$(vicon_headers)\
				$(maths_headers)\
				$(osg_headers)\
				$(geometry_headers)\
				$(rawports_headers)\
				$(x11_headers)\
				$(parser_headers)


libwcl_la_SOURCES=$(network_sources)\
		 $(video_1394_sources)\
		 $(video_uvc_sources)\
		 $(gestures_sources)\
		 $(vicon_sources)\
		 $(maths_sources)\
		 $(osg_sources)\
		 $(geometry_sources)\
		 $(rawports_sources)\
		 $(x11_sources)\
		 $(parser_sources)

