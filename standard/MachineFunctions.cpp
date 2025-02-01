#include "MachineFunctions.hpp"
#include "../execution/Array.hpp"
#include "../execution/Memory.hpp"
void MachineFunctions::printLine(GobLang::Machine *machine)

{
    GobLang::MemoryValue *v = machine->getStackTopAndPop();
    if (v == nullptr)
    {
        return;
    }
    switch (v->type)
    {
    case GobLang::Type::Null:
        std::cout << "null" << std::endl;
        break;
    case GobLang::Type::Number:
        std::cout << std::get<float>(v->value) << std::endl;
        break;
    case GobLang::Type::Bool:
        std::cout << (std::get<bool>(v->value) ? "true" : "false") << std::endl;
        break;
    case GobLang::Type::Int:
        std::cout << std::get<int32_t>(v->value) << std::endl;
        break;
    case GobLang::Type::UserData:
        std::cerr << "Invalid data type" << std::endl;
        break;
    case GobLang::Type::MemoryObj:
        std::cerr << std::get<GobLang::MemoryNode *>(v->value)->toString() << std::endl;
        break;
    case GobLang::Type::NativeFunction:
        std::cerr << "Native function: " << &std::get<GobLang::FunctionValue>(v->value) << std::endl;
        break;
    }
    delete v;
}

void MachineFunctions::createArrayOfSize(GobLang::Machine *machine)
{
    GobLang::MemoryValue *sizeVal = machine->getStackTopAndPop();
    machine->pushToStack(GobLang::MemoryValue{
        .type = GobLang::Type::MemoryObj,
        .value = machine->createArrayOfSize(std::get<int32_t>(sizeVal->value))});
    delete sizeVal;
}
