/**
 * Lex File for the OBJ file format
 *
 */

%option yyclass="wcl::OBJFormatScanner"
%option nomain
%option noyywrap
%option prefix="OBJFormatScanner_yy" outfile="lex.yy.c"
%option debug
%option c++
%option yylineno
%x DATA
%x FACEDATA

     
%{
#include <sstream>
#include "parser/OBJParser.h"
#include "OBJFormatGrammar.hh"
#include "parser/OBJFormatScanner.h"
#include <string.h>

using namespace std;
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
^o	{ BEGIN DATA; return GROUP;	       } /* We treat objects as groups */
#.*\n  { return COMMENT;          }


 /**************************************************************
                    MTL Specific classes
 **************************************************************/
^newmtl { BEGIN DATA; return NEW_MTL;          }
^Kd     { return DIFFUSE;          }
^Ka     { return AMBIENT;          }
^Ks     { return SPECULAR;         }
^Ke     { return EMISSIVE;         }
^Tf     { return OPACITY;          }
^Tr     { return OPACITY;	   }
^d      { return OPACITY;          }
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
<INITIAL>[-+0-9]+\.[0-9]+(e[-+0-9]+){0,1} {
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
<FACEDATA>[ \t]+ ;
<FACEDATA>\/ { return SEPARATOR; };

<FACEDATA>\n	{ BEGIN INITIAL; }

. { 
    stringstream s;
    s << "Invalid Character: \"";
    s << yytext;
    s << "\" line: " ;
    s << lineno();

    parser->parseError(ParserException::INVALID_SYNTAX, s.str()); 
}

%%

void wcl::OBJFormatScanner::LexerError(const char *str)
{
    stringstream s;
    s << "You Found A Bug in the Scanner" << endl;
    s << "Please Contact libWCL writers with the file your trying to read for a fix" << endl;
    s << "Error is: " << str;
    s << "Last Character: " << yytext << " Last Line: " << lineno();
    parser->parseError(ParserException::INVALID_SYNTAX, s.str());
}
