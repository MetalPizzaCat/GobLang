cmake_minimum_required(VERSION 3.5.0)
project(goblang VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(USE_STATIC_BUILD ON CACHE BOOL "Build static executable")

add_compile_definitions(GOB_LANG_VERSION_MAJOR=0)
add_compile_definitions(GOB_LANG_VERSION_MINOR=6)
add_compile_definitions(GOB_LANG_VERSION_PATCH=0)

add_compile_definitions(LINES_BEFORE_ERROR=3)
add_compile_definitions(LINES_AFTER_ERROR=3)

add_compile_definitions(DEFAULT_MIN_RAND_INT=0)
add_compile_definitions(DEFAULT_MAX_RAND_INT=2147483647)
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
    execution/Exception.hpp
    execution/Exception.cpp
    execution/Function.hpp
    execution/Structure.hpp
    execution/Structure.cpp
)


list(APPEND STD_SOURCE_FILES execution/Type.hpp
    standard/MachineFunctions.hpp
    standard/MachineFunctions.cpp
    standard/File.hpp
    standard/File.cpp
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
    compiler/ReversePolishGenerator.hpp
    compiler/ReversePolishGenerator.cpp
    compiler/FunctionTokenSequence.hpp
    compiler/Disassembly.hpp
)

add_executable(goblang
    main.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
    ${STD_SOURCE_FILES}
)

add_executable(indev
    indev.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
    ${STD_SOURCE_FILES}
)

add_executable(gobtest
    test.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
)

add_library(goblanglib  SHARED GobLang.hpp ${COMPILER_SOURCE_FILES}
${COMMON_SOURCE_FILES}
${STD_SOURCE_FILES})

target_include_directories(goblanglib PUBLIC .)

if(${USE_STATIC_BUILD})
    message("Running a static build")
    target_link_options(goblang PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(goblanglib PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(indev PRIVATE -static-libgcc -static-libstdc++)
endif()