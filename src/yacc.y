%{
/*
 * grammar.y
 *
 * Pascal grammar in Yacc format, based originally on BNF given
 * in "Standard Pascal -- User Reference Manual", by Doug Cooper.
 * This in turn is the BNF given by the ANSI and ISO Pascal standards,
 * and so, is PUBLIC DOMAIN. The grammar is for ISO Level 0 Pascal.
 * The grammar has been massaged somewhat to make it LALR, and added
 * the following extensions.
 *
 * constant expressions
 * otherwise statement in a case
 * productions to correctly match else's with if's
 * beginnings of a separate compilation facility
 */

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include "node.h"
#include "symtab.h"
#include "codegen.h"
#include "y.tab.h"
#include "logger.h"
namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

#ifndef NULL
    #define NULL 0
#endif

int globalLogLevel;
static char tmpStr[256];
static Logger logger("[Scanner & Parser]", &globalLogLevel);

 /* Called by yyparse on error.  */
extern int yylineno;
extern char *yytext;
int yylex (YYSTYPE *lvalp, YYLTYPE *llocp);
void yyerror (char const *s) {
    sprintf (tmpStr, "%s: at line %d symbol'%s'", s, yylineno, yytext);
    logger.error(tmpStr);
}

void yyrestart  (FILE * input_file);

extern FILE *yyin;
Node *AST, *ASTroot;

%}
%union {
    char * raw;
}

%type <raw> Tok_INTNUM Tok_REALNUM Tok_STRINGNUM Tok_IDENTIFIER Tok_MOD_IDENTIFIER
%type <raw> Tok_JBC Tok_LOOP_LABEL

%union {
    Node * node;
}

%type <node> prog identifier_list identifier declarations type standard_type subprogram_declarations subprogram_declaration
%type <node> subprogram_head arguments parameter_list optional_var compound_statement optional_statements statement_list
%type <node> statement variable tail  procedure_statement expression_list expression simple_expression term factor sign
%type <node> addop mulop relop num
%type <node> jarguments jbody import import_list conditional_expression and_expression or_expression not_expression
%type <node> downto if_statement mod_identifier loop_label use_list
/* %type <node> mod_identifier_list */

%token Tok_ARRAY Tok_DO Tok_ELSE Tok_END Tok_FUNCTION Tok_IF Tok_NOT Tok_OF Tok_PBEGIN Tok_PROCEDURE Tok_PROGRAM Tok_THEN Tok_VAR Tok_WHILE
%token Tok_ASSIGNMENT Tok_COLON Tok_COMMA Tok_DOT Tok_DOTDOT Tok_EQUAL Tok_notEQUAL Tok_GE Tok_GT Tok_LBRAC Tok_LE Tok_LPAREN Tok_LT Tok_MINUS
%token Tok_PLUS Tok_RBRAC Tok_RPAREN Tok_SEMICOLON Tok_SLASH Tok_STAR Tok_STRING Tok_IDENTIFIER Tok_INTEGER
%token Tok_REAL Tok_INTNUM Tok_REALNUM Tok_STRINGNUM Tok_JFUNCTION Tok_JBC Tok_MODULE Tok_IMPORT Tok_FROM
%token Tok_REPEAT Tok_UNTIL Tok_FOR Tok_TO Tok_DOWN Tok_QUESTION Tok_AND Tok_OR Tok_BREAK Tok_CONTINUE Tok_RETURN
%token Tok_ADD_ASSIGN Tok_MINUS_ASSIGN Tok_STAR_ASSIGN Tok_SLASH_ASSIGN Tok_MOD_IDENTIFIER Tok_LOOP_LABEL
%token Tok_USE Tok_CLASS
%start prog

%locations
%define api.pure

%nonassoc LOWER_THAN_ELSE
%nonassoc Tok_ELSE

%%

prog :
    Tok_PROGRAM identifier Tok_LPAREN identifier_list Tok_RPAREN Tok_SEMICOLON use_list
    import_list
    declarations subprogram_declarations compound_statement Tok_DOT
    {
        $$ = MKN7(N_PROG, $2, MKN2(N_ARGV, $4, MKN0(N_TYPE_FILE)), $7, $8, $9, $10, $11);
        AST = $$;
		logger.info(yylloc, "- Reduce -- prog := PROGRAM id ( identifier_list ) ; declarations subprogram_declarations compound_statement");
    }
    | Tok_MODULE identifier Tok_SEMICOLON subprogram_declarations Tok_DOT
    {
        $$ = MKN2(N_MODULE, $2, $4);
        AST = $$;
        logger.info(yylloc, "- Reduce -- module := MODULE id ; subprogram_declarations .");
    }
    | Tok_CLASS identifier Tok_SEMICOLON import_list declarations subprogram_declarations Tok_DOT
    {
        $$ = MKN4(N_CLASS, $2, $4, $5, $6);
        AST = $$;
        logger.info(yylloc, "- Reduce -- module := CLASS id ; subprogram_declarations .");
    }
    ;

