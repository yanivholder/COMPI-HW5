//
// Created by barav on 14/12/2020.
//

#include "symbol_table.h"
#include "hw3_output.hpp"
#include <vector>

extern  int yylineno;

SymbolTable::SymbolTable() {
    m_offsets = stack<int>();
    m_scopeTable = stack<vector<Symbol*>>();
    m_symbolMap = map<string, Symbol*>();
    m_offsets.push(0);

    openScope();
    vector<pair<string, string>> v1 = vector<pair<string, string>>();
    v1.push_back(pair<string, string>("STRING", "something_that_no_one_will_use1"));
    vector<pair<string, string>> v2 = vector<pair<string, string>>();
    v2.push_back(pair<string, string>("INT", "something_that_no_one_will_use2"));


    insertFuncSymbol("print", "VOID", v1);
    insertFuncSymbol("printi", "VOID", v2);
}


void SymbolTable::openScope() {
    vector<Symbol*> new_scope;
    m_offsets.push(m_offsets.top());
    m_scopeTable.push(new_scope);
}

void SymbolTable::closeScope() {
    //output::endScope();
    vector<Symbol*> last = m_scopeTable.top();

    for(auto it_sym = last.begin(); it_sym != last.end(); it_sym++) {
        //(*it_sym)->print_sym();
        m_symbolMap.erase((*it_sym)->m_name);
    }

    m_offsets.pop();
    m_scopeTable.pop();
}

void SymbolTable::closeGlobalScope()  {
    auto main_pair  = m_symbolMap.find("main");


    if (main_pair == m_symbolMap.end()) {
        output::errorMainMissing();
        exit(0);
    }

    if (main_pair->second->m_type != "FUNC") {
        output::errorMainMissing();
        exit(0);
    }

    FuncSymbol* main_func = (FuncSymbol*)main_pair->second;
    if (main_func->m_retType != "VOID") {
        output::errorMainMissing();
        exit(0);
    }

    if (main_func->m_argTypes.size() != 0) {
        output::errorMainMissing();
        exit(0);
    }

    closeScope();

}


void SymbolTable::insertSymbol(const string& name, const string& type) {
    if(m_symbolMap.find(name) != m_symbolMap.end()) {
        output::errorDef(yylineno ,name);
        exit(0);
    }
    Symbol* new_smb = new Symbol(name, type, m_offsets.top());
    m_scopeTable.top().push_back(new_smb);
    m_offsets.top() +=1;
    m_symbolMap.insert(pair<string, Symbol*>(name, new_smb));

}



void SymbolTable::insertFuncSymbol(const string& name, const string& retType, vector<pair<string, string>>& args) {
    check_name(name);
    vector<string> v = vector<string>();

    for (auto it = args.begin(); it != args.end(); it++){
        string arg_type = (*it).first;
        string arg_name = (*it).second;
        check_name(arg_name);
        v.push_back(arg_type);
    }

    FuncSymbol* new_smb = new FuncSymbol(name, retType,v, 0);
    m_scopeTable.top().push_back(new_smb);
    m_symbolMap.insert(pair<string, Symbol*>(name, new_smb));
}

void SymbolTable::check_name(const string& name) {
    if(m_symbolMap.find(name) != m_symbolMap.end()) {
        output::errorDef(yylineno ,name);
        exit(0);
    }
}

void SymbolTable:: initScopeArgs(vector<pair<string, string>>& args){
    int cur_offset = -1;
    for (auto it = args.begin(); it != args.end(); it ++){
        check_name(it->second);
        Symbol* new_sm = new Symbol(it->second, it->first, cur_offset);
        m_scopeTable.top().push_back(new_sm);
        m_symbolMap.insert(pair<string, Symbol*>(it->second, new_sm));
        cur_offset --;
    }
}


Symbol* SymbolTable::findSymbol(const string& name) {
    auto sm = m_symbolMap.find(name);
    if (sm == m_symbolMap.end()) {
        output::errorUndef(yylineno, name);
        exit(0);
    }
    return sm->second;
}

FuncSymbol*  SymbolTable::findFuncSymbol(const string& name, const vector<string>& argTypes){

    // checking if symbol exists
    auto sm = m_symbolMap.find(name);
    if (sm == m_symbolMap.end()) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }


    //checking if type of symbol is Func.
    if (sm->second->m_type != "FUNC") {
            output::errorUndefFunc(yylineno, name);
            exit(0);
    }


    FuncSymbol* func = (FuncSymbol*)sm->second;

    // checking if their mismatch between types
    if (argTypes.size() != func->m_argTypes.size()) {
        output::errorPrototypeMismatch(yylineno, name, func->m_argTypes);
        exit(0);
    }

    for (unsigned long i=0; i < argTypes.size(); i++) {
        if ( !(argTypes.at(i) == func->m_argTypes.at(i) || (argTypes.at(i) == "BYTE" && func->m_argTypes.at(i) == "INT") ) ) {
            output::errorPrototypeMismatch(yylineno, name, func->m_argTypes);
            exit(0);
        }
    }

    return func;
}

Symbol::Symbol(string name, string type, int offset) {
    m_name = name;
    m_type = type;
    m_offset = offset;

}
void Symbol::print_sym() {
    output::printID(m_name, m_offset, m_type);
}

FuncSymbol::FuncSymbol(string name, string retType, vector<string> argTypes, int offset) : Symbol(name, "FUNC", offset){
    m_retType = retType;
    m_argTypes = argTypes;
    if (retType == "VOID")
        m_irRetType = "void";
    else
        m_irRetType = "i32";
    m_irArgTypes = "(";
    if (!argTypes.empty()) {
        m_irArgTypes += "i32";
        for (unsigned long i=0; i < argTypes.size() - 1; i++) {
            m_irArgTypes += ", i32";
        }
    }
    m_irArgTypes += ")";
}


void FuncSymbol::print_sym() {
    output::printID(m_name, m_offset, output::makeFunctionType(m_retType, m_argTypes));
}

void SymbolTable::s_print(string s){
    cout << s << endl;
}