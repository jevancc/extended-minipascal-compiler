%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <string>
    #include "node.h"
    #include "y.tab.h"
    #include "logger.h"

    int colnum = 1;
    #define YY_USER_ACTION { \
            yylloc->first_line = yylineno; \
            yylloc->first_column = colnum; \
            colnum=colnum+yyleng; \
            yylloc->last_column=colnum; \
            yylloc->last_line = yylineno; \
            }

    void printShift (int tokenType) {
        sprintf(tmpStr, "=> Shift %s", getNodeTokenName(tokenType));
        logger.info(Location(yylineno, colnum-yyleng), string(tmpStr));
    }

%}

%option bison-locations
%option yylineno

A [aA]
B [bB]
C [cC]
D [dD]
E [eE]
F [fF]
G [gG]
H [hH]
I [iI]
J [jJ]
K [kK]
L [lL]
M [mM]
N [nN]
O [oO]
P [pP]
Q [qQ]
R [rR]
S [sS]
T [tT]
U [uU]
V [vV]
W [wW]
X [xX]
Y [yY]
Z [zZ]
NQUOTE [^']
DIGIT [0-9]

%%

{A}{R}{R}{A}{Y}              { printShift(Tok_ARRAY); return(Tok_ARRAY); }
{D}{O}                       { printShift(Tok_DO); return(Tok_DO); }
{E}{L}{S}{E}                 { printShift(Tok_ELSE); return(Tok_ELSE); }
{E}{N}{D}                    { printShift(Tok_END); return(Tok_END); }
{F}{U}{N}{C}{T}{I}{O}{N}     { printShift(Tok_FUNCTION); return(Tok_FUNCTION); }
{I}{F}                       { printShift(Tok_IF); return(Tok_IF); }
{N}{O}{T}                    { printShift(Tok_NOT); return(Tok_NOT); }
{O}{F}                       { printShift(Tok_OF); return(Tok_OF); }
{B}{E}{G}{I}{N}              { printShift(Tok_PBEGIN); return(Tok_PBEGIN); }
{P}{R}{O}{C}{E}{D}{U}{R}{E}  { printShift(Tok_PROCEDURE); return(Tok_PROCEDURE); }
{P}{R}{O}{G}{R}{A}{M}        { printShift(Tok_PROGRAM); return(Tok_PROGRAM); }
{T}{H}{E}{N}                 { printShift(Tok_THEN); return(Tok_THEN); }
{V}{A}{R}                    { printShift(Tok_VAR); return(Tok_VAR); }
{W}{H}{I}{L}{E}              { printShift(Tok_WHILE); return(Tok_WHILE); }
{I}{N}{T}{E}{G}{E}{R}        { printShift(Tok_INTEGER); return(Tok_INTEGER); }
{R}{E}{A}{L}                 { printShift(Tok_REAL); return(Tok_REAL); }
{S}{T}{R}{I}{N}{G}           { printShift(Tok_STRING); return(Tok_STRING); }
{J}{F}{U}{N}{C}{T}{I}{O}{N}  { printShift(Tok_JFUNCTION); return(Tok_JFUNCTION); }
{M}{O}{D}{U}{L}{E}           { printShift(Tok_MODULE); return(Tok_MODULE);  }
{I}{M}{P}{O}{R}{T}           { printShift(Tok_IMPORT); return(Tok_IMPORT);  }
{F}{R}{O}{M}                 { printShift(Tok_FROM); return(Tok_FROM);    }
{R}{E}{P}{E}{A}{T}           { printShift(Tok_REPEAT); return(Tok_REPEAT);  }
{U}{N}{T}{I}{L}              { printShift(Tok_UNTIL); return(Tok_UNTIL);   }
{F}{O}{R}                    { printShift(Tok_FOR); return(Tok_FOR);     }
{T}{O}                       { printShift(Tok_TO); return(Tok_TO);      }
{D}{O}{W}{N}                 { printShift(Tok_DOWN); return(Tok_DOWN);    }
{A}{N}{D}                    { printShift(Tok_AND); return(Tok_AND);     }
{O}{R}                       { printShift(Tok_OR); return(Tok_OR);      }
{B}{R}{E}{A}{K}              { printShift(Tok_BREAK); return(Tok_BREAK);   }
{C}{O}{N}{T}{I}{N}{U}{E}     { printShift(Tok_CONTINUE); return(Tok_CONTINUE);}
{R}{E}{T}{U}{R}{N}           { printShift(Tok_RETURN); return(Tok_RETURN);  }
{U}{S}{E}                    { printShift(Tok_USE); return(Tok_USE);     }
{C}{L}{A}{S}{S}              { printShift(Tok_CLASS); return(Tok_CLASS);   }

":="                         { printShift(Tok_ASSIGNMENT); return(Tok_ASSIGNMENT); }
":"                          { printShift(Tok_COLON); return(Tok_COLON); }
","                          { printShift(Tok_COMMA); return(Tok_COMMA); }
"."                          { printShift(Tok_DOT); return(Tok_DOT); }
".."                         { printShift(Tok_DOTDOT); return(Tok_DOTDOT); }
"="                          { printShift(Tok_EQUAL); return(Tok_EQUAL); }
"!="                         { printShift(Tok_notEQUAL); return(Tok_notEQUAL); }
">="                         { printShift(Tok_GE); return(Tok_GE); }
">"                          { printShift(Tok_GT); return(Tok_GT); }
"["                          { printShift(Tok_LBRAC); return(Tok_LBRAC); }
"<="                         { printShift(Tok_LE); return(Tok_LE); }
"("                          { printShift(Tok_LPAREN); return(Tok_LPAREN); }
"<"                          { printShift(Tok_LT); return(Tok_LT); }
"-"                          { printShift(Tok_MINUS); return(Tok_MINUS); }
"+"                          { printShift(Tok_PLUS); return(Tok_PLUS); }
"]"                          { printShift(Tok_RBRAC); return(Tok_RBRAC); }
")"                          { printShift(Tok_RPAREN); return(Tok_RPAREN); }
";"                          { printShift(Tok_SEMICOLON); return(Tok_SEMICOLON); }
"/"                          { printShift(Tok_SLASH); return(Tok_SLASH); }
"*"                          { printShift(Tok_STAR); return(Tok_STAR); }
"?"                          { printShift(Tok_QUESTION); return (Tok_QUESTION); }
"+="                         { printShift(Tok_ADD_ASSIGN); return (Tok_ADD_ASSIGN); }
"-="                         { printShift(Tok_MINUS_ASSIGN); return (Tok_MINUS_ASSIGN); }
"*="                         { printShift(Tok_STAR_ASSIGN); return (Tok_STAR_ASSIGN); }
"/="                         { printShift(Tok_SLASH_ASSIGN); return (Tok_SLASH_ASSIGN); }

"`"([^`]*[\n\t]+)*"`" {
    string code = string(yytext);
    code = code.substr(1, code.size()-2);
    yylval->raw = strdup(code.c_str());
    printShift(Tok_JBC);
    return(Tok_JBC);
}

'([^\a\b\f\n\r\t\v']|\\\')*'|\"([^\a\b\f\n\r\t\v\"]|\\\")*\"   {
    yylval->raw = strdup(yytext);
    printShift(Tok_STRINGNUM);
    return(Tok_STRINGNUM);
}

