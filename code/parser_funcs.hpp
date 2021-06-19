//
// Created by yaniv on 19/06/2021.
//

#ifndef CODE_PARSER_FUNCS_HPP
#define CODE_PARSER_FUNCS_HPP

#include "hw3-output.hpp"
#include "sym_tables.hpp"
#include "structs.hpp"
#include "generator.hpp"

SymbolTable symbolTable = SymbolTable();
CodeBuffer& buffer = CodeBuffer::instance();


using namespace output;

typedef enum ops {
    PASSIGN,
    PRELOP,
    PAND,
    POR,
    PNOT,
    PBINOP,
} ops;

extern int yylineno;
int yyerror(const char * message);
extern int yylex();
extern char* yytext;

Exp* do_op(Exp* var1, Exp* var2, ops op);

void vaildate_type(string x, string y);

FuncSymbol* get_func(string name, std::vector<string> type_list);

void validate_func(std::vector<string> expected, std::vector<string> current, string name);

void validate_assign(string var1, string var2);

string get_var_type(string var_name);

void bool_intersection(Exp *bool_exp);

#endif //CODE_PARSER_FUNCS_HPP
