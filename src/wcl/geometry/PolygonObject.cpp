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
			wcl::Vector* v = new wcl::Vector((*(*io))[0], (*(*io))[1], (*(*io))[2]);
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

};

