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

#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <wcl/serial/Serial.h>

using namespace wcl;


Serial::Serial():
    fd(-1),parity(NONE),blocking(BLOCKING),applyImmediately(true)
{
    memset( &this->currstate, 0x0, sizeof( struct termios ));
}

/**
 * Upon destruction of the Serial object, if the port is still open
 * it will be closed and the original parameters of the port restored.
 * If you want to leave the port unrestored, call: close(false) prior
 * to destroying the object
 */
Serial::~Serial()
{
    if( this->isValid()){
	this->close(true);
    }
}

/**
 * Indicate if the serial object has opened the given serial port
 *
 * @return true If the serial port is open, false if it's not
 */
bool Serial::isValid() const
{
    if( this->fd <= -1 ){
	return false;
    }

    return true;
}

/**
 * Indicate if the port is in Blocking or non blocking mode
 *
 * @returns if the port is in blocking or non blocking mode.
 */
Serial::BlockingMode Serial::getBlockingMode() const
{
    return this->blocking;
}

/**
 * Set the blocking mode for the serial port. If the serial port is opened
 * an attempt will be made to change the current mode of the conneciton. If
 * the port is not open, this call will remember the setting for when the
 * port is opened.
 *
 * @param mode The mode to use on this serial port
 * @return true if the call was successful, false if the mode could not be set
 */
bool Serial::setBlockingMode( const BlockingMode mode )
{
    int flags;

    if ( this->isValid())

        // In order to set the blocking flag, we must be careful not
        // to stop on any other flags that have been set. Hence
        // we obtain all the flags and adjust the blocking flag only
        flags = ::fcntl(this->fd, F_GETFL, 0 /* ALLFLAGS */);

        switch( mode ) {
            case BLOCKING:
                flags &=~O_NONBLOCK;
                if ( ::fcntl( this->fd, F_SETFL, flags ) == 0 ){
		    this->blocking=mode;
		    if( this->inputmode == RAW ){
			// If we are in raw mode, then we must also set the minimum characters
			// we must receive before a read will return else we do not
			// block. We also disable the read timer.
			this->currstate.c_cc[VMIN]=1;
			this->currstate.c_cc[VTIME]=0;
			this->apply();
		    }
		    return true;
                }
                break;
            case NONBLOCKING:
                flags |= O_NONBLOCK;
                if ( ::fcntl( this->fd, F_SETFL, flags ) == 0 ){
		    this->blocking=mode;
                    return true;
                }
                break;
    }

    return false;
}

/**
 * Obtain the file descriptior associated with this object
 *
 * @return The current file descriptor. May be -1 if it's not valid
 */
int Serial::operator *() const
{
    return this->fd;
}

/**
 * Open the serial port based on the given parameters.
 *
 * Note: If the serial port is opened with parity set, parity checking will
 * be enabled and the parity bit will also be stripped from the data.
 * This can be disabled via the setParityCheck function. Also by default
 * the DCD signal is ignored by default. If you want to use it, pass in DCD to
 * the signal parameter.
 *
 * If the port is already opened, it will first be closed and reset to the
 * original state when it was opened. Then reopened again.
 *
 * @param device (ie: Linux: /dev/ttyS0, BSD: /dev/ttycua0)
 * @param baud The baud rate to open the port at
 * @param databits The amount of databits used in the connection
 * @param parity The parity bits to use. (See Note above)
 * @param stops The amount of stop bits to use
 * @param inputmode Wheter to use line or character mode
 * @param flow The flow control to use
 * @param blocking Whether to use blocking or non blocking mode
 * @param signals What additional signals to use (See Note above)
 * @return true on a successful open false otherwise, (check errno for why)
 */
