#include "Array.hpp"
#include "Value.hpp"
#include "Exception.hpp"
GobLang::ArrayNode::ArrayNode(size_t size)
{
    m_data = std::vector<Value>(size);
}
void GobLang::ArrayNode::setItem(size_t i, Value const &item)
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
    if ((Type)item.index() == Type::MemoryObj && std::get<MemoryNode *>(item) != this)
    {
        std::get<MemoryNode *>(item)->increaseRefCount();
    }
    if ((Type)m_data[i].index() == Type::MemoryObj && std::get<MemoryNode *>(m_data[i]) != this)
    {
        std::get<MemoryNode *>(m_data[i])->decreaseRefCount();
    }
    m_data[i] = item;
}

GobLang::Value *GobLang::ArrayNode::getItem(size_t i)
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

std::string GobLang::ArrayNode::toString(bool pretty, size_t depth)
{
    std::string text = "[";
    for (size_t i = 0; i < m_data.size(); i++)
    {
        text += valueToString(m_data[i], pretty, depth);
        if (i != m_data.size() - 1)
        {
            text += ",";
        }
    }
    return text + "]";
}

void GobLang::ArrayNode::append(Value const &item)
{
    // check if object that we are setting is itself to avoid creating a ref cycle
    if ((Type)item.index() == Type::MemoryObj && std::get<MemoryNode *>(item) != this)
    {
        std::get<MemoryNode *>(item)->increaseRefCount();
    }
    m_data.push_back(item);
}

GobLang::ArrayNode::~ArrayNode()
{
    for (std::vector<Value>::iterator it = m_data.begin(); it != m_data.end(); it++)
    {
        if ((Type)it->index() == Type::MemoryObj)
        {
            std::get<MemoryNode *>((*it))->decreaseRefCount();
        }
    }
}
