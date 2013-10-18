SUBDIRS = gtest
AM_CPPFLAGS = -I$(SRC) $(GTEST_CPPFLAGS)
AM_CXXFLAGS = $(GTEST_CXXFLAGS)

# TESTS -- Programs run automatically by "make check"
# check_PROGRAMS -- Programs built by "make check" but not necessarily run
TESTS = 
check_PROGRAMS = 

TESTS += func_test
check_PROGRAMS += func_test
func_test_SOURCES = Geometry.cpp $(SRC)
func_test_CPPFLAGS = -I gtest/include
func_test_LDADD = ../../wcl/.libs/libwcl.la gtest/lib/libgtest_main.la \
		  gtest/lib/libgtest.la \
		  $(GTEST_LIBS)
