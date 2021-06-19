//
// Created by barav on 14/12/2020.
//

#ifndef INC_236360_HW3_SYMBOL_TABLE_H
#define INC_236360_HW3_SYMBOL_TABLE_H

#include <stack>
#include <map>
#include <vector>
#include <string>
#include <iostream>
//#include "structures.h"

using namespace std;

class Symbol {
public:
    Symbol(string name, string type, int offset);
    string m_name;
    string m_type;
    int m_offset;
    virtual void print_sym();
};

class FuncSymbol : public Symbol {
public:
    FuncSymbol(string name, string retType, vector<string> argTypes, int offset);
    string m_retType;
    string m_irRetType;
    vector<string> m_argTypes;
    string m_irArgTypes;
    void print_sym() override;
};

class SymbolTable {
    stack<int> m_offsets;
    stack<vector<Symbol*>> m_scopeTable;
    map<string, Symbol*> m_symbolMap;

public:
    SymbolTable();
    void openScope();
    void closeScope();
    void closeGlobalScope();
    void insertSymbol(const string& name, const string& type);
    void insertFuncSymbol(const string& name, const string& retType, vector<pair<string, string>>& args);
    Symbol* findSymbol(const string& name);
    FuncSymbol* findFuncSymbol(const string& name, const vector<string>& argTypes);
    void check_name(const string& name);
    /** for functions, meant to initialize the formal arguments in the scope **/
    void initScopeArgs(vector<pair<string, string>>& args);
    void s_print(string s);

};


#endif //INC_236360_HW3_SYMBOL_TABLE_H
