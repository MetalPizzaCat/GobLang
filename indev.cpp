#include <iostream>
#include <algorithm>
#include <fstream>
#include "codegen/Parser.hpp"
#include "codegen/CodeGenerator.hpp"
#include "execution/Machine.hpp"
#include "execution/NativeStructure.hpp"

#include "standard/MachineFunctions.hpp"
#include "codegen/Disassembly.hpp"

using namespace GobLang;
class NativeNode : public GobLang::Struct::NativeStructureObjectNode
{
public:
    inline static const std::string ClassName = "NativeObject";
    explicit NativeNode(std::string const &path, NativeStructureInfo const *info) : NativeStructureObjectNode(info), m_path(path) {}

    void doAThing(int32_t a, int32_t b, int32_t c)
    {
        std::cout << "Bazinga: " << this << " -> " << a << ',' << b << ',' << c << ',' << std::endl;
    }

    static void nativeDoAThing(GobLang::Machine *m)
    {
        std::unique_ptr<MemoryValue> selfObj = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
        std::unique_ptr<MemoryValue> p3 = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
        std::unique_ptr<MemoryValue> p2 = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
        std::unique_ptr<MemoryValue> p1 = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());

        if (selfObj->type == Type::MemoryObj)
        {
            if (NativeNode *self = dynamic_cast<NativeNode *>(std::get<MemoryNode *>(selfObj->value)); self != nullptr)
            {
                self->doAThing(std::get<int32_t>(p1->value), std::get<int32_t>(p2->value), std::get<int32_t>(p3->value));
                return;
            }
        }
        throw RuntimeException("Invalid parameter");
    }

    static void constructor(GobLang::Machine *m)
    {
        using namespace GobLang;
        std::unique_ptr<MemoryValue> strParam = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());

        if (strParam->type == Type::MemoryObj)
        {
            if (StringNode *str = dynamic_cast<StringNode *>(std::get<MemoryNode *>(strParam->value)); str != nullptr)
            {
                NativeNode *native = new NativeNode(str->getString(), m->getNativeStructure(NativeNode::ClassName));
                m->addObject(native);
                m->pushObjectToStack(native);
                return;
            }
        }
        throw RuntimeException("Invalid parameter");
    }

    std::string toString(bool pretty = false, size_t depth = 0) override { return NativeNode::ClassName + "(" + m_path + ")"; }
    virtual ~NativeNode() {}

private:
    std::string m_path;
    void _secret() {}
};

int main()
{
    size_t s = sizeof(std::variant<bool, char, float, int32_t, uint32_t, MemoryNode *, FunctionValue>);
    size_t s1 = sizeof( uint32_t);
    std::string file = "./code.gob";
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

    // GobLang::Compiler::Parser comp("func rec(text, count){if (count < 10) {print(count); print(\": \");print_line(text); rec(text, count + 1);}} rec(\"hello\", 0);");
    GobLang::Codegen::Parser comp(lines);
    comp.parse();
    comp.printCode();
    GobLang::Codegen::CodeGenerator gen(comp);
    GobLang::Codegen::ByteCode bytes =  gen.getByteCode();
    GobLang::Codegen::byteCodeToText(bytes.operations);

    GobLang::Machine machine(bytes);
    MachineFunctions::bind(&machine);
    // machine.createType("NativeObject", NativeNode::constructor, {{"do_a_thing", NativeNode::nativeDoAThing}});
    std::vector<size_t> debugPoints = {};
    while (!machine.isAtTheEnd())
    {
        if (std::find(debugPoints.begin(), debugPoints.end(), machine.getProgramCounter()) != debugPoints.end())
        {
            std::cout << "Debugging at " << std::hex << machine.getProgramCounter() << std::dec << ". Memory state: " << std::endl;
            machine.printGlobalsInfo();
            machine.printVariablesInfo();
            machine.printStack();
        }
        machine.step();
    }
    // std::cout << "Value of a = " << std::get<int32_t>(machine.getVariableValue("a").value) << std::endl;
    return EXIT_SUCCESS;
}