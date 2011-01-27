/**
 * Lex File for the OBJ file format
 *
 */

%option nomain
%option noyywrap
%option prefix="OBJFormat_" outfile="lex.yy.c"
%option debug
%option c++
%x DATA
%x FACEDATA
     
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
^g      { BEGIN DATA; return GROUP;            }
^s      { BEGIN DATA; return SMOOTHING_GROUP;    }
^v      { return VERTEX;           }
^vt     { return TEX_COORD;        }
^vn     { return NORMAL;           }
^f      { BEGIN FACEDATA; return FACE;             }
^mtllib { BEGIN DATA; return MTL_LIB; }
^usemtl { BEGIN DATA; return USE_MTL;          }
#.*\n  { return COMMENT;          }


 /**************************************************************
                    MTL Specific classes
 **************************************************************/
^newmtl { BEGIN DATA; return NEW_MTL;          }
^Kd     { return DIFFUSE;          }
^Ka     { return AMBIENT;          }
^Ks     { return SPECULAR;         }
^Tf|^Tr { return OPACITY;          }
^Ni     { return REFRACTION_INDEX; }
^Ns     { return SPECULAR_EXP;     }
^map_Kd { BEGIN DATA; return DIFFUSE_MAP;      }
^map_Ka { BEGIN DATA; return AMBIENT_MAP;      }
^map_Ks { BEGIN DATA; return SPECULAR_MAP;     }
^map_d  { BEGIN DATA; return ALPHA_MAP;        }
^map_bump|^bump { BEGIN DATA; return BUMP_MAP; }
^illum  { return ILLUM;		   }


 /**************************************************************
                   Data that we care about
 **************************************************************/

 /* Floating point number */
<INITIAL>[-+0-9]+\.[0-9]+ {
	yylval.d = atof(yytext);
	return DOUBLE;
}

 /* integer number */
<INITIAL,FACEDATA>[0-9]+ {
	yylval.i = atoi(yytext);
	return INT;
}


 /* string */
<DATA>([^ \r\t\n][^\r\t\n]+[^ \r\t\n]|[^ \r\t\n]+) {
	strcpy(yylval.str,yytext);
	BEGIN INITIAL;
	return STRING;
}

 /**************************************************************
                   General stuff, separators
 **************************************************************/

 /* Squeltch whitespace */
<INITIAL,DATA>[ \t\n\r]  ;

 /* Face separators */
<FACEDATA>[/ \t]	;

<FACEDATA>\n	{ BEGIN INITIAL; }

. { printf("lex Unknown character, ignoring = '%s'\n", yytext); };

%%
