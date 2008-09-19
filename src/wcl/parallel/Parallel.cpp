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

/* Note This code must be compiled with -O2 so inb/outb exist */

#include <stdio.h>
#include <unistd.h>

#ifdef LINUX
#include <sys/types.h>
#include <sys/io.h>
#endif

#include "Parallel.h"


/* Offsets to the port registers */
#define DATA_OFFSET    0x0
#define SIGNAL_OFFSET  0x1
#define CONTROL_OFFSET 0x2

namespace wcl {

/**
 * Create a new Parallel Port instance. Note this does
 * not open the port, you must call 'open' in order to do that.
 */
Parallel::Parallel():
    base(Parallel::NONE),data(0),control(0),signal(0)
{}


/**
 * Destroy the class and close the parallel port.
 * The port is left in the last state
 */
Parallel::~Parallel()
{
    if( this->base ){
	this->close();
    }

    data=control=signal=0;
}


/**
 * Attempt to open the given parallel port. The port is opened
 * in the current state and information about the ports current
 * state is read and stored.
 *
 * @param port The port to open
 * @return true if the port could be opened, false otherwise
 */
bool Parallel::open( const Parallel::Port port )
{
#ifdef LINUX
    if( ioperm(port, 3 /* 3 registers bytes */, 1 /* turn port on*/) == 0){
	this->base = port;

	// Obtain the current state of each register
	data = inb( this->base + DATA_OFFSET );
	control = inb( this->base + CONTROL_OFFSET);
	signal = inb( this->base + SIGNAL_OFFSET );

	return true;
    }

#endif
    return false;
}

/**
 * Close the parallel port if it's been opened
 */
void Parallel::close()
{
    if( this-> base ){
#ifdef LINUX
	// We force the close and don't care about the result
	ioperm(this->base, 3, 0);
	this->base = Parallel::NONE;
#endif
    }
}

/**
 * Set the given data line to be high or low
 *
 * @param line The line to set
 * @param state The state to set the line too (true = high, false=low)
 */
bool Parallel::setDataLine( const DataLine line, bool state )
{
    if( this->base ){
	this->data  = ~(this->data & line );
	if( state ) this->data |= line;

	outb( this->data, this->base + DATA_OFFSET);
    }
    return false;
}

/**
 * Obtain the value for the given pin on the data line.
 * This request queries the actual port
 *
 * @param line The line to find the status for
 */
bool Parallel::getDataLine( const DataLine line)
{
    // Since we are quering the port, we take the opertunity
    // to update our internal representation at the same time
    if ( this->base )
	return ( (this->data = inb(this->base+ DATA_OFFSET)) & line );

    return false;
}

/**
 * Set the control line to the given value
 * (high=true, low=false)
 *
 * @param line The line to set
 * @param state The to set the control line too
 */
bool Parallel::setControl( const Control line, bool state)
{
    if( this->base ){
	this->control  = ~(this->control & line );
	if( state ) this->control |= line;

	outb( this->data, this->base + CONTROL_OFFSET);
	return true;
    }
    return false;
}

/**
 * Obtain the state of the requested control line
 *
 * return true if the line is high, false if it is low
 */
bool Parallel::getControl( const Control line)
{
    // Since we are quering the port, we take the opertunity
    // to update our internal representation at the same time
    if ( this->base )
	return ( (this->control = inb(this->base+ CONTROL_OFFSET)) & line );

    return false;
}

/**
 * Obtain the value for the given pin on the signal lines.
 * This request queries the actual port
 *
 * @param line The line to find the status for
 */
bool Parallel::getSignal( const Signal line)
{

    // Since we are quering the port, we take the opertunity
    // to update our internal representation at the same time
    if ( this->base )
	return ( (this->signal = inb(this->base+ SIGNAL_OFFSET)) & line );

    return false;
}

/**
 * Write the given value to the data lines of the Parallel port
 *
 * @param ch The character to write
 * @return true if the write was successful, false if it was not
 */
bool Parallel::write( const char ch)
{
    if ( this-> base ){
	outb( ch, this->base + DATA_OFFSET );
	return true;
    }

    return false;
}

/* XXX/TODO Not Yet Implemented, needs bidirectional support
   bool Parallel::read ( const char *) const
   {}
 */

} // namespace wcl
