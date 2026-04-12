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
     * @brief Represents a complex data object that doesn't fit on stack and shouldn't be constantly recreated
     *
     * This is used for handling any type of non plain data like strings, arrays, structures, etc.
     *
     */
    class Object
    {
    public:
        explicit Object() = default;
        /**
         * @brief Should be deleted by the garbage collector or not
         *
         * @return true
         * @return false
         */
        bool isDead() const { return m_dead || m_refCount <= 0; }

        void increaseRefCount();

        void decreaseRefCount();

        int32_t getRefCount() const { return m_refCount; }

        /// @brief Whether this object has been added to garbage collector
        /// @return true if  this object is being tracked by the garbage collector
        bool isRegistered() const { return m_registered; }

        /// @brief Mark object as registered with garbage collector
        void registerGC() { m_registered = true; }

        /**
         * @brief Check if this memory value is equal to other value. This should be overriden to have type specific to avoid java situation
         *
         * @param other
         * @return true
         * @return false
         */
        virtual bool equalsTo(Object *other);

        /**
         * @brief Convert given object into a string representation
         *
         * @param pretty If true then this object should be printed with type decorations. Only is relevant for string types
         * @return std::string String representation
         */
        virtual std::string toString() const;

        virtual ~Object() = default;

    private:
        /// @brief Is marked for deletion by garbage collector?
        bool m_dead = false;

        /// @brief How many references to this object exist. Once it zero object should be deleted
        int32_t m_refCount = 0;

        /// @brief if true then this object is handled by garbage collector, otherwise it has not yet been added to the list
        bool m_registered = false;
    };

}