#pragma once
#include "Memory.hpp"

namespace GobLang
{
    class ArrayNode : public MemoryNode
    {
    public:
        explicit ArrayNode(size_t size);

        void setItem(size_t i, Value const &item);
        Value *getItem(size_t i);

        std::string toString(bool pretty, size_t depth) override;

        size_t getSize() const { return m_data.size(); }

        void append(Value const& item);

        virtual ~ArrayNode();

    private:
        std::vector<Value> m_data;
    };
} // namespace SimpleLang
