cmake_minimum_required(VERSION 3.19)
project(code)

set(CMAKE_CXX_STANDARD 17)

add_executable(HW5
        bp.cpp
        bp.hpp
        hw3-output.cpp
        hw3-output.hpp
        structs.hpp
        sym_tables.cpp
        sym_tables.hpp
        generator.hpp
        generator.cpp
        parser_funcs.cpp
        parser_funcs.hpp)