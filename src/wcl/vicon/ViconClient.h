/*-
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
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


/*
 * Vicon Interface.
 *
 * Michael Marner (michael.marner@unisa.edu.au)
 */

#ifndef TARSUS_CLIENT
#define TARSUS_CLIENT

#include <string>
#include <vector>
#include <wcl/network/TCPSocket.h>

#include "TrackedObject.h"


/**
 * Interface to the Vicon Motion Capture System. 
 * Provides access to motion capture data from the mocap system.
 *
 * @author Michael Marner (marnermr@cs.unisa.edu.au)
 */
namespace wcl 
{

	/**
	 * Provides a connection to the Vicon system.
	 *
	 * To talk to the MoCap system you create an object of this class.
	 * You then call update() to get the latest data from the system,
	 * so update() will probabaly be called inside your render loop.
	 *
	 * To get data from the system you can just hold a reference to one
	 * of the TrackedObjects. For example, using OpenSceneGraph you
	 * would create a Callback node that stores a TrackedObject and then
	 * in the update loop you would get the data from the TrackedObject
	 * and update any scene graph nodes.
	 */
	class ViconClient
	{
		public: 

			/*
			 * Vicon Packets
			 */
			static const int CLOSE = 0;
			static const int INFO = 1;
			static const int DATA = 2;
			static const int STREAMING_ON = 3;
			static const int STREAMING_OFF = 4;

			/*
			 * Packet Types
			 */
			static const int REQUEST = 0;
			static const int REPLY = 1;


			/**
			 * Constructor.
			 * Attempts to connect to the hostname supplied.
			 * Then, it constructs a list of channels that the
			 * server is sending, and fills the channel objects
			 * with their initial data.
			 * 
			 * @param hostname The hostname (or IP) of the server to connect to.
			 * @param port The port to connect to, by default this is 800.
			 */
			ViconClient(std::string hostname, int port=800);

			/**
			 * Destructor.
			 * Closes and releases the connection to the server.
			 */
			virtual ~ViconClient();

			/**
			 * Returns a list of channels that the server is sending out.
			 * Note that a channel contains a single value, eg the object's
			 * location in x.
			 * 
			 * To get the encapsulated data, the objects vector should be used.
			 * 
			 * @return A vector containing the list of channel names.
			 * @throw Throws a std::string if the connection to the server fails.
			 */
			std::vector<std::string> getChannelNames(); 

			/**
			 * Fills the tracked objects with the latest frame of data from the server.
			 *
			 * @throw Throws a std::string if the connection fails or invalid data is received
			 *        from the server.
			 */
			void update();

			/**
			 * Gets a tracked object with the specified name.
			 * 
			 * @param name The object to search for.
			 * @return A pointer to the object, or 0 if it is not found.
			 */
			TrackedObject* getObject(std::string name);

		private:
			/**
			 * The socket handling the connection to the server.
			 */
			TCPSocket* socket;

			/**
			 * The current frame that we have received data for.
			 */
			double time;

			/**
			 * Reads a channel name from the server.
			 *
			 * @return A string containing the name of the cnannel.
			 */
			std::string readChannel();

			/**
			 * Createst the list of tracked objects.
			 * 
			 * This function looks quite convoluted, and it is. The Vicon system
			 * gives back information in a messy way, so we have to do a bit of 
			 * work to get everything into a format we can use.
			 *
			 * @throw Throws a std::string if
			 *         o The connection to the server is lost
			 *         o The server responds with invalid data
			 *         o The data received from the server cannot be used
			 *           To reconstruct valid TrackedObject objects.
			 */
			void loadTrackedObjects();

			/**
			 * The list of objects.
			 */
			std::vector<TrackedObject> objects;

	};

};

#endif

