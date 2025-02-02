#include <iostream>
#include <string>
#include <cassert>
#include <variant>
#include <vector>
#include <functional>
#include <map>
#include <cstdint>

#include "compiler/Parser.hpp"
#include "compiler/Compiler.hpp"
#include "execution/Machine.hpp"

#include "execution/Machine.hpp"
#include "standard/MachineFunctions.hpp"

void runParsing(std::string const &code)
{
    std::cout << "Source: " << code << std::endl;
    GobLang::Compiler::Parser comp(code);
    comp.parse();
    comp.printInfoTable();
    comp.printCode();

    GobLang::Compiler::Compiler compiler(comp);
    compiler.compile();
    compiler.printCode();
}
int main(int, char **)
{
    std::string code1 = "let a = 'j'";
    std::string code2 = "res = a && (b < 2 or b > 5)";
    runParsing(code1);
    //runParsing(code2);
    return EXIT_SUCCESS;
}
