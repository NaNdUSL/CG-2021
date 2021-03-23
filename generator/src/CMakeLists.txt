cmake_minimum_required(VERSION 3.5)

# Project Name
PROJECT(generator)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_compile_options(-Wall)
add_executable(${PROJECT_NAME} main.cpp)