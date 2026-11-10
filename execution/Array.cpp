#include "Array.hpp"
#include "Value.hpp"
#include "Exception.hpp"
#include <format>
GobLang::ArrayObject::ArrayObject(size_t size)
{
    m_data = std::vector<Value>(size);
}
void GobLang::ArrayObject::setItem(size_t i, Value const &item)
{
    if (i >= m_data.size())
    {
        throw RuntimeException(std::format("Attempted to read out of bounds of the array. i = {} in array of size {}", i, m_data.size()));
    }
    // check if object that we are setting is itself to avoid creating a ref cycle
    if ((Type)item.index() == Type::Array && std::get<ArrayObject *>(item) != this)
    {
        ValueOperations::increaseValueRefCount(item);
    }
    if ((Type)m_data[i].index() == Type::Array && std::get<ArrayObject *>(m_data[i]) != this)
    {
        ValueOperations::decreaseValueRefCount(m_data[i]);
    }
    m_data[i] = item;
}

std::optional<GobLang::Value> GobLang::ArrayObject::getItem(size_t i) const
{

    if (i < m_data.size())
    {
        return m_data.at(i);
    }
    else
    {
        throw RuntimeException(std::format("Attempted to read out of bounds of the array. i = {} in array of size {}", i, m_data.size()));
        return {};
    }
}

std::string GobLang::ArrayObject::toString() const
{
    std::string text = "[";
    for (size_t i = 0; i < m_data.size(); i++)
    {
        text += ValueOperations::toString(m_data[i]);
        if (i != m_data.size() - 1)
        {
            text += ",";
        }
    }
    return text + "]";
}

void GobLang::ArrayObject::append(Value const &item)
{
    // check if object that we are setting is itself to avoid creating a ref cycle
    if ((Type)item.index() == Type::Array && std::get<ArrayObject *>(item) != this)
    {
        ValueOperations::decreaseValueRefCount(item);
    }
    m_data.push_back(item);
}

GobLang::ArrayObject::~ArrayObject()
{
    for (std::vector<Value>::iterator it = m_data.begin(); it != m_data.end(); it++)
    {
        ValueOperations::decreaseValueRefCount(*it);
    }
}
