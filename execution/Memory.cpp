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

void GobLang::MemoryNode::push_back(MemoryNode *node)
{
    MemoryNode *curr = this;
    while (curr->m_next != nullptr)
    {
        curr = curr->m_next;
    }
    curr->m_next = node;
}

bool GobLang::MemoryNode::equalsTo(MemoryNode *other)
{
    return other == this;
}

bool GobLang::StringNode::equalsTo(MemoryNode *other)
{
    if(StringNode* otherStr = dynamic_cast<StringNode*>(other); otherStr != nullptr)
    {
        return otherStr->getString() == getString();
    }
    return false;
}
