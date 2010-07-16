/*
 * Filename: Bluetooth.cpp
 * Author: (C) Ross Smith
 * Email: ross@r-smith.net
 * Creation Date: Reused
 * Recreation: 09-21-2007  21:34:35
 *
 * Description: 
 *
 * This class is used to connect to bluetooth devices, I am curently using the
 * bluez libs to do this with RFCOMM.
 *
 */

#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


#include "Bluetooth.h"


namespace wcl
{

	Bluetooth::Bluetooth() 
		: fd(0), connected(false)
	{
	}

	Bluetooth::~Bluetooth(){
	}

	int Bluetooth::connect(std::string destmac, bool blocking){

		this->destmac = destmac;

		/*Socket Address*/
		struct sockaddr_rc addr = {0};

		/*Get the socket file descriptor*/
		fd = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

		if(fd<=0){
			std::cerr << ("Unable to create socket %s", destmac.c_str());
			return false;
		}

		/*Get the current socket flags*/
		int flags = fcntl (fd, F_GETFL, 0);
		if (flags < 0){
			std::cerr << ("Error retrieving file descriptor flags\n");
			return false;
		}

		/*If we want to use a non blocking connect*/
		if(!blocking){
			if (fcntl (fd, F_SETFL, flags | O_NONBLOCK) < 0){
				std::cerr << ("Failed to set non blocking mode\n");
				return false;
			}
		}

		/**/
		addr.rc_family = AF_BLUETOOTH;

		/*Set the channel? have seen it set to 0 in some examples*/
		addr.rc_channel = (uint8_t) 1;

		/*set the connection parameters (who to connect to)*/
		str2ba( destmac.c_str(), &addr.rc_bdaddr );

		int state = ::connect(fd, (struct sockaddr *)&addr, sizeof(addr));

		return state;
	}

	int Bluetooth::readRaw(std::string &buffer, int bytes){

		/*allocate the memory for the read call*/
		char data [bytes];
		int result = read (fd, data, bytes);

		/*Nuke anything in the buffer if no data was read*/
		if (result <= 0)
			buffer.erase ();

		/*Set the string to the data read*/
		else
			buffer.assign (data, result);

		return result;
	}



	bool Bluetooth::setBlocking(bool blocking){

		/*Get the current socket flags*/
		int flags = fcntl (fd, F_GETFL, 0);
		if (flags < 0){
			std::cerr << ("Error retrieving file descriptor flags\n");
			return false;
		}

		int state = false;

		/*Set blocking mode*/
		if(blocking)
			flags &= ~O_NONBLOCK;
		/*Set non blocking mode*/
		else
			flags |= O_NONBLOCK;

		if (fcntl (fd, F_SETFL, flags) < 0)
			state = false;

		return state;
	}

	bool Bluetooth::reConnect(bool blocking){
		close();
		return connect(destmac, blocking);
	}

	/*Lets us know how much data is waiting on the socket, also a -1 occurs if the
	 * device has been disconnected.*/
	int Bluetooth::dataAvailable(){
		int readmask = 0;

		fd_set rfds;
		struct timeval tv;
		FD_ZERO(&rfds);
		FD_SET(getFD(), &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 1;
		int nfound;
		//if ((nfound = select (getFD()+1, &rfds, 0, 0, &tv)) == -1){
		/*
		   message("Sellecting...\n");
		   if ((nfound = select (getFD(), &rfds, 0, 0, &tv)) == -1){
		   std::cout << "Select failed" << std::endl;
		   }
		   else{
		   message("NFound: %d\n", nfound);
		   */
		//message("Here...\n");
		//if (FD_ISSET(getFD(), &rfds)){
		//std::cout << "Ready for reading: " << nfound << std::endl;
		int MAX_BUF = 128;
		char buf[MAX_BUF];
		//int length = recv(getFD(), &buf, 128, 0);
		int recvFlags =
			MSG_PEEK;      /*Return data without removing it from the queue*/
		//| MSG_TRUNC;  /*Return the real length of the packet even if longer than the buffer*/
		int length = recv(getFD(), buf, MAX_BUF, 0);
		//std::cerr << "Ready for reading: " << length << std::endl;
		return length;

		/*Detect a disconnect*/
		if(length == 0){
			return 0;
		}
		else{
			return length;
		}

		//}
		//else
		//  return 0;
		//  }

	}

	int Bluetooth::close(){
		return ::close(this->fd);
	}

	int Bluetooth::writeChar(char *c){
		return write(this->fd, c, 1);
	}

	int Bluetooth::writeLine(std::string line){
		int result =  ::write(this->fd, line.c_str(), line.size());
		return (bool)result;
	}

	bool Bluetooth::flush(){

		std::string buf;
		int result = readRaw(buf, 1024*64);
		if(result > 0)
			return true;
		else
			return false;
	}

	int Bluetooth::getFD(){ 
		return this->fd; 
	}

	std::string Bluetooth::getName(){
		return this->devName;
	}


	std::vector<BluetoothDevice> Bluetooth::scanForDevices()
	{
		std::vector<BluetoothDevice> devices;

		inquiry_info *ii = NULL;
		int max_rsp, num_rsp;
		int dev_id, sock, len, flags;
		int i;
		char addr[19] = { 0 };
		char name[248] = { 0 };

		dev_id = hci_get_route(NULL);
		sock = hci_open_dev( dev_id );
		if (dev_id < 0 || sock < 0) {
			perror("opening socket");
			exit(1);
		}

		len  = 8;
		max_rsp = 255;
		flags = IREQ_CACHE_FLUSH;
		ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

		num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
		if( num_rsp < 0 ) perror("hci_inquiry");

		for (i = 0; i < num_rsp; i++) {
			ba2str(&(ii+i)->bdaddr, addr);
			memset(name, 0, sizeof(name));
			if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
						name, 0) < 0)
				strcpy(name, "[unknown]");
			printf("%s  %s\n", addr, name);
			BluetoothDevice d(name, addr);
			devices.push_back(d);
		}

		free( ii );
		::close( sock );

		return devices;
	}

} // namespace
