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

void MachineFunctions::getSizeof(GobLang::Machine *machine)
{
    GobLang::MemoryValue *array = machine->getStackTopAndPop();
    if (array->type != GobLang::Type::MemoryObj)
    {
        throw GobLang::RuntimeException("Attempted to get a size of a non array object");
    }
    if (GobLang::ArrayNode *arrayNode = dynamic_cast<GobLang::ArrayNode *>(std::get<GobLang::MemoryNode *>(array->value)); arrayNode != nullptr)
    {
        machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::Int, .value = (int32_t)arrayNode->getSize()});
    }
    else if (GobLang::StringNode *strNode = dynamic_cast<GobLang::StringNode *>(std::get<GobLang::MemoryNode *>(array->value)); strNode != nullptr)
    {
        machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::Int, .value = (int32_t)strNode->getSize()});
    }
    delete array;
}
