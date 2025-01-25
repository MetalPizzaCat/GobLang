#include <iostream>
#include "compiler/Compiler.hpp"

int main()
{
    SimpleLang::Compiler::Compiler comp("a + 22");
    comp.parse();
    comp.printInfoTable();
    std::cout << "This is the compiler executable" << std::endl;
    return EXIT_SUCCESS;
}