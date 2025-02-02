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
    std::cout << GobLang::valueToString(*v) << std::endl;
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
