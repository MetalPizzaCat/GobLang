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
    if (std::optional<Closure const *> f1 = state.getGlobalVariableAsType<Closure const *>("f1"); f1.has_value())
    {
        std::cout << "calling from c++" << std::endl;
        state.executeClosure(*f1.value());
    }
}

void print(State &state)
{
    if (std::optional<Value> v = state.popFromStack(); v.has_value())
    {
        std::cout << ValueOperations::toString(v.value()) << std::endl;
    }
}
int main()
{
    std::cout << "Indev language testing executable. Only used for testing out language features during development. DO NOT USE" << std::endl;
    State state;
    // state.setGlobalVariable("test_hello", state.createClosure(&test_hello));
    state.setGlobalVariable("print", state.createClosure(&print));

    Closure const *main = state.createClosure(std::vector<uint8_t>{
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

    Closure const *f1 = state.createClosure(std::vector<uint8_t>{
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
    // state.setGlobalVariable("f1", f1);
    state.executeClosure(*state.loadString(R"CODE(        
func fib(n){
   if(n <= 1){
      return n;
   }
   return fib(n - 1) + fib(n - 2);
}

print(fib(20));)CODE"));
    return EXIT_SUCCESS;
}