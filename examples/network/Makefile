# OS independent cpp flags
CXX			= g++ -Wall -g3 -I../../.. -L../../


SOURCES	= tcpclient.cpp tcpserver.cpp udpclient.cpp udpserver.cpp
OFILES = $(SOURCES:.cpp=.o)

ALL_OFILES = $(OFILES) 

TARGETS = tcpserver tcpclient udpclient udpserver

all:  $(OFILES) $(TARGETS)

tcpserver: tcpserver.cpp
	$(CXX) -o tcpserver tcpserver.o -lwcl

tcpclient: tcpclient.cpp 
	$(CXX) -o tcpclient tcpclient.o -lwcl

udpserver: udpserver.cpp
	$(CXX) -o udpserver udpserver.o -lwcl

udpclient: udpclient.cpp
	$(CXX) -o udpclient udpclient.o -lwcl

clean:
	rm -f $(TARGETS) *~ *.o
