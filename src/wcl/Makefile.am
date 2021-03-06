# $Id$
#
# This is the main Makefile defined for libwcl
# it indicates what is built, what requires what, etc
#

#
# Environment
#
AUTOMAKE_OPTIONS=subdir-objects
libwcl_la_includedir=$(includedir)/wcl
AM_CPPFLAGS= -I@top_srcdir@/src/ -DWCL_DLL -DWCL_DLL_EXPORTS -fvisibility=hidden
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
		  util/CircularBuffer.h \
	      api.h\
	      IO.h

common_sources=Exception.cpp\
	       IO.cpp
    

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
			geometry/Intersection.h \
			geometry/BoundingBox.h \
			geometry/BoundingSphere.h\
			geometry/Face.h \
			geometry/GeometryLoader.h \
			geometry/KMeans.h \
			geometry/Line.h \
			geometry/LineSegment.h \
			geometry/OBJGeometry.h \
			geometry/Plane.h \
			geometry/PolygonObject.h \
			geometry/Vertex.h \
			geometry/Ray.h

geometry_sources+=\
			geometry/BoundingBox.cpp \
			geometry/BoundingSphere.cpp\
			geometry/Face.cpp \
			geometry/GeometryLoader.cpp \
			geometry/Line.cpp \
			geometry/LineSegment.cpp \
			geometry/OBJGeometry.cpp \
			geometry/Plane.cpp \
			geometry/PolygonObject.cpp \
			geometry/Vertex.cpp \
			geometry/Ray.cpp

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

BUILT_SOURCES +=	parser/OBJFormatGrammar.cc 
# 					parser/OBJFormatScanner.cc

if OLD_AM
BUILT_SOURCES += parser/OBJFormatGrammar.h
else 
BUILT_SOURCES += parser/OBJFormatGrammar.hh
endif

CLEANFILES+=parser/OBJFormatGrammar.h\
	   /parser/OBJFormatGrammar.cc\
	   /parser/OBJFormatScanner.cc
endif

#####################################################################
#
# Tracker support
#
#
# Vicon MoCap
#
tracking_headers=\
			tracking/TrackedObject.h \
			tracking/Tracker.h \
			tracking/DummyTracker.h\
			tracking/DummyTrackedObject.h

tracking_sources=\
			tracking/DummyTracker.cpp\
			tracking/DummyTrackedObject.cpp

if ENABLE_TRACKING_LAZYSUSAN
tracking_headers+=\
		    tracking/LazySusan.h \
		    tracking/LazySusanTrackedObject.h 

tracking_sources+=\
		    tracking/LazySusan.cpp \
		    tracking/LazySusanTrackedObject.cpp 
endif

if ENABLE_TRACKING_VICON
tracking_headers+=\
			tracking/ViconTrackedObject.h \
			tracking/ViconClient.h \
			tracking/VirtualTrackedObject.h \
			tracking/VirtualTracker.h

tracking_sources+=\
			tracking/ViconTrackedObject.cpp \
			tracking/ViconClient.cpp \
			tracking/VirtualTrackedObject.cpp \
			tracking/VirtualTracker.cpp
endif

if ENABLE_TRACKING_POLHEMUS
tracking_headers+=\
			tracking/Polhemus.h \
			tracking/PolhemusTrackedObject.h 

tracking_sources+=\
			tracking/Polhemus.cpp \
			tracking/PolhemusTrackedObject.cpp 
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

#
# PTGrey Camera Support
#
camera_ptgrey_headers=
camera_ptgrey_sources=

if ENABLE_CAMERA_PTGREY
camera_uvc_headers+=camera/PTGreyCamera.h\
		   camera/PTGreyCameraFactory.h
camera_uvc_sources+=camera/PTGreyCamera.cpp\
		   camera/PTGreyCameraFactory.cpp

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
rawports_headers+=rawports/IODevice.h

if ENABLE_SERIAL
rawports_headers+=rawports/Serial.h
rawports_sources+=rawports/Serial.cpp
endif

if ENABLE_PARALLEL
rawports_headers+=rawports/Parallel.h
rawports_sources+=rawports/Parallel.cpp
endif

if ENABLE_BLUETOOTH
rawports_headers+=rawports/Bluetooth.h
rawports_sources+=rawports/Bluetooth.cpp
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


#####################################################################
#
# Structured Light
#
structuredlight_headers=
structuredlight_sources=
if ENABLE_STRUCTUREDLIGHT
structuredlight_headers+=graphics/structuredlight/GrayCode.h
structuredlight_sources+=graphics/structuredlight/GrayCode.cpp
endif

####################################################################
#
# Remote Projector
#
projectorcontrol_headers=
projectorcontrol_sources=
if ENABLE_PROJECTORCONTROL
        projectorcontrol_headers+=projector/RemoteProjector.h \
								  projector/MatrixSwitch.h
        projectorcontrol_sources+=projector/RemoteProjector.cpp \
								  projector/MatrixSwitch.cpp
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
				$(camera_ptgrey_headers)\
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
				$(wiimote_headers) \
				$(structuredlight_headers) \
                $(projectorcontrol_headers)

libwcl_la_SOURCES=$(common_sources)\
                 $(network_sources)\
		 $(camera_sources)\
		 $(camera_1394_sources)\
		 $(camera_ptgrey_sources)\
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
		 $(wiimote_sources) \
		 $(structuredlight_sources) \
         $(projectorcontrol_sources)
