#include "MachineFunctions.hpp"
#include "../execution/Array.hpp"
#include "../execution/Memory.hpp"
void MachineFunctions::printLine(SimpleLang::Machine *machine)

{
    SimpleLang::MemoryValue *v = machine->getStackTopAndPop();
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
    case SimpleLang::Type::Bool:
        std::cout << (std::get<bool>(v->value) ? "true" : "false") << std::endl;
        break;
    case SimpleLang::Type::Int:
        std::cout << std::get<int32_t>(v->value) << std::endl;
        break;
    case SimpleLang::Type::UserData:
        std::cerr << "Invalid data type" << std::endl;
        break;
    case SimpleLang::Type::MemoryObj:
        std::cerr << std::get<SimpleLang::MemoryNode *>(v->value)->toString() << std::endl;
        break;
    case SimpleLang::Type::NativeFunction:
        std::cerr << "Native function: " << &std::get<SimpleLang::FunctionValue>(v->value) << std::endl;
        break;
    }
    delete v;
}

void MachineFunctions::createArrayOfSize(SimpleLang::Machine *machine)
{
    SimpleLang::MemoryValue *sizeVal = machine->getStackTopAndPop();
    machine->pushToStack(SimpleLang::MemoryValue{
        .type = SimpleLang::Type::MemoryObj,
        .value = machine->createArrayOfSize(std::get<int32_t>(sizeVal->value))});
    delete sizeVal;
}
