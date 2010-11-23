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
AM_CPPFLAGS=-DWCL_DLL -DWCL_DLL_EXPORTS -fvisibility=hidden
AM_LDFLAGS=
AM_YFLAGS=-d

#
# Predefines, needed for the conditionals below
#
CLEANFILES=
BUILT_SOURCES=

########################################################
#
# Common FILES
# 
common_headers=debug.h\
	      CircularQueue\
	      Singleton\
	      Exception.h\
	      api.h

common_sources=Exception.cpp
    

########################################################
# 
# Networking Support
# 
network_headers=
network_sources=

if ENABLE_NETWORK
network_headers+=\
              network/Socket.h\
              network/SocketException.h\
              network/TCPServer.h\
              network/TCPSocket.h \
              network/UDPPacket.h\
              network/UDPServer.h\
              network/UDPSocket.h

network_sources+=\
              network/Socket.cpp\
              network/SocketException.cpp\
              network/TCPServer.cpp\
              network/TCPSocket.cpp\
              network/UDPServer.cpp\
              network/UDPSocket.cpp
endif


######################################################
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

######################################################
#
# Wiimote Stuff
#
wiimote_headers=
wiimote_sources=

if ENABLE_WIIMOTE
wiimote_headers+=\
				 wiimote/Wiimote.h \
				 wiimote/WiimoteException.h

wiimote_sources+=\
				 wiimote/Wiimote.cpp \
				 wiimote/WiimoteException.cpp

endif

########################################################
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
			geometry/OBJGeometry.h \
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

#########################################################
#
# Parsers
#
parser_headers=
parser_sources=

if ENABLE_PARSERS
parser_sources+=	parser/OBJFormatGrammar.yy \
			parser/OBJFormatScanner.ll \
			parser/OBJParser.cpp \
			parser/Parser.cpp \
			parser/ParserException.cpp

parser_headers+=	parser/OBJParser.h \
			parser/Parser.h \
			parser/ParserException.h

BUILT_SOURCES+=		OBJFormatGrammar.cc \
		       	OBJFormatGrammar.h \
			OBJFormatScanner.cc
CLEANFILES+=OBJFormatGrammar.h\
	   OBJFormatGrammar.cc\
	   OBJFormatScanner.cc
endif

#####################################################################
#
# Tracker support
#
#
# Vicon MoCap
#
tracking_headers=
tracking_sources=

if ENABLE_VICON
tracking_headers+=\
		    tracking/LazySusan.h \
		    tracking/LazySusanTrackedObject.h \
			tracking/Polhemus.h \
			tracking/PolhemusTrackedObject.h \
			tracking/TrackedObject.h \
			tracking/Tracker.h \
			tracking/ViconTrackedObject.h \
			tracking/ViconClient.h \
			tracking/VirtualTrackedObject.h \
			tracking/VirtualTracker.h

tracking_sources+=\
		    tracking/LazySusan.cpp \
		    tracking/LazySusanTrackedObject.cpp \
			tracking/Polhemus.cpp \
			tracking/PolhemusTrackedObject.cpp \
			tracking/ViconTrackedObject.cpp \
			tracking/ViconClient.cpp \
			tracking/VirtualTrackedObject.cpp \
			tracking/VirtualTracker.cpp

endif

if ENABLE_TRACKING_ARTOOLKITPLUS
tracking_headers+=tracking/ARToolKitPlusTracker.h\
		  tracking/ARToolKitPlusTrackedObject.h

tracking_sources+=tracking/ARToolKitPlusTracker.cpp\
		  tracking/ARToolKitPlusTrackedObject.cpp
endif


##################################################################
# 
# Video Support
# 
# 
video_headers=
video_sources=
if ENABLE_VIDEO
video_headers+=video/VideoDecoder.h
video_sources+=video/VideoDecoder.cpp
endif

##################################################################
# 
# Camera Support
# 
# 

#
# Common camera
# 
camera_headers=
camera_sources=

if ENABLE_CAMERA
camera_headers+=camera/Camera.h\
		camera/CameraException.h\
	       camera/CameraFactory.h

camera_sources+=camera/Camera.cpp\
		camera/CameraException.cpp\
	       camera/CameraFactory.cpp
endif

#
# 1394 Camera Support
#
camera_1394_headers=
camera_1394_sources=

if ENABLE_CAMERA_1394
camera_1394_headers+=camera/DC1394Camera.h\
		    camera/DC1394CameraFactory.h
camera_1394_sources+=camera/DC1394Camera.cpp\
		    camera/DC1394CameraFactory.cpp
endif

#
# UVC Camera Support
#
camera_uvc_headers=
camera_uvc_sources=

if ENABLE_CAMERA_UVC
camera_uvc_headers+=camera/UVCCamera.h\
		   camera/UVCCameraFactory.h
camera_uvc_sources+=camera/UVCCamera.cpp\
		   camera/UVCCameraFactory.cpp

endif

#
# Virtual Camera Support
# 
camera_virtualcamera_headers=
camera_virtualcamera_sources=

if ENABLE_CAMERA_VIRTUAL
camera_virtualcamera_headers+=camera/VirtualCamera.h
camera_virtualcamera_sources+=camera/VirtualCamera.cpp
endif

#######################################################################
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


#######################################################################
#
# Raw ports
#
rawports_headers=
rawports_sources=
if ENABLE_RAWPORTS
rawports_headers+=rawports/Bluetooth.h\
				  rawports/IODevice.h\
                  rawports/Parallel.h\
		          rawports/Serial.h

rawports_sources+=rawports/Bluetooth.cpp\
                  rawports/Parallel.cpp\
		          rawports/Serial.cpp
endif




#####################################################################
#
# X11 Specific 
#
x11_headers=
x11_sources=
if ENABLE_X11
x11_headers+=display/XDisplay.h
x11_sources+=display/XDisplay.cpp
AM_CPPFLAGS+=@X11_CFLAGS@
AM_LDFLAGS+=@X11_LIBS@
endif



####################################################################
#
# What to build and what to install
#
lib_LTLIBRARIES=libwcl.la
nobase_libwcl_la_include_HEADERS=$(common_headers)\
			        $(network_headers)\
				$(camera_headers)\
				$(camera_1394_headers)\
				$(camera_uvc_headers)\
				$(camera_virtualcamera_headers)\
				$(gestures_headers)\
				$(tracking_headers)\
				$(maths_headers)\
				$(osg_headers)\
				$(geometry_headers)\
				$(rawports_headers)\
				$(x11_headers)\
				$(parser_headers)\
				$(video_headers) \
				$(wiimote_headers) 


libwcl_la_SOURCES=$(common_sources)\
                 $(network_sources)\
		 $(camera_sources)\
		 $(camera_1394_sources)\
		 $(camera_uvc_sources)\
		 $(camera_virtualcamera_sources)\
		 $(gestures_sources)\
		 $(tracking_sources)\
		 $(maths_sources)\
		 $(osg_sources)\
		 $(geometry_sources)\
		 $(rawports_sources)\
		 $(x11_sources)\
		 $(parser_sources)\
		 $(video_sources) \
		 $(wiimote_sources) 

