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


#include <wcl/geometry/OBJGeometry.h>
#include "OBJMaterialParser.h"
#include <fstream>
#include <sstream>

namespace wcl {


    MaterialLibrary OBJMaterialParser::parse(const std::string& filename)  {
        using namespace std;

        ifstream in(filename.c_str());
        if (in.fail()) {
            throw ParserException("Could not open file");
        }

        MaterialLibrary lib;
        OBJMaterial* activeMaterial = NULL;

        unsigned lineNumber = 1;

        try {
            while (!in.eof() && in.good()) {

                string line;
                getline(in, line);

                istringstream tokens(line);

                string lineType;
                tokens >> lineType;
                switch (getLineType(lineType)) {
                    case NEWMTL:
                        {
                            string name;
                            tokens >> name;
                            activeMaterial = lib.addMaterial(name);
                        }
                        break;
                    case KA:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        activeMaterial->ambient = parseVector(tokens);
                        activeMaterial->valid = activeMaterial->valid | OBJMaterial::M_AMBIENT;
                        break;
                    case KD:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        activeMaterial->diffuse = parseVector(tokens);
                        activeMaterial->valid = activeMaterial->valid | OBJMaterial::M_DIFFUSE;
                        break;
                    case KS:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        activeMaterial->specular = parseVector(tokens);
                        activeMaterial->valid = activeMaterial->valid | OBJMaterial::M_SPECULAR;
                        break;
                    case NS:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        activeMaterial->specularExp = parseDouble(tokens);
                        activeMaterial->valid = activeMaterial->valid | OBJMaterial::M_SPECULAREXP;
                        break;
                    case TR:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        activeMaterial->opacity = parseDouble(tokens);
                        activeMaterial->valid = activeMaterial->valid | OBJMaterial::M_OPACITY;
                        break;
                    case ILLUM:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        activeMaterial->illumGroup = parseDouble(tokens);
                        activeMaterial->valid = activeMaterial->valid | OBJMaterial::M_OPACITY;
                        break;
                    case MAP_KA:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->ambientMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_AMBIENT_MAP;
                        break;
                    case MAP_KD:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->diffuseMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_DIFFUSE_MAP;
                        break;
                    case MAP_KS:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->specularMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_SPECULAR_MAP;
                        break;
                    case MAP_NS:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->specularExpMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_SPECULAREXP_MAP;
                        break;
                    case MAP_D:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->alphaMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_ALPHA_MAP;
                        break;
                    case MAP_BUMP:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->bumpMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_BUMP_MAP;
                        break;
                    case MAP_DISP:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->displacementMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_DISPLACEMENT_MAP;
                        break;
                    case MAP_STENCIL:
                        if (activeMaterial == NULL) {
                            throw ParserException("found material properties before newmtl declaration");
                        }
                        tokens >> activeMaterial->stencilMap;
                        activeMaterial->maps = activeMaterial->maps | OBJMaterial::M_STENCIL_MAP;
                        break;
                }
                lineNumber++;
            }

            return MaterialLibrary(lib);
        }
        catch(ParserException& e) {
            stringstream ss;
            ss << e.what() << " on line " << lineNumber << "of " << filename;
            throw ParserException(ss.str().c_str());
        }
    }


    OBJMaterialParser::LineType OBJMaterialParser::getLineType(const std::string& token) const {
        // material library lines
        if (token == "newmtl")
            return NEWMTL;
        if (token == "Ka")
            return KA;
        if (token == "Kd")
            return KD;
        if (token == "Ks")
            return KS;
        if (token == "Ns")
            return NS;
        if (token == "Tr")
            return TR;
        if (token == "illum")
            return ILLUM;
        if (token == "map_Ka")
            return MAP_KA;
        if (token == "map_Kd")
            return MAP_KD;
        if (token == "map_Ks")
            return MAP_KS;
        if (token == "map_Ns")
            return MAP_NS;
        if (token == "map_d")
            return MAP_D;
        if (token == "map_bump" || token == "bump")
            return MAP_BUMP;
        if (token == "disp")
            return MAP_DISP;
        if (token == "decal")
            return MAP_STENCIL;

        std::stringstream ss;
        ss << "Unknown line type: " << token;
        throw ParserException(ss.str().c_str());
    }

    wcl::Vector OBJMaterialParser::parseVector(std::istringstream& tokens) const {
        double x,y,z;
        tokens >> x;
        tokens >> y;
        tokens >> z;
        return wcl::Vector(x,y,z);
    }

    double OBJMaterialParser::parseDouble(std::istringstream& tokens) const {
        double v;
        tokens >> v;
        return v;
    }

};

