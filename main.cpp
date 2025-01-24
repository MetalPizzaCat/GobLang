#include <iostream>
#include <string>
#include <cassert>
#include <variant>
#include <vector>
#include <functional>
#include <map>
#include <cstdint>

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
    SimpleLang::Machine machine;

    machine.addStringConst("print");
    machine.addStringConst("some_var");
    machine.addIntConst(69);

    machine.addFunction(MachineFunctions::printLine, "print");

    machine.addOperation(SimpleLang::Operation::PushConstString);
    machine.addUInt8(1);
    machine.addOperation(SimpleLang::Operation::PushConstInt);
    machine.addUInt8(0);
    machine.addOperation(SimpleLang::Operation::Set);

    machine.addOperation(SimpleLang::Operation::PushConstString);
    machine.addUInt8(1);
    machine.addOperation(SimpleLang::Operation::Get);

    machine.addOperation(SimpleLang::Operation::PushConstString);
    machine.addUInt8(0);
    machine.addOperation(SimpleLang::Operation::Get);
    machine.addOperation(SimpleLang::Operation::Call);
    while (!machine.isAtTheEnd())
    {
        machine.step();
    }

    SimpleLang::Value strVal = new SimpleLang::StringNode("hello world");
    assert(std::holds_alternative<SimpleLang::MemoryNode *>(strVal));
    return EXIT_SUCCESS;
}
