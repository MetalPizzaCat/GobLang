#include <iostream>
#include <algorithm>
#include <fstream>
#include "compiler/Parser.hpp"
#include "compiler/Compiler.hpp"
#include "execution/Machine.hpp"
#include "compiler/Validator.hpp"

#include "standard/MachineFunctions.hpp"

void test1(GobLang::Machine *machine)
{
    GobLang::MemoryValue *b = machine->getStackTopAndPop();
    GobLang::MemoryValue *a = machine->getStackTopAndPop();
    std::cout << "A: " << std::get<int32_t>(a->value) << " B: " << std::get<int32_t>(b->value) << std::endl;
    delete a;
    delete b;
}

void byteCodeToText(std::vector<uint8_t> const &bytecode)
{
    size_t address = 0;
    for (std::vector<uint8_t>::const_iterator it = bytecode.begin(); it != bytecode.end(); it++)
    {
        std::vector<GobLang::OperationData>::const_iterator opIt = std::find_if(
            GobLang::Operations.begin(),
            GobLang::Operations.end(),
            [it](GobLang::OperationData const &a)
            {
                return (uint8_t)a.op == *it;
            });
        if (opIt != GobLang::Operations.end())
        {
            std::cout << std::hex << address << std::dec << ": " << (opIt->text) << " ";
            if (opIt->argCount == sizeof(GobLang::ProgramAddressType))
            {
                size_t reconAddr = 0x0;
                for (int32_t i = 0; i < sizeof(GobLang::ProgramAddressType); i++)
                {
                    it++;
                    size_t offset = ((sizeof(GobLang::ProgramAddressType) - i - 1)) * 8;
                    reconAddr |= (size_t)(*it) << offset;
                }
                address += sizeof(GobLang::ProgramAddressType);
                std::cout << std::hex << reconAddr << std::dec;
                ;
            }
            else
            {
                for (int32_t i = 0; i < opIt->argCount; i++)
                {
                    it++;
                    address += 1;
                    std::cout << std::to_string(*it);
                }
            }
            address++;
            std::cout << std::endl;
        }
    }
}
int main()
{
    std::string file = "./code.gob";
    std::vector<std::string> lines;
    std::ifstream codeFile(file);
    if (!codeFile.is_open())
    {
        std::cerr << "Unable to open code file" << std::endl;
        return EXIT_FAILURE;
    }
    std::string to;
    while (std::getline(codeFile, to, '\n'))
    {
        lines.push_back(to);
    }

    GobLang::Compiler::Parser comp(lines);
    int a = 3;
    bool b = !(a < 3 || (a > 1 && !(a < 1)));
    std::cout << (b ? "true" : "false") << std::endl;
    comp.parse();
    comp.printCode();
    GobLang::Compiler::Validator validator(comp);
    validator.validate();
    GobLang::Compiler::Compiler compiler(comp);
    compiler.compile();
    compiler.printCode();
    compiler.generateByteCode();
    byteCodeToText(compiler.getByteCode().operations);

    GobLang::Machine machine(compiler.getByteCode());
    machine.addFunction(MachineFunctions::getSizeof, "sizeof");
    machine.addFunction(MachineFunctions::printLine, "print_line");
    machine.addFunction(MachineFunctions::print, "print");
    machine.addFunction(MachineFunctions::createArrayOfSize, "array");
    machine.addFunction(MachineFunctions::input, "input");
    machine.addFunction(MachineFunctions::Math::toInt, "to_int");
    machine.addFunction(MachineFunctions::Math::randomIntInRange, "rand_range");
    machine.addFunction(MachineFunctions::Math::randomInt, "rand");
    std::vector<size_t> debugPoints = {};
    while (!machine.isAtTheEnd())
    {
        if (std::find(debugPoints.begin(), debugPoints.end(), machine.getProgramCounter()) != debugPoints.end())
        {
            std::cout << "Debugging at " << std::hex << machine.getProgramCounter() << std::dec << ". Memory state: " << std::endl;
            machine.printGlobalsInfo();
            machine.printVariablesInfo();
            machine.printStack();
        }
        machine.step();
    }
    // std::cout << "Value of a = " << std::get<int32_t>(machine.getVariableValue("a").value) << std::endl;
    return EXIT_SUCCESS;
}