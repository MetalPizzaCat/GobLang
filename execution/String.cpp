#include "String.hpp"


std::string GobLang::StringNode::toString(bool pretty, size_t depth)
{
    if (pretty)
    {
        return "\"" + getString() + "\"";
    }
    return getString();
}

char GobLang::StringNode::getCharAt(size_t ind)
{
    return m_str[ind];
}

void GobLang::StringNode::setCharAt(char ch, size_t ind)
{
    m_str[ind] = ch;
}

bool GobLang::StringNode::equalsTo(MemoryNode *other)
{
    if (StringNode *otherStr = dynamic_cast<StringNode *>(other); otherStr != nullptr)
    {
        return otherStr->getString() == getString();
    }
    return false;
}
