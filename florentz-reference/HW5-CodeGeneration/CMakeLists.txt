cmake_minimum_required(VERSION 3.19)
project(HW5_CodeGeneration)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(HW5_CodeGeneration
        bp.cpp
        bp.hpp
        hw3_output.cpp
        hw3_output.hpp
        structures.h
        symbol_table.cpp
        symbol_table.h
        utils.cpp
        utils.h
        var_generator.cpp
        var_generator.h)
