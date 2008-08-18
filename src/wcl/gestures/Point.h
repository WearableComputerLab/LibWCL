/**
 * A Gesture Recognition system in C++.
 * This library is based on the following paper:
 *
 * Wobbrock, J, Wilson, A, Li, Y 2007
 * Gestures without Libraries, Toolkits or Training:
 * A $1 Recognizer for User Interface Prototypes
 * UIST 2007
 *
 * @author Michael Marner (marnermr@cs.unisa.edu.au)
 */


#ifndef POINT_H
#define POINT_H

namespace gestures
{

	/**
	 * Represents a 2D point.
	 */
	class Point
	{
		public:
			Point(double x, double y);
			Point();

			double x;
			double y;

	};

};

#endif

