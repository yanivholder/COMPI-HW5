//
// Created by yaniv on 19/06/2021.
//

#include "generator.hpp"
#include <string>
using namespace std;

size_t Generator::var_num = 0;
size_t Generator::str_num = 0;

Generator::Generator() {}

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
