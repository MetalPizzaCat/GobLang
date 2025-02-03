#include <iostream>
#include <cassert>

#include "compiler/Parser.hpp"
#include "compiler/Validator.hpp"

int main(int, char **)
{
    using namespace GobLang::Compiler;
    Parser p("c = a + (a - 5) / g;");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.id(p.getTokens().begin()));
    assert(v.assignment(p.getTokens().begin(), endIt));

    return EXIT_SUCCESS;
}