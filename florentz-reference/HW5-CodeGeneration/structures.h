//
// Created by barav on 14/12/2020.
//

#ifndef INC_236360_HW3_STRUCTURES_H
#define INC_236360_HW3_STRUCTURES_H

#include "symbol_table.h"
#include "bp.hpp"

using namespace std;


struct Exp {
    string m_name;
    string m_type;
    int m_val;
    string m_reg;
    string m_size;
    vector<pair<int,BranchLabelIndex>> m_trueList;
    vector<pair<int,BranchLabelIndex>> m_falseList;
    //  only used for set
    int from;
    int to;

    explicit Exp(const string& type) {
        this->m_type = string(type);
        this->m_trueList = vector<pair<int,BranchLabelIndex>>();
        this->m_falseList = vector<pair<int,BranchLabelIndex>>();
    }

    explicit Exp(const string& type, int from, int to) {
        this->m_type = string(type);
        this->m_trueList = vector<pair<int,BranchLabelIndex>>();
        this->m_falseList = vector<pair<int,BranchLabelIndex>>();
        this->from = from;
        this->to = to;
    }



    explicit Exp(Symbol* sm) {
        this->m_type = sm->m_type;
        this->m_name = sm->m_name;
        this->m_trueList = vector<pair<int,BranchLabelIndex>>();
        this->m_falseList = vector<pair<int,BranchLabelIndex>>();
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

struct TypeList {
    vector<string> m_typeList;
    TypeList() {
        this->m_typeList = vector<string>();
    }
    friend ostream& operator<<(ostream& os, const TypeList& sl) {
        for (auto & it : sl.m_typeList) {
            os << it << ", ";
        }
        os << endl;
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

typedef union Node
{
    Exp* exp;
    Id* id;
    FuncArg* funcArg;
    FuncArgsList* funcArgsList;
    TypeList* typeList;
    ExpList* expList;
    int val;
    Label* label;
    Block* block;
    Scope* scope;
} STYPE;

#define YYSTYPE STYPE	// Tell Bison to use STYPE as the stack type


#endif //INC_236360_HW3_STRUCTURES_H
