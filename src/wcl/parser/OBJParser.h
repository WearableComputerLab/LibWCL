/*-
 * Copyright (c) 2009 Benjamin Close <Benjamin.Close@clearchain.com>
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
#ifndef WCL_PARSER_OBJPARSER_H
#define WCL_PARSER_OBJPARSER_H

#include <iostream>
#include <wcl/parser/Parser.h>
#include <wcl/geometry/OBJGeometry.h>

namespace wcl {

/**
 * An OBJParser that understands the .obj File format. The parser
 * builds up a geometry object consisting of all elements of the OBJ
 * file format. This can be obtained via the call to getGeometry().
 * The memory for this object is maintained by the OBJParser. Destroying
 * the class instance frees up the memory associated with the Geometry
 */
class OBJParser: public Parser
{
    public:
        OBJParser(std::istream &);
        ~OBJParser();

        void parse();
        void print();

        const OBJGeometry &getGeometry();

        void addMaterialLibrary( const std::string &);
        void addMaterial(const std::string &);
        void addGroup(const std::string &);
        void addVertex(const double, const double, const double);
        void addVertexTexture(const double, const double);
        void addNormal(const double, const double, const double);
        void addShaderGroup( const std::string &);
	//Add a triangle
        void addFace(const double, const double, const double,
                     const double, const double, const double,
                     const double, const double, const double);
	//Add a quad
        void addFace(const double, const double, const double,
                     const double, const double, const double,
                     const double, const double, const double,
		     const double, const double, const double);
        void useMaterial(const std::string &);
        void setMaterialDiffuse(const double, const double, const double);
        void setMaterialAmbience(const double, const double, const double);
        void setMaterialSpecular(const double, const double, const double);
        void setMaterialOpacity(const double, const double, const double);
        void setMaterialRefractionIndex( const double );
        void setMaterialSpecularExponent(const double );
        void setMaterialDiffuseMap(const std::string &);
        void setMaterialIlluminationGroup( const int );

        static int scanner(OBJParser *);

    private:
        std::istream *input;
        OBJGeometry data;
        OBJMaterial *material;
        OBJGroup *group;

        OBJParser(const OBJParser &); /* NIMP */

        bool hasGroup();
        bool hasMaterial();
};

};

#endif
