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
    // check if object that we are setting is itself to avoid creating a ref cycle
    if (item.type == Type::MemoryObj && std::get<MemoryNode *>(item.value) != this)
    {
        std::get<MemoryNode *>(item.value)->increaseRefCount();
    }
    if (m_data[i].type == Type::MemoryObj && std::get<MemoryNode *>(m_data[i].value) != this)
    {
        std::get<MemoryNode *>(m_data[i].value)->decreaseRefCount();
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

GobLang::ArrayNode::~ArrayNode()
{
    for (std::vector<MemoryValue>::iterator it = m_data.begin(); it != m_data.end(); it++)
    {
        if (it->type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(it->value)->decreaseRefCount();
        }
    }
}
