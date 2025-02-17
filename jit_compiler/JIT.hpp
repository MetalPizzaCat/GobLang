#pragma once
#include "../compiler/ByteCode.hpp"
#include "../execution/Machine.hpp"
#include <cstddef>
#include <iostream>
#include <sys/mman.h>

namespace JIT {

class Virtual : public GobLang::Machine {
public:
    Virtual(const GobLang::Compiler::ByteCode& code)
        : GobLang::Machine(code)
    {
    }
    Virtual()
        : GobLang::Machine()
    {
    }

    void _callLocal()
    {
        using namespace GobLang;
        size_t funcId = (size_t)m_operations[m_programCounter + 1];
        m_callStack.push_back(m_programCounter + 1);
        m_programCounter = m_functions[funcId].start - 1;
        std::vector<MemoryValue> args = std::vector<MemoryValue>(m_functions[funcId].arguments.size());

        std::vector<MemoryValue>& variableFrame = m_variables.back();

        for (std::vector<MemoryValue>::reverse_iterator it = args.rbegin(); it != args.rend(); it++) {
            *it = _getFromTopAndPop();
            // for the entirety of the value being in the function we assume that it is in use so we can not delete it
            if (it->type == Type::MemoryObj) {
                std::get<MemoryNode*>(it->value)->increaseRefCount();
            }
        }
        m_variables.push_back(args);
        m_operationStack.push_back({});
        std::cout << "called back\n";
    }
};

class Native {

    void* mem_page = MAP_FAILED;
    size_t page_len = 0;

public:
    ~Native()
    {
        if (mem_page != MAP_FAILED) {
            munmap(mem_page, page_len);
        }
    }

    void execute();
    void compile();
};
}
