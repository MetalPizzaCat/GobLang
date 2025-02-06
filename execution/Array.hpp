#pragma once
#include "Memory.hpp"

namespace GobLang
{
    struct MemoryValue;
    class ArrayNode : public MemoryNode
    {
    public:
        explicit ArrayNode(size_t size);

        void setItem(size_t i, MemoryValue const &item);
        MemoryValue *getItem(size_t i);

        std::string toString() override;

        size_t getSize() const { return m_data.size(); }

        virtual ~ArrayNode();

    private:
        std::vector<MemoryValue> m_data;
    };
} // namespace SimpleLang
