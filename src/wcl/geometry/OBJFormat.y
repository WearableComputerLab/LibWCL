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
#include <string>

// The name of the parameter 
// we can access in the rules
#define YYPARSE_PARAM param

// Needed to call the tokenizer
extern int yylex();
extern void yyerror( const char *);
%}
// Tokens returned by lex
%token GROUP VERTEX TEX_COORD NORMAL FACE MTL_LIB HASH NEWLINE SLASH DOUBLE INT STRING USE_MTL SHADING_GROUP
%token NEW_MTL DIFFUSE AMBIENT SPECULAR OPACITY REFRACTION_INDEX SPECULAR_EXP DIFFUSE_MAP

%union {
    int i;
    double d;
    char str[MAXWORD]; 
}

// Types
%type <d> DOUBLE
%type <str> STRING
%type <i> INT

// Where to start grammar parsing
%start request

%%
request:
	|	material_lib
	|   group
	|   vertex
	|	vertextexture
	|	vertexnormal
	|	face
	|	shadinggroup
	|	error {
			return 1;
		}
	;

material_lib:	MTL_LIB STRING NEWLINE
		{
		    // PARAM is what ever you define it to be
		    //ServerThread *thread  = (ServerThread *)param;
		    printf("Material %s\n", $2);
		};

group:		GROUP STRING NEWLINE
		{
		    printf("Group %s\n", $2);
		}

vertex:		VERTEX DOUBLE DOUBLE DOUBLE NEWLINE
		{
		    printf("vertex %f,%f,%f\n", $2, $3, $4);
		}

vertextexture:	TEX_COORD DOUBLE DOUBLE DOUBLE NEWLINE
		{
		    printf("vertextexture %f,%f,%f\n", $2, $3, $4);
		}

vertexnormal:	NORMAL DOUBLE DOUBLE DOUBLE NEWLINE
		{
		    printf("normal %f,%f,%f\n", $2, $3, $4);
		}

shadinggroup:	SHADING_GROUP STRING NEWLINE
		{
		    printf("Shading Group %s\n", $2);
		}

face:		FACE  INT SLASH INT SLASH INT  INT SLASH INT SLASH INT  INT SLASH INT SLASH INT  INT SLASH INT SLASH INT NEWLINE
		{
		    printf("Face: %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", 
			   $2,$4,$6 
			   $7,$9,$11,
			   $12,$14,$16);
		}

%%

/**
 * Error handling is done elsewhere,
 * we simply do nothing
 */
void yyerror( const char *)
{}

