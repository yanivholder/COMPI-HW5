//
// Created by barav on 11/01/2021.
//

#include "var_generator.h"
#include <string>
using namespace std;

VarGenerator::VarGenerator() : m_currVarNum(0), m_currStrNum(0) {}

string VarGenerator::freshVar() {
    string var = "%t" + to_string(m_currVarNum);
    m_currVarNum++;
    return var;
}
string VarGenerator::freshString() {
    string str = "@str" + to_string(m_currStrNum);
    m_currStrNum++;
    return str;
}