bool
Serial::open( const char *device,
	      const BaudRate baud,
	      const DataBits databits,
	      const Parity parity,
	      const StopBits stops,
	      const InputMode inputmode,
	      const FlowControl flow,
	      const BlockingMode blocking,
              const Flush flush,
	      const Signal signals)
{
    // Open mask
    int mask =  O_RDWR		// Read/write mode
	      | O_NOCTTY;	// Disassociate from the controlling terminal

    // If the port is already open, close it
    if ( this->isValid()){
	if ( !this->close(true) )
	    return false;
    }

    // Determine if the user cares about the DCD line, normally if the DCD line
    // was low and an open was called, the open call would hang until DCD was
    // present
    if ( ! (signals & DCD) )
	mask |= O_NONBLOCK; //Note: O_NDELAY=O_NONBLOCK (except under HPUnix)

    this->fd = ::open( device, mask );
    if ( this->fd == -1){
	return false;
    }

    /*
     * Save the current state of the device
     */
    if ( tcgetattr( this->fd, &origstate ) < 0 ){
	this->close();
	return false;
    }

    // Start with a fresh state
    memset( &this->currstate, 0x0, sizeof( struct termios ));

    // Enable the receiver & set local (detached from console mode)
    this->currstate.c_cflag |= ( CLOCAL | CREAD );

    // Don't apply the below straight away, we setup the state
    // then apply the result
    this->applyImmediately=false;

    this->setBaudRate(baud);
    this->setDataBits(databits);
    this->setStopBits(stops);
    this->setParity(parity); // Note setDataBits must be called first
    this->setInputMode(inputmode);
    this->setFlowControl(flow);

    // Flush based on what the user requested before setting mode
    this->flush(flush);

    // Now settings have been configured we do the actual apply
    this->applyImmediately=true;

    // Apply the settings
    if( !this->apply())
        return false;

    // Blocking changes occur immediately so we do this after the port setup
    this->setBlockingMode(blocking);

    return true;
}

/**
 * Flush any buffers that might be attached to the serial port (ie the fifo's).
 * What we actually flush is determined based upon the what parameter.
 *
 * @param what Input, output or both
 * @return true if the flush was successful, false otherwise, (check errno why)
 */
bool
Serial::flush(const Flush what)
{
    if ( !this->isValid())
        return false;

    if( tcflush(this->fd, what) == -1 )
	return false;

    return true;
}

/**
 * Wait until any data in the output buffer for the serial port is actually
 * written. This method will block until the data is written
 *
 * @return true when the drain is complete, false otherwise, (check errno why)
 */
bool
Serial::drain()
{
    if ( !this->isValid())
	return false;

    if ( tcdrain( this->fd ) == -1 )
	return false;

    return true;
}

/**
 * Close the serial port if it is open.
 * This sets the DTR signal low, (ie modems will hang up). Optionally restore
 * the original state of the port before closing. Note: this function will
 * return false if the state cannot be restored. It will still close the port
 * regardless of the restore state.
 *
 * @param restore True indicates the original state of the port should be restored
 * @return true if the close was successful, false otherwise (check errno)
 */
bool
Serial::close(bool restore)
{
    if ( !this->isValid())
	return false;

    bool retval = true;
    if ( restore ){
	if ( !this->applyParams( this->origstate ))
	    retval = false;
    }

    if ( ::close( this->fd ) == -1 )
	retval = false;

    this->fd = -1;
    return retval;
}

/**
 * Read data from the serial port into the buffer. Note this method
 * does not guarentee size bytes are read, only that up to size may be read.
 * It all depends on the blocking state. If the serial::isvalid is false, this
 * method will simply return 0;
 *
 * @param buffer The buffer to store read data into, should be at least 'size' long
 * @param size The size of the buffer
 * @return The amount of bytes read, -1 on error
 */
ssize_t
Serial::read ( void *buffer, size_t size )
{
    if( this->isValid())
	return ::read( this->fd, buffer, size );

    return -1;
}

/**
 * Write the given buffer to the port. The buffer should be at least size long.
 * If serial::isvalid is false this method will simply return 0; Note this
 * method only guarentees data is written to the serial ports buffer (if any).
 * If you want to confirm it's gone to the port call drain();
 *
 * @param buffer The buffer to read from (at least size long)
 * @param size The size of the buffer to write
 * @return The amount of bytes read, -1 on error
 */
ssize_t
Serial::write( const void *buffer, size_t size )
{
    if ( this->isValid())
	return ::write( this->fd, buffer, size );

    return 0;
}

/**
 * Write the given string to the serial port. Note if the serial port is
 * invalid, this method will simply return 0;
 *
 * @param str The string to write
 * @return The amount of bytes read, -1 on error
 */
ssize_t
Serial::write( const std::string &str )
{
    return this->write( str.c_str(), str.size());
}

/**
 * Obtain the amount of bytes currrenly available on the input buffer
 *
 * @return the amount of bytes available or -1 on error
 */
ssize_t
Serial::getAvailableCount() const
{
    if( isValid()){
        int bytes;

        if (ioctl(this->fd, FIONREAD, &bytes) == -1 )
            return -1;

        return bytes;
    }

    return false;
}

/**
 * Get the current baud rate of the connection. This method can be called
 * regardless of whether the port is open or not. If closed it indicates what
 * the flow control will be when opening the port.
 *
 * @return the Current baudrate as indicated by Serial::BaudRate
 */
Serial::BaudRate
Serial::getBaudRate() const
{
    return (BaudRate) cfgetispeed( &this->currstate );
}

