#ifndef SIXDOFOBJECT_H_
#define SIXDOFOBJECT_H_

#include "TrackedObject.h"

namespace vicon
{

	/**
	 * Class to represent a tracked object with 6DOF.
	 * This class has both a position and rotation.
	 */
	class SixDofObject : public TrackedObject
	{
		public:

			/**
			 * Constructor.
			 * 
			 * @param name The name of this object.
			 */
			SixDofObject(std::string name);

			/**
			 * Destructor.
			 */
			virtual ~SixDofObject();


			/**
			 * Updates the position and rotation data of this object using 
			 * data from the Tarsus server.
			 *
			 * @param array An array of doubles received from the Vicon server
			 *              That this object should read from.
			 *
			 * @param offset The index into the array where this object will start
			 *               reading data from. This int is modified to reflect
			 *               how many values have been read.
			 * 
			 */
			void updateData(double* array, int & offset);

			/**
			 * Returns a string representation of this object.
			 *
			 * @return A string representation of this object.
			 */
			std::string toString();

	};

};

#endif /*SIXDOFOBJECT_H_*/

