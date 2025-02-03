#include <iostream>
#include <cassert>

#include "compiler/Parser.hpp"
#include "compiler/Validator.hpp"

using namespace GobLang::Compiler;

void testBlock()
{
    Parser p("{let c = a + (3 - 0); let g = wawa; wawa = (w / 2);}");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.block(p.getTokens().begin(), endIt));
}

void testArray()
{
    Parser p("arr[0]");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.arrayAccess(p.getTokens().begin(), endIt));
}

void testArray2D()
{
    Parser p("arr[0][9]");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.arrayAccess(p.getTokens().begin(), endIt));
}

void testArray2()
{
    Parser p("arr[2 * (a - 3)]");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.arrayAccess(p.getTokens().begin(), endIt));
}

void testArrayAssign()
{
    Parser p("arr[2 * (a - 3)] = val + val;");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.arrayAssignment(p.getTokens().begin(), endIt));
}

void testArrayAssign2D()
{
    Parser p("arr[2 * (a - 3)][array[3][4]] = val + val[9];");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.arrayAssignment(p.getTokens().begin(), endIt));
}

void testArrayUse()
{
    Parser p("let b = arr[2 * (a - 3)];");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.localVarCreation(p.getTokens().begin(), endIt));
}

void testCall()
{
    Parser p("a()");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.call(p.getTokens().begin(), endIt));
}

void testCallArg()
{
    Parser p("a(2)");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.call(p.getTokens().begin(), endIt));
}

void testCallArgs()
{
    Parser p("a(2, 2+2, a[0], a(b))");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.call(p.getTokens().begin(), endIt));
}

void testBlockArray()
{
    Parser p("{let c = a + (3 - 0); let g = wawa[3]; wawa = (w / 2);}");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.block(p.getTokens().begin(), endIt));
}
int main(int, char **)
{
    // testArray();
    // testArray2();
    // testArray2D();
    // testArrayUse();
    // testArrayAssign();
    // testArrayAssign2D();
    // testBlock();
    // testCall();
    // testCallArg();
    // testCallArgs();
    // testBlockArray();

    Parser p("let a = 3;print(a);if");
    p.parse();
    p.printCode();
    Validator v(p);
    v.validate();

    return EXIT_SUCCESS;
}