#pragma once
#include "Memory.hpp"
#include "Value.hpp"
namespace GobLang
{
    class FunctionRef : public MemoryNode
    {
    public:
        explicit FunctionRef(FunctionValue const *func, MemoryNode *owner = nullptr);
        bool hasOwner() const { return m_owner != nullptr; }
        MemoryNode *getOwner() { return m_owner; }
        FunctionValue const *getFunction() const { return m_func; }

    private:
        /// @brief The object that owns this function. If null then this function is static
        MemoryNode *m_owner;
        /// @brief The actual function object. Either an int to reference id of a function or NativeFunction object
        FunctionValue const *m_func;
    };
}