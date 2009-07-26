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
#include <math.h>

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
		std::vector<wcl::Vertex*>::const_iterator i;
		for (i = object.vertexList.begin(); i < object.vertexList.end(); ++i)
		{
			//hows that for some indirection!
			//basically we need a Vector (not a pointer) so we can use the
			//array acess operators
			wcl::Vertex* v = new Vertex(*(*i));
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
		std::vector<wcl::Vertex*>::iterator i;
		for (i = vertexList.begin(); i < vertexList.end(); ++i)
		{
			wcl::Vertex* v = (*i);
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

		std::vector<wcl::Vertex*>::const_iterator io;
		for (io = object.vertexList.begin(); io < object.vertexList.end(); ++io)
		{
			//hows that for some indirection!
			//basically we need a Vector (not a pointer) so we can use the
			//array acess operators
			wcl::Vertex* v = new wcl::Vertex(*(*io));
			vertexList.push_back(v);
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
		std::vector<wcl::Vertex*>::iterator i;
		for (i = vertexList.begin(); i < vertexList.end(); ++i)
		{
			wcl::Vertex* v = (*i);
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
		std::vector<wcl::Vertex*>::const_iterator it;
		for (it = vertexList.begin();it<vertexList.end();++it)
		{
			//let the addPoint method handle the logic
			b.addPoint((*it)->position);
		}

		//return the box
		return b;
	}


	void PolygonObject::classifyFaces(const PolygonObject& object)
	{
		Face* f;
		std::vector<Face*>::iterator it;
		for (it = faceList.begin(); it < faceList.end(); ++it)
		{
			f = *it;
			f->v1->adjacentVerts.insert(f->v2);
			f->v1->adjacentVerts.insert(f->v3);
			f->v2->adjacentVerts.insert(f->v1);
			f->v2->adjacentVerts.insert(f->v3);
			f->v3->adjacentVerts.insert(f->v1);
			f->v3->adjacentVerts.insert(f->v2);
		}

		for (it = faceList.begin(); it < faceList.end(); ++it)
		{
			f = *it;
			if (f->quickClassify() == false)
			{
				object.raytraceClassify(*f);

				if (f->v1->status == UNKNOWN)
					f->v1->mark(f->status);
				if (f->v2->status == UNKNOWN)
					f->v2->mark(f->status);
				if (f->v3->status == UNKNOWN)
					f->v3->mark(f->status);
			}
		}
	}

	void PolygonObject::raytraceClassify(Face& f) const
	{
		wcl::Vector p0;
		p0.setSize(3);
		p0[0] = (f.v1->position[0] + f.v2->position[0] + f.v3->position[0]) /3.0;
		p0[1] = (f.v1->position[1] + f.v2->position[1] + f.v3->position[1]) /3.0;
		p0[2] = (f.v1->position[2] + f.v2->position[2] + f.v3->position[2]) /3.0;
		Line ray(f.getNormal(), p0);

		bool success;
		double dotProduct, distance;
		wcl::Vector intersectionPoint;
		Face* closestFace = NULL;
		double closestDistance;

		do
		{
			success = true;
			closestDistance = std::numeric_limits<double>::max();

			std::vector<Face*>::const_iterator it;
			for (it = faceList.begin(); it < faceList.end(); ++it)
			{
				Face* face = *it;
				dotProduct = face->getNormal().dot(ray.getDirection());
				intersectionPoint = ray.intersect(f.getPlane());

				//FIXME check for parallelness
				if (true)
				{
					distance = (ray.getPosition()-intersectionPoint).normal();
					if (fabs(distance)<TOL && fabs(dotProduct)<TOL)
					{
						ray.perturbDirection();
						success = false;
						break;
					}
					if (fabs(distance)<TOL && fabs(dotProduct)>TOL)
					{
						if (face.hasPoint(intersectionPoint))
						{
							closestFace = face;
							closestDistance = 0;
							break;
						}
					}
					else if (fabs(dotProduct)>TOL && distance > TOL)
					{
						if (distance < closestDistance)
						{
							if (face.hasPoint(intersectionPoint))
							{
								closestDistance = distance;
								closestFace = face;
							}
						}
					}
				}
			}
		}
		while (!success);
		if (closestFace == NULL)
		{
			status = OUTSIDE;
		}
		else
		{
			dotProduct = closestFace->getNormal().dot(ray.getDirection());

			if (fabs(closestDistance) < TOL)
			{
				if (dotProduct > TOL)
				{
					status = SAME;
				}
				else if (dotProduct < -TOL)
				{
					status = OPPOSITE;
				}
			}
			else if (dotProduct > TOL)
			{
				status = INSIDE;
			}
			else if (dotProduct < -TOL)
			{
				status = OUTSIDE;
			}
		}
	}

	void PolygonObject::splitFace(int index, const LineSegment& segment1, const LineSegment& segment2)
	{
		Vertex* startPosvertex, endPosVertex;
		wcl::Vector startPos, endPos;
		double startDist, endDist;
		LineSegment::LineIntersectType startType, endType, middleType;

		Face* face = faceList[index];
		Vertex* startVertex = segment1.startVert;
		Vertex* endVertex = segment1.endVert;

		// setup the starting values
		if (segment2.startDistance > segment1.startDistance + TOL)
		{
			startDist = segment2.startDistance;
			startType = segment1.middleType;
			startPos = segment2.startPos;
		}
		else
		{
			startDist = segment1.startDistance;
			startType = segment1.startType;
			startPos = segment1.startPos;
		}

		// setup the ending values
		if (segment2.endDistance < segment1.endDistance - TOL)
		{
			endDist = segment2.endDistance;
			endType = segment1.middleType;
			endPos = segment2.endPos;
		}
		else
		{
			endDist = segment1.endDistance;
			endType = segment1.endType;
			endPos = segment1.endPos;
		}

		middleType = segment1.middleType;

		// see whether it is a boundary
		if (startType == LineSegment::VERTEX)
		{
			startVertex->setStatus(BOUNDARY);
		}
		if (endType == LineSegment::VERTEX)
		{
			endVertex->setStatus(BOUNDARY);
		}

		/*
		 * Now the rest of this function is messy because there are a lot of 
		 * cases to consider.
		 */

		//Vertex - nothing - vertex
		if (startType == LineSegment::VERTEX && endType == LineSegment::VERTEX)
			return;

		if (middleType == LineSegment::EDGE)
		{
			//decide what edge we need to split...
			int splitEdge;
			if ((startVertex == face->v1 && endVertex == face->v2) || (startVertex == face->v2 && endVertex == face->v1))
				splitEdge = 1;
			else if ((startVertex == face->v2 && endVertex == face->v3) || (startVertex == face->v3 && endVertex == face->v2))
				splitEdge = 2;
			else
				splitEdge = 3;

			//vertex-edge-edge
			if (startType == LineSegment::VERTEX)
			{
				breakFaceInTwo(index, endPos, splitEdge);
				return;
			}
			else if (endType == LineSegment::VERTEX)
			{
				breakFaceInTwo(index, endPos, splitEdge);
			}
			else
			{
				if ((startVertex == face->v1 && endVertex == face->v2) || 
					(startVertex == face->v2 && endVertex == face->v3) ||
					(startVertex == face->v3 && endVertex == face->v1))
				{
					breakFaceInThree(index, startPos, endPos, splitEdge);
				}
				else
				{
					breakFaceInThree(index, endPos, startPos, splitEdge);
				}
			}
			return;
		}
		//vertex-face-edge
		else if (startType == LineSegment::VERTEX && endType == LineSegment::EDGE)
		{
			breakFaceInTwo(index, endPos, endVertex);
		}
		//edge face vertex
		else if (startType == LineSegment::EDGE && endType == LineSegment::VERTEX)
		{
			breakFaceInTwo(index, startPos, startVertex);
		}
		//vertex face face
		else if (startType == LineSegment::VERTEX && endType == LineSegment::FACE)
		{
			breakFaceInThree(index, endPos, startVertex);
		}
		//face face vertex
		else if (startType == LineSegment::FACE && endType == LineSegment::VERTEX)
		{
			breakFaceInThree(index, startPos, endVertex);
		}
		//edge face edge
		else if (startType == LineSegment::EDGE && endType == LineSegment::EDGE)
		{
			breakFaceInThree(index, startPos, endPos, startVertex, endVertex);
		}
		//edge face face
		else if (startType == LineSegment::EDGE && endType == LineSegment::FACE)
		{
			breakFaceInFour(index, startPos, endPos, startVertex);
		}
		//face face edge
		else if (startType == LineSegment::FACE && endType == LineSegment::EDGE)
		{
			breakFaceInFour(index, endPos, startPos, endVertex);
		}
		// face face face
		else if (startType == LineSegment::FACE && endType == LineSegment::FACE)
		{
			wcl::Vector segmentVector(startPos[0] - endPos[0], startPos[1] - endPos[1], startPos[2] - endPos[2]);

			if (fabs(segmentVector[0]) < TOL && fabs(segmentVector[1]) < TOL && fabs(segmentVector[2]) < TOL)
			{
				breakFaceInThree(index, startPos);
				return;
			}

			int linedVertex;
			wcl::Vector linedVertexPos;
			wcl::Vector vertexVector(endPos[0] - (face->v1->position[0]), endPos[1] - (face->v1->position[1]), endPos[2] - (face->v1->position[2]));
			vertexVector = vertexVector.unit();
			double dot1 = fabs(segmentVector.dot(vertexVector));
			vertexVector = wcl::Vector(endPos[0] - (face->v2->position[0]), endPos[1] - (face->v2->position[1]), endPos[2] - (face->v2->position[2]));
			vertexVector = vertexVector.unit();
			double dot2 = fabs(segmentVector.dot(vertexVector));
			vertexVector = wcl::Vector(endPos[0] - (face->v3->position[0]), endPos[1] - (face->v3->position[1]), endPos[2] - (face->v3->position[2]));
			vertexVector = vertexVector.unit();
			double dot3 = fabs(segmentVector.dot(vertexVector));
			if (dot1 > dot2 && dot1 > dot3)
			{
				linedVertex = 1;
				linedVertexPos = face->v1->position;
			}
			else if (dot2 > dot3 && dot2 > dot1)
			{
				linedVertex = 2;
				linedVertexPos = face->v2->position;
			}
			else
			{
				linedVertex = 3;
				linedVertexPos = face->v3->position;
			}

			if ((linedVertexPos - startPos).normal() > (linedVertexPos - endPos).normal())
			{
				breakFaceInFive(index, startPos, endPos, linedVertex);
			}
			else
			{
				breakFaceInFive(index, endPos, startPos, linedVertex);
			}
		}
	}

	Vertex* PolygonObject::addVertex(const wcl::Vector& position, IntersectStatus v)
	{
		Vertex* vertex = new Vertex(position, v);
		std::vector<wcl::Vertex*>::iterator it;
		for (it = vertexList.begin(); it < vertexList.end(); ++it)
		{
			if (*(*it) == *vertex)
				break;
		}
		if (it == vertexList.end())
		{
			vertexList.push_back(vertex);
			return vertex;
		}
		else
		{
			delete vertex;
			return *it;
		}
	}

	Face* PolygonObject::addFace(Vertex* v1, Vertex* v2, Vertex* v3)
	{
		if (!(*v1 == *v2 || *v1 == *v3 || *v2 == *v3))
		{
			Face* f = new Face(v1, v2, v3);
			if (f->getArea() > TOL)
			{
				faceList.push_back(f);
				return f;
			}
			else
			{
				delete f;
			}
		}
		return NULL;
	}


	void PolygonObject::breakFaceInTwo(int index, wcl::Vector newPos, int splitEdge)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v = addVertex(newPos, BOUNDARY);

		switch(splitEdge)
		{
			case 1:
				addFace(f->v1, v, f->v3);
				addFace(v, f->v2, f->v3);
				break;
			case 2:
				addFace(f->v2, v, f->v1);
				addFace(v, f->v3, f->v1);
				break;
			case 3:
				addFace(f->v3, v, f->v2);
				addFace(v, f->v1, f->v2);
				break;
			default:
				throw std::string("EDGE MUST BE LESS THAN 3");
				break;
		}
	}

	void PolygonObject::breakFaceInTwo(int index, wcl::Vector newPos, Vertex* endVertex)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v = addVertex(newPos, BOUNDARY);

		if (*endVertex == *(f->v1))
		{
			addFace(f->v1, v, f->v3);
			addFace(v, f->v2, f->v3);
		}
		else if (*endVertex == *(f->v2))
		{
			addFace(f->v2, v, f->v1);
			addFace(v, f->v3, f->v1);
		}
		else
		{
			addFace(f->v3, v, f->v2);
			addFace(v, f->v1, f->v2);
		}
	}


	void PolygonObject::breakFaceInThree(int index, wcl::Vector newPos1, wcl::Vector newPos2, int splitEdge)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v1 = addVertex(newPos1, BOUNDARY);
		Vertex* v2 = addVertex(newPos2, BOUNDARY);

		switch(splitEdge)
		{
			case 1:
				addFace(f->v1, v1, f->v3);
				addFace(v1, v2, f->v3);
				addFace(v2, f->v2, f->v3);
				break;
			case 2:
				addFace(f->v2, v1, f->v1);
				addFace(v1, v2, f->v1);
				addFace(v2, f->v3, f->v1);
				break;
			case 3:
				addFace(f->v3, v1, f->v2);
				addFace(v1, v2, f->v2);
				addFace(v2, f->v1, f->v2);
				break;
			default:
				throw std::string("EDGE MUST BE LESS THAN 3");
				break;
		}
	}

	void PolygonObject::breakFaceInThree(int index, wcl::Vector newPos, Vertex* endVertex)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v = addVertex(newPos, BOUNDARY);

		if (*endVertex == *(f->v1))
		{
			addFace(f->v1, f->v2, v);
			addFace(f->v2, f->v3, v);
			addFace(f->v3, f->v1, v);
		}
		else if (*endVertex == *(f->v2))
		{
			addFace(f->v2, f->v3, v);
			addFace(f->v3, f->v1, v);
			addFace(f->v1, f->v2, v);
		}
		else
		{
			addFace(f->v3, f->v1, v);
			addFace(f->v1, f->v2, v);
			addFace(f->v2, f->v3, v);
		}
	}

	void PolygonObject::breakFaceInThree(int index, wcl::Vector newPos1, wcl::Vector newPos2, Vertex* startVertex, Vertex* endVertex)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v1 = addVertex(newPos1, BOUNDARY);
		Vertex* v2 = addVertex(newPos2, BOUNDARY);

		if (*startVertex == *(f->v1) && *endVertex == *(f->v2))
		{
			addFace(f->v1, v1, v2);
			addFace(f->v1, v2, f->v3);
			addFace(v1, f->v2, v2);
		}
		else if (*startVertex == *(f->v2) && *endVertex == *(f->v1))
		{
			addFace(f->v1, v2, v1);
			addFace(f->v1, v1, f->v3);
			addFace(v2, f->v2, v1);
		}
		else if (*startVertex == *(f->v2) && *endVertex == *(f->v3))
		{
			addFace(f->v2, v1, v2);
			addFace(f->v2, v2, f->v1);
			addFace(v1, f->v3, v2);
		}
		else if (*startVertex == *(f->v3) && *endVertex == *(f->v2))
		{
			addFace(f->v2, v2, v1);
			addFace(f->v2, v1, f->v1);
			addFace(v2, f->v3, v1);
		}
		else if (*startVertex == *(f->v3) && *endVertex == *(f->v1))
		{
			addFace(f->v3, v1, v2);
			addFace(f->v3, v2, f->v2);
			addFace(v1, f->v1, v2);
		}
		else
		{
			addFace(f->v3, v2, v1);
			addFace(f->v3, v1, f->v2);
			addFace(v2, f->v1, v1);
		}
	}


	void PolygonObject::breakFaceInThree(int index, wcl::Vector newPos)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v = addVertex(newPos, BOUNDARY);

		addFace(f->v1, f->v2, v);
		addFace(f->v2, f->v3, v);
		addFace(f->v3, f->v1, v);
	}


	void PolygonObject::breakFaceInFour(int index, wcl::Vector newPos1, wcl::Vector newPos2, Vertex* endVertex)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v1 = addVertex(newPos1, BOUNDARY);
		Vertex* v2 = addVertex(newPos2, BOUNDARY);

		if (*endVertex == *(f->v1))
		{
			addFace(f->v1, v1, v2);
			addFace(v1, f->v2, v2);
			addFace(f->v2, f->v3, v2);
			addFace(f->v3, f->v1, v2);
		}
		else if (*endVertex == *(f->v2))
		{
			addFace(f->v2, v1, v2);
			addFace(v1, f->v3, v2);
			addFace(f->v3, f->v1, v2);
			addFace(f->v1, f->v2, v2);
		}
		else
		{
			addFace(f->v3, v1, v2);
			addFace(v1, f->v1, v2);
			addFace(f->v1, f->v2, v2);
			addFace(f->v2, f->v3, v2);
		}
	}

	void PolygonObject::breakFaceInFive(int index, wcl::Vector newPos1, wcl::Vector newPos2, int linedVertex)
	{
		Face* f = faceList[index];
		faceList.erase(faceList.begin() + index);

		Vertex* v1 = addVertex(newPos1, BOUNDARY);
		Vertex* v2 = addVertex(newPos2, BOUNDARY);

		double cont = 0;
		if (linedVertex == 1)
		{
			addFace(f->v2, f->v3, v1);
			addFace(f->v2, v1, v2);
			addFace(f->v3, v2, v1);
			addFace(f->v2, v2, f->v1);
			addFace(f->v3, f->v1, v2);
		}
		else if (linedVertex == 2)
		{
			addFace(f->v3, f->v1, v1);
			addFace(f->v3, v1, v2);
			addFace(f->v1, v2, v1);
			addFace(f->v3, v2, f->v2);
			addFace(f->v1, f->v2, v2);
		}
		else
		{
			addFace(f->v1, f->v2, v1);
			addFace(f->v1, v1, v2);
			addFace(f->v2, v2, v1);
			addFace(f->v1, v2, f->v3);
			addFace(f->v2, f->v3, v2);
		}
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
									Line line = face1Plane.intersect(face2Plane);
									LineSegment segment1(line, *face1, signFace1Vert1, signFace1Vert2, signFace1Vert3);
									LineSegment segment2(line, *face2, signFace2Vert1, signFace2Vert2, signFace2Vert3);

									if (segment1.intersect(segment2))
									{
										int lastNumFaces = faceList.size();
										this->splitFace(i, segment1, segment2);

										if (face1 != faceList[i])
										{
											if (*face1 == *(faceList[faceList.size() -1]))
											{
												if (i != faceList.size() -1)
												{
													faceList.erase(faceList.end() - 1);
													faceList.insert(faceList.begin() + i, face1);
												}
												else
												{
													continue;
												}
											}
											else
											{
												i--;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
};

