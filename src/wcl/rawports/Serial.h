/*-
 * Copyright (c) 2008 Benjamin Close <Benjamin.Close@clearchain.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef WCL_SERIAL_H
#define WCL_SERIAL_H

#include <sys/types.h>
#include <termios.h>
#include <string>
#include <wcl/api.h>

#include "IODevice.h"

namespace wcl {

/**
 * This class implements RS232 processing under unix using POSIX standards,
 * with support for a few non compliant posix features. If you need strict POSIX
 * compliance, define _POSIX_SOURCE_
 *
 * Note: http://www.easysw.com/~mike/serial/serial.html is a great reference for
 * RS232 information. http://digilander.libero.it/robang/rubrica/serial.htm has
 * details about the meanings of the different termio options
 *
 * At the time of writing there was 3 different ways to do serial these are:
 * o Termios
 * o Termio
 * o pre termio (really adhoc)
 *
 * This class implements RS232 support via termios
 *
 * RS232 Has the following Signals/Lines
 *	o GND - Logic Ground
 *	    Needed for operation ( A referance voltage for other signals)
 *	o TXD - Transmitted data
 *          Transmit data from PC->Device, either 1 (mark) or 0 (space)
 *	o RXD - Received data
 *	    Received data from Device->PC, 1(mark), 0 (space)
 *	o DCD - Data Carrier Detect
 *	    Indicates the device connected is online, not always used
 *	o DTR - Data Terminal Ready
 *          Indicates the computer is ready to send to the device, optional
 *	o CTS - Clear To Send
 *	    Received from the device, indicates the device is ready to receive
 *	    data from the PC. (optional)
 *	o RTS - Request To Send
 *	    Signal line indicating if the device is allowed to send to the pc.
 *	    (optional)
 *	o RING - Incomming Call
 *	    Singal line indicating there is an incomming connection (used by
 *	    modems)
 *
 *  CTS/RTS are used for hardware flow control and can be ignored. Most things
 *  however make use of them. So the minium a cable needs is GND,TXD,RXD. However 
 *  the normal to include flow control: GND, TXD, RXD, CTS, RTS. Modems normally
 *  also include the RING Signal line.
 *
 */
class WCL_API Serial : public IODevice
{
public:
    enum StopBits { ONE=0,
		    TWO=CSTOPB };

    enum Parity   { NONE = 0,	 //Enabling Parity (E,O,S) will enable the parity
		    ODD  = 1,	 //checking & parity bit stripping
		    EVEN = 2,	 //use extra params to setParity to
		    SPACE= 3  }; //disable checking/stripping

    enum DataBits { DB_FIVE  = CS5,
		    DB_SIX   = CS6,
		    DB_SEVEN = CS7,
		    DB_EIGHT = CS8 };	// 8 is the norm

    enum BaudRate { BAUD_0     = B0,  // Drops DTR line
		    BAUD_300   = B300,
		    BAUD_1200  = B1200,
		    BAUD_2400  = B2400,
		    BAUD_4800  = B4800,
		    BAUD_9600  = B9600,
#ifdef B14400
                    BAUD_14400 = B14400, // Not all O/S support 14400, so only use it if supported
#endif
		    BAUD_19200 = B19200,
		    BAUD_38400 = B38400,
		    BAUD_57600 = B57600,
#ifndef _POSIX_SOURCE_
		    // Note these speeds are not POSIX compliant
		    // Speeds above 115200 are not supported by standard PC
		    // hardware
		    BAUD_115200 = B115200,
		    BAUD_230400 = B230400,
		    BAUD_460800 = B460800,
		    BAUD_921600 = B921600
#endif
		    };

    enum FlowControl {
		    DISABLED = 0,
		    RTSCTS   = 1,	// Hardware (RTS/CTS lines)
		    XONXOFF  = 2,	// Software
                    RTSCTS_XONXOFF= 3    // HW & Software
		     };

    enum InputMode { RAW,		// Character mode
		     LINE };		// Line mode


    // The lines/Signal listed below can be used with the
    // setLine and getSignal methods to individually get
    // the state of a signal. The enum numbers relate to the pin of a 9 pin DSub
    // serial connector. The comments below indicate the equivilant 25 pin DSUB
    enum Line	   {
		    TXD = 3, // OUT Signal (Transmit data) D25 pin 2
		    DTR = 4, // OUT Signal (Data Terminal Ready) D25 pin 20
		  //GND = 5  // Ground, Not Usable D25 pin 7
		    RTS = 7, // OUT Signal (Request To Send) D25 pin 4
		   };

