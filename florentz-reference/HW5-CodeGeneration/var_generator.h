//
// Created by barav on 11/01/2021.
//

#ifndef INC_236360_HW5_VAR_GENERATOR_H
#define INC_236360_HW5_VAR_GENERATOR_H


#include <string>
using namespace std;

class VarGenerator {
    size_t m_currVarNum;
    size_t m_currStrNum;

public:
    VarGenerator();
    string freshVar();
    string freshString();
};

#endif //INC_236360_HW5_VAR_GENERATOR_H
