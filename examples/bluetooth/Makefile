# OS independent cpp flags
CXX			= g++ -Wall -g3 -I../../.. -L../../ `pkg-config --cflags --libs libwcl`


SOURCES	= main.cpp
OFILES = $(SOURCES:.cpp=.o)

ALL_OFILES = $(OFILES) 

TARGETS = bluetooth_test

all:  $(OFILES) $(TARGETS)

bluetooth_test: main.o
	$(CXX) -o bluetooth_test main.o


clean:
	rm -f $(TARGETS) *~ *.o
