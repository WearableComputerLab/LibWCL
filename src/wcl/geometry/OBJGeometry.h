/*-
 * Copyright (c) 2009 Michael Marner <michael@20papercups.net>
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

#ifndef OBJ_GEOMETRY
#define OBJ_GEOMETRY

#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include <wcl/api.h>
#include <wcl/maths/Vector.h>

#include "MaterialLibrary.h"

namespace wcl
{
	/**
	 * An OBJVertex represents an index into the Vertex/normal/uv
	 * vectors. It is possible that a point in an obj file does not
	 * have a normal or uv associated with it. In that case the value
	 * for normalIndex/uvIndex will be -1.
	 */
	struct WCL_API OBJVertex
	{
		int pointIndex;
		int normalIndex;
		int uvIndex;

        OBJVertex() : pointIndex(-1), normalIndex(-1), uvIndex(-1) {}
	};


	struct OBJSmoothing; //forward dec

	struct WCL_API OBJFace
	{
        OBJMaterial *material;
        OBJSmoothing *smoothing;
        std::vector<OBJVertex*> verts;
	};

	struct WCL_API OBJSmoothing
	{
		std::string name;
		std::vector<OBJFace *> faces;
	};

        struct WCL_API OBJGroup
        {
                std::string name;
                std::vector<OBJFace *> faces;
        };

	struct WCL_API OBJGeometry
	{
        std::vector<wcl::Vector> points;
        std::vector<wcl::Vector> normals;
        std::vector<wcl::Vector> texcoords;
        std::vector<wcl::OBJFace* > faces;
        std::vector<wcl::OBJGroup *>groups;
        std::vector<wcl::OBJGroup *>objects;
        std::vector<wcl::OBJSmoothing *>smoothing;
        std::map<std::string, wcl::OBJGroup *>groupsMap;
        std::map<std::string, wcl::OBJGroup *>objectsMap;
        std::map<std::string, wcl::OBJSmoothing *>smoothingMap;
        MaterialLibrary materials;

        virtual ~OBJGeometry();

        OBJGeometry& operator=(const OBJGeometry& rhs);
        OBJGeometry(const OBJGeometry& rhs);
        OBJGeometry() {}

        void clear();
	};
}

#endif

