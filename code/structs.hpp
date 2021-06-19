//
// Created by yaniv on 22/05/2021.
//

#ifndef HW3_CODE_STRUCTS_HPP
#define HW3_CODE_STRUCTS_HPP

#include <iostream>
#include <string>
#include <vector>
using namespace std;


struct Exp {
    string m_varName;
    string m_type;
    int m_val;

    explicit Exp(const string& type) {
        this->m_type = string(type);
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

typedef union Node
{
    Exp* exp;
    Id* id;
    FuncArg* funcArg;
    FuncArgsList* funcArgsList;
    TypeList* typeList;
    int val;
} STYPE;

#define YYSTYPE STYPE








#endif //HW3_CODE_STRUCTS_HPP
