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
 * A Gesture Recognition system in C++
 * This library is based on the following paper:
 *
 * Wobbrock, J, Wilson, A, Li, Y 2007
 * Gestures without Libraries, Toolkits or Training:
 * A $1 Recognizer for User Interface Prototypes
 * UIST 2007
 *
 * @author Michael Marner (marnermr@cs.unisa.edu.au)
 */

#include <iostream>
#include <float.h>
#include <math.h>
#include <cstdlib>

#include "Gestures.h"

using namespace wcl;

Gestures::Gestures(int numPoints)
{
	this->numPoints = numPoints;
}

Gestures::~Gestures()
{

}

std::string Gestures::recognise(PointList points)
{
	if (points.size() == 0)
	{
		return NULL;
	}

	double b = DBL_MAX;
	double score;
	GestureTemplate *currentBest;

	std::vector<GestureTemplate>::iterator it;

	for (it = templates.begin(); it < templates.end(); it++)
	{
		double d = distanceAtBestAngle(points, *it, -THETA, THETA, DEVIATION);
		if (d < b)
		{
			b = d;
			currentBest = &(*it);
		}
	}
	score = 1 - b / 0.5 * sqrt(BOX_SIZE*BOX_SIZE+ BOX_SIZE*BOX_SIZE);
	return currentBest->name;
}

PointList Gestures::prepare(PointList &points)
{
	if (points.size() == 0)
	{
		throw std::string("PointList passed to prepare is empty!");
	}
	PointList resampled = resample(points);
	resampled = rotateToZero(resampled);
	resampled = scaleToSquare(resampled, BOX_SIZE);
	resampled = translateToOrigin(resampled);
	return resampled;
}


PointList Gestures::resample(PointList& points)
{
	if (points.size() == 0)
	{
		throw std::string("PointList passed to resample is empty!");
	}
	double I = pathLength(points) / (numPoints-1);
	double D = 0;
	PointList newPoints;
	newPoints.push_back(points[0]);
	for (unsigned int i=1; i < points.size(); i++)
	{
		double d = distance(points[i-1], points[i]);
		if (D + d >= I)
		{
			double qx = points[i-1][0] + ((I-D)/d) * (points[i][0] - points[i-1][0]);
			double qy = points[i-1][1] + ((I-D)/d) * (points[i][1] - points[i-1][1]);

			newPoints.push_back(wcl::Vector(qx, qy));
			PointList::iterator it = points.begin();
			points.insert(it+i, wcl::Vector(qx,qy));
			D = 0;
		}
		else {
			D += d;
		}
	}
	return newPoints;
}


double Gestures::pathLength(PointList& a)
{
	double d = 0;
	for (unsigned int i=1;i<a.size();i++)
	{
		d += distance (a[i-1], a[i]);
	}

	return abs(d);
}


PointList Gestures::rotateToZero(PointList & points)
{
	wcl::Vector c = centroid(points);

	double theta = atan2(c[1] - points[0][1], c[0] - points[0][0]);

	theta *= -1;

	PointList::iterator it;
	PointList newPoints;

	for (it = points.begin();it<points.end();it++)
	{
		wcl::Vector p = *it;
		wcl::Vector q(2);
		q[0] = (p[0] - c[0]) * cos(theta) - (p[1] - c[1])*sin(theta) + c[0];
		q[1] = (p[0] - c[0]) * sin(theta) + (p[1] - c[1])*cos(theta) + c[1];

		newPoints.push_back(q);
	}
	return newPoints;
}


/**
 * Scales a list of points to a bounding box.
 * Note that this operates on the list of points passed in!
 */
PointList Gestures::scaleToSquare(PointList &points, double size)
{
	BoundingSquare b = getBoundingSquare(points);
	
	double width = abs(b.p1[0] - b.p2[0]);
	double height= abs(b.p1[1] - b.p2[1]);
	
	PointList::iterator it;
	PointList newPoints;
	for (it = points.begin();it<points.end();it++)
	{
		wcl::Vector p = *it;
		p[0] = p[0] * (size/width);
		p[1] = p[1] * (size/height);
		newPoints.push_back(p);
	}
	return newPoints;
}


