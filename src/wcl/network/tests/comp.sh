g++ -c tcpclient.cpp serial.cpp
g++ tcpclient.o serial.o -o user -lwcl
