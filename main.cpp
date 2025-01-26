#include <iostream>
#include <string>
#include <cassert>
#include <variant>
#include <vector>
#include <functional>
#include <map>
#include <cstdint>

#include "compiler/Parser.hpp"
#include "compiler/Compiler.hpp"
#include "execution/Machine.hpp"

#include "execution/Machine.hpp"

namespace MachineFunctions
{
    void printLine(SimpleLang::Machine *machine)
    {
        SimpleLang::MemoryValue *v = machine->getStackTop();
        if (v == nullptr)
        {
            return;
        }
        switch (v->type)
        {
        case SimpleLang::Type::Null:
            std::cout << "null" << std::endl;
            break;
        case SimpleLang::Type::Number:
            std::cout << std::get<float>(v->value) << std::endl;
            break;
        case SimpleLang::Type::Int:
            std::cout << std::get<int32_t>(v->value) << std::endl;
            break;
        case SimpleLang::Type::UserData:
            std::cerr << "Invalid data type" << std::endl;
            break;
        case SimpleLang::Type::MemoryObj:
            std::cerr << "Invalid data type" << std::endl;
            break;
        case SimpleLang::Type::NativeFunction:
            std::cerr << "Native function: " << &std::get<SimpleLang::FunctionValue>(v->value) << std::endl;
            break;
        }
    }
}

int main(int, char **)
{
    std::string code = "a = 5 + 99 - piss; a = 2 + a;";
    std::cout << "Source: " << code << std::endl;
    SimpleLang::Compiler::Parser comp(code);
    comp.parse();
    comp.printInfoTable();
    comp.printCode();

    return EXIT_SUCCESS;
}
