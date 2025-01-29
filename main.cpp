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


int main(int, char **)
{
    std::string code = "arr[2+2];";
    std::cout << "Source: " << code << std::endl;
    SimpleLang::Compiler::Parser comp(code);
    comp.parse();
    comp.printInfoTable();
    comp.printCode();

    SimpleLang::Compiler::Compiler compiler(comp);
    compiler.compile();
    compiler.printCode();
    
    return EXIT_SUCCESS;
}
