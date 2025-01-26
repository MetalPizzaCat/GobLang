#include <iostream>
#include <algorithm>
#include "compiler/Parser.hpp"
#include "compiler/Compiler.hpp"
#include "execution/Machine.hpp"

#include "standard/MachineFunctions.hpp"

void test1(SimpleLang::Machine *machine)
{
    SimpleLang::MemoryValue *b = machine->getStackTopAndPop();
    SimpleLang::MemoryValue *a = machine->getStackTopAndPop();
    std::cout << "A: " << std::get<int32_t>(a->value) << " B: " << std::get<int32_t>(b->value) << std::endl;
    delete a;
    delete b;
}

void byteCodeToText(std::vector<uint8_t> const &bytecode)
{
    for (std::vector<uint8_t>::const_iterator it = bytecode.begin(); it != bytecode.end(); it++)
    {
        std::vector<SimpleLang::OperationData>::const_iterator opIt = std::find_if(
            SimpleLang::Operations.begin(),
            SimpleLang::Operations.end(),
            [it](SimpleLang::OperationData const &a)
            {
                return (uint8_t)a.op == *it;
            });
        if (opIt != SimpleLang::Operations.end())
        {
            std::cout << (opIt->text) << " ";
            for (int32_t i = 0; i < opIt->argCount; i++)
            {
                it++;
                std::cout << std::to_string(*it);
            }
            std::cout << std::endl;
        }
    }
}
int main()
{
    std::string code = "a = 9 + 3; print(a); print(a + a); test1(a,3)";
    std::cout << "Source: " << code << std::endl;
    SimpleLang::Compiler::Parser comp(code);
    comp.parse();
    comp.printInfoTable();
    comp.printCode();
    SimpleLang::Compiler::Compiler compiler(comp);
    compiler.compile();
    compiler.printCode();
    compiler.generateByteCode();
    byteCodeToText(compiler.getByteCode().operations);
    std::cout << "Executing code" << std::endl;
    SimpleLang::Machine machine(compiler.getByteCode());
    machine.createVariable("piss", SimpleLang::MemoryValue{.type = SimpleLang::Type::Int, .value = 69});
    machine.addFunction(MachineFunctions::printLine, "print");
    machine.addFunction(test1, "test1");
    while (!machine.isAtTheEnd())
    {
        machine.step();
    }
    // std::cout << "Value of a = " << std::get<int32_t>(machine.getVariableValue("a").value) << std::endl;
    return EXIT_SUCCESS;
}