use_list :
    use_list Tok_USE identifier Tok_SEMICOLON
    {
        $$ = $1;
        APC($$, MKN1(N_USE, $3));
        logger.info(yylloc, "- Reduce -- import_list := ");
    }
    |
    {
        $$ = MKN0(N_USE_LIST);
        logger.info(yylloc, "- Reduce -- import_list := ");
    }
    ;

import_list :
    import_list import Tok_SEMICOLON
    {
        $$ = $1;
        APC($$, $2);
        logger.info(yylloc, "- Reduce -- import_list := import _list import ;");
    }
    |
    {
        $$ = MKN0(N_IMPORT_LIST);
        logger.info(yylloc, "- Reduce -- import_list := ");
    }
    ;

import :
    Tok_IMPORT identifier
    {
        $$ = MKN1(N_IMPORT, $2);
        logger.info(yylloc, "- Reduce -- import := import id");
    }
    | Tok_IMPORT Tok_STAR Tok_FROM identifier
    {
        $$ = MKN2(N_IMPORT, $4, MKN0(N_NULL));
        logger.info(yylloc, "- Reduce -- import := import * from id");
    }
    | Tok_IMPORT identifier_list Tok_FROM identifier
    {
        $$ = MKN2(N_IMPORT, $4, $2);
        logger.info(yylloc, "- Reduce -- import := import id_list from id");
    }
    ;

/* mod_identifier_list :
    mod_identifier
    {
        $$ = MKN1(N_ID_LIST, $1);
        logger.info(yylloc, "- Reduce -- identifier_list := identifier");
	}
    | mod_identifier_list Tok_COMMA mod_identifier
    {
        $$ = $1;
        APC($$, $3);
		logger.info(yylloc, "- Reduce -- identifier_list := identifier_list , identifier");
	}
    ; */


identifier_list :
    identifier
    {
        $$ = MKN1(N_ID_LIST, $1);
        logger.info(yylloc, "- Reduce -- identifier_list := identifier");
	}
	| identifier_list Tok_COMMA identifier
    {
        $$ = $1;
        APC($$, $3);
		logger.info(yylloc, "- Reduce -- identifier_list := identifier_list , identifier");
	}
    ;

mod_identifier :
    Tok_MOD_IDENTIFIER
    {
        $$ = MKN0(N_MOD_ID);
        //$$->raw = $1;
        $$->sval = strdup($1);
        logger.info(yylloc, "- Reduce -- identifier := mod_id");
    }
    | identifier
    {
        $$ = $1;
    }
    ;

loop_label :
    Tok_LOOP_LABEL
    {
        $$ = MKN0(N_ID);
        $$->sval = strdup($1);
        logger.info(yylloc, "- Reduce -- loop_label := label");
    }
    |
    {
        $$ = NULL;
        logger.info(yylloc, "- Reduce -- loop_label := lambda");
    }
    ;

identifier :
    Tok_IDENTIFIER
    {
        $$ = MKN0(N_ID);
        //$$->raw = $1;
        $$->sval = strdup($1);
        logger.info(yylloc, "- Reduce -- identifier := id");
    }
    ;

declarations :
    declarations Tok_VAR identifier_list Tok_COLON type Tok_SEMICOLON
    {
        $$ = $1;
        APC($$, MKN2(N_VAR_DECL, $3, $5));
		logger.info(yylloc, "- Reduce -- declarations := declarations VAR identifier_list : type ;");
	}
	|
    {
        $$ = MKN0(N_VAR_DECL_LIST);
		logger.info(yylloc, "- Reduce -- declarations := lambda");
	}
    ;

type :
    standard_type
    {
        $$ = $1;
		logger.info(yylloc, "- Reduce -- type := standard_type");
	}
	| Tok_ARRAY Tok_LBRAC num Tok_DOTDOT num Tok_RBRAC Tok_OF type
    {
        $$ = MKN3(N_TYPE_ARRAY, $3, $5, $8);
		logger.info(yylloc, "- Reduce -- type := ARRAY [ num .. num ] OF type");
	}
    | identifier
    {
        $$ = $1;
        $$->nodeType = N_TYPE_CLASS;
        logger.info(yylloc, "- Reduce -- type := identifier");
    }
    ;


standard_type :
    Tok_INTEGER
    {
        $$ = MKN0(N_TYPE_INT);
		logger.info(yylloc, "- Reduce -- standard_type := INTEGER");
	}
	| Tok_REAL
    {
        $$ = MKN0(N_TYPE_REAL);
		logger.info(yylloc, "- Reduce -- standard_type := REAL");
	}
    | Tok_STRING
    {
        $$ = MKN0(N_TYPE_STRING);
		logger.info(yylloc, "- Reduce -- standard_type := STRING");
	}
    ;

