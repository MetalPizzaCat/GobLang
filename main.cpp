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
    // runParsing(code1);
    // runParsing(code2);

    std::vector<uint8_t> bytes;
    size_t addr = 0x5f7d1f48;
    for (int32_t i = sizeof(size_t) - 1; i >= 0; i--)
    {
        size_t offset = (sizeof(uint8_t) * i) * 8;
        const size_t mask = 0xff;
        size_t num = addr & (mask << offset);
        size_t numFixed = num >> offset;

        bytes.push_back(numFixed);
    }
    size_t reconAddr = 0x0;
    for (int32_t i = 0; i < sizeof(size_t); i++)
    {
        size_t offset = ((sizeof(size_t) - i - 1)) * 8;
        reconAddr |= (size_t)(bytes[i]) << offset;
    }
    std::cout << std::hex << reconAddr << std::endl;
    for (auto it = bytes.begin(); it != bytes.end(); it++)
    {
        std::cout << std::hex << (size_t)(*it) << std::endl;
    }
    return EXIT_SUCCESS;
}
