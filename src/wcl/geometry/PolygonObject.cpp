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


#include <limits>

#include "PolygonObject.h"


namespace wcl
{

	PolygonObject::PolygonObject(std::string id) 
	{
		this->id = id;
	}

	PolygonObject::PolygonObject(const PolygonObject& object)
	{
		//copy over the vertices
		std::vector<wcl::Vector*>::const_iterator i;
		for (i = object.vertexList.begin(); i < object.vertexList.end(); ++i)
		{
			//hows that for some indirection!
			//basically we need a Vector (not a pointer) so we can use the
			//array acess operators
			wcl::Vector* v = new wcl::Vector((*(*i))[0], (*(*i))[1], (*(*i))[2]);
			vertexList.push_back(v);
		}

		//copy the faces
		std::vector<Face*>::const_iterator j;
		for (j = object.faceList.begin(); j < object.faceList.end(); ++j)
		{
			//use the brand new copy constructor 
			Face* f = new Face(*(*j));
			faceList.push_back(f);
		}

		this->id = object.id;
	}

	const PolygonObject& PolygonObject::operator=(const PolygonObject& object)
	{
		if (this == &object)
		{
			return *this;
		}

		//delete whatever we currently have
		std::vector<wcl::Vector*>::iterator i;
		for (i = vertexList.begin(); i < vertexList.end(); ++i)
		{
			wcl::Vector* v = (*i);
			delete v;
		}
		std::vector<Face*>::const_iterator j;
		for (j = faceList.begin(); j < faceList.end(); ++j)
		{
			Face* f = (*j);
			delete f;
		}

		vertexList.clear();
		faceList.clear();

		std::vector<wcl::Vector*>::const_iterator io;
		for (io = object.vertexList.begin(); io < object.vertexList.end(); ++io)
		{
			//hows that for some indirection!
			//basically we need a Vector (not a pointer) so we can use the
			//array acess operators
			wcl::Vector* v = new wcl::Vector((*(*io))[0], (*(*io))[1], (*(*io))[2]); vertexList.push_back(v);
		}

		//copy the faces
		std::vector<Face*>::const_iterator jo;
		for (jo = object.faceList.begin(); jo < object.faceList.end(); ++jo)
		{
			//use the brand new copy constructor 
			Face* f = new Face(*(*j));
			faceList.push_back(f);
		}


		return (*this);
	}

	PolygonObject::~PolygonObject()
	{
		std::vector<wcl::Vector*>::iterator i;
		for (i = vertexList.begin(); i < vertexList.end(); ++i)
		{
			wcl::Vector* v = (*i);
			delete v;
		}
		std::vector<Face*>::iterator j;
		for (j = faceList.begin(); j < faceList.end(); ++j)
		{
			Face* f = (*j);
			delete f;
		}
	}


	wcl::BoundingBox PolygonObject::getBoundingBox() const
	{
		//create a bounding box
		BoundingBox b;

		//loop over all points
		std::vector<wcl::Vector*>::const_iterator it;
		for (it = vertexList.begin();it<vertexList.end();++it)
		{
			//let the addPoint method handle the logic
			b.addPoint(*(*it));
		}

		//return the box
		return b;
	}


	void PolygonObject::splitFaces(const PolygonObject& object)
	{
		Face* face1;
		Face* face2;
		Plane face1Plane;
		Plane face2Plane;
		double distFace1Vert1, distFace1Vert2, distFace1Vert3, distFace2Vert1, distFace2Vert2, distFace2Vert3;
		int signFace1Vert1, signFace1Vert2, signFace1Vert3, signFace2Vert1, signFace2Vert2, signFace2Vert3;
		int numFacesBefore = faceList.size();
		int numFacesSize = faceList.size();
		int facesIgnored = 0;

		if (this->getBoundingBox().overlaps(object.getBoundingBox()))
		{
			for (int i = 0; i<this->faceList.size(); ++i)
			{
				face1 = faceList[i];

				if (face1->getBoundingBox().overlaps(object.getBoundingBox()))
				{
					for (int j=0; j<object.faceList.size(); ++j)
					{
						face2 = object.faceList[j];
						if (face1->getBoundingBox().overlaps(face2->getBoundingBox()))
						{
							face1Plane = face1->getPlane();
							face2Plane = face2->getPlane();

							distFace1Vert1 = face2Plane.distanceFrom(face1->v1->position);
							distFace1Vert2 = face2Plane.distanceFrom(face1->v2->position);
							distFace1Vert3 = face2Plane.distanceFrom(face1->v3->position);

							signFace1Vert1 = (distFace1Vert1>TOL ? 1 : (distFace1Vert1<-TOL? -1 : 0));
							signFace1Vert2 = (distFace1Vert2>TOL ? 1 : (distFace1Vert2<-TOL? -1 : 0));
							signFace1Vert3 = (distFace1Vert3>TOL ? 1 : (distFace1Vert3<-TOL? -1 : 0));

							if (!(signFace1Vert1 == signFace1Vert2 && signFace1Vert2==signFace1Vert3))
							{
								distFace2Vert1 = face1Plane.distanceFrom(face2->v1->position);
								distFace2Vert2 = face1Plane.distanceFrom(face2->v2->position);
								distFace2Vert3 = face1Plane.distanceFrom(face2->v3->position);

								signFace2Vert1 = (distFace2Vert1>TOL ? 1 : (distFace2Vert1<-TOL? -1 : 0));
								signFace2Vert2 = (distFace2Vert2>TOL ? 1 : (distFace2Vert2<-TOL? -1 : 0));
								signFace2Vert3 = (distFace2Vert3>TOL ? 1 : (distFace2Vert3<-TOL? -1 : 0));

								if (!(signFace2Vert1==signFace2Vert2 && signFace2Vert2 == signFace2Vert3))
								{
									Line p = face1Plane.intersect(face2Plane);
									

};

