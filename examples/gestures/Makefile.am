AM_LDFLAGS=@top_srcdir@/src/wcl/libwcl.la @PKGCONFIG_OTHERLIBS@ @EXAMPLE_LIBS@
AM_CXXFLAGS=@PKGCONFIG_OTHERINCLUDES@ -I@top_srcdir@/src/ @EXAMPLE_INCLUDES@

noinst_PROGRAMS=gestures
gestures_SOURCES=Canvas.cpp\
		 GestureIO.cpp\
		 main.cpp\
		 moc_Canvas.cpp\
		 moc_GestureIO.cpp

moc_Canvas.cpp: Canvas.h
	moc-qt4 $< -o $@

moc_GestureIO.cpp: GestureIO.h
	moc-qt4 $< -o $@

clean-local:
	rm -f moc_Canvas.cpp moc_GestureIO.cpp