subprogram_declarations :
	subprogram_declarations subprogram_declaration Tok_SEMICOLON
    {
        $$ = $1;
        APC($$, $2);
		logger.info(yylloc, "- Reduce -- subprogram_declarations := subprogram_declarations subprogram_declaration ;");
	}
	|
    {
        $$ = MKN0(N_SUBPROGRAM_DECL_LIST);
		logger.info(yylloc, "- Reduce -- subprogram_declarations := lambda");
	}
    ;


subprogram_declaration :
	subprogram_head declarations compound_statement
    {
        $$ = MKN3(N_SUBPROGRAM_DECL, $1, $2, $3);
		logger.info(yylloc, "- Reduce -- subprogram_declaration := subprogram_head declarations compound_statement");
	}
    | Tok_JFUNCTION identifier Tok_LPAREN jarguments Tok_RPAREN Tok_COLON type Tok_SEMICOLON jbody
    {
        $$ = MKN2(N_JFUNCTION_DECL, MKN3(N_FUNCTION_HEAD, $2, $4, $7), $9);
        logger.info(yylloc, "- Reduce -- subprogram_declaration := JFUNCTION id ( jarguments ) : type ; jbody");
    }
    | Tok_JFUNCTION identifier Tok_LPAREN jarguments Tok_RPAREN Tok_SEMICOLON jbody
    {
        $$ = MKN2(N_JFUNCTION_DECL, MKN3(N_PROCEDURE_HEAD, $2, $4, MKN0(N_NULL)), $7);
        logger.info(yylloc, "- Reduce -- subprogram_declaration := JFUNCTION id ( jarguments ) ; jbody");
    }
    ;

jarguments :
    type
    {
        $$ = MKN1(N_PARAMETER_LIST, $1);
        logger.info(yylloc, "- Reduce -- jarguments ::= type");
    }
    | type Tok_COMMA jarguments
    {
        $$ = $3;
        APCL($$, $1);
        logger.info(yylloc, "- Reduce -- jarguments ::= type ; jarguments");
    }
    |
    {
        $$ = MKN0(N_PARAMETER_LIST);
        logger.info(yylloc, "- Reduce -- jarguments ::=");
    }
    ;

jbody :
    Tok_PBEGIN Tok_JBC Tok_END
    {
        $$ = MKN0(N_JBYTECODE);
        $$->sval = strdup($2);
        logger.info(yylloc, "- Reduce -- jbody ::= JAVA BYTECODE");
    }
    ;

subprogram_head :
    Tok_FUNCTION identifier arguments Tok_COLON standard_type Tok_SEMICOLON
    {
        $$ = MKN3(N_FUNCTION_HEAD, $2, $3, $5);
		logger.info(yylloc, "- Reduce -- subprogram_head ::= FUNCTION id arguments : standard_type ;");
	}
	| Tok_PROCEDURE identifier arguments Tok_SEMICOLON
    {
        $$ = MKN3(N_PROCEDURE_HEAD, $2, $3, MKN0(N_NULL));
		logger.info(yylloc, "- Reduce -- subprogram_head :=  PROCEDURE id arguments ;");
	}
    ;


arguments :
    Tok_LPAREN parameter_list Tok_RPAREN
    {
        $$ = $2;
        $$->nodeType = N_PARAMETER_LIST;
		logger.info(yylloc, "- Reduce -- arguments ::= ( parameter_list )");
	}
	|
    {
        $$ = MKN0(N_PARAMETER_LIST);
		logger.info(yylloc, "- Reduce -- arguments ::= lambda");
	}
    ;


parameter_list :
    optional_var identifier_list Tok_COLON type
    {
        $$ = MKN1(N_PARAMETER_LIST, MKN2(N_PARAMETER, $2, $4));
		logger.info(yylloc, "- Reduce -- parameter_list ::= optional_var identifier_list : type");
	}
	| optional_var identifier_list Tok_COLON type Tok_SEMICOLON parameter_list
    {
        $$ = $6;
        APCL($$, MKN2(N_PARAMETER, $2, $4));
		logger.info(yylloc, "- Reduce -- parameter_list ::= optional_var identifier_list : type ; parameter_list");
	}
    ;


optional_var :
    Tok_VAR
    {
        $$ = NULL;
		logger.info(yylloc, "- Reduce -- optional_var ::= VAR ");
	}
    |
    {
        $$ = NULL;
		logger.info(yylloc, "- Reduce -- optional_var ::= lambda ");
	}
    ;


compound_statement :
    Tok_PBEGIN optional_statements Tok_END
    {
        $$ = $2;
  		logger.info(yylloc, "- Reduce -- compound_statement ::= begin optional_statements end ");
  	}
    ;


optional_statements :
    statement_list
    {
        $$ = $1;
		logger.info(yylloc, "- Reduce -- optional_statements ::= statement_list ");
	}
    ;

