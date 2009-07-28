LDFLAGS=	-lwcl




all:		clean vicontest

clean:		
			rm -f *.o

vicontest:	main.o
			g++ main.o -o patriottest $(LDFLAGS) 

main.o:		main.cpp
			g++ -c main.cpp
