/**
 * Lex File for the OBJ file format
 *
 */

%option nomain
%option noyywrap
%option nodefault
%option nounput
%option caseless 


%%

/**************************************************************
                   OBJ Specific classes
**************************************************************/
g      { return GROUP;            }
s      { return SHADING_GROUP;    }
v      { return VERTEX;           }
vt     { return TEX_COORD;        }
vn     { return NORMAL;           }
f      { return FACE;             }
mtllib { return MTL_LIB;          }
usemtl { return USE_MTL;          }
#      { return HASH;             }


/**************************************************************
                   MTL Specific classes
**************************************************************/
newmtl { return NEW_MTL;          }
Kd     { return DIFFUSE;          }
Ka     { return AMBIENT;          }
Ks     { return SPECULAR;         }
Tf     { return OPACITY;          }
Ni     { return REFRACTION_INDEX; }
Ns     { return SPECULAR_EXP;     }
map_Kd { return DIFFUSE_MAP;      }


/**************************************************************
                  Data that we care about 
**************************************************************/

/* Floating point number */
[0-9]+.[0-9]+ {
	yylval.d = atof(yytext);
	return DOUBLE;
}

/* integer number */
[0-9]+ {
	yylval.i = atoi(yytext);
	return INT;
}

/* string */
[a-z0-9._-]+ {
	yylval.text = yytext;
	return STRING;
}

/**************************************************************
                  General stuff, separators 
**************************************************************/
\n     { return NEWLINE;          }
/      { return SLASH;            } 


/* Squeltch whitespace */
[ \t]+ ;

/* ignore things we don't know about */
. ;

%%



/**
 * The function that initiates the parsing of a line
 * We first tell the scanner that the line is in memory
 * and is a string. We then reassign the buffer 
 * and tell it to scann it - deleteing the buffer
 * afterwards.
 *
 * @param thread The thread that called the scanner
 * @param str The string that we want to scan
 * @return 0 on success, !0 on any other error
 */
int parse( ServerThread *thread, const char *str )
{
    YY_BUFFER_STATE buffer = server_scan_string(str);
    if(buffer==NULL){
	perror("yy_scan_bytes");
	return true;
    }
    #ifdef WIN32
    param = (void *)thread;
    int retval =serverparse();
    #else
    int retval =serverparse((void*) thread);
    #endif
    server_switch_to_buffer( buffer );
    server_delete_buffer(buffer);

    return retval;
}
