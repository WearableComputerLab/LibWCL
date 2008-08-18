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
#include "Marker.h"
#include "SixDofObject.h"

namespace vicon
{

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
			std::vector<TrackedObject*> objects;


	};

};

#endif

