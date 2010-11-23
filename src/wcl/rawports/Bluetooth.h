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
#include <vector>

#include <wcl/api.h>

#include "IODevice.h"

namespace wcl
{
	struct WCL_API BluetoothDevice
	{
		std::string name;
		std::string mac;

		BluetoothDevice() {}

		BluetoothDevice(std::string n, std::string m)
			: name(n), mac(m) {}
	};

	class WCL_API Bluetooth : public IODevice {
		public:


			Bluetooth();
			virtual ~Bluetooth();

			// for IODevice
			virtual bool setBlockingMode(BlockingMode m);
			virtual bool close(bool restoreState = true);
			virtual ssize_t read ( void *buffer, size_t size );
			virtual ssize_t write(const std::string& line);
			virtual ssize_t write( const void *buffer, size_t size );
			virtual ssize_t getAvailableCount();

			// TODO: Fix isValid in Bluetooth
			virtual bool isValid() const {return true;};

			static std::vector<BluetoothDevice> scanForDevices();

			/*IODevice Sigs*/
			int connect(std::string dev, bool blocking);
			bool reConnect(bool blocking);
			int readRaw(std::string&, int bytes);
			int writeChar(char* c);
			virtual bool flush(const Flush = BOTH);
			int getFD();
			std::string getName();
			//char * scan();
			
		private:
			int fd;
			bool connected;
			std::string devName;
			std::string destmac;

	};

}
#endif


