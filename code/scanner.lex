%{

/* Declarations section */
#include "structs.hpp"
#include "hw3-output.hpp"
#include "parser.tab.hpp"

%}

%option yylineno
%option noyywrap

digit           ([0-9])
nonzerodigit    ([1-9])
letter          ([a-zA-Z])
whitespace      ([\t\n ])

%%

void                        return VOID;
int                         return INT;
byte                        return BYTE;
b                           return B;
bool                        return BOOL;
and                         return AND;
or                          return OR;
not                         return NOT;
true                        return TRUE;
false                       return FALSE;
return                      return RETURN;
if                          return IF;
else                        return ELSE;
while                       return WHILE;
break                       return BREAK;
continue                    return CONTINUE;
switch                      return SWITCH;
case                        return CASE;
default                     return DEFAULT;
:                           return COLON;
;                           return SC;
,                           return COMMA;
\(                          return LPAREN;
\)                          return RPAREN;
\{                          return LBRACE;
\}                          return RBRACE;
=                           return ASSIGN;
==                          return RELOP_EQ;
!=                          return RELOP_NOT_EQ;
\<                          return RELOP_LT;
\>                          return RELOP_GT;
\<=                         return RELOP_LTE;
\>=                         return RELOP_GTE;
\+                          return BINOP_ADD;
\-                          return BINOP_SUB;
\*                          return BINOP_MUL;
\/                          return BINOP_DIV;
{letter}({letter}|{digit})* {
                                string name(yytext);
                                yylval.id = new Id(name);
                                return ID;
                            }
0|({nonzerodigit}{digit}*)  {
                                yylval.val = atoi(yytext);
                                return NUM;
                            }
\"([^\n\r\"\\]|\\[rnt\"\\])+\" {
	                                string name(yytext);
	                                // TODO understand why saving as an ID
                                    yylval.id = new Id(name.substr(1, name.size() - 2));
	                                return STRING;
                               }

[\t\r\n ]                   ;
\/\/[^\r\n]*(\r|\n|\r\n)?   ;

.                           {
                                output::errorLex(yylineno);
                                exit(1);
                            }

%%
