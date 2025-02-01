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
    size_t address = 0;
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
            std::cout << std::hex << address << std::dec << ": " << (opIt->text) << " ";
            if (opIt->argCount == sizeof(SimpleLang::ProgramAddressType))
            {
                size_t reconAddr = 0x0;
                for (int32_t i = 0; i < sizeof(SimpleLang::ProgramAddressType); i++)
                {
                    it++;
                    size_t offset = ((sizeof(SimpleLang::ProgramAddressType) - i - 1)) * 8;
                    reconAddr |= (size_t)(*it) << offset;
                }
                address += sizeof(SimpleLang::ProgramAddressType);
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
    // std::string code = "a = 2; if(a) { print(a); print(b)} elif(c) {test1()} else {print(0)}";
    std::string code = R"CLM(a = array(3);
a[0] = 6;
a[2] = 9;
c = a[2];
print(a[0]);
if(a[0] == 6) {
    print("you stupid");
    if (c == a[2]){
        print(a[1]);
        }
}
elif (a[0] == 0) {
    print("lmao");
})CLM";
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
    machine.addFunction(MachineFunctions::createArrayOfSize, "array");
    machine.addFunction(test1, "test1");
    while (!machine.isAtTheEnd())
    {
        machine.step();
    }
    // std::cout << "Value of a = " << std::get<int32_t>(machine.getVariableValue("a").value) << std::endl;
    return EXIT_SUCCESS;
}