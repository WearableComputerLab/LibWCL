#ifndef TRACKEDMARKER_H_
#define TRACKEDMARKER_H_

#include <wcl/network/TCPSocket.h>
#include "TrackedObject.h"

namespace vicon {

	/**
	 * Represents a tracked marker.
	 * Markers only have a position, no orientation information.
	 */
	class Marker : public TrackedObject
	{
		public:
			/**
			 * Constructor.
			 * 
			 * @param name The name of this object.
			 */
			Marker(std::string name);

			/**
			 * Destructor.
			 */
			virtual ~Marker();

			/**
			 * Updates the position data of this object using data from
			 * the Tarsus server.
			 * 
			 * @param array The array of values read from the Vicon server.
			 * @param offset The starting index where this marker should read values from.
			 *               Offset is updated by this method, adding the number of values
			 *               read by this object.
			 */
			virtual void updateData(double* array, int & offset); 


			/**
			 * Returns a string representation of this object.
			 *
			 * @return A String representation of this marker.
			 */
			std::string toString();



	};

};

#endif /*TRACKEDMARKER_H_*/
