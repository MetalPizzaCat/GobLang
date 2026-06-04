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

void len(State &state)
{
    if (std::optional<Value> v = state.popFromStack(); v.has_value())
    {
        if (std::holds_alternative<GobLang::ArrayObject *>(v.value()))
        {
            ArrayObject const *obj = (ArrayObject *)std::get<ArrayObject *>(v.value());
            state.pushToStack(obj->getSize());
        }
        else if (std::holds_alternative<GobLang::StringObject *>(v.value()))
        {
            ArrayObject const *obj = (ArrayObject *)std::get<StringObject *>(v.value());
            state.pushToStack(obj->getSize());
        }
        
    }
}
int main()
{
    std::cout << "Indev language testing executable. Only used for testing out language features during development. DO NOT USE" << std::endl;
    State state;
    // state.setGlobalVariable("test_hello", state.createClosure(&test_hello));
    state.setGlobalVariable("print", state.createClosure(&print));
    state.setGlobalVariable("len", state.createClosure(&len));

    // state.setGlobalVariable("f1", f1);
    state.executeClosure(*state.loadString(R"CODE(        
func for_each(array, f){
    let i = 0;
    while(i < len(array)){
        f(array[i]);
        i += 1;
    }}

func cond(a){
    print(a);
}

let test = [1,2,3,4,5,6,7];

for_each(test,cond);)CODE"));

    return EXIT_SUCCESS;
}