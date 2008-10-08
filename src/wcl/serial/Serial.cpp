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

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <wcl/serial/Serial.h>

using namespace wcl;


Serial::Serial():
    fd(-1), parity(NONE),parityCheckEnabled(false),blocking(BLOCKING)
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

    if ( !this->isValid()){
	return false;
    }

    // Don't bother setting the mode if it's the default
    if ( this->blocking == blocking )
	return true;

    // In order to set the blocking flag, we must be careful not
    // to stop on any other flags that have been set. Hence
    // we obtain all the flags and adjust the blocking flag only
    flags = ::fcntl(this->fd, F_GETFL, 0 /* ALLFLAGS */);

    switch( blocking ) {
        case BLOCKING:
            flags &=~O_NONBLOCK;
            if ( ::fcntl( this->fd, F_SETFL, flags ) == 0 ){
                return true;
            }
            break;
        case NONBLOCKING:
            flags |= O_NONBLOCK;
            if ( ::fcntl( this->fd, F_SETFL, flags ) == 0 ){
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
	      const Signal signals)
{
    // Open mask
    int mask =  O_RDWR		// Read/write mode
	      | O_NOCTTY;	// Disassociate from the controlling terminal

    // If the port is already open, close it
    if ( this->isValid()){
	if ( !this->close() )
	    return false;
    }

    // Determine if the user cares about the DCD line, normally if the DCD line
    // was low and an open was called, the open call would hang until DCD was
    // present
    if ( ! (signals & DCD) )
	mask |= O_NDELAY;

    this->fd = ::open( device, mask );
    if ( this->fd ){
	return false;
    }

    /*
     * Save the current state of the device
     */
    if ( tcgetattr( this->fd, &origstate ) < 0 ){
	this->close();
	return false;
    }

    // Copy the state of the port before modifying it
    memcpy( &this->currstate, &this->origstate, sizeof (struct termios));

    // Set the baud rate, we don't support
    // split speeds, instead input/output are the same
    cfsetispeed( &this->currstate, baud );
    cfsetospeed( &this->currstate, baud );

    // Set data bits
    this->currstate.c_cflag &= ~CSIZE;
    this->currstate.c_cflag |= databits;

    // Set stop bits
    switch ( stops ){
	case ONE: this->currstate.c_cflag &= ~CSTOPB; break;
	case TWO: this->currstate.c_cflag |= CSTOPB; break;
    }

    // Set parity / Parity Check
    // We must store the parity setting else we can't
    // distinguish between odd and space parity when reporting parity / databits
    // to the user
    this->parity = parity;
    this->currstate.c_cflag &= ~PARENB;

    switch( parity ){
	case 'N':
	    break;
	case 'S':
	    // Increase databits by one needed by space parity
	    // The way we do this seems - // benjsc 20070118
	    this->currstate.c_cflag &= ~CSIZE;
	    this->currstate.c_cflag |= (databits + CS6 );
	    /* fallthrough */
	case 'O':
	    this->currstate.c_cflag |= PARODD;
	    this->currstate.c_iflag |= (INPCK | ISTRIP );
	    this->parityCheckEnabled = true;
	    break;
	case 'E':
	    this->currstate.c_cflag |= PARENB;
	    this->currstate.c_cflag &= ~PARODD;
	    this->currstate.c_iflag |= (INPCK | ISTRIP );
	    this->parityCheckEnabled = true;
	    break;
    }


    // Setup the input mode correctly
    this->input = input;
    switch ( inputmode ){
	case RAW: this->currstate.c_lflag |= ( ICANON | ECHO | ECHOE ); break;
	case LINE: this->currstate.c_lflag &= ~(  ICANON | ECHO |ECHOE /*| XXXSIG*/ ); break;
    }

    // Setup hw/sw flow control
    this->flow = flow;
    switch ( flow ){
	case NONE:
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
    }

    // Enable the receiver & set local (detached from console mode)
    this->currstate.c_cflag |= ( CLOCAL | CREAD );

    /*
     * XXX
    if( this->applySettings( this->currstate ) == false ){
	this->close( true );
	return false;
    }
    */
    if( tcsetattr( this->fd, TCSANOW | TCSAFLUSH, &this->currstate ) < 0 ){
	return false;
    }

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
    //XXXX
    /*
    if ( restore ){
	if ( !this->applyParams( this->origstate ))
	    retval = false;
    }
    */

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
 */
ssize_t
Serial::read ( void *buffer, size_t size )
{
    if( this->isValid())
	return ::read( this->fd, buffer, size );

    return 0;
}

/**
 * Write the given buffer to the port. The buffer should be at least size long.
 * If serial::isvalid is false this method will simply return 0; Note this
 * method only guarentees data is written to the serial ports buffer (if any).
 * If you want to confirm it's gone to the port call drain();
 *
 * @param buffer The buffer to read from (at least size long)
 * @param size The size of the buffer to write
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
 */
ssize_t
Serial::write( const std::string &str )
{
    return this->write( str.c_str(), str.size());
}

/**
 * Get the current baud rate of the connection
 *
 * @return the Current baudrate
 */
Serial::BaudRate
Serial::getBaudRate() const
{
    return (BaudRate) cfgetispeed( &this->currstate );
}

/**
 * Return the amount of stop bits currently in use
 *
 * @return The stopbits in use
 */
Serial::StopBits
Serial::getStopBits() const
{
    if ( this->currstate.c_cflag & CSTOPB )
	return Serial::TWO;

    return Serial::ONE;
}

/**
 * Obtain the current parity setting of the serial port
 *
 * @return The current parity setting
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
 * use on the port are 1 less.
 *
 * @return the amount of databits set
 */
Serial::DataBits
Serial::getDataBits() const
{
    unsigned int db = this->currstate.c_cflag & CSIZE;
    //FIXME/XXX:
    //return this->parity == 'S' ? db - CS6 : db;
    return Serial::DB_FIVE;
}

/**
 * Obtain the current method of flow control in use
 */
Serial::FlowControl
Serial::getFlowControl() const
{
    return this->flow;
}

/**
 * Obtain the inputmode that the serial port is configured for
 */
Serial::InputMode
Serial::getInputMode() const
{
    return this->input;
}

bool
Serial::setStopBits( const StopBits bits)
{
}

bool
Serial::setParity( const Parity parity)
{}

bool
Serial::setDataBits( const DataBits bits)
{}

bool
Serial::setBaudRate( const BaudRate rate)
{}

bool
Serial::setFlowControl( const FlowControl flow)
{}

bool
Serial::setInputMode( const InputMode state)
{}

// See Parity enum above for details about these
bool
Serial::setParityCheck( const bool state)
{
}

#if 0 //XXX
/**
 * Obtain the current state of the parity checking. Parity checking is
 * automatically enabled when a parity type that supports checking is enabled.
 * Optionally the parity checking can be disabled.
 *
 * @return If parity checking is currently in use or not
 */
bool
Serial::getParityCheck() const;
{
    return this->checkEnabled;
}

/**
 * Set a particular line to the requested state (ie true = HIGH, false = low)
 * Using this method may render previously set options no longer valid
 * as changing lines affects flow control. Hence if you use this function
 * it's assumed you know what your doing
 *
 * @param line The line to change
 * @param state The state to set the line into
 */
bool
Serial::setLine ( const Line line, bool state)
{
///XXX
}

bool
Serial::getSignal ( const Signal signal)
{
    //XXXX
}

bool
Serial::scanBaudRate( const char testchar, const BaudRate baudstotry )
{
    if ( !this->isValid())
	return false;

    //XXX
}


/**
 * Given a list of settings, apply these settings to the actual
 * serial port. Internally the state of the class will be updated
 * to reflect the state of the port
 *
 * @param settings The settings to apply
 */
bool
Serial::applyParams( const struct termios &settings)
{

    // Configure the port, flush buffers
    if( tcsetattr( this->fd, TCSANOW | TCSAFLUSH, &this->currstate ) < 0 ){
	this->close();
	return false;
    }

    //XXXXXX
}
#endif
