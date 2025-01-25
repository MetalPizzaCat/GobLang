#include "Machine.hpp"
#include <iostream>
#include <vector>
SimpleLang::Machine::Machine(Compiler::ByteCode const &code)
{
    m_constInts = code.ints;
    m_constStrings = code.ids;
    m_operations = code.operations;
}
void SimpleLang::Machine::addFunction(FunctionValue const &func, std::string const &name)

{
    m_variables[name] = MemoryValue{.type = Type::NativeFunction, .value = func};
}
void SimpleLang::Machine::step()
{
    if (m_programCounter >= m_operations.size())
    {
        return;
    }
    switch ((Operation)m_operations[m_programCounter])
    {
    case Operation::Add:
        addInt();
        break;
    case Operation::Sub:
        subInt();
        break;
    case Operation::Call:
        call();
        break;
    case Operation::Set:
        set();
        break;
    case Operation::Get:
        get();
        break;
    case Operation::PushConstInt:
        pushConstInt();
        break;
    case Operation::PushConstString:
        pushConstString();
        break;
    default:
        std::cerr << "Invalid op code: " << (int32_t)m_operations[m_programCounter] << std::endl;
        break;
    }
    m_programCounter++;
}

SimpleLang::MemoryValue *SimpleLang::Machine::getStackTop()

{
    if (m_operationStack.empty())
    {
        return nullptr;
    }
    else
    {
        return &m_operationStack[m_operationStack.size() - 1];
    }
}

void SimpleLang::Machine::createVariable(std::string const &name, MemoryValue const &value)
{
    m_variables[name] = value;
}
