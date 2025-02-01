#include "Array.hpp"

#include "Value.hpp"
GobLang::ArrayNode::ArrayNode(size_t size)
{
    m_data = std::vector<MemoryValue>(size);
}
void GobLang::ArrayNode::setItem(size_t i, MemoryValue const &item)
{
    m_data[i] = item;
}

GobLang::MemoryValue *GobLang::ArrayNode::getItem(size_t i)
{
    if (i < m_data.size())
    {
        return &m_data[i];
    }
    else
    {
        return nullptr;
    }
}