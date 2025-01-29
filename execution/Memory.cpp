#include "Memory.hpp"
void SimpleLang::MemoryNode::insert(MemoryNode *node)
{
    if (node != nullptr)
    {
        MemoryNode *prevNext = m_next;
        m_next = node;
        node->m_next = prevNext;
    }
}

void SimpleLang::MemoryNode::push_back(MemoryNode *node)
{
    MemoryNode *curr = this;
    while (curr->m_next != nullptr)
    {
        curr = curr->m_next;
    }
    curr->m_next = node;
}


