/*-
 * Copyright (c) 2012 Michael Marner <michael@20papercups.net>
 *
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


#ifndef LIBWCL_MATRIX_SWITCH_H
#define LIBWCL_MATRIX_SWITCH_H


#include <wcl/api.h>
#include <wcl/network/TCPSocket.h>


namespace wcl
{
	/**
	 * A simple abstraction around the Emcore VX Modular Matrix Router.
	 *
	 * This allows programs to route signals in the matrix switch.
	 *
	 * @author Michael Marner <michael@20papercups.net>
	 */
	class WCL_API MatrixSwitch
	{
		public:
			MatrixSwitch(const std::string& ipAddress, unsigned int switchID);
			~MatrixSwitch();

			/**
			 * Routes a signal on the matrix switch.
			 *
			 * Connects the input specified to the output specified,
			 * does not mess with any other connections.
			 *
			 * @param inputID The input on the switch, inputID > 0, <=32
			 * @param outputID The output on the switch, inputID > 0, <=32
			 *
			 * @return Returns true if the connection was made, false
			 *         if the parameters were invalid.
			 */
			bool route(unsigned int inputID, unsigned int outputID);

			/**
			 * Disconnects the signal from the output, leaving that output
			 * unrouted.
			 *
			 * @param outputID The output on the swtich to disconnect.
			 * @return True if successful, false if outputID was invalid.
			 */
			bool disconnect(unsigned int outputID);

		private:
			std::string mIP;
			unsigned int mSwitchID;
	};
};

#endif