statement_list :
    statement
    {
        $$ = MKN1(N_STATEMENT_LIST, $1);
		logger.info(yylloc, "- Reduce -- statement_list ::= statement ");
	}
	| statement_list Tok_SEMICOLON statement
    {
        $$ = $1;
        APC($$, $3);
		logger.info(yylloc, "- Reduce -- statement_list ::= statement_list ; statement ");
	}
    ;


if_statement :
    Tok_IF and_expression Tok_THEN statement  %prec LOWER_THAN_ELSE
    {
        $$ = MKN2(N_IF, $2, $4);
        logger.info(yylloc, "- Reduce -- if_statement ::=  IF and_expression THEN statement");
    }
    | Tok_IF and_expression Tok_THEN statement Tok_ELSE statement
    {
        $$ = MKN3(N_IF, $2, $4, $6);
        logger.info(yylloc, "- Reduce -- if_statement ::=  IF and_expression THEN statement ELSE statement ");
    }
    ;

statement :
    variable Tok_ADD_ASSIGN conditional_expression
    {
        $$ = MKN2(N_ASSIGN, $1, $3);
        $$->op = OP_ADD;
        $1->nodeType = N_VAR_REF;
        logger.info(yylloc, "- Reduce -- statement ::= variable += expression ");
    }
    | variable Tok_MINUS_ASSIGN conditional_expression
    {
        $$ = MKN2(N_ASSIGN, $1, $3);
        $$->op = OP_SUB;
        $1->nodeType = N_VAR_REF;
        logger.info(yylloc, "- Reduce -- statement ::= variable -= expression ");
    }
    | variable Tok_STAR_ASSIGN conditional_expression
    {
        $$ = MKN2(N_ASSIGN, $1, $3);
        $$->op = OP_MUL;
        $1->nodeType = N_VAR_REF;
        logger.info(yylloc, "- Reduce -- statement ::= variable *= expression ");
    }
    | variable Tok_SLASH_ASSIGN conditional_expression
    {
        $$ = MKN2(N_ASSIGN, $1, $3);
        $$->op = OP_DIV;
        $1->nodeType = N_VAR_REF;
        logger.info(yylloc, "- Reduce -- statement ::= variable /= expression ");
    }
    | variable Tok_ASSIGNMENT conditional_expression
    {
        $$ = MKN2(N_ASSIGN, $1, $3);
        $$->op = OP_NULL;
        $1->nodeType = N_VAR_REF;
        logger.info(yylloc, "- Reduce -- statement ::= variable := expression ");
	}
	| procedure_statement
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- statement ::= procedure_statement ");
	}
	| compound_statement
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- statement ::= compound_statement ");
	}
    | if_statement
    {
        $$ = $1;
    }
    /* | expression Tok_QUESTION statement Tok_COLON statement
    {
        $$ = MKN3(N_IF, $1, $3, $5);
        logger.info(yylloc, "- Reduce -- statement ::=  expression ? statement : statement ");
	} */
	| Tok_WHILE and_expression Tok_DO statement
    {
        $$ = MKN2(N_WHILE, $2, $4);
        logger.info(yylloc, "- Reduce -- statement ::= WHILE and_expression DO statement ");
	}
    | Tok_REPEAT statement Tok_UNTIL and_expression
    {
        $$ = MKN2(N_REPEAT, $2, $4);
        logger.info(yylloc, "- Reduce -- statement ::= REPEAT statement UNTIL and_expression ");
    }
    | Tok_FOR variable Tok_ASSIGNMENT conditional_expression downto simple_expression Tok_DO statement
    {
        $2->nodeType = N_VAR_REF;
        $$ = MKN4(N_FOR, MKN2(N_ASSIGN, $2, $4), $6, $8, $5);
        logger.info(yylloc, "- Reduce -- statement ::= FOR var := exp DOWNTO exp Do statement ");
    }
    | loop_label Tok_COLON Tok_WHILE and_expression Tok_DO statement
    {
        $$ = MKN3(N_WHILE, $4, $6, $1);
        logger.info(yylloc, "- Reduce -- statement ::= label : WHILE and_expression DO statement ");
	}
    | loop_label Tok_COLON Tok_REPEAT statement Tok_UNTIL and_expression
    {
        $$ = MKN3(N_REPEAT, $4, $6, $1);
        logger.info(yylloc, "- Reduce -- statement ::= label : REPEAT statement UNTIL and_expression ");
    }
    | loop_label Tok_COLON Tok_FOR variable Tok_ASSIGNMENT conditional_expression downto simple_expression Tok_DO statement
    {
        $4->nodeType = N_VAR_REF;
        $$ = MKN5(N_FOR, MKN2(N_ASSIGN, $4, $6), $8, $10, $7, $1);
        logger.info(yylloc, "- Reduce -- statement ::= label : FOR var := exp DOWNTO exp Do statement ");
    }
    | Tok_BREAK loop_label
    {
        if ($2) {
            $$ = MKN1(N_BREAK, $2);
        } else {
            $$ = MKN0(N_BREAK);
        }
        logger.info(yylloc, "- Reduce -- statement ::= BREAK");
    }
    | Tok_CONTINUE loop_label
    {
        if ($2) {
            $$ = MKN1(N_CONTINUE, $2);
        } else {
            $$ = MKN0(N_CONTINUE);
        }
        logger.info(yylloc, "- Reduce -- statement ::= BREAK");
    }
    | Tok_RETURN
    {
        $$ = MKN1(N_RETURN, MKN0(N_NULL));
        logger.info(yylloc, "- Reduce -- statement ::= RETURN");
    }
    | Tok_RETURN conditional_expression
    {
        $$ = MKN1(N_RETURN, $2);
        logger.info(yylloc, "- Reduce -- statement ::= RETURN conditional_expression");
    }
	|
    {
        $$ = NULL;
        //$$ = MKN0(N_STATEMENT);
        logger.info(yylloc, "- Reduce -- statement ::= lambda ");
    }
    ;

