#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include "Type.hpp"
namespace GobLang
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
        bool isDead() const { return m_dead || m_refCount <= 0; }
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
         * @brief Erase current m_next by replacing it with the child of that object. Does not call any memory freeing functions
         *
         */
        void eraseNext();

        /**
         * @brief Insert a node at the end of the chain
         *
         * @param node Node to insert
         */
        void pushBack(MemoryNode *node);

        void increaseRefCount();

        void decreaseRefCount();

        int32_t getRefCount() const { return m_refCount; }

        /**
         * @brief Size of the memory chain
         *
         * @return size_t
         */
        size_t length();

        /**
         * @brief Check if this memory value is equal to other value. This should be overriden to have type specific to avoid java situation
         *
         * @param other
         * @return true
         * @return false
         */
        virtual bool equalsTo(MemoryNode *other);

        /**
         * @brief Convert given object into a string representation
         * 
         * @param pretty If true then this object should be printed with type decorations. Only is relevant for string types
         * @return std::string String representation
         */
        virtual std::string toString(bool pretty = false) { return "Memory object"; }

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

        int32_t m_refCount = 0;
    };

    class StringNode : public MemoryNode
    {
    public:
        explicit StringNode(std::string const &str) : m_str(str) {}

        std::string const &getString() { return m_str; }

        std::string toString(bool pretty) override;

        char getCharAt(size_t ind);

        void setCharAt(char ch, size_t ind);

        /**
         * @brief Compare other memory node and return true if both contain same sequence of characters
         *
         * @param other
         * @return true
         * @return false
         */
        bool equalsTo(MemoryNode *other) override;

        size_t getSize() const { return m_str.size(); }

        virtual ~StringNode() = default;

    private:
        std::string m_str;
    };

}