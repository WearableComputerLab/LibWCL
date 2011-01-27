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
#include <sstream>
#define YYPARSE_PARAM param
#define YYLEX_PARAM (wcl::OBJParser *)param
#include "parser/OBJParser.h"

extern void yyerror(const char *);
using namespace wcl;

#define yylex wcl::OBJParser::scanner
%}

// Tokens returned by lex
%token GROUP VERTEX TEX_COORD NORMAL FACE MTL_LIB DOUBLE INT STRING USE_MTL SMOOTHING_GROUP COMMENT
%token NEW_MTL DIFFUSE AMBIENT SPECULAR OPACITY REFRACTION_INDEX SPECULAR_EXP ILLUM 
%token DIFFUSE_MAP AMBIENT_MAP SPECULAR_MAP ALPHA_MAP BUMP_MAP
%expect 2
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
	|	request smoothinggroup
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
		| SPECULAR_EXP DOUBLE
		{
		    OBJParser *parser = (OBJParser *)param;
		    parser->setMaterialSpecularExponent($2);
		}
		| DIFFUSE_MAP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialDiffuseMap($2);
		}
		| AMBIENT_MAP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialAmbientMap($2);
		}
		| SPECULAR_MAP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialSpecularMap($2);
		}
		| ALPHA_MAP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialAlphaMap($2);
		}
		| BUMP_MAP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setMaterialBumpMap($2);
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

smoothinggroup:   SMOOTHING_GROUP STRING
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->setSmoothingGroup($2);
		} 
		;

face:		facestart vertexgroup

facestart:	FACE	
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->newFace();
		}
		;

vertexgroup:	vertex
		| vertexgroup vertex

vertex: 	INT INT INT
		{
		    OBJParser *parser  = (OBJParser *)param;
		    parser->addFaceVertex($1,$2,$3);
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

void doFatal(const char *str)
{
    printf("You Found A Bug in the Scanner\n");
    printf("Please Contact libWCL writers with the file your trying to read for a fix\n");
    printf("Program will now exit\n");
    printf("Error: %s\n", str);
    exit(2);
}
