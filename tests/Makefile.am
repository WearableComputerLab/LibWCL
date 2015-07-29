# TESTS -- Programs run automatically by "make check"
# check_PROGRAMS -- Programs built by "make check" but not necessarily run


TESTS = func_test
check_PROGRAMS = func_test

func_test_SOURCES =  BoundingBox.cpp \
					 Line.cpp \
					 OBJMaterialParser.cpp \
					 OBJParser.cpp \
					 Ray.cpp 

func_test_CPPFLAGS = -I gtest/include -I ../src/

func_test_LDFLAGS = -Lgtest/lib -lgtest -lgtest_main -lpthread 

func_test_LDADD =  \
				  ../src/wcl/.libs/libwcl.la
