//
// Created by yaniv on 22/05/2021.
//

#ifndef HW3_CODE_STRUCTS_HPP
#define HW3_CODE_STRUCTS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "bp.hpp"

using namespace std;


struct Exp {
    string m_name, m_type, m_size, m_reg;
    int m_val;
    size_t string_len;
    vector<pair<int,BranchLabelIndex>> m_trueList;
    vector<pair<int,BranchLabelIndex>> m_falseList;

    explicit Exp(const string& type, const string& reg_name = "") {
        this->m_type = string(type);
        this->m_reg = string(reg_name);
    }
};


struct Id {
    string m_name;

    explicit Id(const string& name) {
        this->m_name = string(name);
    }
};

struct FuncArg {
    pair<string, string> m_arg;

    FuncArg(const string& type, const string& name) {
        this->m_arg = pair<string, string>(type, name);
    }
    friend ostream& operator<<(ostream& os, const FuncArg& fa) {
        os << "Type: " << fa.m_arg.first << "\tName: " << fa.m_arg.second << std::endl;
        return os;
    }
};

struct FuncArgsList {
    vector<pair<string, string>> m_argsList;

    FuncArgsList() {
        this->m_argsList = vector<pair<string, string>>();
    }
    friend ostream& operator<<(ostream& os, const FuncArgsList& fal) {
        for (auto & it : fal.m_argsList) {
            os << "Type: " << it.first << "\tName: " << it.second << std::endl;
        }
        return os;
    }
};

struct ExpList {
    vector<Exp*> m_exp;
    vector<string> m_typeList;

    ExpList() {
        this->m_exp = vector<Exp*>();
        this->m_typeList = vector<string>();
    }

    friend ostream& operator<<(ostream& os, const ExpList& sl) {
        for (auto & it : sl.m_typeList) {
            os << it << ", ";
        }
        os << endl;
        return os;
    }
};

struct Label {
    string m_label;
    explicit Label(const string& label_name) {
        this->m_label = string(label_name);
    }
};

struct Block {
    vector<pair<int,BranchLabelIndex>> m_nextList;
    vector<pair<int,BranchLabelIndex>> m_breakList;
    vector<pair<int,BranchLabelIndex>> m_continueList;
    Block() {
        this->m_nextList = vector<pair<int,BranchLabelIndex>>();
        this->m_breakList = vector<pair<int,BranchLabelIndex>>();
        this->m_continueList = vector<pair<int,BranchLabelIndex>>();
    }
};

struct Scope {
    Exp* m_exp;
    string m_label;
    Block* m_block;

    Scope (Exp* exp, const string& label_name, const vector<pair<int,BranchLabelIndex>>& next_list, const vector<pair<int,BranchLabelIndex>>& break_list, const vector<pair<int,BranchLabelIndex>>& continue_list) {
        if (exp) {
            this->m_exp = new Exp(*exp);
        } else {
            this->m_exp = nullptr;
        }
        this->m_label = string(label_name);
        this->m_block = new Block();
        this->m_block->m_nextList = vector<pair<int,BranchLabelIndex>>(next_list);
        this->m_block->m_breakList = vector<pair<int,BranchLabelIndex>>(break_list);
        this->m_block->m_continueList = vector<pair<int,BranchLabelIndex>>(continue_list);
    }
};

struct Case
{
    Block* statements;
    string m_label;
    int jump_to;
    int m_val;
    Case(int arg_m_val, string arg_m_label, const vector<pair<int,BranchLabelIndex>>& next_list, const vector<pair<int,BranchLabelIndex>>& break_list, const vector<pair<int,BranchLabelIndex>>& continue_list) {

        this->statements = new Block();
        this->statements->m_nextList = vector<pair<int,BranchLabelIndex>>(next_list);
        this->statements->m_breakList = vector<pair<int,BranchLabelIndex>>(break_list);
        this->statements->m_continueList = vector<pair<int,BranchLabelIndex>>(continue_list);
        this->m_label =arg_m_label;
        this->m_val = arg_m_val;
    }

};

struct CaseStack
{
    std::stack<Case*> case_stack;
    Case* default_case;
    Case* last_case;

    vector<pair<int,BranchLabelIndex>> m_nextList;
    CaseStack() {
        this->default_case = nullptr;
    }
    CaseStack(std::stack<Case*> arg_case_stack) {
        this->case_stack = std::stack<Case*>(arg_case_stack);
        this->default_case = nullptr;

    }

    
};




typedef union Node
{
    Exp* exp;
    Id* id;
    FuncArg* funcArg;
    FuncArgsList* funcArgsList;
    ExpList* expList;
    int val;
    Label* label;
    Block* block;
    Scope* scope;
    CaseStack* case_stack;
    Case* case_struct;
} STYPE;

#define YYSTYPE STYPE








#endif //HW3_CODE_STRUCTS_HPP
