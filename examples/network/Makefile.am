AM_LDFLAGS=@top_srcdir@/src/wcl/libwcl.la @PKGCONFIG_OTHERLIBS@ @EXAMPLE_LIBS@
AM_CXXFLAGS=@PKGCONFIG_OTHERINCLUDES@ -I@top_srcdir@/src/ @EXAMPLE_INCLUDES@

noinst_PROGRAMS=tcpserver\
	     tcpclient\
	     udpserver\
	     udpclient
tcpserver_SOURCES=tcpserver.cpp
tcpclient_SOURCES=tcpclient.cpp
udpserver_SOURCES=udpserver.cpp
udpclient_SOURCES=udpclient.cpp