PointList Gestures::translateToOrigin(PointList &points)
{
	wcl::Vector c = centroid(points);
	PointList::iterator it;
	PointList newPoints;
	for (it = points.begin();it<points.end();it++)
	{
		wcl::Vector p = *it;
		p[0] = p[0] - c[0];
		p[1] = p[1] - c[1];
		newPoints.push_back(p);
	}
	return newPoints;
}


double Gestures::distanceAtBestAngle(PointList &points, GestureTemplate &t, double thetaA, double thetaB, double deviation)
{

	double magicNumber = 0.5*(-1 + sqrt(5));

	double x1 = magicNumber*thetaA + (1-magicNumber)*thetaB;
	double f1 = distanceAtAngle(points, t, x1);
	double x2 = (1-magicNumber)*thetaA + magicNumber*thetaB;
	double f2 = distanceAtAngle(points, t, x2);

	while (abs(thetaB - thetaA) > deviation)
	{
		if (f1 < f2)
		{
			thetaB = x2;
			x2 = x1;
			f2 = f1;
			x1 = magicNumber*thetaA + (1-magicNumber)*thetaB;
			f1 = distanceAtAngle(points, t, x1);
		}
		else
		{
			thetaA = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1-magicNumber)*thetaA + magicNumber*thetaB;
			f2 = distanceAtAngle(points, t, x2);
		}
	}
	if (f1 < f2)
		return f1;
	return f2;

}


double Gestures::distanceAtAngle(PointList& points, GestureTemplate &t, double theta)
{
	PointList newPoints = rotateBy(points, theta);
	return pathDistance(newPoints, t.points);
}

double Gestures::pathDistance(PointList &a, PointList &b)
{
	double d = 0;
	for (unsigned int i = 0; i< a.size(); i++)
	{
		d += distance (a[i], b[i]);
	}
	return d/a.size();
}


PointList Gestures::rotateBy(PointList& points, double theta)
{
	wcl::Vector c = centroid(points);
	PointList::iterator it;
	PointList newPoints;
	for (it = points.begin();it<points.end();it++)
	{
		wcl::Vector p = *it;
		double qx = (p[0] - c[0])*cos(theta) - (p[1] - c[1])*sin(theta) + c[0];
		double qy = (p[0] - c[0])*sin(theta) + (p[1] - c[1])*cos(theta) + c[1];
		newPoints.push_back(wcl::Vector(qx,qy));
	}
	return newPoints;
}


double Gestures::distance(wcl::Vector a, wcl::Vector b)
{
	double deltaX = b[0] - a[0];
	double deltaY = b[1] - a[1];
	return sqrt(deltaX*deltaX + deltaY*deltaY);
}

wcl::Vector Gestures::centroid(PointList & points)
{
	double maxX = DBL_MIN;
	double minX = DBL_MAX;
	double maxY = DBL_MIN;
	double minY = DBL_MAX;

	PointList::iterator it;
	for (it = points.begin();it<points.end();it++)
	{
		wcl::Vector p = *it;
		if (p[0] < minX)
			minX = p[0];
		if (p[0] > maxX)
			maxX = p[0];
		if (p[1] < minY)
			minY = p[1];
		if (p[1] > maxY)
			maxY = p[1];
	}
	return wcl::Vector((minX + maxX)/2, (minY + maxY)/2);
}

BoundingSquare Gestures::getBoundingSquare(PointList &points)
{
	double maxX = DBL_MIN;
	double minX = DBL_MAX;
	double maxY = DBL_MIN;
	double minY = DBL_MAX;

	PointList::iterator it;
	for (it = points.begin();it<points.end();it++)
	{
		wcl::Vector p = *it;
		if (p[0] < minX)
			minX = p[0];
		if (p[0] > maxX)
			maxX = p[0];
		if (p[1] < minY)
			minY = p[1];
		if (p[1] > maxY)
			maxY = p[1];
	}
	BoundingSquare b;
	b.p1 = wcl::Vector(minX, minY);
	b.p2 = wcl::Vector(maxX, maxY);
	return b;
}

void Gestures::addTemplate(PointList points, std::string name)
{
	GestureTemplate t;
	t.points = points;
	t.name = name;
	templates.push_back(t);
}