"'"[a-zA-Z_][a-zA-Z0-9_]* {
    yylval->raw = strdup(yytext);
    printShift(Tok_LOOP_LABEL);
    return(Tok_LOOP_LABEL);
}

[a-zA-Z_][a-zA-Z0-9_]*(\.[a-zA-Z_][a-zA-Z0-9_]*)+ {
    yylval->raw = strdup(yytext);
    printShift(Tok_MOD_IDENTIFIER);
    return(Tok_MOD_IDENTIFIER);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval->raw = strdup(yytext);
    printShift(Tok_IDENTIFIER);
    return(Tok_IDENTIFIER);
}

{DIGIT}+ {
    yylval->raw = strdup(yytext);
    printShift(Tok_INTNUM);
    return(Tok_INTNUM);
}
{DIGIT}+("."{DIGIT}+([eE][+-]?{DIGIT}+)?|[eE][+-]?{DIGIT}+) {
    yylval->raw = strdup(yytext);
    printShift(Tok_REALNUM);
    return(Tok_REALNUM);
}

"//"[^\n]*                   {
    sprintf(tmpStr, "Comment at line %d : %s", yylineno, yytext);
    logger.info(tmpStr);
}
[ \t\f]    ;
\r         ;
\n    { colnum = 1; }
.    {
    sprintf(tmpStr, "Lexical analyzer error at line %d : %s", yylineno, yytext);
    logger.error(tmpStr);
}

%%

/*
int main(int argc, char** argv) {
  int i, tok;
  if (argc >= 2) {
      yyin = fopen(argv[1], "r");
  }

  for (i = 0; 1==1; i++) {
    tok = yylex();
    fprintf(stderr, "%3d-th token(type:%15s) on line %d : %s\n", i, tok, line_no, yytext);
  }
  return(0);
}

yywrap() {
  fclose(yyin);
  exit(1);
}
*/
