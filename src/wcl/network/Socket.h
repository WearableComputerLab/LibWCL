/**
 * Classes and defines for creating and maintaining networking
 *
 * $ClearChain: projects/clearchain/src/network/Socket.h,v 1.5 2006/05/08 06:03:16 benjsc Exp $
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <string>

#ifdef WIN32
	#include <windows.h>
	#include <winsock.h>
	typedef int socklen_t;
#else					  /* Unix Includes */
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <signal.h>
#endif

class Socket
{

	public:
		enum BlockingMode { BLOCKING, NONBLOCKING };

		virtual ~Socket();
		virtual int read ( void *buffer, size_t size );
		virtual int write( const void *buffer, size_t size );
		virtual int write( const std::string & );
		virtual void close();
		virtual bool isValid() const;
		virtual bool setBlockingMode( const BlockingMode );
		virtual BlockingMode getBlockingMode() const;

		int operator *() const;
	
	protected:
		int sockfd;	
		sockaddr_in address;

		Socket();
		virtual bool bind( const unsigned port);

};


class SocketException
{
public:
    SocketException(const Socket *);
    virtual ~SocketException();

    int getCause() const;
    const std::string getReason() const;

private:
    int sockid;
    int errornumber;
};

#endif
