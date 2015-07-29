/*-
 * Copyright (c) 2015 Michael Marner <michael@20papercups.net>
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
#include <stack>
#include <string>
#include <sstream>
#include <wcl/parser/ParserException.h>
#include <wcl/geometry/OBJGeometry.h>


namespace wcl {


    /**
     * An OBJParser that understands the .obj File format. The parser
     * builds up a geometry object consisting of all elements of the OBJ
     * file format. This can be obtained via the call to getGeometry().
     * The memory for this object is maintained by the OBJParser. Destroying
     * the class instance frees up the memory associated with the Geometry
     */
    class WCL_API OBJParser
    {
        public:
            enum LineType {
                // Found in OBJ file
                FACE,
                GROUP,
                MTLLIB,
                NOP,
                NORMAL,
                OBJECT,
                TEXCOORD,
                USEMTL,
                VERTEX,
            };

            wcl::OBJGeometry* parse(const std::string& filename);

            void parseLine(const std::string& line, OBJGeometry& obj);
            LineType getLineType(const std::string& token) const;
            wcl::Vector parseVector(std::istringstream& tokens) const;
            OBJVertex parseVertex(const std::string& token);
            
        private:

            // the base directory. We need this because OBJ assumes all assets
            // (textures, material libraries, etc) reside in the same
            // directory, which may not be the CWD!
            std::string base;
            OBJGeometry obj;
            OBJMaterial* currentMaterial;
            OBJGroup* currentGroup;
            OBJGroup* currentObject;
            OBJSmoothing* currentSmoothing;



    };

};

#endif