    enum Signal {
		    DCD = 1,  // IN  Signal (Data Carrier Detect) D25 pin 8
		    //RXD = 2,  // IN  Signal (Received data) D25 pin 3
		               //  This pin logic state can't be detected by most pc
			       //  hardware as the uart doesn't provide gpio
			       //  access to the line.
		    DSR = 4,  // IN  Signal (Data Send Ready) D25 pin 6
		    CTS = 8,  // IN  Signal (Clear To Send) D25 pin 5
		    RI  = 16, // IN  Signal (Ring Indicator) D25 pin 22 };
    };


    Serial();
    virtual ~Serial();

    bool open( const char *device,
	       const BaudRate,
	       const DataBits = DB_EIGHT ,
	       const Parity = NONE,
	       const StopBits = ONE,
	       const InputMode = RAW,
	       const FlowControl = DISABLED,
	       const BlockingMode = BLOCKING,
               const Flush = INPUT,
	       const Signal= (const Signal)0 );  // Set this to DCD to NOT ignore the DCD signal

    virtual bool flush( const Flush = BOTH  );	// Clear fifos, don't write it
    bool drain();			// Wait for data to be written

    bool close(bool = true);	// close & optionall restores preopen state

    virtual ssize_t read ( void *buffer, size_t size );
    virtual ssize_t write( const void *buffer, size_t size );
    virtual ssize_t write( const std::string & );
	virtual void writeUntil( void *buffer, const size_t size);
	virtual void readUntil(void *buffer, size_t size);
    virtual ssize_t getAvailableCount();

    virtual bool isValid() const;

    BaudRate getBaudRate() const;
    BlockingMode getBlockingMode() const;
    StopBits getStopBits() const;
    Parity getParity() const;
    DataBits getDataBits() const;
    FlowControl getFlowControl() const;
    InputMode getInputMode() const;

    // When calling theses, check the return values. If
    // a call is not valid on the given port it will fail and return
    // false.
    virtual bool setBlockingMode( const BlockingMode );
    bool setStopBits( const StopBits );
    bool setParity( const Parity, const bool strip=true, const bool check=true, const bool mark=false );
    bool setDataBits( const DataBits );
    bool setBaudRate( const BaudRate );
    bool setFlowControl( const FlowControl );
    bool setInputMode( const InputMode );

    // See Parity enum above for details about these
    bool getParityCheck() const;
    bool getParityMark() const;
    bool getParityStrip() const;

    // Set / Get the state of a particular signal
    bool setLine ( const Line, bool );
    bool getSignal ( const Signal );
    Signal getSignals();

    // Attempt to determine the baud rate automatically
    bool scanBaudRate();

    // Current baud rate scanner has fixed character/baudrate set
    bool scanBaudRate( const char = '\n', const BaudRate = (const BaudRate)(
                       BAUD_1200
                       | BAUD_2400
                       | BAUD_4800
                       | BAUD_9600
#ifdef B14400
                       | BAUD_14400
#endif
                       | BAUD_19200
                       | BAUD_38400
                       | BAUD_57600
#ifndef _POSIX_SOURCE_
                       | BAUD_115200
#endif
		       )
		     );

    // Get the actual file descriptor
    int operator *() const;

private:
    int fd;
    Parity parity;
    bool parityCheckEnabled;
    InputMode input;
    BlockingMode blocking;
    FlowControl flow;
    bool applyImmediately;

    // original state of the port
    struct termios origstate;

    /*
     * Maintains the current state of the port
     * Consists of:
     *	c_cflag - control options
     *  c_lflag - line options
     *  c_iflag - input options
     *  c_oflag - ouput options
     *  c_cc - control character
     *  c_ispeed - input baudrate
     *  c_ospeed - output baudrate
     */
    struct termios currstate;

    bool apply();
    bool applyParams( const struct termios &);

    // For now we prevent copying of the object
    // This is due to requiring locking on the port so
    // we don't get unknown states. - benjsc 20070118
    Serial(const Serial &);
    Serial operator = (const Serial &);
};


}; //namespace wcl

#endif

