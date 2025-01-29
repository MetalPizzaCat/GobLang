#pragma once
#include <string>
#include <vector>
#include "Type.hpp"
namespace SimpleLang
{
    /**
     * @brief Class used to represent interpreter memory by using a linked list
     *
     */
    class MemoryNode
    {
    public:
        /**
         * @brief Should be deleted by the garbage collector or not
         *
         * @return true
         * @return false
         */
        bool isDead() const { return m_dead; }
        /**
         * @brief Get the next node in the list
         *
         * @return MemoryNode*
         */
        MemoryNode *getNext() { return m_next; }
        /**
         * @brief Insert a new memory node after this one. Current child node will become child of the inserted node
         *
         * @param node Node to insert
         */
        void insert(MemoryNode *node);

        /**
         * @brief Insert a node at the end of the chain
         *
         * @param node Node to insert
         */
        void push_back(MemoryNode *node);

        virtual ~MemoryNode() = default;

    private:
        /**
         * @brief Next value in memory
         *
         */
        MemoryNode *m_next = nullptr;
        /**
         * @brief Is marked for deletion by garbage collector?
         *
         */
        bool m_dead = false;
    };

    class StringNode : public MemoryNode
    {
    public:
        explicit StringNode(std::string const &str) : m_str(str) {}

        std::string const &getString() { return m_str; }

        virtual ~StringNode() = default;

    private:
        std::string m_str;
    };

    
}