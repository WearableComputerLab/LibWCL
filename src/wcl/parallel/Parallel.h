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
#ifndef WCL_PARALLEL_PARALLEL_H
#define WCL_PARALLEL_PARALLEL_H

namespace wcl {

/**
 * The parallel class represents a class capable of controlling the
 * parallel port in COMPATABILE/SPP based on the ieee1284-1994 specifications.
 * The parallel port can be put into
 * the following modes, many set in bios these are:
 *
 *  o   compatible mode, also called Centronics mode  (SPP in bios)
 *  o   bidirectional 8/4-bits mode, also called NIBBLE mode
 *  o   byte mode, also called PS/2 mode
 *  o   Extended Capability Port mode, (ECP in bios)
 *  o   Enhanced Parallel Port mode, (EPP in bios)
 *  o   mixed ECP+EPP or ECP+PS/2 modes (ECP+EPP in bios)
 *
 * If you want to use this class make sure SPP is set in bios.
 *
 * This class allows basic functionality of the various lines
 * of the parallel port and allows basic data to be send down it
 * vi the data lines.
 *
 * In SPP Mode the pining for a standard Parallel port is:
 * (See http://www.clearchain.com/wiki/Wiring for schematics)
 * (and http://www.beyondligc.org/spp/parallel.htm)
 *
 *  1	Strobe
 *  2   Data 0 (Output)
 *  3   Data 1 (Output)
 *  4   Data 2 (Output)
 *  5   Data 3 (Output)
 *  6   Data 4 (Output)
 *  7   Data 5 (Output)
 *  8   Data 6 (Output)
 *  9   Data 7 (Output)
 * 10   ACK    (Input)
 * 11   BUSY   (Input)
 * 12   Paper Empty (Input)
 * 13   Select (Input)
 * 14   Auto Feed (Input/Output)
 * 15   ERROR   (Input)
 * 16   INIT    (Input/Output)
 * 17   SELECT IN (Input/Output)
 * 18 Signal Ground (Not controllable)
 * 19 Signal Ground (Not controllable)
 * 20 Signal Ground (Not controllable)
 * 21 Signal Ground (Not controllable)
 * 22 Signal Ground (Not controllable)
 * 23 Signal Ground (Not controllable)
 * 24 Signal Ground (Not controllable)
 * 25 Signal Ground (Not controllable)
 */
class Parallel
{
public:

    /* The io address of the control registers for the port */
    enum Port {
	NONE = 0,
	LPT1 = 0x378,
	LPT2 = 0x278,
	LPT3 = 0x3bc // Normally found on daughter boards
    };

    /* The various output only data lines */
    /* Note: These can be bidirectional but is not currently implemented (see
     * http://www.beyondlogic.org/spp/parallel.htm for details
     * These are based on PORT + 0 (data register)
     */
    enum DataLine {
	DATA0=0x01, // Pin 2
	DATA1=0x02, // Pin 3
	DATA2=0x04, // Pin 4
	DATA3=0x08, // Pin 5
	DATA4=0x10, // Pin 6
	DATA5=0x20, // Pin 7
	DATA6=0x40, // Pin 8
	DATA7=0x80, // Pin 9
    };

    /* The various input only signal lines */
    /* These are based on PORT + 1 (status register) */
    enum Signal {
	ACK     =0x20, // Pin 10
	BUSY    =0x80, // Pin 11
	PAPEROUT=0x10, // Pin 12
	SELECT  =0x08, // Pin 13
	ERROR   =0x04, // Pin 15
    };

    /* varios i/o control lines */
    /* These are based on PORT + 2 (control register) */
    enum Control {
	STROBE  =0x01, // Pin 1
	LINEFEED=0x02, // Pin 14
	INIT    =0x04, // Pin 16
        SELECTIN=0x08, // Pin 17

	/* The following are not associated with a pin */
	/* and are internal to the chip */
	/* This is not yet implemented
	IRQACK  =0x08  // Enable an interrupt on line change
	BIDIRECT=0x10  // Enable bidirectional data
	*/
    };

    Parallel();
    virtual ~Parallel();

    bool open( const Port );
    void close();
    bool setDataLine( const DataLine, bool state );
    bool getDataLine( const DataLine ) const;
    bool setControl( const Control, bool state);
    bool getControl( const Control ) const;
    bool getSignal( const Signal ) const;
    bool write( const char );

    /* XXX/TODO Not Yet Implemented, needs bidirectional support  - benjsc
     * 20080916
    bool read ( const char *) const;
    */

private:
    Port base;
    int8_t data;
    int8_t control;
    int8_t signal;
};


}; // namespace wcl

#endif
