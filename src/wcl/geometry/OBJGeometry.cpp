/*-
 * Copyright (c) 2014 Michael Marner <michael@20papercups.net>
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

#include <vector>

using std::vector;

namespace wcl {

    OBJGeometry::OBJGeometry(const OBJGeometry& rhs) {
        // PREFILL! 
        for (std::vector<wcl::OBJSmoothing*>::const_iterator it = rhs.smoothing.begin(); it < rhs.smoothing.end(); ++it) {
            OBJSmoothing* s = new OBJSmoothing();
            s->name = (*it)->name;
            smoothing.push_back(s);
            smoothingMap[s->name] = s;
        }

        // copy points
        for (std::vector<wcl::Vector>::const_iterator it = rhs.points.begin(); it < rhs.points.end(); ++it) {
            points.push_back(*it);
        }

        // copy normals!
        for (std::vector<wcl::Vector>::const_iterator it = rhs.normals.begin(); it < rhs.normals.end(); ++it) {
            normals.push_back(*it);
        }

        // copy texcoords
        for (std::vector<wcl::Vector>::const_iterator it = rhs.texcoords.begin(); it < rhs.texcoords.end(); ++it) {
            texcoords.push_back(*it);
        }

        // create new materials!
        for (std::vector<OBJMaterial*>::const_iterator it = rhs.materials.begin(); it < rhs.materials.end(); ++it) {
            OBJMaterial* m = new OBJMaterial(**it);
            materials.push_back(m);
            materialMap[m->name] = m;
        }

        // create new groups!
        for (std::vector<OBJGroup*>::const_iterator it = rhs.groups.begin(); it < rhs.groups.end(); ++it) {

            OBJGroup* g = new OBJGroup();
            g->name = (*it)->name;
            groups.push_back(g);
            groupsMap[g->name] = g;

            // create new faces!
            for (std::vector<OBJFace*>::const_iterator fit = (*it)->faces.begin(); fit < (*it)->faces.end(); ++fit) {
                OBJFace* f = new OBJFace();
                if (materialMap.find((*fit)->material->name) != materialMap.end()) {
                    f->material = materialMap[(*fit)->material->name];
                }
                if (smoothingMap.find((*fit)->smoothing->name) != smoothingMap.end()) {
                    f->smoothing= smoothingMap[(*fit)->smoothing->name];
                }
                f->smoothing->faces.push_back(f);
                g->faces.push_back(f);

                // create new vertices for the damn face!
                for (std::vector<OBJVertex*>::const_iterator vit = (*fit)->verts.begin(); vit < (*fit)->verts.end(); ++vit) {
                    OBJVertex* v = new OBJVertex(**vit);
                    f->verts.push_back(v);
                }
            }
        }
    }

    OBJGeometry& OBJGeometry::operator=(const OBJGeometry& rhs) {
        // empty this!
        clear();

        // PREFILL! 
        for (std::vector<wcl::OBJSmoothing*>::const_iterator it = rhs.smoothing.begin(); it < rhs.smoothing.end(); ++it) {
            OBJSmoothing* s = new OBJSmoothing();
            s->name = (*it)->name;
            smoothing.push_back(s);
            smoothingMap[s->name] = s;
        }

        // copy points
        for (std::vector<wcl::Vector>::const_iterator it = rhs.points.begin(); it < rhs.points.end(); ++it) {
            points.push_back(*it);
        }

        // copy normals!
        for (std::vector<wcl::Vector>::const_iterator it = rhs.normals.begin(); it < rhs.normals.end(); ++it) {
            normals.push_back(*it);
        }

        // copy texcoords
        for (std::vector<wcl::Vector>::const_iterator it = rhs.texcoords.begin(); it < rhs.texcoords.end(); ++it) {
            texcoords.push_back(*it);
        }

        // create new materials!
        for (std::vector<OBJMaterial*>::const_iterator it = rhs.materials.begin(); it < rhs.materials.end(); ++it) {
            OBJMaterial* m = new OBJMaterial(**it);
            materials.push_back(m);
            materialMap[m->name] = m;
        }

        // create new groups!
        for (std::vector<OBJGroup*>::const_iterator it = rhs.groups.begin(); it < rhs.groups.end(); ++it) {

            OBJGroup* g = new OBJGroup();
            g->name = (*it)->name;
            groups.push_back(g);
            groupsMap[g->name] = g;

            // create new faces!
            for (std::vector<OBJFace*>::const_iterator fit = (*it)->faces.begin(); fit < (*it)->faces.end(); ++fit) {
                OBJFace* f = new OBJFace();
                f->material = materialMap[(*fit)->material->name];
                f->smoothing= smoothingMap[(*fit)->smoothing->name];
                f->smoothing->faces.push_back(f);
                g->faces.push_back(f);

                // create new vertices for the damn face!
                for (std::vector<OBJVertex*>::const_iterator vit = (*fit)->verts.begin(); vit < (*fit)->verts.end(); ++vit) {
                    OBJVertex* v = new OBJVertex(**vit);
                    f->verts.push_back(v);
                }
            }
        }
    }



    OBJGeometry::~OBJGeometry() {
        clear();
    }

    void OBJGeometry::clear() {
        // Free up the OBJGeometry and all components
        for(vector<OBJMaterial *>::iterator it = materials.begin();
                it != materials.end(); ++it ){
            OBJMaterial *m=*it;
            delete m;
        }
        for(vector<OBJGroup *>::iterator it = groups.begin();
                it != groups.end();
                ++it ){

            OBJGroup *g = *it;

            for( vector<OBJFace *>::iterator fit = g->faces.begin();
                    fit != g->faces.end();
                    ++fit ){

                OBJFace *f = *fit;

                for( vector<OBJVertex *>::iterator vit = f->verts.begin();
                        vit != f->verts.end();
                        ++vit ){
                    OBJVertex *v = *vit;
                    delete v;
                }

                delete f;
            }
            delete g;
        }
        points.clear();
        normals.clear();
        texcoords.clear();
        materials.clear();
        groups.clear();
        smoothing.clear();
        materialMap.clear();
        groupsMap.clear();
        smoothingMap.clear();
    }
};
