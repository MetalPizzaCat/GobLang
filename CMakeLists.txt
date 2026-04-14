cmake_minimum_required(VERSION 3.20.0)
project(goblang VERSION 0.7.1 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(USE_STATIC_BUILD ON CACHE BOOL "Build static executable")

add_compile_definitions(GOB_LANG_VERSION_MAJOR=0)
add_compile_definitions(GOB_LANG_VERSION_MINOR=7)
add_compile_definitions(GOB_LANG_VERSION_PATCH=1)

add_compile_definitions(MAX_PRINT_DEPTH=1000)

add_compile_definitions(LINES_BEFORE_ERROR=3)
add_compile_definitions(LINES_AFTER_ERROR=3)

add_compile_definitions(DEFAULT_MIN_RAND_INT=0)
add_compile_definitions(DEFAULT_MAX_RAND_INT=2147483647)
list(APPEND CODEGEN_FILES
    codegen/CodeGenerator.cpp
    codegen/CodeNode.cpp
    codegen/Parser.cpp
    codegen/Token.cpp
    codegen/Builder.cpp
    codegen/CodeGenValue.cpp
    codegen/FunctionContext.cpp
)

list(APPEND COMMON_SOURCE_FILES 
    execution/Type.cpp
    execution/Value.cpp
    execution/State.cpp
    execution/Memory.cpp
    execution/Array.cpp
    execution/String.cpp
    execution/Exception.cpp
    execution/Closure.cpp
    execution/Error.cpp
)




add_executable(goblang
    main.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
    ${CODEGEN_FILES}
    #${STD_SOURCE_FILES}
)

add_executable(indev
    indev.cpp
    ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
    ${CODEGEN_FILES}
    #${STD_SOURCE_FILES}
)


add_library(goblanglib SHARED GobLang.hpp ${COMPILER_SOURCE_FILES}
    ${COMMON_SOURCE_FILES}
    #${STD_SOURCE_FILES}
    )

target_include_directories(goblanglib PUBLIC .)

if(${USE_STATIC_BUILD})
    message("Running a static build")
    target_link_options(goblang PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(goblanglib PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(indev PRIVATE -static-libgcc -static-libstdc++)
endif()
