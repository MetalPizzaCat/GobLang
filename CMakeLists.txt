cmake_minimum_required(VERSION 3.5.0)
project(sila VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND COMMON_SOURCE_FILES execution/Type.hpp
    execution/Type.cpp
    execution/Machine.hpp
    execution/Machine.cpp
    execution/Memory.hpp
    execution/Memory.cpp
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
)

add_executable(sila
    main.cpp
    ${STD_SOURCE_FILES}
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
)

add_executable(silac
    compiler.cpp
    ${STD_SOURCE_FILES}
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
)

target_link_options(sila PRIVATE -static-libgcc -static-libstdc++)
target_link_options(silac PRIVATE -static-libgcc -static-libstdc++)