downto :
    Tok_DOWN Tok_TO
    {
        $$ = MKN0(N_OP);
        $$->op = OP_SUB;
        logger.info(yylloc, "- Reduce -- downto ::= DOWN TO ");
    }
    | Tok_TO
    {
        $$ = MKN0(N_OP);
        $$->op = OP_ADD;
        logger.info(yylloc, "- Reduce -- downto ::= DOWN ");
    }
    ;

variable :
    mod_identifier tail
    {
        $$ = $2;
        $$->nodeType = N_VAR;
        $$->sval = $1->sval;
        //$$ = MKN2(N_VAR, $1, $2);
        logger.info(yylloc, "- Reduce -- variable ::= id tail ");
    }
    ;

tail :
    Tok_LBRAC conditional_expression Tok_RBRAC tail
    {
        $$ = $4;
        APCL($$, $2);
        logger.info(yylloc, "- Reduce -- tail ::= [ conditional_expression ] tail ");
    }
	|
    {
        $$ = MKN0(N_TAIL);
        logger.info(yylloc, "- Reduce -- tail ::= lambda ");
    }
    ;

procedure_statement :
    mod_identifier
    {
        $$ = $1;
        $$->nodeType = N_SUBPROGRAM;
        logger.info(yylloc, "- Reduce -- procedure_statement ::= id ");
    }
	| mod_identifier Tok_LPAREN expression_list Tok_RPAREN
    {
        $$ = $1;
        $3->nodeType = N_ARGUMENT_LIST;
        APC($$, $3);
        $$->nodeType = N_SUBPROGRAM;
        logger.info(yylloc, "- Reduce -- procedure_statement ::= id ( expression_list )");
    }
    ;


expression_list :
    conditional_expression
    {
        $$ = MKN1(N_EXPRESSION_LIST, $1);
        logger.info(yylloc, "- Reduce -- expression_list ::= conditional_expression");
    }
	| expression_list Tok_COMMA conditional_expression
    {
        $$ = $1;
        APC($$, $3);
        logger.info(yylloc, "- Reduce -- expression_list ::= expression_list , conditional_expression");
    }
    ;

conditional_expression :
    and_expression
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- conditional_expression ::= and_expression");
    }
    | and_expression Tok_QUESTION conditional_expression Tok_COLON conditional_expression
    {
        $$ = MKN3(N_IF_TERNARY, $1, $3, $5);
        logger.info(yylloc, "- Reduce -- conditional_expression ::= and_expression ? conditional_expression : conditional_expression");
    }

and_expression :
    or_expression
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- and_expression ::= or_expression");
    }
    | and_expression Tok_AND or_expression
    {
        if ($1->nodeType != N_AND) {
            $$ = MKN1(N_AND, $1);
        } else {
            $$ = $1;
        }
        APC($$, $3);
        logger.info(yylloc, "- Reduce -- and_expression ::= and or_expression");
    }
    ;

or_expression :
    not_expression
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- or_expression ::= not_expression");
    }
    | or_expression Tok_OR not_expression
    {
        if ($1->nodeType != N_OR) {
            $$ = MKN1(N_OR, $1);
        } else {
            $$ = $1;
        }
        APC($$, $3);
        logger.info(yylloc, "- Reduce -- or_expression ::= or not_expression");
    }
    ;

not_expression :
    expression
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- not_expression ::= expression");
    }
    | Tok_NOT expression
    {
        $$ = MKN1(N_NOT, $2);
        logger.info(yylloc, "- Reduce -- not_expression ::= not expression");
    }
    ;

expression :
    simple_expression
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- expression ::= simple_expression ");
    }
	| simple_expression relop simple_expression
    {
        $$ = $2;
        APC($$, $1);
        APC($$, $3);
        logger.info(yylloc, "- Reduce -- expression ::= simple_expression relop simple_expression");
    }
    ;

