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

/**
 * The implementation of the OBJ C++ Parser front end.
 * The real work is done by Lex and Yacc, this class just pulls it all
 * together
 */

#include "OBJParser.h"
#define yyFlexLexer OBJFormat_FlexLexer
#include <FlexLexer.h>
#include <fstream>

extern int yyparse(void *);
extern int yydebug;

using namespace std;

namespace wcl {


OBJParser::OBJParser(istream &stream):
    input(&stream), material(NULL), group(NULL)
{
}

OBJParser::~OBJParser()
{
    // Free up the OBJGeometry and all components
    // We don't add this to the structs as we are trying to keep
    // them extremely clean
    for(vector<OBJMaterial *>::iterator it = this->data.materials.begin();
        it != this->data.materials.end(); ++it ){
        OBJMaterial *m=*it;
        delete m;
    }
    for(vector<OBJGroup *>::iterator it = this->data.groups.begin();
        it != this->data.groups.end();
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

/**
 * Initiate the lex/yacc tokenisers/scanner for the OBJ format
 * We tell lex to use the istream that we have setup hence
 * we don't do any file handling directly. This has the benefit
 * that if lex needs to rescan it handles all the rewinds, etc
 * diretly. We also don't need to read the entire file into memory
 * first which may not be desirable (ie really large obj files);
 */

void wcl::OBJParser::parse()
{
    yydebug=this->debug;
    if( yyparse(this) ){
        this->parseError(ParserException::UNKNOWN_ERROR,"Unknown syntax in file");
    }
}

int wcl::OBJParser::scanner(OBJParser *p)
{
    OBJFormat_FlexLexer *lexer = new OBJFormat_FlexLexer(p->input,&cout);
    lexer->set_debug(p->debug);
    return lexer->yylex();
}

void OBJParser::OBJParser::addMaterialLibrary( const std::string &lib)
{
     // Open the new library file, parse the file as being part of the
     // current parse tree by switching the input files. Then
     // return things back to normal and continue parsing the original input
     // XXX/TODO: This is really nasty how it opens a new file for the material
     // a better approach would be to use the exising itstream - benjsc 20090828
     ifstream s(lib.c_str());
     istream *old = this->input;
     this->input= &s;
     this->parse();
     this->input = old;
     this->parse();
}

void OBJParser::addGroup(const std::string &name)
{
    OBJGroup *g = new OBJGroup;
    g->name = name;
    this->data.groups.push_back(g);
    this->data.groupsmap[name]=g;
    this->group = g;
}

void OBJParser::addVertex(const double v1, const double v2, const double v3)
{
    Vector v(v1,v2,v3);
    this->data.points.push_back(v);
}

void OBJParser::addVertexTexture(const double t1, const double t2)
{
    Vector v(t1,t2);
    this->data.texcoords.push_back(v);
}

void OBJParser::addNormal(const double n1, const double n2, const double n3)
{
    Vector v(n1, n2, n3);
    this->data.normals.push_back(v);
}

void OBJParser::addShaderGroup( const std::string &sgroup )
{
#warning OBJParser: Note Shader Group support is not implemented
    //NOTIMP, let user know
    cout <<"OBJParser::addShaderGroup| ShaderGroup Not Implemented, ignoring "
          << sgroup
          << endl;
}

void OBJParser::addFace(
             const double vi1, const double ti1, const double ni1,
             const double vi2, const double ti2, const double ni2,
             const double vi3, const double ti3, const double ni3)
{
#warning OBJParser: Note Only Faces consisting of Triangles are currently supported

    if( !this->group )
        this->parseError(ParserException::INVALID_INPUT,"No Group, to add Face too");

    OBJFace *face = new OBJFace;
    OBJVertex *v1 = new OBJVertex;
    OBJVertex *v2 = new OBJVertex;
    OBJVertex *v3 = new OBJVertex;

    face->verts.push_back( v1 );
    face->verts.push_back( v2 );
    face->verts.push_back( v3 );

    face->material = this->material;
    v1->pointIndex = vi1;
    v1->normalIndex= ni1;
    v1->uvIndex    = ti1;

    v2->pointIndex = vi2;
    v2->normalIndex= ni2;
    v2->uvIndex    = ti2;

    v3->pointIndex = vi3;
    v3->normalIndex= ni3;
    v3->uvIndex    = ti3;

    this->group->faces.push_back(face);
}

void OBJParser::useMaterial(const std::string &name)
{
    map<string, OBJMaterial *>::iterator it;

    // search for the material named in the map
    it = this->data.materialmap.find(name);
    if(it==this->data.materialmap.end()){
        string s="Material ";
        s+=name;
        s+=" Not Found In OBJ File";
        this->parseError(ParserException::INVALID_INPUT, s);
    }

    this->material = it->second;
}

void OBJParser::setMaterialDiffuse(const double c1, const double c2, const double c3)
{
    if( ! this->material ){
        this->parseError(ParserException::INVALID_INPUT, "Attempt to set diffuse value with no material present");
    }

    this->material->valid |= OBJMaterial::M_DIFFUSE;
    this->material->diffuse=Vector(c1,c2,c3);
}

void OBJParser::setMaterialAmbience(const double c1, const double c2, const double c3)
{
    if( ! this->material ){
        this->parseError(ParserException::INVALID_INPUT, "Attempt to set ambience value with no material present");
    }

    this->material->valid |= OBJMaterial::M_AMBIENT;
    this->material->ambient=Vector(c1,c2,c3);
}

void OBJParser::setMaterialSpecular(const double c1, const double c2, const double c3)
{
    if( ! this->material ){
        this->parseError(ParserException::INVALID_INPUT, "Attempt to set specular value with no material present");
    }

    this->material->valid |= OBJMaterial::M_SPECULAR;
    this->material->specular=Vector(c1,c2,c3);
}

void OBJParser::setMaterialOpacity(const double c1, const double c2, const double c3)
{
    if( ! this->material ){
        this->parseError(ParserException::INVALID_INPUT, "Attempt to set opacity value with no material present");
    }
    this->material->valid |= OBJMaterial::M_OPACITY;
    this->material->opacity=Vector(c1,c2,c3);
}

void OBJParser::setMaterialRefractionIndex( const double index)
{
    if( ! this->material ){
        this->parseError(ParserException::INVALID_INPUT, "Attempt to set refraction index with no material present");
    }
    this->material->valid |= OBJMaterial::M_REFRACTION;
    this->material->refractionIndex=index;
}

void OBJParser::setMaterialDiffuseMap(const std::string &path)
{
#warning OBJParser: Diffuse Map Loading Is Not Yet Implemented
    //NOTIMP, let user know
    cout <<"OBJParser::setMaterialDiffuseMap| Diffuse Map loading not Implemented, ignoring "
          << path
          << endl;
}

void OBJParser::setMaterialIlluminationGroup(const int group)
{
#warning OBJParser: Illumination Group Loading Is Not Yet Implemented
    //NOTIMP, let user know
    cout <<"OBJParser::setMaterialIlluminationGroup| Illumination Group loading not Implemented, ignoring "
          << group
          << endl;
}

void OBJParser::addMaterial(const std::string &name)
{
    OBJMaterial *m = new OBJMaterial;
    m->name = name;
    m->valid=OBJMaterial::M_NONE;
    this->data.materials.push_back(m);
    this->data.materialmap[name]=m;
    this->material = m;
}

void OBJParser::setMaterialSpecularExponent(const double value)
{
    if( ! this->material ){
        this->parseError(ParserException::INVALID_INPUT, "Attempt to set refraction index with no material present");
    }
    this->material->valid |= OBJMaterial::M_SPECULAREXP;
    this->material->specularExp = value;
}

void OBJParser::print()
{
    printf("Materials:\n"
           "-----------------------------------------------\n");
    for(vector<OBJMaterial *>::iterator it = this->data.materials.begin();
        it != this->data.materials.end(); ++it ){
        OBJMaterial *m=*it;
        printf(" + Name: %s valid[%d]\n", m->name.c_str(), m->valid);
        if( m->valid & OBJMaterial::M_DIFFUSE )
            printf("   dif  [ %f,%f,%f]\n",
                   m->diffuse[0], m->diffuse[1], m->diffuse[2]);
        if( m->valid & OBJMaterial::M_AMBIENT )
            printf("   amb  [ %f,%f,%f]\n",
                   m->ambient[0], m->ambient[1], m->ambient[2]);
        if(m->valid & OBJMaterial::M_SPECULAR )
            printf("   spec [%f,%f,%f]\n",
                   m->specular[0], m->specular[1], m->specular[2]);
        if(m->valid & OBJMaterial::M_OPACITY )
            printf("   opac [%f,%f,%f]\n",
                   m->opacity[0], m->opacity[1], m->opacity[2]);
        if(m->valid & OBJMaterial::M_REFRACTION)
            printf( "  refInd %f\n", m->refractionIndex);
        if(m->valid & OBJMaterial::M_SPECULAREXP)
            printf("   specexp %f\n",m->specularExp);
    }

    printf("\n");
    printf("Vertexes\n"
           "---------------------------------------------\n");
    unsigned count=0;
    for(vector<Vector>::iterator it=this->data.points.begin();
        it!= this->data.points.end();
        ++it){
        Vector v = *it;
        printf("V %d (%f,%f,%f)\n", count++, v[0], v[1], v[2]);
    }

    printf("\n");
    printf("Texcoords\n"
           "---------------------------------------------\n");
    count=0;
    for(vector<Vector>::iterator it=this->data.texcoords.begin();
        it!= this->data.texcoords.end();
        ++it){
        Vector v = *it;
        printf("T %d (%f,%f)\n", count++, v[0], v[1] );
    }

    printf("\n");
    printf("Normals\n"
           "---------------------------------------------\n");
    count=0;
    for(vector<Vector>::iterator it=this->data.normals.begin();
        it!= this->data.normals.end();
        ++it){
        Vector v = *it;
        printf("N %d (%f,%f,%f)\n", count++, v[0], v[1], v[2]);
    }

    printf("\n");
    printf("Groups & Faces:\n"
           "-----------------------------------------------\n");
    for(vector<OBJGroup *>::iterator it = this->data.groups.begin();
        it != this->data.groups.end();
        ++it ){

        OBJGroup *g = *it;
        printf(" + Group: %s\n", g->name.c_str());

        for( vector<OBJFace *>::iterator fit = g->faces.begin();
             fit != g->faces.end();
             ++fit ){

            OBJFace *f = *fit;

            printf(" +--+ Face: ");
            printf("     - Material: %s\n",
                   f->material?f->material->name.c_str():"<NONE>");

            bool first=true;
            for( vector<OBJVertex *>::iterator vit = f->verts.begin();
                 vit != f->verts.end();
                 ++vit ){
                OBJVertex *v = *vit;
                Vector vec=this->data.points[v->pointIndex-1];
                Vector t=this->data.texcoords[v->uvIndex-1];
                Vector n=this->data.normals[v->normalIndex-1];
                printf("    | V %d (%f,%f,%f)  N %d (%f,%f,%f) T %d (%f,%f)\n",
                       v->pointIndex, vec[0],vec[1],vec[2],
                       v->uvIndex,  n[0],n[1],n[2],
                       v->normalIndex, t[0],t[1]);
            }

        }
    }
}

/*
 * Return the parsed geometry of the class.
 * It's assumed the user has called OBJParser::parse()
 * prior to this call, else the returned geometry will be empty
 *
 * @return The parsed geometry object
 */
const OBJGeometry& OBJParser::getGeometry()
{
	return data;
}

};
