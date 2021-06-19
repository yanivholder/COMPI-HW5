//
// Created by yaniv on 19/06/2021.
//

#include "generator.hpp"
#include <string>
using namespace std;

Generator::Generator() : var_num(0), str_num(0) {}

string Generator::new_tmp() {
    string var = "%t" + to_string(var_num);
    var_num++;
    return var;
}

string Generator::new_string() {
    string str = "@s" + to_string(str_num);
    str_num++;
    return str;
}
