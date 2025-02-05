#include "Array.hpp"
#include "Value.hpp"
#include "Exception.hpp"
GobLang::ArrayNode::ArrayNode(size_t size)
{
    m_data = std::vector<MemoryValue>(size);
}
void GobLang::ArrayNode::setItem(size_t i, MemoryValue const &item)
{
    if (i >= m_data.size())
    {
        throw RuntimeException(
            std::string("Attempted to read out of bounds of the array. i = ") +
            std::to_string(i) +
            " in array of size " +
            std::to_string(m_data.size()));
    }
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
        throw RuntimeException(
            std::string("Attempted to read out of bounds of the array. i = ") +
            std::to_string(i) +
            " in array of size " +
            std::to_string(m_data.size()));
        return nullptr;
    }
}

std::string GobLang::ArrayNode::toString()
{
    std::string text = "[";
    for (size_t i = 0; i < m_data.size(); i++)
    {
        text += valueToString(m_data[i]);
        if (i != m_data.size() - 1)
        {
            text += ",";
        }
    }
    return text + "]";
}
