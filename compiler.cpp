#include <iostream>
#include "compiler/Parser.hpp"
#include "compiler/Compiler.hpp"
#include "execution/Machine.hpp"
int main()
{
    std::string code = "a = 5 + 99 - piss";
    std::cout << "Source: " << code << std::endl;
    SimpleLang::Compiler::Parser comp(code);
    comp.parse();
    comp.printInfoTable();
    comp.printCode();
    SimpleLang::Compiler::Compiler compiler(comp);
    compiler.compile();
    compiler.printCode();
    compiler.generateByteCode();
    std::cout << "Executing code" << std::endl;

    SimpleLang::Machine machine(compiler.getByteCode());
    machine.createVariable("piss", SimpleLang::MemoryValue{.type = SimpleLang::Type::Int, .value = 69});
    while (!machine.isAtTheEnd())
    {
        machine.step();
    }
    std::cout << "Value of a = " << std::get<int32_t>(machine.getVariableValue("a").value) << std::endl;
    return EXIT_SUCCESS;
}