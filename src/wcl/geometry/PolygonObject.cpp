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

	void PolygonObject::splitFace(int index, const LineSegment& segment1, const LineSegment& segment2)
	{
		Vertex* startPosvertex, endPosVertex;
		wcl::Vector startPos, endPos;
		double startDist, endDist;
		LineIntersectType startType, endType, middleType;

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
			startVertex.setStatus(Vertex::BOUNDARY);
		}
		if (endType == LineSegment::VERTEX)
		{
			endVertex.setStatus(Vertex::BOUNDARY);
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
			if ((startVertex == face.v1 && endVertex == face.v2) || (startVertex == face.v2 && endVertex == face.v1))
				splitEdge = 1;
			else if ((startVertex == face.v2 && endVertex == face.v3) || (startVertex == face.v3 && endVertex == face.v2))
				splitEdge = 2;
			else
				splitEdge = 3;

			//vertex-edge-edge
			if (startType == LineSegment::VERTEX)
			{
				breakFaceInTwo(facePos, endPos, splitEdge);
				return;
			}
			else if (endType == LineSegment::VERTEX)
			{
				breakFaceInTwo(facePos, endPos, splitEdge);
			}
			else
			{
				if ((startVertex == face.v1 && endVertex == face.v2) || 
					(startVertex == face.v2 && endVertex == face.v3) ||
					(startVertex == face.v3 && endVertex == face.v1))
				{
					breakFaceInThree(facePos, startPos, endPos, splitEdge);
				}
				else
				{
					breakFaceInThree(facePos, endPos, startPos, splitEdge);
				}
			}
			return;
		}
		//vertex-face-edge
		else if (startType == LineSegment::VERTEX && endType == LineSegment::EDGE)
		{
			breakFaceInTwo(facePos, endPos, endVertex);
		}
		//edge face vertex
		else if (startType == LineSegment::EDGE && endType == LineSegment::VERTEX)
		{
			breakFaceInTwo(facePos, startPos, startVertex);
		}
		//vertex face face
		else if (startType == LineSegment::VERTEX && endType == LineSegment::FACE)
		{
			breakFaceInThree(facePos, endPos, startVertex);
		}
		//face face vertex
		else if (startType == LineSegment::FACE && endType == LineSegment::VERTEX)
		{
			breakFaceInThree(facePos, startPos, endVertex);
		}
		//edge face edge
		else if (startType == LineSegment::EDGE && endType == LineSegment::EDGE)
		{
			breakFaceInThree(facePos, startPos, endPos, startVertex, endVertex);
		}
		//edge face face
		else if (startType == LineSegment::EDGE && endType == LineSegment::FACE)
		{
			breakFaceInFour(facepos, startPos, endPos, startVertex);
		}
		//face face edge
		else if (startType == LineSegment::FACE && endType == LineSegment::EDGE)
		{
			breakFaceInFour(facePos, endPos, startPos, endVertex);
		}
		// face face face
		else if (startType == LineSegment::FACE && endType == LineSegment::FACE)
		{
			wcl::Vector segmentVector(startPos[0] - endPos[0], startPos[1] - endPos[1], startPos[2] - endPos[2]);

			if (abs(segmentVector[0]) < TOL && abs(segmentVector[1]) < TOL && abs(segmentVector[2]) < TOL)
			{
				breakFaceInThree(facePos, startPos);
				return;
			}

			int linedVertex;
			wcl::Vector linedVertexPos;
			wcl::Vector vertexVector(endPos[0] - face.v1[0], endPos[1] - face.v1[1], endPos[2] - face.v1[2]);
			vertexVector = vertexVector.unit();
			double dot1 = abs(segmentVector.dot(vertexVector));
			vertexVector = wcl::Vector(endPos[0] - face.v2[0], endPos[1] - face.v2[1], endPos[2] - face.v2[2]);
			vertexVector = vertexVector.unit();
			double dot2 = abs(segmentVector.dot(vertexVector));
			vertexVector = wcl::Vector(endPos[0] - face.v3[0], endPos[1] - face.v3[1], endPos[2] - face.v3[2]);
			vertexVector = vertexVector.unit();
			double dot3 = abs(segmentVector.dot(vertexVector));
			if (dot1 > dot2 && dot1 > dot3)
			{
				linedVertex = 1;
				linedVertexPos = face.v1.position;
			


			


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
									LineSegment segment1(line, face1, signFace1Vert1, signFace1Vert2, signFace1Vert3);
									LineSegment segment2(line, face2, signFace2Vert1, signFace2Vert2, signFace2Vert3);

									if (segment1.intersect(segment2))
									{
										int lastNumFaces = faceList.size();
										this.splitFace(i, segment1, segment2);
									

};

