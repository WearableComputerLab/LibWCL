#ifndef TRACKEDOBJECT_H_
#define TRACKEDOBJECT_H_

#include <string>

#include <wcl/network/TCPSocket.h>

namespace vicon
{

	/**
	 * Represents an object that can be tracked by the Vicon system.
	 * 
	 * This is an abstract class, the concrete subclasses Marker and SixDofOjbect
	 * will be used.
	 */
	class TrackedObject
	{
		public:

			/**
			 * Constructor.
			 * @param name The name of the channel.
			 */
			TrackedObject(std::string name);

			/**
			 * Destructor.
			 */
			virtual ~TrackedObject();

			/**
			 * Updates the data stored in this object.
			 * 
			 * @param array The array of data to read from
			 * @param offset The location in the array to start reading from. Note that
			 *               the object should increment this!
			 */
			virtual void updateData(double* array, int & offset) = 0;

			/**
			 * Returns a string representation of the object.
			 */
			virtual std::string toString() = 0;

			/**
			 * Returns the name of the object.
			 */
			virtual std::string getName();


		protected:

			std::string name;

			/**
			 * Location of this object in x.
			 */
			double x;

			/**
			 * Location of this object in y.
			 */
			double y;

			/**
			 * Location of this object in z.
			 */
			double z;

			double rx;
			double ry;
			double rz;

			double occluded;
	};

};

#endif /*TRACKEDOBJECT_H_*/
