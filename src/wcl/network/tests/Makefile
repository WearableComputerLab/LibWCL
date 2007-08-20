
# OS independent cpp flags
CXX			= g++ -Wall -g -O2


SOURCES	= tcpclient.cpp tcpserver.cpp
OFILES = $(SOURCES:.cpp=.o)

ALL_OFILES = $(OFILES) 

TARGETS = tcpserver tcpclient

all:  $(OFILES) $(TARGETS)

tcpserver:
	$(CXX) -o tcpserver tcpserver.o -lwcl

tcpclient:
	$(CXX) -o tcpclient tcpclient.o -lwcl

# rule that calulates the prerequisites for any .cpp file
%.d: %.cpp
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*, \1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(SOURCES:.cpp=.d) 

clean:
	rm -f $(TARGETS) *~ *.o