/**
 * Return the amount of stop bits currently in use. This method can be called
 * regardless of whether the port is open or not. If closed it indicates what
 * the flow control will be when opening the port.
 *
 * @return The stopbits in use as indicated by Serial::StopBits
 */
Serial::StopBits
Serial::getStopBits() const
{
    if ( this->currstate.c_cflag & CSTOPB )
	return Serial::TWO;

    return Serial::ONE;
}

/**
 * Obtain the current parity setting of the serial port. This method can be called
 * regardless of whether the port is open or not. If closed it indicates what
 * the flow control will be when opening the port.
 *
 * @return The current parity setting as indicated by Serial::Parity
 */
Serial::Parity
Serial::getParity() const
{
    return this->parity;
}

/**
 * Obtain the databits currently in use for the conneciton.
 * Note: If space parity is being used, this will be reported as
 * the original bits specified in the open call, though the actual bits in
 * use on the port are 1 less.This method can be called
 * regardless of whether the port is open or not. If closed it indicates what
 * the flow control will be when opening the port.
 *
 * @return the amount of databits set
 */
Serial::DataBits
Serial::getDataBits() const
{
    unsigned int db = this->currstate.c_cflag & CSIZE;
    return (DataBits)(this->parity == 'S' ? db - CS6 : db);
}

/**
 * Obtain the current method of flow control in use. This method can be called
 * regardless of whether the port is open or not. If closed it indicates what
 * the flow control will be when opening the port.
 *
 * @return A valid flow control as indicated by the Serial::FlowControl enum
 */
Serial::FlowControl
Serial::getFlowControl() const
{
    return this->flow;
}

/**
 * Obtain the inputmode that the serial port is configured for.This method can be called
 * regardless of whether the port is open or not. If closed it indicates what
 * the flow control will be when opening the port.
 *
 * @return A valid InputMode as indicated by Serial::InputMode
 */
Serial::InputMode
Serial::getInputMode() const
{
    return this->input;
}

/**
 * Set the count of stop bits that are used when reading the port. These stop
 * bits are stripped off before the data propogates up to userland hence you'll
 * never see them.
 *
 * @param bits The amount of stop bits to use
 * @return true on a successful set, false otherwise
 */
bool
Serial::setStopBits( const StopBits bits)
{
    if( this->isValid()){
        // Set stop bits
        switch ( bits ){
            case ONE: this->currstate.c_cflag &= ~CSTOPB; break;
            case TWO: this->currstate.c_cflag |= CSTOPB; break;
        }
        return this->apply();
    }
    return false;
}

/**
 * Set whether parity is enabled/disabled. If enabled (either odd or Even) then
 * parity check is enabled, unless check is false.
 * If it it enabled via Space parity checking, databits are incremented by one
 * to accomodate the space parity. It is also possible to ask for unstripped
 * data via strip=false. In this situation the caller will receive raw
 * unstripped data.
 *
 * @param parity The parity type to use
 * @param strip If the parity type has parity whether to strip the parity bits  or return them (default true)
 * @param check If the parity type has parity whether to check the parity (default true)
 * @param mark If parity type has parity and strip is disabled, mark the parity with special
 * characters - a NUL character (000 octal) is sent to your program before every
 * character with a parity error. Otherwise, a DEL (177 octal) and NUL character
 * is sent along with the bad character
 * @return True on a successful set, false on failure, closed port
 */
bool
Serial::setParity( const Parity parity, const bool strip, const bool check, const bool mark)
{
    if( this->isValid()){
        // Set parity / Parity Check
        // We must store the parity setting else we can't
        // distinguish between odd and space parity when reporting parity / databits
        // to the user
        this->parity = parity;
        this->currstate.c_cflag &= ~PARENB;

        switch( parity ){
            case NONE:
                break;
            case SPACE:
                // Increase databits by one needed by space parity
                this->currstate.c_cflag &= ~CSIZE;
                this->currstate.c_cflag |= (this->getDataBits() + CS6 );
                /* fallthrough */
            case ODD:
                this->currstate.c_cflag |= PARODD;

                if( check )
                    this->currstate.c_iflag |= INPCK;
                else
                    this->currstate.c_iflag |= IGNPAR;

                if( strip )
                    this->currstate.c_iflag |= ISTRIP;
                if( mark )
                    this->currstate.c_iflag |= PARMRK;
                break;
            case EVEN:
                this->currstate.c_cflag |= PARENB;
                this->currstate.c_cflag &= ~PARODD;
                if( check )
                    this->currstate.c_iflag |= INPCK;
                else
                    this->currstate.c_iflag |= IGNPAR;
                if( strip )
                    this->currstate.c_iflag |= ISTRIP;
                if( mark )
                    this->currstate.c_iflag |= PARMRK;
                break;
        }

        return this->apply();
    }

    return false;
}

