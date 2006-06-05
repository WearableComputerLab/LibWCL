OSTYPE := $(shell uname)

CXX = g++ -Wall -g -O2

#osx specific flags
ifeq ($(OSTYPE),Darwin)
	CXX_MACOSX = -DMACOSX

	CXX += $(CXX_MACOSX)
endif

SOURCES = AbstractIEEE1394Cam.cpp IEEE1394Cam.cpp

OFILES = $(SOURCES:.cpp=.o)

all: $(OFILES)

# rule that calulates the prerequisites for any .cpp file
%.d: %.cpp
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*, \1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(SOURCES:.cpp=.d)

clean:
	rm -rf *.o
	rm -rf *.d
