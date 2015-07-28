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


#ifndef LIBWCL_OBJ_MATERIAL_PARSER_H
#define LIBWCL_OBJ_MATERIAL_PARSER_H

#include <wcl/api.h>
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
    class WCL_API OBJMaterialParser
    {
        public:
            enum LineType {
                NEWMTL,
                KA,
                KD,
                KS,
                NS,
                TR,
                ILLUM,
                MAP_KA,
                MAP_KD,
                MAP_KS,
                MAP_NS,
                MAP_D,
                MAP_BUMP,
                MAP_DISP,
                MAP_STENCIL
            };

            wcl::MaterialLibrary parse(const std::string& filename);
            LineType getLineType(const std::string& token) const;

            wcl::Vector parseVector(std::istringstream& tokens) const;
            double parseDouble(std::istringstream& tokens) const;

        private:
            OBJMaterial* currentMaterial;

    };
};


#endif

