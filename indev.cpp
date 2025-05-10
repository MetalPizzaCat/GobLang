#include <iostream>
#include <algorithm>
#include <fstream>
#include "codegen/Parser.hpp"
#include "codegen/CodeGenerator.hpp"
#include "execution/Machine.hpp"
#include "execution/NativeStructure.hpp"

#include "standard/MachineFunctions.hpp"
#include "codegen/Disassembly.hpp"

// #define INDEV_DEBUG_TREE_ONLY
#define INDEV_DEBUG_RUN_FULL_CODE
#define INDEV_DEBUG_SHOW_TREE

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
        if (NativeNode *self = m->popObjectFromStack<NativeNode>(); self != nullptr)
        {
            int32_t p3 = m->popFromStack<int32_t>();
            int32_t p2 = m->popFromStack<int32_t>();
            int32_t p1 = m->popFromStack<int32_t>();

            self->doAThing(p1, p2, p3);
        }
        else
        {
            throw RuntimeException("Invalid parameter");
        }
    }

    static void constructor(GobLang::Machine *m)
    {
        using namespace GobLang;
        if (StringNode *str = m->popObjectFromStack<StringNode>(); str != nullptr)
        {
            NativeNode *native = new NativeNode(str->getString(), m->getNativeStructure(NativeNode::ClassName));
            m->addObject(native);
            m->pushObjectToStack(native);
        }
        else
        {
            throw RuntimeException("Invalid parameter");
        }
    }

    std::string toString(bool pretty = false, size_t depth = 0) override { return NativeNode::ClassName + "(" + m_path + ")"; }
    virtual ~NativeNode() {}

private:
    std::string m_path;
    void _secret() {}
};

static void testOrder(GobLang::Machine *m)
{
    int32_t p3 = m->popFromStack<int32_t>();
    int32_t p2 = m->popFromStack<int32_t>();
    int32_t p1 = m->popFromStack<int32_t>();

    std::cout << "Order is : " << p1 << ',' << p2 << ',' << p3 << std::endl;
}
int main()
{
    size_t s = sizeof(Value);
    size_t s1 = sizeof(uint32_t);
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

#ifdef INDEV_DEBUG_TREE_ONLY
    gen.generate();
    gen.printTree();
#else
    GobLang::Codegen::ByteCode bytes = gen.getByteCode();
#ifdef INDEV_DEBUG_SHOW_TREE
    gen.printTree();
#endif
    GobLang::Codegen::byteCodeToText(bytes.operations);
#ifdef INDEV_DEBUG_RUN_FULL_CODE
    GobLang::Machine machine(bytes);
    MachineFunctions::bind(&machine);
    machine.createType(NativeNode::ClassName, NativeNode::constructor, {{"do_a_thing", NativeNode::nativeDoAThing}});
    machine.addFunction(testOrder, "testOrder");
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
#endif
#endif
    // std::cout << "Value of a = " << std::get<int32_t>(machine.getVariableValue("a").value) << std::endl;
    return EXIT_SUCCESS;
}