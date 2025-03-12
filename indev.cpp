#include <iostream>
#include <algorithm>
#include <fstream>
#include "compiler/Parser.hpp"
#include "compiler/ReversePolishGenerator.hpp"
#include "compiler/Compiler.hpp"
#include "execution/Machine.hpp"
#include "compiler/Validator.hpp"

#include "standard/MachineFunctions.hpp"
#include "compiler/Disassembly.hpp"
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

    // GobLang::Compiler::Parser comp("func rec(text, count){if (count < 10) {print(count); print(\": \");print_line(text); rec(text, count + 1);}} rec(\"hello\", 0);");
    GobLang::Compiler::Parser comp(lines);
    comp.parse();
    comp.printCode();
    GobLang::Compiler::Validator validator(comp);
    //validator.validate();
    GobLang::Compiler::ReversePolishGenerator rev(comp);
    rev.compile();
    rev.printCode();
    rev.printFunctions();
    rev.printStructs();
    GobLang::Compiler::Compiler compiler(rev);
    compiler.generateByteCode();
    // compiler.printLocalFunctionInfo();
    GobLang::Compiler::byteCodeToText(compiler.getByteCode().operations);

    GobLang::Machine machine(compiler.getByteCode());
    MachineFunctions::bind(&machine);
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