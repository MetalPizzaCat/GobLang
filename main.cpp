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

#include "codegen/Parser.hpp"
#include "codegen/CodeGenerator.hpp"
#include "codegen/ByteCode.hpp"
#include "execution/Machine.hpp"

#include "standard/MachineFunctions.hpp"

#include "codegen/Disassembly.hpp"

int main(int argc, char **argv)
{
    std::vector<std::string> VersionArgs = {"-v", "--version"};
    std::vector<std::string> HelpArgs = {"-h", "--help"};
    std::vector<std::string> FileArgs = {"-i", "--input"};
    std::vector<std::string> DecompArgs = {"-s", "--showbytes"};
    std::vector<std::string> RecursionArgs = {"-p", "--printdepth"};
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }
    std::vector<std::string>::iterator verIt = std::find_first_of(args.begin(), args.end(), VersionArgs.begin(), VersionArgs.end());
    if (verIt != args.end())
    {
        std::cout << "GobLang v" << GOB_LANG_VERSION_MAJOR << "." << GOB_LANG_VERSION_MINOR << "." << GOB_LANG_VERSION_PATCH << std::endl;
        return EXIT_SUCCESS;
    }

    verIt = std::find_first_of(args.begin(), args.end(), HelpArgs.begin(), HelpArgs.end());
    if (verIt != args.end())
    {
        std::cout << "GobLang v" << GOB_LANG_VERSION_MAJOR << "." << GOB_LANG_VERSION_MINOR << std::endl;
        std::cout << "Usage: goblang [-i file | -h | -v]" << std::endl;
        std::cout << "Options" << std::endl;
        std::cout << "-v | --version    : Display version of the interpreter" << std::endl;
        std::cout << "-h | --help       : View help about the interpreter" << std::endl;
        std::cout << "-i | --input      : Run code from file in a given location" << std::endl;
        std::cout << "-s | --showbytes  : Show bytecode before running code" << std::endl;
        std::cout << "-p | --printdepth : Set the max print recursion depth" << std::endl;
        return EXIT_SUCCESS;
    }

    verIt = std::find_first_of(args.begin(), args.end(), RecursionArgs.begin(), RecursionArgs.end());
    if (verIt != args.end())
    {
        if (verIt + 1 == args.end())
        {
            std::cerr << "Missing input stack size" << std::endl;
            return EXIT_FAILURE;
        }
        try
        {
            GobLang::MAX_PRINT_RECURSION_DEPTH = std::stoul(*(verIt + 1));
        }
        catch (std::invalid_argument const &e)
        {
            std::cerr << "Invalid stack recursion number" << std::endl;
            return EXIT_FAILURE;
        }
        catch (std::out_of_range const &e)
        {
            std::cerr << "Invalid stack recursion number" << std::endl;
            return EXIT_FAILURE;
        }
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
        GobLang::Codegen::Parser comp(lines);
        comp.parse();
        GobLang::Codegen::CodeGenerator gen(comp);
        GobLang::Codegen::ByteCode byteCode = gen.getByteCode();

        verIt = std::find_first_of(args.begin(), args.end(), DecompArgs.begin(), DecompArgs.end());
        if (verIt != args.end())
        {
            GobLang::Codegen::byteCodeToText(byteCode.operations);
        }
        GobLang::Machine machine(byteCode);
        MachineFunctions::bind(&machine);
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
    catch (GobLang::Codegen::ParsingError e)
    {
        size_t maxLine = std::min(lines.size(), e.getRow() + LINES_AFTER_ERROR);
        size_t spaces = std::to_string(maxLine).size() + 2;
        for (int32_t i = std::max(0, (int32_t)e.getRow() - LINES_BEFORE_ERROR); i < maxLine; i++)
        {
            std::string lineId = std::to_string(i + 1);
            size_t spaceCount = 0;
            if (lineId.size() < maxLine)
            {
                spaceCount = maxLine - lineId.size();
            }
            std::cout << (lineId) << ": " << lines[i] << std::endl;
            if (i == e.getRow())
            {
                std::cout << std::string(std::max((int32_t)e.getColumn() - 1, 0) + spaces, ' ') << "~~~" << std::endl;
            }
        }
        std::cout << e.what() << std::endl;
    }
    catch (GobLang::RuntimeException e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // runParsing(code2);
    return EXIT_SUCCESS;
}
