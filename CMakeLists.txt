cmake_minimum_required(VERSION 3.5.0)
project(sila VERSION 0.1.0 LANGUAGES C CXX)


list(APPEND COMMON_SOURCE_FILES execution/Type.hpp
    execution/Type.cpp
    execution/Machine.hpp
    execution/Machine.cpp
    execution/Memory.hpp
    execution/Memory.cpp)



list(APPEND COMPILER_SOURCE_FILES execution/Type.hpp
    compiler/Compiler.hpp
    compiler/Compiler.cpp
    compiler/Token.hpp
    compiler/Token.cpp)

add_executable(sila
    main.cpp
    ${COMMON_SOURCE_FILES}
)

add_executable(silac
    compiler.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
)

target_link_options(sila PRIVATE -static-libgcc -static-libstdc++)
