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
	};

	struct WCL_API OBJMaterial
	{
                enum _valid { M_NONE=0,
                       M_DIFFUSE=1,
                       M_AMBIENT=2,
                       M_SPECULAR=4,
                       M_OPACITY=8,
                       M_REFRACTION=16,
                       M_SPECULAREXP=32,
		       M_ILLUMGROUP=64,
		       M_EMISSIVE=128,

                       // Valid maps
                       M_DIFFUSE_MAP=1,
                       M_AMBIENT_MAP=2,
                       M_SPECULAR_MAP=4,
                       M_ALPHA_MAP=8,
                       M_BUMP_MAP=16
                        };
                uint64_t valid;
                uint32_t maps;
                std::string name;           // newmtl [NAME]
                Vector diffuse;             // Kd
                Vector ambient;             // Ka
                Vector specular;            // Ks
                double opacity;             // Tr|Tf|d
                double refractionIndex;     // Ni
                double specularExp;         // Ns
		uint32_t illumGroup;	    // illum
                std::string diffuseMap;     // map_Kd
                std::string ambientMap;     // map_Ka
                std::string specularMap;    // map_Ks
                std::string alphaMap;	    // map_d
                std::string bumpMap;	    // map_bump | bump
		Vector emissive;	    // Ke
	};

	struct OBJSmoothing; //forward dec

	struct WCL_API OBJFace
	{
                OBJMaterial *material;
		OBJSmoothing *smoothing;
		std::vector<OBJVertex *> verts;
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
                std::vector<wcl::OBJMaterial *> materials;
                std::vector<wcl::OBJGroup *>groups;
		std::vector<wcl::OBJSmoothing *>smoothing;
		std::map<std::string, wcl::OBJMaterial *> materialMap;
                std::map<std::string, wcl::OBJGroup *>groupsMap;
		std::map<std::string, wcl::OBJSmoothing *>smoothingMap;
	};
}

#endif

