LDFLAGS=	-lX11 -lXext -L/usr/local/lib -lwcl -lraw1394 -ldc1394 




all:		clean patriot fastrak 

clean:		
			rm -f *.o

patriot:	patriot_main.o
			g++ patriot_main.o -o patriottest $(LDFLAGS) 

patriot_main.o:		Patriot.cpp
			g++ -c Patriot.cpp -o patriot_main.o

fastrak:	fastrak_main.o
			g++ fastrak_main.o -o fastraktest $(LDFLAGS) 

fastrak_main.o:		FasTrak.cpp
			g++ -c FasTrak.cpp -o fastrak_main.o
