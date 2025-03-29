#include "Memory.hpp"
void GobLang::MemoryNode::insert(MemoryNode *node)
{
    if (node != nullptr)
    {
        MemoryNode *prevNext = m_next;
        m_next = node;
        node->m_next = prevNext;
    }
}

void GobLang::MemoryNode::eraseNext()
{
    if (m_next != nullptr)
    {
        m_next = m_next->m_next;
    }
}

void GobLang::MemoryNode::pushBack(MemoryNode *node)
{
    if (node == nullptr)
    {
        // don't pollute the memory
        return;
    }
    MemoryNode *curr = this;
    while (curr->m_next != nullptr)
    {
        curr = curr->m_next;
    }
    curr->m_next = node;
}

void GobLang::MemoryNode::increaseRefCount()
{
    m_refCount++;
}

void GobLang::MemoryNode::decreaseRefCount()
{
    m_refCount--;
}

size_t GobLang::MemoryNode::length()
{
    MemoryNode *curr = this->m_next;
    size_t size = 1;
    for (; curr != nullptr; curr = curr->m_next)
    {
        size++;
    }
    return size;
}

bool GobLang::MemoryNode::equalsTo(MemoryNode *other)
{
    return other == this;
}

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
