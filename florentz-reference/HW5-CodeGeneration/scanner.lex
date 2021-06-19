%{

/* Declarations section */
#include "structures.h"
#include "hw3_output.hpp"
#include "parser.tab.hpp"

%}

%option yylineno
%option noyywrap

letter          ([a-zA-Z])
digit           ([0-9])
nonzero         ([1-9])
whitespace      ([\t\r\n ])

%%

void                        return VOID;
int                         return INT;
byte                        return BYTE;
b                           return B;
bool                        return BOOL;
set                         return SET;
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
\;                          return SC;
\,                          return COMMA;
\(                          return LPAREN;
\)                          return RPAREN;
\{                          return LBRACE;
\}                          return RBRACE;
\[                          return LBRACKET;
\]                          return RBRACKET;
\=                          return ASSIGN;
\=\=                        return EQUALS;
!\=                         return NOT_EQUALS;
\<                          return LT;
>                           return GT;
\<\=                        return LTE;
\>\=                        return GTE;
in                          return IN;
\+                          return ADDBINOP;
\-                          return SUBBINOP;
\*                          return MULBINOP;
\/                          return DIVBINOP;
\.\.                        return DOTS;
{letter}({letter}|{digit})*         {
                                        string s(yytext);
                                        yylval.id = new Id(s);
                                        return ID;
                                    }
0|({nonzero}{digit}*)               {
                                        string s(yytext);
                                        yylval.val = stoi(s);
                                        return NUM;
                                    }
\"([^\n\r\"\\]|\\[rnt\"\\])+\"      {
                                        string s(yytext);
                                        yylval.id = new Id(s.substr(1, s.size() - 2));
                                        return STRING;
                                    }
\/\/[^\r\n]*[\r|\n|\r\n]?           ;
{whitespace}                        ;
.                                   output::errorLex(yylineno); exit(0);

%%
