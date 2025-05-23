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

void testUnary()
{
    Parser p("!a");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.unaryExpr(p.getTokens().begin(), endIt));
}

void testFunction()
{
    Parser p("func a1 (){a = 2;}");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.function(p.getTokens().begin(), endIt));
}

void testFunctionArgs()
{
    Parser p("func b2(a,b,c,d){a = 2; return;}");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.function(p.getTokens().begin(), endIt));
}

void testFunctionArgs2()
{
    Parser p("func b2(a,b,c,d){a = 2; return a + b + c + d;}");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.function(p.getTokens().begin(), endIt));
}

void testArrayCreation()
{
    Parser p("[1,2,3]");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.arrayCreation(p.getTokens().begin(), endIt));
}

void testArrayCreationNest()
{
    Parser p("[1,[2],[3,3]]");
    p.parse();
    p.printCode();
    Validator v(p);
    Validator::TokenIterator endIt;
    assert(v.arrayCreation(p.getTokens().begin(), endIt));
}
int main(int, char **)
{
    testArray();
    testArray2();
    testArray2D();
    testArrayUse();
    testArrayAssign();
    testArrayAssign2D();
    testBlock();
    testCall();
    testCallArg();
    testCallArgs();
    testBlockArray();
    testUnary();
    testFunction();
    testFunctionArgs2();
    testArrayCreation();
    testArrayCreationNest();
    return EXIT_SUCCESS;
}