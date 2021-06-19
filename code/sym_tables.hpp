//
// Created by yaniv on 15/05/2021.
//

#ifndef HW3_CODE_SYM_TABLES_HPP
#define HW3_CODE_SYM_TABLES_HPP

#include <stack>
#include <vector>
#include "string"

using namespace std;

typedef pair<string, string> StrPair;
typedef vector<string> StrVec;
typedef vector<StrPair> StrPairVec;

class Symbol {
public:
    string name;
    string type;
    int offset;
    Symbol(string name, string type, int offset);
    virtual void print();
};

class VarSymbol : public Symbol {
public:
    VarSymbol(string name, string type, int offset);
    void print() override;
};

class FuncSymbol : public Symbol {
public:
    StrVec args_type = StrVec();
    string return_type = "";
    FuncSymbol(string name, string return_type, StrVec& args_type);
    void print() override;
};

typedef vector<Symbol*> SymVec;

class Table {
public:
    SymVec symbols = SymVec();
    Table* parent = nullptr;
    Table() = default;
    explicit Table(Table* parent) : parent(parent) {}
    ~Table() = default;
};

typedef vector<Table*> TableVec;

class SymbolTable {
public:
    stack<int> offset_stack;
    TableVec tables_stack;

    SymbolTable();
    Symbol* search_symbol(const string& name, bool is_func);
    Symbol* search_symbol_in_all_scopes(const string& name, bool is_func);
    void open_scope();
    void open_first_scope();
    void close_scope();
    void close_global_scope();
    void add_var_symbol(const string& name, const string& type);
    void add_func_symbol(const string& name, const string& return_type, StrPairVec& args);
    void add_func_args(StrPairVec& args);
    void check_name_not_exists_in_scope(const string& name, bool is_func);
};

#endif //HW3_CODE_SYM_TABLES_HPP
