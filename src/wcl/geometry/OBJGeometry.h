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

#include <map>
#include <string>
#include <vector>
#include <wcl/api.h>
#include <wcl/maths/Vector.h>

namespace wcl
{
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
                       M_SPECULAREXP=32};
                int valid;
                std::string name;          // newmtl [NAME]
                Vector diffuse;             // Kd
                Vector ambient;             // Ka
                Vector specular;            // Ks
                Vector opacity;             // Tf
                double refractionIndex;     // Ni
                double specularExp;         // Ns
                // Diffusionmap???
	};

	struct WCL_API OBJFace
	{
                OBJMaterial *material;
		std::vector<OBJVertex *> verts;
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
		std::map<std::string, wcl::OBJMaterial *> materialmap;
                std::map<std::string, wcl::OBJGroup *>groupsmap;
	};
}

#endif

