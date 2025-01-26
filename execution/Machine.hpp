#pragma once
#include <map>
#include <vector>
#include <cstdint>
#include <string>
#include <cassert>

#include "Type.hpp"
#include "Memory.hpp"
#include "../compiler/ByteCode.hpp"

namespace SimpleLang
{
    class Machine
    {
    public:
        explicit Machine()
        {
        }

        explicit Machine(Compiler::ByteCode const &code);

        void addOperation(Operation op)
        {
            m_operations.push_back((uint8_t)op);
        }

        void addUInt8(uint8_t val)
        {
            m_operations.push_back(val);
        }

        void addStringConst(std::string const &str)
        {
            m_constStrings.push_back(str);
        }

        void addIntConst(int32_t val)
        {
            m_constInts.push_back(val);
        }

        size_t getProgramCounter() const
        {
            return m_programCounter;
        }

        bool isAtTheEnd() const
        {
            return m_programCounter >= m_operations.size();
        }
        void addFunction(FunctionValue const &func, std::string const &name);
        void step();

        MemoryValue *getStackTop();

        MemoryValue getVariableValue(std::string const &name) { return m_variables[name]; }

        /**
         * @brief Create a custom variable that will be accessible in code. Useful for binding with c code
         *
         * @param name Name of the variable
         * @param value Value of the variable
         */
        void createVariable(std::string const &name, MemoryValue const &value);

        ~Machine()
        {
            delete m_memoryRoot;
        }

    private:
        inline void addInt()
        {
            MemoryValue a = m_operationStack[m_operationStack.size() - 1];
            MemoryValue b = m_operationStack[m_operationStack.size() - 2];
            m_operationStack.pop_back();
            m_operationStack.pop_back();
            Value c = std::get<int32_t>(a.value) + std::get<int32_t>(b.value);
            m_operationStack.push_back(MemoryValue{.type = Type::Int, .value = c});
        }

        inline void subInt()
        {
            MemoryValue a = m_operationStack[m_operationStack.size() - 1];
            MemoryValue b = m_operationStack[m_operationStack.size() - 2];
            m_operationStack.pop_back();
            m_operationStack.pop_back();
            Value c = std::get<int32_t>(b.value) - std::get<int32_t>(a.value);
            m_operationStack.push_back(MemoryValue{.type = Type::Int, .value = c});
        }

        inline void set()
        {
            // (name val =)
            MemoryValue val = m_operationStack[m_operationStack.size() - 1];
            MemoryValue name = m_operationStack[m_operationStack.size() - 2];
            StringNode *memStr = dynamic_cast<StringNode *>(std::get<MemoryNode *>(name.value));
            if (memStr != nullptr)
            {
                m_variables[memStr->getString()] = val;
            }
        }

        inline void get()
        {
            MemoryValue name = m_operationStack[m_operationStack.size() - 1];
            m_operationStack.pop_back();
            assert(std::holds_alternative<MemoryNode *>(name.value));
            StringNode *memStr = dynamic_cast<StringNode *>(std::get<MemoryNode *>(name.value));
            if (memStr != nullptr)
            {
                m_operationStack.push_back(m_variables[memStr->getString()]);
            }
        }

        inline void call()
        {
            MemoryValue func = m_operationStack[m_operationStack.size() - 1];
            m_operationStack.pop_back();
            if (std::holds_alternative<FunctionValue>(func.value))
            {
                std::get<FunctionValue>(func.value)(this);
            }
        }

        inline void pushConstInt()
        {
            m_operationStack.push_back(MemoryValue{.type = Type::Int, .value = m_constInts[(size_t)m_operations[m_programCounter + 1]]});
            m_programCounter++;
        }

        inline void pushConstString()
        {
            MemoryNode *root = m_memoryRoot;
            std::string &str = m_constStrings[(size_t)m_operations[m_programCounter + 1]];
            StringNode *node = nullptr;
            // avoid making instance for each call, check if there is anything that uses this already
            while (root != nullptr)
            {
                if (StringNode *strNode = dynamic_cast<StringNode *>(root); strNode != nullptr && strNode->getString() == str)
                {
                    node = strNode;
                    break;
                }
                root = root->getNext();
            }
            if (node == nullptr)
            {
                node = new StringNode(str);
                m_memoryRoot->push_back(node);
            }
            m_programCounter++;
            m_operationStack.push_back(MemoryValue{.type = Type::MemoryObj, .value = node});
        }
        MemoryNode *m_memoryRoot = new MemoryNode();
        size_t m_programCounter = 0;
        std::vector<uint8_t> m_operations;
        std::vector<MemoryValue> m_operationStack;
        std::map<std::string, MemoryValue> m_variables;
        std::vector<int32_t> m_constInts;
        std::vector<float> m_constFloats;
        std::vector<std::string> m_constStrings;
    };
}