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
    SimpleLang::Compiler::Parser comp(code);
    comp.parse();
    comp.printInfoTable();
    comp.printCode();

    SimpleLang::Compiler::Compiler compiler(comp);
    compiler.compile();
    compiler.printCode();
}
int main(int, char **)
{
    std::string code1 = "if(a){print();} if(a){print();} else {if(b){print(40);}}";
    std::string code2 = "if(a){print();} else {print();} if(a){print();} elif(b){print(40);} elif(b){print(40);} else {print();}";
    runParsing(code1);
    runParsing(code2);
    return EXIT_SUCCESS;
}