simple_expression :
    term
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- simple_expression ::= term ");
    }
	| simple_expression addop term
    {
        $$ = $2;
        APC($$, $1);
        APC($$, $3);
        logger.info(yylloc, "- Reduce -- simple_expression ::= simple_expression addop term");
    }
    ;

term :
    factor
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- term ::= factor");
    }
    | sign factor
    {
        $$ = $1;
        APC($$, $2);
        logger.info(yylloc, "- Reduce -- term ::= sign factor");
    }
	| term mulop factor
    {
        $$ = $2;
        APC($$, $1);
        APC($$, $3);
        logger.info(yylloc, "- Reduce -- term ::= term mulop factor");
    }
    ;


factor :
    mod_identifier tail
    {
        $$ = $2;
        $$->nodeType = N_VAR_OR_SUBPROGRAM;
        $$->sval = $1->sval;
        //$$ = MKN2(N_VAR, $1, $2);
        logger.info(yylloc, "- Reduce -- factor ::= id tail ");
    }
    | standard_type Tok_LPAREN conditional_expression Tok_RPAREN
    {
        $$ = MKN1(N_TYPE_CONVERT, $3);
        $$->op = $1->nodeType;
        logger.info(yylloc, "- Reduce -- factor ::= standard_type ( conditional_expression )");
    }
	| mod_identifier Tok_LPAREN expression_list Tok_RPAREN
    {
        $$ = $1;
        $3->nodeType = N_ARGUMENT_LIST;
        APC($$, $3);
        $$->nodeType = N_SUBPROGRAM;
        logger.info(yylloc, "- Reduce -- factor ::= id ( expression_list )");
    }
	| num
    {
        $$ = $1;
        logger.info(yylloc, "- Reduce -- factor ::= num ");
    }
	| Tok_LPAREN conditional_expression Tok_RPAREN
    {
        $$ = $2;
        logger.info(yylloc, "- Reduce -- factor ::= ( conditional_expression )");
    }
	/* | Tok_NOT factor
    {
        $$ = MKN1(N_NOT, $2);
        logger.info(yylloc, "- Reduce -- factor ::= not factor");
    } */
    ;

sign :
    Tok_PLUS
    {
        $$ = MKN0(N_SIGN);
        $$->op = OP_ADD;
        logger.info(yylloc, "- Reduce -- sign ::= + ");
    }
    | Tok_MINUS
    {
        $$ = MKN0(N_SIGN);
        $$->op = OP_SUB;
        logger.info(yylloc, "- Reduce -- sign ::= - ");
    }
    ;

addop :
    Tok_PLUS
    {
        $$ = MKN0(N_OP);
        $$->op = OP_ADD;
        logger.info(yylloc, "- Reduce -- addop ::= + ");
    }
    | Tok_MINUS
    {
        $$ = MKN0(N_OP);
        $$->op = OP_SUB;
        logger.info(yylloc, "- Reduce -- addop ::= - ");
    }
    ;

mulop :
    Tok_STAR
    {
        $$ = MKN0(N_OP);
        $$->op = OP_MUL;
        logger.info(yylloc, "- Reduce -- mulop ::= * ");
    }
    | Tok_SLASH
    {
        $$ = MKN0(N_OP);
        $$->op = OP_DIV;
        logger.info(yylloc, "- Reduce -- mulop ::= / ");
    }
    ;

relop :
    Tok_LT
    {
        $$ = MKN0(N_OP);
        $$->op = OP_LT;
        logger.info(yylloc, "- Reduce -- relop ::= <");
    }
	| Tok_GT
    {
        $$ = MKN0(N_OP);
        $$->op = OP_GT;
        logger.info(yylloc, "- Reduce -- relop ::= >");
    }
	| Tok_EQUAL
    {
        $$ = MKN0(N_OP);
        $$->op = OP_EQ;
        logger.info(yylloc, "- Reduce -- relop ::= =");
    }
	| Tok_LE
    {
        $$ = MKN0(N_OP);
        $$->op = OP_LE;
        logger.info(yylloc, "- Reduce -- relop ::= <=");
    }
	| Tok_GE
    {
        $$ = MKN0(N_OP);
        $$->op = OP_GE;
        logger.info(yylloc, "- Reduce -- relop ::= >=");
    }
	| Tok_notEQUAL
    {
        $$ = MKN0(N_OP);
        $$->op = OP_NE;
        logger.info(yylloc, "- Reduce -- relop ::= != ");
    }
    ;

num :
    Tok_INTNUM
    {
        $$ = MKN0(N_INT);
        sscanf($1, "%d", &($$->ival));
        logger.info(yylloc, "- Reduce -- num ::= INTNUM ");
    }
    | Tok_REALNUM
    {
        $$ = MKN0(N_REAL);
        sscanf($1, "%lf", &($$->rval));
        logger.info(yylloc, "- Reduce -- num ::= REALNUM ");
    }
    | Tok_STRINGNUM
    {
        $$ = MKN0(N_STRING);
        string s($1);
        $$->sval = strdup(s.substr(1, s.size() - 2).c_str());
        /*strncpy($$->sval, $1 + 1, strlen($1 + 1) - 1);*/
        logger.info(yylloc, "- Reduce -- num ::= STRINGNUM ");
    }
    ;


