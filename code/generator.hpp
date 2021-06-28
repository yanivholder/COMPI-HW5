//
// Created by yaniv on 19/06/2021.
//

#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
using namespace std;

class Generator {

public:
    static size_t var_num;
    static size_t str_num;

    Generator();

    static string new_tmp();
    static string new_string();
};

#endif //CODE_GENERATOR_HPP
