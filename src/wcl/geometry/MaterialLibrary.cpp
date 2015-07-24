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

#include <wcl/Exception.h>
#include "MaterialLibrary.h"

namespace wcl {

    MaterialLibrary::~MaterialLibrary() {
        clear();
    }

    MaterialLibrary& MaterialLibrary::operator=(const MaterialLibrary& rhs) {
        clear();

        // create new materials!
        for (std::vector<OBJMaterial*>::const_iterator it = rhs.materials.begin(); it < rhs.materials.end(); ++it) {
            addMaterial((**it).name, **it);
        }

        return *this;
    }

    MaterialLibrary::MaterialLibrary(const MaterialLibrary& rhs) {
        for (std::vector<OBJMaterial*>::const_iterator it = rhs.materials.begin(); it < rhs.materials.end(); ++it) {
            addMaterial((**it).name, **it);
        }
    }

    void MaterialLibrary::clear() {
        for(std::vector<OBJMaterial *>::iterator it = materials.begin();
                it != materials.end(); ++it ){
            OBJMaterial *m=*it;
            delete m;
        }

        materials.clear();
        materialMap.clear();
    }

    OBJMaterial* MaterialLibrary::addMaterial(const std::string& name, const wcl::OBJMaterial& m) {
        // replace the existing material, if found
        if (materialMap.find(name) != materialMap.end()) {
            materialMap[name]->operator=(m);
            return materialMap[name];
        }
        else {
            OBJMaterial* nm = new OBJMaterial(m);
            materials.push_back(nm);
            materialMap[name] = nm;
            return nm;
        }
    }

    void MaterialLibrary::addMaterial(const std::string& name) {
        // replace the existing material, if found
        if (materialMap.find(name) != materialMap.end()) {
            return materialMap[name];
        }
        else {
            OBJMaterial* nm = new OBJMaterial();
            materials.push_back(nm);
            materialMap[name] = nm;
            return nm
        }
    }

    void MaterialLibrary::removeMaterial(const std::string& name) {
        using namespace std;

        if (materialMap.find(name) != materialMap.end()) {
            materialMap.erase(name);

            vector<OBJMaterial*>::iterator it;
            for(it = materials.begin(); it < materials.end(); ++it) {
                if ((*it)->name == name) {
                    break;
                }
            }
            materials.erase(it);
        }
    }

    OBJMaterial* MaterialLibrary::getMaterial(const std::string& name) {
        if (materialMap.find(name) == materialMap.end()) {
            throw wcl::Exception("Material not found in library");
        }
        return materialMap[name];
    }

    const std::vector<OBJMaterial*>& MaterialLibrary::getMaterials() {
        return materials;
    }

};
