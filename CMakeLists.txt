cmake_minimum_required(VERSION 3.5.0)
project(calc VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_executable(calc 
    main.cpp
    main.hpp
)

