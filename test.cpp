#include <iostream>
#include <cassert>

#include "compiler/Parser.hpp"
#include "compiler/Validator.hpp"

bool f1()
{
    std::cout << "f1" << std::endl;
    return false;
}

bool f2()
{
    std::cout << "f2" << std::endl;
    return false;
}
int main(int, char **)
{
    using namespace GobLang::Compiler;

    bool b = f1() || f2();
    // {let c = a + (3 - 0); let g = wawa; wawa = (w / 2);}
    Parser p("{let c = a + (3 - 0); let g = wawa; wawa = (w / 2);}");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.block(p.getTokens().begin(), endIt));

    return EXIT_SUCCESS;
}