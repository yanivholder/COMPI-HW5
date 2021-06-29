//
// Created by yaniv on 19/06/2021.
//

#ifndef CODE_PARSER_FUNCS_HPP
#define CODE_PARSER_FUNCS_HPP

#include "hw3-output.hpp"
#include "sym_tables.hpp"
#include "structs.hpp"
#include "generator.hpp"

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

Exp* do_op(Exp* var1, Exp* var2, ops op, string specific_op, CodeBuffer& buffer);

void vaildate_type(const string& x, const string& y);

FuncSymbol* get_func(const string& name, const std::vector<string>& type_list, SymbolTable& symbolTable);

void validate_func(std::vector<string> expected, const std::vector<string>& current, string& name);

void validate_assign(const string& var1, const string& var2);

string get_var_type(string& var_name, SymbolTable& symbolTable);

void exp_to_bool(Exp *bool_exp, CodeBuffer& buffer);

void call_to_ir(Exp* call, Id* id, ExpList* expList, FuncSymbol* f, bool is_with_args, CodeBuffer& buffer);

void declarePrerequisites(CodeBuffer& buffer);


#endif //CODE_PARSER_FUNCS_HPP
