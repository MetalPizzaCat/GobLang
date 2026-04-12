#include "String.hpp"

std::string GobLang::StringObject::toString() const
{
    return "\"" + getString() + "\"";
}

char GobLang::StringObject::getCharAt(size_t ind)
{
    return m_str[ind];
}

void GobLang::StringObject::setCharAt(char ch, size_t ind)
{
    m_str[ind] = ch;
}

bool GobLang::StringObject::equalsTo(Object *other)
{
    if (StringObject *otherStr = dynamic_cast<StringObject *>(other); otherStr != nullptr)
    {
        return otherStr->getString() == getString();
    }
    return false;
}