%%

fs::path inputFile;
fs::path outputFile(fs::current_path()/fs::path("Main.jar"));
fs::path tmpWorkSpace(fs::temp_directory_path()/fs::path("mini-pascal-compiler")/fs::unique_path());
fs::path buildPath(tmpWorkSpace/fs::path("build"));
fs::path jCodePath(tmpWorkSpace);

bool disAST = false;
bool disSymTab = false;
static vector<fs::path> libPath({ fs::current_path() });

Node* parseModule (string fp, string base = ".pm") {
    int res;
    fp = fp + base;

    fs::path file(fp);
    bool fileExists = false;
    for (auto &dir : libPath) {
        if (fs::exists(dir/file)) {
            file = dir/file;
            fileExists = true;
            break;
        }
    }
    if (!fileExists) {
        sprintf(tmpStr, "%s :library not found",  file.generic_string().c_str());
        logger.error(tmpStr);
        return NULL;
    }

    if (freopen(file.generic_string().c_str(), "r", stdin) == NULL) {
        sprintf(tmpStr, "%s :Open input file error", file.generic_string().c_str());
        logger.error(tmpStr);
        return NULL;
    }
    yyrestart(yyin);
    yylineno = 1;
    res = yyparse();
    if (res != 0) {
        sprintf(tmpStr, "========== Parse %s failed ==========", file.generic_string().c_str());
        logger.error(tmpStr);
        return NULL;
    }
    fclose(stdin);
    return AST;
}

SymTable SymbolTable;

