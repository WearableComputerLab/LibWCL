/*-
 * Copyright (c) 2010 Michael Marner <michael@20papercups.net>
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

#ifndef WCL_WIIMOTE_EXCEPTION_H
#define WCL_WIIMOTE_EXCEPTION_H

#include <wcl/Exception.h>

namespace wcl {

	/**
	 * Exception class used by the Wiimote.
	 */
	class WCL_API WiimoteException: public Exception
	{
		public:
			/**
			 * The types of exceptions the wiimote may throw.
			 */
			enum Error
			{
				/**
				 * The MAC address passed in is invalid.
				 */
				BAD_MAC,

				/**
				 * The attempt to connect timed out. This usually
				 * means the user forgot to turn the wiimote on.
				 */
				CONNECTION_TIMEOUT,

				/**
				 * The socket disconnected.
				 */
				DISCONNECT,

				/**
				 * A command failed to execute.
				 */
				COMMAND_FAILURE

			};

			WiimoteException(const char * message, Error type);

			/**
			 * Gets the nice message, suitable for displaying
			 * as an error.
			 */
			virtual const char *what() const throw();

			/**
			 * Gets the type of error this exception was
			 * caused by.
			 */
			Error getType();

		private:
			const char *mReason;
			Error mType;
			
	};

}; // namespace wcl


#endif
