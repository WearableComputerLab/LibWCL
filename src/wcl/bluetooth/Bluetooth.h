/*
 * Filename: Bluetooth.cpp
 * Author: (C) Ross Smith
 * Email: ross@r-smith.net
 * Creation Date: 09-21-2007  22:14:11
 */

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>
#include <sys/file.h>

#include <string>

class Bluetooth {
	private:
		int fd;
		bool connected;
		std::string devName;
		std::string destmac;

	protected:

	public:
		Bluetooth();
		virtual ~Bluetooth();

		/*IODevice Sigs*/
		int connect(std::string dev, bool blocking);
		bool reConnect(bool blocking);
		int close();
		int readRaw(std::string&, int bytes);
		int dataAvailable();
		int writeLine(std::string line);
		int writeChar(char* c);
		bool setBlocking(bool state);
		bool flush();
		int getFD();
		std::string getName();
		//char * scan();
};
#endif


