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

/**
 * A $1 Gesture Recognition system.
 * This library is based on the following paper:
 *
 * Wobbrock, J, Wilson, A, Li, Y 2007
 * Gestures without Libraries, Toolkits or Training:
 * A $1 Recognizer for User Interface Prototypes
 * UIST 2007
 *
 * @author Michael Marner (marnermr@cs.unisa.edu.au)
 */

#ifndef GESTURES_H
#define GESTURES_H

#define BOX_SIZE 5
#define THETA 45
#define DEVIATION 2

#include <string>
#include <vector>

#include <wcl/api.h>
#include <wcl/maths/Vector.h>

namespace wcl 
{
	/**
	 * Definition of a point
	 */
	typedef wcl::Vector Point;

	/**
	 * Type used for storing a list of Points.
	 * Is templated std::vector
	 */
	typedef std::vector<wcl::Vector> PointList;

	/**
	 * Internal format used for storing gesture templates.
	 * Consists of a PointList and a name for the gesture.
	 */
	struct WCL_API GestureTemplate
	{
		PointList points;
		std::string name;
	};

	/**
	 * Struct representing the bounding box of a gesture.
	 * This is used during scaling.
	 */
	struct WCL_API BoundingSquare
	{
		wcl::Vector p1;
		wcl::Vector p2;
	};


	/**
	 * A Class for solving gesture recognition.
	 *
	 * To use this class you would instantiate an object.
	 * You would then load in a set of gesture templates from the filesystem using 
	 * prepare(), followed by addTemplate().
	 *
	 *
	 * To do recognition, you would record a set of points. This would typically come from a
	 * mouse or other input device.
	 *
	 * Once the list of points is recorded, you would pass these to prepare() so the gesture
	 * is resampled, rotated, translated and scaled appropriately.
	 *
	 * Finally, the prepared list of points should be passed to recognised(), the name of the
	 * gesture is then returned.
	 *
	 */
	class WCL_API Gestures
	{
		public:
		/**
		 * Constructor.
		 *
		 * @param numPoints The number of points that gestures
		 *                  will be resampled to.
		 *                  By default this is 64.
		 */
		Gestures(int numPoints = 64);

		/**
		 * Destructor.
		 */
		~Gestures();


		/**
		 * Runs the recognising algorithm on a set of points, and returns
		 * the name of the gesture it finds.
		 *
		 * Note that this algorithm assumes that the list of points passed
		 * in contains the same number as the stored templates contain.
		 * You should call prepare on the list of points to prepare them
		 * for recognition.
		 *
		 * @param points A list of points containing the gesture to recognise.
		 * @return The name of the string it recognised.
		 * @throw Throws a std::string if points is empty.
		 */
		std::string recognise(PointList points);

		
		/**
		 * Prepares a list of points for recognition.
		 *
		 * Does the following operations on the points:
		 *  o Resamples so the list contains the same number of points as the templates do
		 *  o Rotates the gesture so the angle of the first point and centroid is aligned to X
		 *  o Scales the gesture to a square
		 *  o Translates the gesture so the centoid is at 0,0
		 *
		 *  This function should be called on a list of points before passing
		 *  the list to recognise.
		 *
		 *  @param points The list of points to prepare
		 *  @return A new list of points suitable for recognition.
		 *  @throw Throws a std::string if points is empty
		 */
		PointList prepare(PointList &points);

		/**
		 * Adds a new gesture to the list of templates.
		 *
		 * The PointList should first be prepared using the prepare function.
		 *
		 * @param points The PointList that represents this gesture.
		 * @param name The name of this new gesture.
		 */
		void addTemplate(PointList points, std::string name);


		/**
		 * Rotates a gesture so that the angle between the centroid and the first
		 * point is aligned to the X axis.
		 *
		 * This function is part of of prepare, and usually won't need to be called
		 * by itself.
		 *
		 * @param points A list of points to rotate
		 * @return A rotated list of points.
		 */
		PointList rotateToZero(PointList &points);

		/**
		 * Non-uniformly scales a gesture to a specific size square.
		 *
		 * This function is part of prepare and usually won't need to be called
		 * by itself.
		 *
		 * @param points The list of points to scale.
		 * @param size The size of the square to resize to.
		 * @return A list of points representing a scaled version of the gesture.
		 */
		PointList scaleToSquare(PointList &points, double size);

		/**
		 * Translates a gesture so that the centroid is located at 0,0.
		 *
		 * This function is part of prepare and usually won't need to be called
		 * by itself.
		 *
		 * @param points A list of points to translate.
		 * @return A list of points representing the translated gesture.
		 */
		PointList translateToOrigin(PointList &points);

		/**
		 * Resamples a gesture to a 64 points.
		 */
		PointList resample(PointList & points);

		/**
		 * Calculates the length of the path taken of a gesture.
		 *
		 * @return The length of the gesture's path.
		 */
		double pathLength(PointList & points);

		/**
		 * Calculates the distance between 2 points.
		 */
		double distance (wcl::Vector a, wcl::Vector b);

		/**
		 * Finds the central point of a gesture.
		 */
		wcl::Vector centroid(PointList &points);

		/**
		 * Calculates the bounding box of a gesture.
		 */
		BoundingSquare getBoundingSquare(PointList &points);

		/**
		 * Rotates a gesture by a specific angle.
		 */
		PointList rotateBy(PointList &points, double theta);

		private:
		double distanceAtBestAngle(PointList &points, GestureTemplate& t, double thetaA, double thetaB, double delta);
		double distanceAtAngle(PointList &points, GestureTemplate &t, double theta);
		double pathDistance(PointList &a, PointList &b);
		/**
		 * Internal storage of templates
		 */
		std::vector<GestureTemplate> templates;

		int numPoints;

	};

};

#endif
