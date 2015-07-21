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


#ifndef LIBWCL_MATERIAL_LIBRARY_H
#define LIBWCL_MATERIAL_LIBRARY_H

#include <map>
#include <string>
#include <vector>
#include <wcl/api.h>
#include <wcl/maths/Vector.h>

namespace wcl {

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
                Vector opacity;             // Tr
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

    class WCL_API MaterialLibrary {
        public:
            MaterialLibrary() {};
            MaterialLibrary(const MaterialLibrary& rhs);
            virtual ~MaterialLibrary();
            MaterialLibrary& operator=(const MaterialLibrary& rhs);

            void addMaterial(std::string name, const wcl::OBJMaterial& m);
            void removeMaterial(std::string name);
            wcl::OBJMaterial* getMaterial(std::string name);
            const std::vector<OBJMaterial*>& getMaterials();

            void clear();

        private:
            std::vector<wcl::OBJMaterial *> materials;
            std::map<std::string, wcl::OBJMaterial *> materialMap;
    };

};

#endif