int main(int argc, char** argv) {
    int res;

    try {
        int logLevel = LOG_LEVEL_ERROR;
        Logger logger("", &logLevel);
        po::options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Displat this information")
            ("version,v", "Display compiler version information")
            ("output,o", po::value<string>(), "Place the output into <arg>")
            ("input,i", po::value<string>(), "Input file")
            ("build,b", po::value<string>(), "Place the jasmin instruction codes into <arg>")
            ("library,l", po::value<std::vector<std::string>>()->multitoken()->
                zero_tokens()->composing(), "Pathes for modules and classes")
            ("ast", "Display abstract syntax tree")
            ("symtab", "Display symbol table")
            ("verbose", "Display the compile information");

        po::positional_options_description p;
        p.add("input", -1);

        po::command_line_parser parser{argc, argv};
        parser.options(desc).positional(p).allow_unregistered().style(
            po::command_line_style::default_style |
            po::command_line_style::allow_slash_for_short);
        po::parsed_options parsed_options = parser.run();

        po::variables_map vm;
        po::store(parsed_options, vm);
        po::notify(vm);
        if (vm.count("help")) {
            cout << "Usage: mini-pascal file [options]" << endl;
            cout << desc << endl;
            return 0;
        }
        if (vm.count("version")) {
            cout << "Mini-Pascal version 1.0.0" << endl;
            return 0;
        }
        if (vm.count("output")) {
            outputFile = fs::absolute(fs::path(vm["output"].as<string>()));
            try {
                fs::create_directories(outputFile.parent_path());
            } catch (const fs::filesystem_error& e) {
                if (e.code() == boost::system::errc::permission_denied) {
                    sprintf(tmpStr, "%s :Permission is denied",
                        outputFile.parent_path().generic_string().c_str());
                    logger.error(tmpStr);
                } else {
                    sprintf(tmpStr, "%s :Create directory failed",
                        outputFile.parent_path().generic_string().c_str());
                    logger.error(tmpStr);
                }
            }
        }
        if (vm.count("input")) {
            inputFile = fs::absolute(fs::path(vm["input"].as<string>()));
            try {
                if (!fs::exists(inputFile) || !fs::is_regular_file(inputFile)) {
                    sprintf(tmpStr, "%s :Invalid input file", inputFile.c_str());
                    logger.error(tmpStr);
                    exit(-1);
                }
            } catch (const fs::filesystem_error& e) {
                if (e.code() == boost::system::errc::permission_denied) {
                    sprintf(tmpStr, "%s :Permission is denied",
                        inputFile.generic_string().c_str());
                    logger.error(tmpStr);
                } else {
                    sprintf(tmpStr, "%s :Read input file failed",
                        inputFile.generic_string().c_str());
                    logger.error(tmpStr);
                }
            }
        } else {
            logger.error("No input file");
            return -1;
        }
        if (vm.count("build")) {
            jCodePath = fs::absolute(fs::path(vm["build"].as<string>()));
            try {
                fs::create_directories(jCodePath);
            } catch (const fs::filesystem_error& e) {
                if (e.code() == boost::system::errc::permission_denied) {
                    sprintf(tmpStr, "%s :Permission is denied",
                        jCodePath.generic_string().c_str());
                    logger.error(tmpStr);
                } else {
                    sprintf(tmpStr, "%s :Create directory failed",
                        jCodePath.generic_string().c_str());
                    logger.error(tmpStr);
                }
            }
        }
        if (vm.count("library")) {
            for (auto &dir : vm["library"].as<vector<string>>()) {
                libPath.push_back(fs::absolute(fs::path(dir)));
            }
            for (auto &dir : libPath) {
                try {
                    if (!fs::exists(dir)) {
                        sprintf(tmpStr, "%s : Target directory does not exist", dir.generic_string().c_str());
                        logger.error(tmpStr);
                        exit(-1);
                    } else if (!fs::is_directory(dir)) {
                        sprintf(tmpStr, "%s : Target is not a directory", dir.generic_string().c_str());
                        logger.error(tmpStr);
                        exit(-1);
                    }
                } catch (const fs::filesystem_error& e) {
                    if (e.code() == boost::system::errc::permission_denied) {
                        sprintf(tmpStr, "%s :Permission is denied",
                            dir.generic_string().c_str());
                        logger.error(tmpStr);
                    } else {
                        sprintf(tmpStr, "%s :Read input file failed",
                            dir.generic_string().c_str());
                        logger.error(tmpStr);
                    }
                }
            }
        }
        if (vm.count("ast")) {
            disAST = true;
        }
        if (vm.count("symtab")) {
            disSymTab = true;
        }
        if (vm.count("verbose")) {
            globalLogLevel = LOG_LEVEL_INFO;
        } else {
            globalLogLevel =  LOG_LEVEL_ERROR;
        }
    } catch (const po::error &ex) {
        cerr << ex.what() << endl;
        return -1;
    }

    try {
        fs::create_directories(tmpWorkSpace);
    } catch (const fs::filesystem_error& e) {
        if (e.code() == boost::system::errc::permission_denied) {
            logger.error("Create workspace directory failed :Permission is denied");
        } else {
            logger.error("Create workspace directory failed");
        }
    }

    if (freopen(inputFile.generic_string().c_str(), "r", stdin) == NULL) {
        sprintf(tmpStr, "%s :Open input file error", inputFile.generic_string().c_str());
        logger.error(tmpStr);
        fs::remove_all(tmpWorkSpace);
        exit(-1);
    }

    res = yyparse();
    if (res == 0)
        logger.info("========== Parse success ==========");
    else {
        logger.error("========== Parse failed ==========");
        fs::remove_all(tmpWorkSpace);
        exit(-1);
    }
    ASTroot = AST;


    sprintf(tmpStr, "============ Checking %s =============",
        inputFile.generic_string().c_str());
    logger.info(tmpStr);
    bool noError = SymbolTable.semanticCheck(ASTroot);
    sprintf(tmpStr, "============ END %s =============",
        inputFile.generic_string().c_str());
    logger.info(tmpStr);

    if (!noError) {
        fs::remove_all(tmpWorkSpace);
        exit(-1);
    }

    if (disAST) {
        fputs("========= AST ==========\n", stderr);
        printTree(ASTroot, 0);
        fputs("========= AST END ==========\n", stderr);
    }
    if (disSymTab) {
        fputs("========= SYMTAB ==========\n", stderr);
        SymbolTable.dumpAll();
        fputs("========= SYMTAB END ==========\n", stderr);
    }

    CodeGen CodeGenerator(ASTroot, SymbolTable, "__Main");
    CodeGenerator.dump();

    // pack JAR
    fs::current_path(tmpWorkSpace);
    {
        cerr << "Creating Java Class files..." << endl;

        fs::create_directories(buildPath);

        system("java -jar $SNAP/usr/lib/mini-pascal/jasmin.jar *.j -d ./build");

        if (!fs::equivalent(jCodePath, fs::current_path())) {
            sprintf(tmpStr, "cp *.j %s", jCodePath.generic_string().c_str());
            system(tmpStr);
        }
    }
    {
        cerr << "Creating Java Jar file..." << endl;

        fs::path metaPath = fs::path("./build/META-INF");
        fs::path metaFile = fs::path("./build/META-INF/MANIFEST.MF");
        fs::create_directories(metaPath);
        FILE* meta = fopen(metaFile.c_str(), "w");
        fprintf(meta, "Main-Class: __Main\n");
        fclose(meta);

        fs::current_path(tmpWorkSpace/fs::path("build"));
        sprintf(tmpStr, "jar cmvf ./META-INF/MANIFEST.MF %s *.class", outputFile.generic_string().c_str());
        system(tmpStr);
    }
    fs::remove_all(tmpWorkSpace);
}

 #include "lex.yy.cc"
