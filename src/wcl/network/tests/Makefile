
# OS independent cpp flags
CXX			= g++ -Wall -g -O2

SOURCES	= serial.cpp tcpclient.cpp
OFILES = $(SOURCES:.cpp=.o)

ALL_OFILES = $(OFILES) 

BUILD_ALL += Driver #name of exe

all: $(BUILD_ALL)

Driver: $(OFILES)
	$(CXX) -o Driver $(ALL_OFILES) -lwcl

# rule that calulates the prerequisites for any .cpp file
%.d: %.cpp
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*, \1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(SOURCES:.cpp=.d) 

clean:
	rm -f Driver *~ *.o

clean_all:
	make clean


