#include <iostream>
#include <string>
#include <cassert>
#include <variant>
#include <vector>
#include <functional>
#include <map>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <fstream>

#include "compiler/Parser.hpp"
#include "compiler/Compiler.hpp"
#include "execution/Machine.hpp"

#include "execution/Machine.hpp"
#include "standard/MachineFunctions.hpp"

int main(int argc, char **argv)
{
    std::vector<std::string> VersionArgs = {"-v", "--version"};
    std::vector<std::string> HelpArgs = {"-h", "--help"};
    std::vector<std::string> FileArgs = {"-i", "--input"};
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }
    std::vector<std::string>::iterator verIt = std::find_first_of(args.begin(), args.end(), VersionArgs.begin(), VersionArgs.end());
    if (verIt != args.end())
    {
        std::cout << "GobLang v" << GOB_LANG_VERSION_MAJOR << "." << GOB_LANG_VERSION_MINOR << std::endl;
        return EXIT_SUCCESS;
    }
    verIt = std::find_first_of(args.begin(), args.end(), HelpArgs.begin(), HelpArgs.end());
    if (verIt != args.end())
    {
        std::cout << "GobLang v" << GOB_LANG_VERSION_MAJOR << "." << GOB_LANG_VERSION_MINOR << std::endl;
        std::cout << "Usage: goblang [-i file | -h | -v]" << std::endl;
        std::cout << "Options" << std::endl;
        std::cout << "-v | --version : Display version of the interpreter" << std::endl;
        std::cout << "-h | --help    : View help about the interpreter" << std::endl;
        std::cout << "-i | --input   : Run code from file in a given location" << std::endl;
        return EXIT_SUCCESS;
    }

    verIt = std::find_first_of(args.begin(), args.end(), FileArgs.begin(), FileArgs.end());
    if (verIt == args.end())
    {
        std::cerr << "No input file provided" << std::endl;
        return EXIT_FAILURE;
    }

    if (verIt + 1 == args.end())
    {
        std::cerr << "Missing file path after file flag" << std::endl;
        return EXIT_FAILURE;
    }
    std::string file = *(verIt + 1);
    std::vector<std::string> lines;
    std::ifstream codeFile(file);
    if (!codeFile.is_open())
    {
        std::cerr << "Unable to open code file" << std::endl;
        return EXIT_FAILURE;
    }
    std::string to;
    while (std::getline(codeFile, to, '\n'))
    {
        lines.push_back(to);
    }

    try
    {
        GobLang::Compiler::Parser comp(lines);
        comp.parse();
        GobLang::Compiler::Compiler compiler(comp);
        compiler.compile();
        compiler.generateByteCode();
        GobLang::Machine machine(compiler.getByteCode());
        machine.addFunction(MachineFunctions::getSizeof, "sizeof");
        machine.addFunction(MachineFunctions::printLine, "print");
        machine.addFunction(MachineFunctions::createArrayOfSize, "array");
        std::vector<size_t> debugPoints = {};
        while (!machine.isAtTheEnd())
        {
            if (std::find(debugPoints.begin(), debugPoints.end(), machine.getProgramCounter()) != debugPoints.end())
            {
                std::cout << "Debugging at " << std::hex << machine.getProgramCounter() << std::dec << std::endl;
                machine.printGlobalsInfo();
                machine.printVariablesInfo();
                machine.printStack();
            }
            machine.step();
        }
    }
    catch (GobLang::Compiler::ParsingError e)
    {

        for (int32_t i = std::max(0, (int32_t)e.getColumn() - 1); i < std::min(lines.size(), e.getColumn() + 2); i++)
        {
            std::cout << lines[e.getColumn()] << std::endl;
            if (i == e.getColumn())
            {
                std::cout << std::string(' ', std::max((int32_t)e.getRow() - 1, 0)) << "~~~" << std::endl;
                std::cout << e.what() << std::endl;
            }
        }
    }
    catch (GobLang::RuntimeException e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // runParsing(code2);
    return EXIT_SUCCESS;
}
