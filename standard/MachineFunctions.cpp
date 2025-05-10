#include "MachineFunctions.hpp"
#include "../execution/Array.hpp"
#include "../execution/Memory.hpp"
#include "File.hpp"
#include <random>
void MachineFunctions::bind(GobLang::Machine *machine)
{
    machine->addFunction(MachineFunctions::getSizeof, "sizeof");
    machine->addFunction(MachineFunctions::printLine, "print_line");
    machine->addFunction(MachineFunctions::print, "print");
    machine->addFunction(MachineFunctions::toString, "str");
    machine->addFunction(MachineFunctions::createArrayOfSize, "array");
    machine->addFunction(MachineFunctions::append, "append");
    machine->addFunction(MachineFunctions::input, "input");
    machine->addFunction(MachineFunctions::Math::toInt, "int");
    machine->addFunction(MachineFunctions::Math::toFloat, "float");
    machine->addFunction(MachineFunctions::Math::randomIntInRange, "rand_range");
    machine->addFunction(MachineFunctions::Math::randomInt, "rand");

    machine->createType("File", File::FileNode::constructor, {{"close", File::FileNode::nativeCloseFile}, {"write", File::FileNode::nativeWriteToFile}, {"is_eof", File::FileNode::nativeIsFileEnded}, {"read_line", File::FileNode::nativeReadLineFromFile}, {"is_open", File::FileNode::nativeIsFileOpen}});
}
void MachineFunctions::printLine(GobLang::Machine *machine)

{
    GobLang::Value v = machine->getStackTopAndPop();
    std::cout << GobLang::valueToString(v, false, 0) << std::endl;
}

void MachineFunctions::print(GobLang::Machine *machine)
{
    std::cout << GobLang::valueToString(machine->getStackTopAndPop(), false, 0);
}

void MachineFunctions::createArrayOfSize(GobLang::Machine *machine)
{
    GobLang::Value sizeVal = machine->getStackTopAndPop();
    machine->pushToStack(GobLang::Value(machine->createArrayOfSize(std::get<int32_t>(sizeVal))));
}

void MachineFunctions::append(GobLang::Machine *machine)
{
    using namespace GobLang;
    GobLang::Value val = machine->getStackTopAndPop();
    if (ArrayNode *arrayNode = machine->popObjectFromStack<ArrayNode>(); arrayNode != nullptr)
    {
        arrayNode->append(val);
    }
    else
    {
        throw GobLang::RuntimeException("Attempted to append to a non array object");
    }
}

void MachineFunctions::getSizeof(GobLang::Machine *machine)
{
    GobLang::MemoryNode *array = machine->popFromStack<GobLang::MemoryNode *>();
    if (array == nullptr)
    {
        throw GobLang::RuntimeException("Attempted to get a size of a non array object");
    }
    if (GobLang::ArrayNode *arrayNode = dynamic_cast<GobLang::ArrayNode *>(array); arrayNode != nullptr)
    {
        machine->pushToStack(GobLang::Value((int32_t)arrayNode->getSize()));
    }
    else if (GobLang::StringNode *strNode = dynamic_cast<GobLang::StringNode *>(array); strNode != nullptr)
    {
        machine->pushToStack(GobLang::Value((int32_t)strNode->getSize()));
    }
}

void MachineFunctions::toString(GobLang::Machine *machine)
{
    GobLang::Value val = machine->getStackTopAndPop();
    machine->pushToStack(GobLang::Value(machine->createString(GobLang::valueToString(val, false, 0))));
}

void MachineFunctions::input(GobLang::Machine *machine)
{
    std::string input;
    getline(std::cin, input);
    machine->pushToStack(GobLang::Value(machine->createString(input)));
}

void MachineFunctions::inputChar(GobLang::Machine *machine)
{
    char ch;
    std::cin >> ch;
    machine->pushToStack(GobLang::Value(ch));
}

void MachineFunctions::Math::toInt(GobLang::Machine *machine)
{
    using namespace GobLang;
    Value value = machine->getStackTopAndPop();
    switch ((Type)value.index())
    {
    case Type::Int:
        machine->pushToStack(value);
        break;
    case GobLang::Type::Float:
        machine->pushToStack(Value((int32_t)std::get<float>(value)));
        break;
    case GobLang::Type::MemoryObj:
        try
        {
            if (StringNode *node = dynamic_cast<StringNode *>(std::get<MemoryNode *>(value)); node != nullptr)
            {
                machine->pushToStack(Value(std::stoi(node->getString())));
                break;
            }
        }
        catch (std::invalid_argument e)
        {
            throw RuntimeException(std::string("Unable to convert string to int. ") + e.what());
        }
        catch (std::out_of_range e)
        {
            throw RuntimeException(std::string("Unable to convert string to int. ") + e.what());
        }
    default:
        throw RuntimeException(std::string("Unable to convert type ") + typeToString((Type)value.index()) + " to int");
    }
}

void MachineFunctions::Math::toFloat(GobLang::Machine *machine)
{
    using namespace GobLang;
    Value value = machine->getStackTopAndPop();
    switch ((Type)value.index())
    {
    case Type::Int:
        machine->pushToStack(Value(std::get<int32_t>(value)));
        break;
    case GobLang::Type::Float:
        machine->pushToStack(value);
        break;
    case GobLang::Type::MemoryObj:
        try
        {
            if (StringNode *node = dynamic_cast<StringNode *>(std::get<MemoryNode *>(value)); node != nullptr)
            {
                machine->pushToStack(Value(std::stof(node->getString())));
                break;
            }
        }
        catch (std::invalid_argument e)
        {
            throw RuntimeException(std::string("Unable to convert string to float. ") + e.what());
        }
        catch (std::out_of_range e)
        {
            throw RuntimeException(std::string("Unable to convert string to float. ") + e.what());
        }
    default:
        throw RuntimeException(std::string("Unable to convert type ") + typeToString((Type)value.index()) + " to float");
    }
}

void MachineFunctions::Math::randomIntInRange(GobLang::Machine *machine)
{
    int32_t max = machine->popFromStack<int32_t>();
    int32_t min = machine->popFromStack<int32_t>();
    if (min >= max)
    {
        throw GobLang::RuntimeException(std::string("Invalid random range. Min: " + std::to_string(min) + " max: " + std::to_string(max)));
    }
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int32_t> distr(min, max);
    machine->pushToStack(GobLang::Value(distr(generator)));
}

void MachineFunctions::Math::randomInt(GobLang::Machine *machine)
{
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

    std::uniform_int_distribution<int32_t> distr(DEFAULT_MIN_RAND_INT, DEFAULT_MAX_RAND_INT);
    machine->pushToStack(GobLang::Value(distr(generator)));
}
