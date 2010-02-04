/**
 * Lex File for the OBJ file format
 *
 */

%option nomain
%option nodefault
%option noyywrap
%option prefix="OBJFormat_" outfile="lex.yy.c"
%option debug
%option c++
     
%{
#include "parser/OBJParser.h"
#include "OBJFormatGrammar.h"
#include <string.h>

#define YY_FATAL_ERROR(msg) doFatal(msg)
extern void doFatal( const char *);
%}

%%

 /**************************************************************
                    OBJ Specific classes
 **************************************************************/
^g      { return GROUP;            }
^s      { return SHADING_GROUP;    }
^v      { return VERTEX;           }
^vt     { return TEX_COORD;        }
^vn     { return NORMAL;           }
^f      { return FACE;             }
^mtllib { return MTL_LIB;          }
^usemtl { return USE_MTL;          }
#.*\n  { return COMMENT;          }


 /**************************************************************
                    MTL Specific classes
 **************************************************************/
^newmtl { return NEW_MTL;          }
^Kd     { return DIFFUSE;          }
^Ka     { return AMBIENT;          }
^Ks     { return SPECULAR;         }
^Tf     { return OPACITY;          }
^Ni     { return REFRACTION_INDEX; }
^Ns     { return SPECULAR_EXP;     }
^map_Kd { return DIFFUSE_MAP;      }
^illum  { return ILLUM;		   }


 /**************************************************************
                   Data that we care about
 **************************************************************/

 /* Floating point number */
[-+0-9]+\.[0-9]+ {
	yylval.d = atof(yytext);
	return DOUBLE;
}

 /* integer number */
[0-9]+ {
	yylval.i = atoi(yytext);
	return INT;
}

 /* string */
[a-zA-Z][a-zA-Z0-9\._-]+ {
	strcpy(yylval.str,yytext);
	return STRING;
}

 /**************************************************************
                   General stuff, separators
 **************************************************************/

 /* Squeltch whitespace */
[\t\n\r ]*  ;

%%
