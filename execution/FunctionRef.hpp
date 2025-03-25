#pragma once
#include "Memory.hpp"
#include "Value.hpp"
namespace GobLang
{
    class FunctionRef : public MemoryNode
    {
    public:
        explicit FunctionRef(FunctionValue const *func, MemoryNode *owner = nullptr);
        explicit FunctionRef(size_t localFuncId);
        bool hasOwner() const { return m_owner != nullptr; }
        bool isLocal() const { return m_localFuncId != -1; }
        size_t getLocalFuncId() const { return m_localFuncId; }
        MemoryNode *getOwner() { return m_owner; }
        FunctionValue const *getFunction() const { return m_func; }

    private:
        /// @brief The object that owns this function. If null then this function is static
        MemoryNode *m_owner;
        /// @brief The actual function object. Either an int to reference id of a function or NativeFunction object
        FunctionValue const *m_func;

        size_t m_localFuncId = -1;
    };
}