/**
 * Set the size of the databits used in the communications.
 * The most common is 8bits, though some devices (ie mobiles sms use 7)
 *
 * @param bits The databits to use (@see Serial::DataBits for options)
 * @return true on a successful set, false otherwise
 */
bool
Serial::setDataBits( const DataBits bits)
{
    if( this->isValid()){
        // Set data bits
        this->currstate.c_cflag &= ~CSIZE;
        this->currstate.c_cflag |= bits;
    }
    return false;
}


/**
 * Set the baud rate when talking to the device. Whilst it's possible to
 * have a different input and output baud rate, this class does not support it.
 * It is also very rarely used.
 *
 * @param rate The baud rate to use
 * @return true on successful set, false otherwise
 */
bool
Serial::setBaudRate( const BaudRate rate)
{
    if( this->isValid()){
        // Set the baud rate, we don't support
        // split speeds, instead input/output are the same
        cfsetispeed( &this->currstate, rate );
        cfsetospeed( &this->currstate, rate );
        return this->apply();
    }

    return false;
}


/**
 * Set the flow control used for the port. We support all possible
 * combinations of hardware and software flow control.
 *
 * @param flow The flow control type to use
 * @return true on successful set, false otherwise
 */
bool
Serial::setFlowControl( const FlowControl flow)
{
    if( this->isValid()){
        // Setup hw/sw flow control
        this->flow = flow;
        switch ( flow ){
            case DISABLED:
                this->currstate.c_cflag &= ~CRTSCTS;
                this->currstate.c_iflag &= ~(IXON | IXOFF | IXANY);
                break;
            case RTSCTS:
                this->currstate.c_cflag |= CRTSCTS;
                this->currstate.c_iflag &= ~(IXON | IXOFF | IXANY);
                break;
            case XONXOFF:
                this->currstate.c_cflag &= ~CRTSCTS;
                this->currstate.c_iflag |= (IXON | IXOFF | IXANY);
                break;
            case RTSCTS_XONXOFF:
                this->currstate.c_cflag |= CRTSCTS;
                this->currstate.c_iflag |= (IXON | IXOFF | IXANY);
                break;
        }
        return this->apply();
    }

    return false;
}

bool
Serial::setInputMode( const InputMode state)
{
    if( this->isValid()){

        // Setup the input mode correctly
        this->input = state;
        switch ( state ){
            case LINE: this->currstate.c_lflag |= ( ICANON | ECHO | ECHOE ); break;
            case RAW: this->currstate.c_lflag &= ~(  ICANON | ECHO |ECHOE | ISIG ); break;
        }
        return this->apply();
    }
    return false;
}

/**
 * Obtain the current state of the parity checking. Parity checking is
 * automatically enabled when a parity type that supports checking is enabled.
 * Optionally the parity checking can be disabled.
 *
 * @return If parity checking is currently in use or not
 */
bool
Serial::getParityCheck() const
{
    return this->parity == NONE ? false: true;
}

/**
 * Obtain the current state of the parity stripping
 *
 * @return If parity stripping is currently in use or not
 */
bool
Serial::getParityStrip() const
{
    return (this->currstate.c_iflag & ISTRIP);
}

/**
 * Obtain the current state of the parity marking
 *
 * @return If parity marking is currently in use or not
 */
bool
Serial::getParityMark() const
{
   return (this->currstate.c_iflag & PARMRK);
}

/**
 * Set a particular line to the requested state (ie true = HIGH, false = low)
 * Using this method may render previously set options no longer valid
 * as changing lines affects flow control. Hence if you use this function
 * it's assumed you know what your doing and that the internal state of the
 * class may not be invalid called.
 *
 * @param line The line to change
 * @param state The state to set the line into
 */
bool
Serial::setLine ( const Line line, bool state)
{
    if( this->isValid()){

        int status;

        if(ioctl(fd, TIOCMGET, &status) == -1 )
            return false;

        switch( line ){
            case TXD:
            assert(0 && "setLine: TXD signal setting is not yet supported");
            /*
            // Need to work this one out
                if( state ) status |= TIOCM_DTR;
                else status &= ~TIOCM_DTR;
                break;
                */
            case DTR:
                if( state ) status |= TIOCM_DTR;
                else status &= ~TIOCM_DTR;
                break;
            case RTS:
                if( state ) status |= TIOCM_RTS;
                else status &= ~TIOCM_RTS;
                break;
        };

        if ( ioctl(fd, TIOCMSET, status) == -1 )
            return false;
    }
    return false;
}


