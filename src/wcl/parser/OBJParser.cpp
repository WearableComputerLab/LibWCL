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


#include "OBJParser.h"

#include <fstream>

namespace wcl {


    wcl::OBJGeometry* OBJParser::parse(const std::string& filename) {
        obj.clear();
        currentMaterial = NULL;
        currentGroup = NULL;
        currentSmoothing = NULL;

        using namespace std;

        ifstream in(filename.c_str());
        if (in.fail()) {
            throw ParserException("Could not open file");
        }

        OBJGeometry obj;

        unsigned lineNumber = 1;

        try {
            while (!in.eof() && in.good()) {

                string line;
                getline(in, line);
                parseLine(line, obj);
                lineNumber++;
            }

            return new OBJGeometry(obj);
        }
        catch(ParserException& e) {
            stringstream ss;
            ss << e.what() << " on line " << lineNumber;
            throw ParserException(ss.str().c_str());
        }
    }


    OBJParser::LineType OBJParser::getLineType(const std::string& token) const {

        if (token == "f") 
            return FACE;
        if (token == "g") 
            return GROUP;
        if (token == "mtllib") 
            return MTLLIB;
        if (token == "vn") 
            return NORMAL;
        if (token == "o") 
            return OBJECT;
        if (token == "vt") 
            return TEXCOORD;
        if (token == "usemtl") 
            return USEMTL;
        if (token == "v") 
            return VERTEX;


        std::stringstream ss;
        ss << "Unknown line type: " << token;
        throw ParserException(ss.str().c_str());
    }

    void OBJParser::parseLine(const std::string& line, OBJGeometry& obj) {
        using namespace std;

        istringstream tokens(line);

        string lineType;
        tokens >> lineType;
        switch (getLineType(lineType)) {
            case VERTEX:
                {
                wcl::Vector v = parseVector(tokens);
                obj.points.push_back(v);
                }
                break;
            case NORMAL: 
                {
                wcl::Vector v = parseVector(tokens);
                obj.normals.push_back(v);
                }
                break;
            case TEXCOORD: 
                {
                wcl::Vector v = parseVector(tokens);
                obj.texcoords.push_back(v);
                }
                break;
            case FACE:
                break;

            default:
                throw ParserException("Unexpected line type found in OBJ");
                break;
        }
    }

    wcl::Vector OBJParser::parseVector(std::istringstream& tokens) const {
        double x,y,z;
        tokens >> x;
        tokens >> y;
        tokens >> z;
        return wcl::Vector(x,y,z);
    }

};


