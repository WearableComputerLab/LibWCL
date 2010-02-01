LDFLAGS=	`pkg-config --libs libwcl`
CXXFLAGS=       -I../../src -Wall -g3


all:		clean artoolkitplustest

clean:		
			rm -f *.o
artoolkitplustest:	main.o
			$(CXX) main.o -o artoolktiplustest $(LDFLAGS) 

main.o:		main.cpp
			$(CXX) $(CXXFLAGS) -c main.cpp
