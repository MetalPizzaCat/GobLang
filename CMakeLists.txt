cmake_minimum_required(VERSION 3.5.0)
project(sila VERSION 0.1.0 LANGUAGES C CXX)

add_executable(sila 
main.cpp
execution/Type.hpp
execution/Type.cpp
execution/Machine.hpp
execution/Machine.cpp
execution/Memory.hpp
execution/Memory.cpp
)

target_link_options(sila PRIVATE -static-libgcc -static-libstdc++)