# OS independent cpp flags
CXX			= g++ -Wall -g3 -I../../.. -L../../ `pkg-config --cflags --libs libwcl`


SOURCES	= tcpclient.cpp tcpserver.cpp udpclient.cpp udpserver.cpp
OFILES = $(SOURCES:.cpp=.o)

ALL_OFILES = $(OFILES) 

TARGETS = tcpserver tcpclient udpclient udpserver

all:  $(OFILES) $(TARGETS)

tcpserver: tcpserver.cpp
	$(CXX) -o tcpserver tcpserver.o

tcpclient: tcpclient.cpp 
	$(CXX) -o tcpclient tcpclient.o

udpserver: udpserver.cpp
	$(CXX) -o udpserver udpserver.o

udpclient: udpclient.cpp
	$(CXX) -o udpclient udpclient.o

clean:
	rm -f $(TARGETS) *~ *.o
