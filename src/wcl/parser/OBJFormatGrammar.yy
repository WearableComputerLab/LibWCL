/**
 * This file contains the grammer that takes tokens from the lexical ananaliser
 * and trys to construct meaning out of them. If a meaning can be found, that
 * associated code block is called. If a meaning cannont be found, the error
 * state is called
 */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define YYPARSE_PARAM param
#define YYLEX_PARAM (wcl::OBJParser *)param
#include "parser/OBJParser.h"

extern void yyerror(const char *);
using namespace wcl;

#define yylex wcl::OBJParser::scanner
%}

// Tokens returned by lex
%token GROUP VERTEX TEX_COORD NORMAL FACE MTL_LIB DOUBLE INT STRING USE_MTL SHADING_GROUP COMMENT
%token NEW_MTL DIFFUSE AMBIENT SPECULAR OPACITY REFRACTION_INDEX SPECULAR_EXP DIFFUSE_MAP ILLUM
%debug

%union {
    int i;
    double d;
    char str[4096];
}

// Types
%type <d> DOUBLE
%type <str> STRING
%type <i> INT

// Where to start grammar parsing
%start request

%%
request:
	|	request material_lib
	|	request group
	|       request vertex
	|	request vertextexture
	|	request vertexnormal
	|	request face
	|	request shadinggroup
	|	request COMMENT
	|	request use_material
	|	error {
			return 1;
		}
	;

material_lib:	MTL_LIB STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addMaterialLibrary($2);
		}
		| material 
		;

material:	NEW_MTL STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addMaterial($2);
		}
		| material_property
		;

material_property:
		DIFFUSE DOUBLE DOUBLE DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialDiffuse($2,$3,$4);
		}
		| AMBIENT DOUBLE DOUBLE DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialAmbience($2,$3,$4);
		}

		| SPECULAR DOUBLE DOUBLE DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialSpecular($2,$3,$4);
		}
		| OPACITY DOUBLE DOUBLE DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialOpacity($2,$3,$4);
		}
		| REFRACTION_INDEX DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialRefractionIndex($2);
		}
		| ILLUM INT
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialIlluminationGroup($2);
		}
		| DIFFUSE_MAP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialDiffuseMap($2);
		}
		;

group:		GROUP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addGroup($2);
		}
		;

vertex:		VERTEX DOUBLE DOUBLE DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addVertex($2,$3,$4);
		}
		;

vertextexture:	TEX_COORD DOUBLE DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addVertexTexture($2,$3);
		}
		;

vertexnormal:	NORMAL DOUBLE DOUBLE DOUBLE
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addNormal($2,$3,$4);
		}
		;

shadinggroup:	SHADING_GROUP INT
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addShaderGroup($2);
		}
		;

face:		FACE  INT INT INT  INT INT INT  INT INT INT
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addFace($2,$3,$4,
				    $5,$6,$7,
				    $8,$9,$10);
		}
		;

use_material:	USE_MTL STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->useMaterial($2);
		}
		;
%%

/**
 * Error handling is done elsewhere,
 * we simply do nothing
 */
void yyerror( const char *)
{
    //TODO: Really should give a good error message here -benjsc 20090827
}

