//
// Created by barav on 14/01/2021.
//

#include "var_generator.h"
#include "bp.hpp"
#include "structures.h"
#include "symbol_table.h"

#ifndef INC_236360_HW5_UTILS_H
#define INC_236360_HW5_UTILS_H


void printOP(string a, string b, string c, string op, CodeBuffer &buffer);

void retVoid(CodeBuffer &buffer);

void retVal(CodeBuffer &buffer, string ret_val);

void createIntersection(Exp *boolExp, CodeBuffer &buffer, VarGenerator &varGenerator);

void makeSet(Exp *setExp, int lowerBound, int upperBound, CodeBuffer &buffer, VarGenerator &varGenerator);

void castSetToInt(Exp *setExp, Exp *intExp, CodeBuffer &buffer, VarGenerator &varGenerator);

void copySet(string, string, CodeBuffer &buffer, VarGenerator &varGenerator);

void setBinop(Exp* result, Exp* firstExp, Exp* secondExp, CodeBuffer &buffer, VarGenerator &varGenerator, string operation);

void setIn(Exp* result, Exp* numExp, Exp* setExp, CodeBuffer &buffer, VarGenerator &varGenerator);


#endif //INC_236360_HW5_UTILS_H
