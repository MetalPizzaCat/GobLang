#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include "Type.hpp"
#include "Structure.hpp"
namespace GobLang
{
    /**
     * @brief Represents a complex data object that is stored in the memory via a linked list.
     *
     * This is used for handling any type of non plain data like strings, arrays, structures, etc.
     *
     */
    class MemoryNode
    {
    public:
        explicit MemoryNode() = default;
        explicit MemoryNode(Struct::Structure const* baseStruct);
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

        /// @brief Whether this object has been added to garbage collector
        /// @return true if  this object is being tracked by the garbage collector
        bool isRegistered() const { return m_registered; }

        /// @brief Mark object as registered with garbage collector
        void registerGC() { m_registered = true; }

        /// @brief Set value of the field if that field exists
        /// @param field Name of the field
        /// @param value Value to assign
        virtual void setField(std::string const &field, Value const &value);

        /// @brief Get value of the field with a given name
        /// @param field Name of the field
        /// @return
        virtual Value getField(std::string const &field);

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
        virtual std::string toString(bool pretty = false, size_t depth = 0);

        virtual ~MemoryNode();

    private:
        /// @brief Next value in memory
        MemoryNode *m_next = nullptr;
        /// @brief Is marked for deletion by garbage collector?
        bool m_dead = false;

        /// @brief How many references to this object exist. Once it zero object should be deleted
        int32_t m_refCount = 0;

        /// @brief if true then this object is handled by garbage collector, otherwise it has not yet been added to the list
        bool m_registered = false;

        /// @brief Descriptor of the fields that the object has
        Struct::Structure const *m_struct = nullptr;

        /// @brief Storage for the values that an object might have
        std::vector<Value> m_fields;
        /// @brief Name to id mapping of the fields used for access
        std::map<std::string, size_t> m_fieldNames;
    };

}