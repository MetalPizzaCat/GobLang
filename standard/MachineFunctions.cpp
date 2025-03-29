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

    machine->createType("File", File::FileNode::constructor, {
        {"close", File::FileNode::nativeCloseFile},
        {"write", File::FileNode::nativeWriteToFile},
        {"is_eof", File::FileNode::nativeIsFileEnded},
        {"read_line", File::FileNode::nativeReadLineFromFile},
        {"is_open", File::FileNode::nativeIsFileOpen}
    });
}
void MachineFunctions::printLine(GobLang::Machine *machine)

{
    GobLang::MemoryValue *v = machine->getStackTopAndPop();
    if (v == nullptr)
    {
        std::cerr << "Invalid value to print" << std::endl;
        return;
    }
    std::cout << GobLang::valueToString(*v, false, 0) << std::endl;
    delete v;
}

void MachineFunctions::print(GobLang::Machine *machine)
{
    GobLang::MemoryValue *v = machine->getStackTopAndPop();
    if (v == nullptr)
    {
        std::cerr << "Invalid value to print" << std::endl;
        return;
    }
    std::cout << GobLang::valueToString(*v, false, 0);
    delete v;
}

void MachineFunctions::createArrayOfSize(GobLang::Machine *machine)
{
    GobLang::MemoryValue *sizeVal = machine->getStackTopAndPop();
    machine->pushToStack(GobLang::MemoryValue{
        .type = GobLang::Type::MemoryObj,
        .value = machine->createArrayOfSize(std::get<int32_t>(sizeVal->value))});
    delete sizeVal;
}

void MachineFunctions::append(GobLang::Machine *machine)
{
    using namespace GobLang;
    GobLang::MemoryValue *val = machine->getStackTopAndPop();
    GobLang::MemoryValue *array = machine->getStackTopAndPop();

    if (array == nullptr || array->type != Type::MemoryObj)
    {
        throw GobLang::RuntimeException("Attempted to append to a non array object");
    }
    if (val == nullptr)
    {
        throw GobLang::RuntimeException("Missing value for the append operation");
    }
    if (GobLang::ArrayNode *arrayNode = dynamic_cast<GobLang::ArrayNode *>(std::get<GobLang::MemoryNode *>(array->value)); arrayNode != nullptr)
    {
        arrayNode->append(*val);
    }
    delete val;
    delete array;
}

void MachineFunctions::getSizeof(GobLang::Machine *machine)
{
    GobLang::MemoryValue *array = machine->getStackTopAndPop();
    if (array->type != GobLang::Type::MemoryObj)
    {
        throw GobLang::RuntimeException("Attempted to get a size of a non array object");
    }
    if (GobLang::ArrayNode *arrayNode = dynamic_cast<GobLang::ArrayNode *>(std::get<GobLang::MemoryNode *>(array->value)); arrayNode != nullptr)
    {
        machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::Int, .value = (int32_t)arrayNode->getSize()});
    }
    else if (GobLang::StringNode *strNode = dynamic_cast<GobLang::StringNode *>(std::get<GobLang::MemoryNode *>(array->value)); strNode != nullptr)
    {
        machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::Int, .value = (int32_t)strNode->getSize()});
    }
    delete array;
}

void MachineFunctions::toString(GobLang::Machine *machine)
{
    GobLang::MemoryValue *val = machine->getStackTopAndPop();
    machine->pushToStack(GobLang::MemoryValue{
        .type = GobLang::Type::MemoryObj,
        .value = machine->createString(GobLang::valueToString(*val, false, 0))});
    delete val;
}

void MachineFunctions::input(GobLang::Machine *machine)
{
    std::string input;
    getline(std::cin, input);
    machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::MemoryObj, .value = machine->createString(input)});
}

void MachineFunctions::inputChar(GobLang::Machine *machine)
{
    char ch;
    std::cin >> ch;
    machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::Char, .value = ch});
}

void MachineFunctions::Math::toInt(GobLang::Machine *machine)
{
    using namespace GobLang;
    MemoryValue *value = machine->getStackTopAndPop();
    switch (value->type)
    {
    case Type::Int:
        machine->pushToStack(*value);
        break;
    case GobLang::Type::Float:
        machine->pushToStack(MemoryValue{.type = Type::Int, .value = (int32_t)std::get<float>(value->value)});
        break;
    case GobLang::Type::MemoryObj:
        try
        {
            if (StringNode *node = dynamic_cast<StringNode *>(std::get<MemoryNode *>(value->value)); node != nullptr)
            {
                machine->pushToStack(MemoryValue{.type = Type::Int, .value = std::stoi(node->getString())});
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
        throw RuntimeException(std::string("Unable to convert type ") + typeToString(value->type) + " to int");
    }
    delete value;
}

void MachineFunctions::Math::toFloat(GobLang::Machine *machine)
{
    using namespace GobLang;
    MemoryValue *value = machine->getStackTopAndPop();
    switch (value->type)
    {
    case Type::Int:
        machine->pushToStack(MemoryValue{.type = Type::Float, .value = (float)std::get<float>(value->value)});
        break;
    case GobLang::Type::Float:
        machine->pushToStack(*value);
        break;
    case GobLang::Type::MemoryObj:
        try
        {
            if (StringNode *node = dynamic_cast<StringNode *>(std::get<MemoryNode *>(value->value)); node != nullptr)
            {
                machine->pushToStack(MemoryValue{.type = Type::Float, .value = std::stof(node->getString())});
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
        throw RuntimeException(std::string("Unable to convert type ") + typeToString(value->type) + " to float");
    }
    delete value;
}

void MachineFunctions::Math::randomIntInRange(GobLang::Machine *machine)
{
    GobLang::MemoryValue *max = machine->getStackTopAndPop();
    GobLang::MemoryValue *min = machine->getStackTopAndPop();
    if (min->type != GobLang::Type::Int || max->type != GobLang::Type::Int)
    {
        throw GobLang::RuntimeException("Random value range values are not type int");
    }
    int32_t minVal = std::get<int32_t>(min->value);
    int32_t maxVal = std::get<int32_t>(max->value);
    if (minVal >= maxVal)
    {
        throw GobLang::RuntimeException(std::string("Invalid random range. Min: " + std::to_string(minVal) + " max: " + std::to_string(maxVal)));
    }
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int32_t> distr(std::get<int32_t>(min->value), std::get<int32_t>(max->value));
    machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::Int, .value = distr(generator)});

    delete min;
    delete max;
}

void MachineFunctions::Math::randomInt(GobLang::Machine *machine)
{
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

    std::uniform_int_distribution<int32_t> distr(DEFAULT_MIN_RAND_INT, DEFAULT_MAX_RAND_INT);
    machine->pushToStack(GobLang::MemoryValue{.type = GobLang::Type::Int, .value = distr(generator)});
}
