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
    std::string code1 = "let a = 90; let b = 90; let c = a + b; if(a == b){ let d = c;} else {let h = a;} i = a";
    std::string code2 = "a = (2 + 2) * 3";
    runParsing(code2);
    // runParsing(code2);
    return EXIT_SUCCESS;
}