/**
 * Obtain the state of a particular signal
 *
 * @param The signal to enquire about
 * @return true if the signal is high, false otherwise (port shut/signal low)
 */
bool
Serial::getSignal ( const Signal signal)
{
    if( this->isValid()){

        Signal all = this->getSignals();
        if ( all == -1 )
            return false;

        if( all & signal )
            return true;

        return false;
    }

    return false;
}


/**
 * Obtain all the current signals of the port
 *
 * @return A bitwise enum of current signals. Or -1 on error
 */
Serial::Signal
Serial::getSignals()
{
    if( this->isValid()){

        int status;
        int all=0;

        ioctl(fd, TIOCMGET, &status);

        //Convert the signals to the Signal class
        if(status & TIOCM_CAR)
            all |= DCD;
#warning "serial: RXD signal is not yet corectly reported"
        /*
           // XXX Need to work this out - benjsc 20090909
        if( status & TIOCM_SR)
            all |= RXD;
            */
        if( status & TIOCM_LE)
            all |= DSR;
        if( status & TIOCM_CTS)
            all |= CTS;
        if( status & TIOCM_RI)
            all |= RI;

        return (Signal)all;
    }

    return (Signal)-1;
}

/**
  * This method attempts to automatically find the baud rate
  * for input based upon an algorithm from Texus Instruments.
  * The method assumes the device continually outputs 'testchar' and
  * proceedsto determine which baud rate the device is running at based upon
  * this. If the baud rate can be determined it is set and true is returned.
  * If the baud rate cannot be found, false is returned. Baudstotry is
  * additionally consulted. If the scanned baud rate is not in this array
  * the detection is classed as failing.
  *
  * This method will set the baud rate a number of times during testing
  * and not return it back to what it was before the call. Upon a successful
  * detection the baudrate will be correctly set.
  *
  * @note At present the only supported test character is \n
  * @param testchar The charcter the device is outputing
  * @param The bitrates to try
  * @return true if the baudrate was detected (baudrate is also set), false * otherwise
  */
bool
Serial::scanBaudRate( const char testchar, const BaudRate baudstotry )
{
    unsigned char ch;
    BaudRate rate;

    assert( testchar != '\n' && "Only \\n is supported as a testchar at the moment");

    if ( !this->isValid())
	return false;

    this->setBaudRate(BAUD_115200);

    // Read the first character
    ch = this->read(&ch, 1);
    switch( ch ){
        case 0x0d: rate = BAUD_115200; break;
        case 0xe6: rate = BAUD_57600; break;
        case 0x1c: rate = BAUD_38400; break;
        case 0xe0: rate = BAUD_19200; break;
#ifdef B14400
        case 0x80: rate = BAUD_14400; break;
#endif
        case 0x00:
                   {
                       usleep(8330); // Slowest wait for 1200Baud (ie guarentee character)

                       this->setBaudRate(BAUD_9600);
                       ch = this->read(&ch,1 );
                       switch(ch ){
                           case 0x0d: rate = BAUD_9600; break;
                           case 0xe6: rate = BAUD_4800; break;
                           case 0x78: rate = BAUD_2400; break;
                           case 0x80: rate = BAUD_1200; break;
                           default:
                                return false;
                       }
                       break;
                   }
        default:
                   return false;
    }

    // Test the found baudrate against those in the list the user wanted
    // if they never wanted that rate, return false (despite the rate being set)
    if( rate & baudstotry == 0 )
        return false;

    // Set the baudrate to the found rate
    this->setBaudRate(rate);

    return true;
}


/**
 * Apply the currently configured settings to the port.
 * If the internal class variable applyImmeidately is set to false this
 * function will always return true without performing any action. This
 * is used by the open call to configure the port before actually applying the
 * settings
 */
bool Serial::apply()
{
    if( this->applyImmediately)
        return applyParams( this->currstate);

    return true;
}

/**
 * Apply the given settings to the serial port
 * Since this function is internal it makes the assumption
 * that the port is valid
 *
 * @param settings The settings to apply
 */
bool
Serial::applyParams( const struct termios &settings)
{
    assert( this->isValid() && "Serial port not valid, Serial.cpp: internal state incorrect - this is a bug" );

    // Configure the port
    if( tcsetattr( this->fd, TCSANOW, &settings ) < 0 ){
	this->close(false);
	return false;
    }
    return true;
}
