#include "Array.hpp"

#include "Value.hpp"
SimpleLang::ArrayNode::ArrayNode(size_t size)
{
    m_data = std::vector<MemoryValue>(size);
}
void SimpleLang::ArrayNode::setItem(size_t i, MemoryValue const &item)
{
    m_data[i] = item;
}

SimpleLang::MemoryValue *SimpleLang::ArrayNode::getItem(size_t i)
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