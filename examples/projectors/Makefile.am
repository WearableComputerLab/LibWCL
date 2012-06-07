AM_LDFLAGS=@top_srcdir@/src/wcl/libwcl.la @PKGCONFIG_OTHERLIBS@ @EXAMPLE_LIBS@
AM_CXXFLAGS=@PKGCONFIG_OTHERINCLUDES@ -I@top_srcdir@/src/ @EXAMPLE_INCLUDES@ -g3 -O0

noinst_PROGRAMS=pcontrol  \
                switch
pcontrol_SOURCES=pcontrol.cpp
switch_SOURCES=MatrixSwitch.cpp
