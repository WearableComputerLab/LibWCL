/*-
 * Copyright (c) 2005-2008 Benjamin Close <Benjamin.Close@clearchain.com>
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
#ifndef WCL_NETWORK_UDPPACKET_H
#define WCL_NETWORK_UDPPACKET_H

#ifndef WIN32
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#endif

namespace wcl {

// Forward Declaration
class UDPSocket;

class UDPPacket
{

    public:
	UDPPacket( void *, const size_t );
	UDPPacket( const size_t );
	~UDPPacket();

	void *getData() const;
	size_t getSize() const;
	sockaddr_in getRecipient() const;

	void setData( void *, const size_t );
	void setRecipient( const sockaddr_in );

    private:
	sockaddr_in address;

	void *data;
	size_t size;

	bool needdelete;
};

/**
 * Allocate a buffer of the given length and perform all required memory management of the 
 * buffer. 
 *
 * @param length The length of the buffer to allocate
 * @throws a string exception on memory allocation error
 */
inline UDPPacket::UDPPacket( const size_t length )
{
    this->size = length;
    this->needdelete =true;
    this->data = (void *) malloc( this->size );
}

/**
 * Create a UDP Packet with the given buffer of the given length. The buffer
 * is not copied but stored as is.
 *
 * @param buffer The buffer to use in the packet
 * @param length The length of the buffer
 */ 
inline UDPPacket::UDPPacket( void *buffer, const size_t length ):
    data(buffer), size(length), needdelete(false){}


/**
 * Free up the memory if we allocated it
 */ 
inline UDPPacket::~UDPPacket()
{
    if( this->needdelete ){
	free( this->data );
    }
}

/**
 * Obtain the data element associated with this packet
 *
 * @return the pointer to the data associated with this Packet
 */
inline void *UDPPacket::getData() const
{
    return this->data;
}

/**
 * Obtain the size of the data element associated with the packet
 *
 * @return the The size of the data element associated with the packet
 */
inline size_t UDPPacket::getSize() const
{
    return this->size;
}

/**
 * obtain the recipient associated with this packet
 */
inline sockaddr_in UDPPacket::getRecipient() const
{
    return this->address;
}

inline void UDPPacket::setData( void *buffer, const size_t length )
{
    this->data = buffer;
    this->size = length;
}

inline void UDPPacket::setRecipient( const sockaddr_in recipient )
{
    this->address = recipient;
}


}; // namespace wcl
#endif
