cmake_minimum_required(VERSION 3.5.0)
project(goblang VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(GOB_LANG_VERSION_MAJOR=0)
add_compile_definitions(GOB_LANG_VERSION_MINOR=1)
list(APPEND COMMON_SOURCE_FILES execution/Type.hpp
    execution/Type.cpp
    execution/Operations.hpp
    execution/Value.hpp
    execution/Value.cpp
    execution/Machine.hpp
    execution/Machine.cpp
    execution/Memory.hpp
    execution/Memory.cpp
    execution/Array.hpp
    execution/Array.cpp
)


list(APPEND STD_SOURCE_FILES execution/Type.hpp
    standard/MachineFunctions.hpp
    standard/MachineFunctions.cpp
)



list(APPEND COMPILER_SOURCE_FILES execution/Type.hpp
    compiler/Compiler.hpp
    compiler/Compiler.cpp
    compiler/Parser.hpp
    compiler/Parser.cpp
    compiler/Token.hpp
    compiler/Token.cpp
    compiler/ByteCode.hpp
    compiler/Lexems.hpp
    compiler/CompilerToken.hpp
    compiler/CompilerToken.cpp
    compiler/SpecialCharacter.hpp
    compiler/CompilerNode.hpp
    compiler/CompilerNode.cpp
    compiler/Validator.hpp
    compiler/Validator.cpp
)

add_executable(goblang
    main.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
    ${STD_SOURCE_FILES}
)

add_executable(gobc
    compiler.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
    ${STD_SOURCE_FILES}
)

add_executable(gobtest
    test.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
)

target_link_options(goblang PRIVATE -static-libgcc -static-libstdc++)
target_link_options(gobc PRIVATE -static-libgcc -static-libstdc++)
