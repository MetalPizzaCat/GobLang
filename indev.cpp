#include <iostream>
#include <algorithm>
#include <fstream>
#include "codegen/Parser.hpp"
#include "codegen/CodeGenerator.hpp"

#include "codegen/Disassembly.hpp"

// #define INDEV_DEBUG_TREE_ONLY
#define INDEV_DEBUG_RUN_FULL_CODE
#define INDEV_DEBUG_SHOW_TREE

#include "execution/State.hpp"

using namespace GobLang;

void test_hello(State &state)
{
    std::cout << "hello world" << std::endl;
}

void print(State &state)
{
    if (std::optional<Value> v = state.popFromStack(); v.has_value())
    {
        std::cout << valueToString(v.value()) << std::endl;
    }
}
int main()
{
    std::cout << "Indev language testing executable. Only used for testing out language features during development. DO NOT USE" << std::endl;
    State state;
    state.setGlobalVariable("test_hello", state.createCppFunction(&test_hello));
    state.setGlobalVariable("print", state.createCppFunction(&print));

    GobFunction const *main = state.createFunction(std::vector<uint8_t>{
                                                       (uint8_t)Instruction::GetGlobal,
                                                       (uint8_t)0,
                                                       (uint8_t)0,
                                                       (uint8_t)0,
                                                       (uint8_t)0,
                                                       (uint8_t)0,
                                                       (uint8_t)0,
                                                       (uint8_t)0,
                                                       (uint8_t)0,
                                                       (uint8_t)Instruction::Call,
                                                   },
                                                   {"test_hello"}, "main");

    GobFunction const *f1 = state.createFunction(std::vector<uint8_t>{
                                                     (uint8_t)Instruction::PushConstString,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)1,
                                                     (uint8_t)Instruction::GetGlobal,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)0,
                                                     (uint8_t)Instruction::Call,
                                                 },
                                                 {"print", "hello world"}, "f1");

    state.execute_closure(Closure(main));
    return EXIT_SUCCESS;
}