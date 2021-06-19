//
// Created by yaniv on 22/05/2021.
//

#include "sym_tables.hpp"
#include "hw3-output.hpp"

 extern int yylineno;

Symbol::Symbol(string name, string type, int offset) : name(name), type(type), offset(offset) {}

void Symbol::print() {}

VarSymbol::VarSymbol(string name, string type, int offset) : Symbol(name, type, offset) {}

void VarSymbol::print() {
    output::printID(name, offset, type);
}

FuncSymbol::FuncSymbol(string name, string return_type, StrVec& args_type) :
    Symbol(name, "FUNC", 0), return_type(return_type), args_type(args_type) {}

void FuncSymbol::print() {
    output::printID(name, offset, output::makeFunctionType(return_type, args_type));
}

string FuncSymbol::ir_params(){
    string res("(");
    if (!this->args_type.empty()){
        res += "i32";
        for (int i=0; i < this->args_type.size(); i++){
            res += ", i32";
        }
    }
    res+=")";
    return  res;
}

SymbolTable::SymbolTable() {
    offset_stack = stack<int>();
    tables_stack = TableVec();
    open_first_scope();

    StrPairVec print_args = StrPairVec();
    print_args.push_back(StrPair("STRING", "arg"));
    add_func_symbol("print", "VOID", print_args);

    StrPairVec printi_args = StrPairVec();
    printi_args.push_back(StrPair("INT", "arg"));
    add_func_symbol("printi", "VOID", printi_args);
}

Symbol* SymbolTable::search_symbol(const string& name, bool is_func) {
    for (auto table_it = tables_stack.rbegin(); table_it != tables_stack.rend(); table_it++) {
        for (auto symbol_it = (*table_it)->symbols.rbegin(); symbol_it != (*table_it)->symbols.rend(); symbol_it++) {
            if(is_func) {
                if ((*symbol_it)->name == name && (*symbol_it)->type == "FUNC") {
                    return (*symbol_it);
                }
            }
            else {
                if ((*symbol_it)->name == name && (*symbol_it)->type != "FUNC") {
                    return (*symbol_it);
                }
            }
        }
    }
    return nullptr;
}

Symbol* SymbolTable::search_symbol_in_all_scopes(const string &name, bool is_func) {
    for (auto table_it = tables_stack.rbegin(); table_it != tables_stack.rend(); table_it++) {
        for (auto symbol_it = (*table_it)->symbols.rbegin(); symbol_it != (*table_it)->symbols.rend(); symbol_it++) {
            if(is_func) {
                if ((*symbol_it)->name == name && (*symbol_it)->type == "FUNC") {
                    return (*symbol_it);
                }
            }
            else {
                if ((*symbol_it)->name == name && (*symbol_it)->type != "FUNC") {
                    return (*symbol_it);
                }
            }
        }
    }
    return nullptr;
}

void SymbolTable::open_scope() {
    auto* t = new Table(tables_stack.back());
    tables_stack.push_back(t);
    offset_stack.push(offset_stack.top());
}

void SymbolTable::open_first_scope() {
    auto* t = new Table(nullptr);
    tables_stack.push_back(t);
    offset_stack.push(0);
}

void SymbolTable::close_scope() {
    output::endScope();
    for (auto symbol : tables_stack.back()->symbols) {
        symbol->print();
    }
    delete tables_stack.back();
    tables_stack.pop_back();
    offset_stack.pop();
}

void SymbolTable::close_global_scope() {
    auto main_symbol = search_symbol("main", true);
    auto* main_func_symbol = (FuncSymbol*) main_symbol;
    if(main_symbol == nullptr || main_func_symbol->return_type != "VOID"
       || !main_func_symbol->args_type.empty()) {
        output::errorMainMissing();
        exit(1);
    }
    close_scope();
}

void SymbolTable::add_var_symbol(const string& name, const string& type) {
    check_name_not_exists_in_scope(name, false);
    auto* s = new VarSymbol(name, type, offset_stack.top());
    tables_stack.back()->symbols.push_back(s);
    offset_stack.top() += 1;
}

void SymbolTable::add_func_symbol(const string& name, const string& return_type, StrPairVec& args) {
    check_name_not_exists_in_scope(name, true);
    StrVec args_type = StrVec();
    for(auto arg_it = args.rbegin(); arg_it != args.rend(); arg_it++) {
        check_name_not_exists_in_scope((*arg_it).second, false);
        args_type.push_back((*arg_it).first);
    }
    auto* f = new FuncSymbol(name, return_type, args_type);
    tables_stack.back()->symbols.push_back(f);
}

void SymbolTable::add_func_args(StrPairVec& args) {
    int args_offset = -1;
    for(auto arg_it = args.rbegin(); arg_it != args.rend(); arg_it++) {
        check_name_not_exists_in_scope((*arg_it).second, false);
        auto* s = new VarSymbol((*arg_it).second, (*arg_it).first, args_offset);
        tables_stack.back()->symbols.push_back(s);
        args_offset -= 1;
    }
}

void SymbolTable::check_name_not_exists_in_scope(const string& name, bool is_func) {
    if(search_symbol_in_all_scopes(name, is_func) != nullptr || search_symbol_in_all_scopes(name, !is_func) != nullptr) {
        output::errorDef(yylineno ,name);
        exit(1);
    }
}