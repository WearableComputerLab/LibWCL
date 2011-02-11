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
#include "parser/OBJParser.h"

extern void yyerror(wcl::OBJParser *, const char *);
using namespace wcl;

#define yylex wcl::OBJParser::scanner
%}

// Tokens returned by lex
%token GROUP VERTEX TEX_COORD NORMAL FACE MTL_LIB DOUBLE INT STRING USE_MTL SMOOTHING_GROUP COMMENT SEPARATOR
%token NEW_MTL DIFFUSE AMBIENT SPECULAR OPACITY REFRACTION_INDEX SPECULAR_EXP ILLUM EMISSIVE
%token DIFFUSE_MAP AMBIENT_MAP SPECULAR_MAP ALPHA_MAP BUMP_MAP
%error-verbose
%expect 4
%parse-param { wcl::OBJParser *parser }
%lex-param { wcl::OBJParser *parser }
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
		    parser->addMaterialLibrary($2);
		}
		| material 
		;

material:	NEW_MTL STRING
		{
		    parser->addMaterial($2);
		}
		| material_property
		;

material_property:
		DIFFUSE DOUBLE DOUBLE DOUBLE
		{
		    parser->setMaterialDiffuse($2,$3,$4);
		}
		| AMBIENT DOUBLE DOUBLE DOUBLE
		{
		    parser->setMaterialAmbience($2,$3,$4);
		}

		| SPECULAR DOUBLE DOUBLE DOUBLE
		{
		    parser->setMaterialSpecular($2,$3,$4);
		}
		| OPACITY DOUBLE DOUBLE DOUBLE
		{
		    parser->setMaterialOpacity($2,$3,$4);
		}
		| REFRACTION_INDEX DOUBLE
		{
		    parser->setMaterialRefractionIndex($2);
		}
		| ILLUM INT
		{
		    parser->setMaterialIlluminationGroup($2);
		}
		| SPECULAR_EXP DOUBLE
		{
		    parser->setMaterialSpecularExponent($2);
		}
		| DIFFUSE_MAP STRING
		{
		    parser->setMaterialDiffuseMap($2);
		}
		| AMBIENT_MAP STRING
		{
		    parser->setMaterialAmbientMap($2);
		}
		| SPECULAR_MAP STRING
		{
		    parser->setMaterialSpecularMap($2);
		}
		| ALPHA_MAP STRING
		{
		    parser->setMaterialAlphaMap($2);
		}
		| BUMP_MAP STRING
		{
		    parser->setMaterialBumpMap($2);
		}
		| EMISSIVE DOUBLE DOUBLE DOUBLE
		{
		    parser->setMaterialEmissive($2,$3,$4);
		}
		;

group:		GROUP STRING
		{
		    parser->addGroup($2);
		}
		;

vertex:		VERTEX DOUBLE DOUBLE DOUBLE
		{
		    parser->addVertex($2,$3,$4);
		}
		;

vertextexture:	TEX_COORD DOUBLE DOUBLE
		{
		    parser->addVertexTexture($2,$3);
		}
		;

vertexnormal:	NORMAL DOUBLE DOUBLE DOUBLE
		{
		    parser->addNormal($2,$3,$4);
		}
		;

smoothinggroup:   SMOOTHING_GROUP STRING
		{
		    parser->setSmoothingGroup($2);
		} 
		;

face:		facestart vertexgroup

facestart:	FACE	
		{
		    parser->newFace();
		}
		;

vertexgroup:	vertex
		| vertexgroup vertex

vertex: 	INT SEPARATOR INT SEPARATOR INT
		{
		    parser->addFaceVertex($1,$3,$5);
		} 
		| INT SEPARATOR SEPARATOR INT
		{
		    parser->addFaceVertex($1,0,$4);
		}
		| INT SEPARATOR INT SEPARATOR
		{
		    parser->addFaceVertex($1,$3,0);
		}
		| INT SEPARATOR SEPARATOR
		{
		    parser->addFaceVertex($1,0,0);
		}
		;

use_material:	USE_MTL STRING
		{
		    parser->useMaterial($2);
		}
		;
%%

/**
 * Error handling: Report what the error was as an exception
 *
 * @param parser The parser object acting as a driver to yacc
 * @param m The message to be reported
 */
void yyerror( wcl::OBJParser *parser, const char *m)
{
    std::stringstream s;
    s << m;
    s << " Line:";
    s << parser->getLineNo();
    s << "";
    parser->parseError(ParserException::INVALID_SYNTAX, s.str());
}
