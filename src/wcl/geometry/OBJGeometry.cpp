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


    OBJGeometry::~OBJGeometry() {
        // Free up the OBJGeometry and all components
        // We don't add this to the structs as we are trying to keep
        // them extremely clean
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
    }
};
