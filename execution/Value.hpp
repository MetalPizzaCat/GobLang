#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <variant>
#include <string>
#include "Type.hpp"
#include "../TypeSizes.hpp"

namespace GobLang
{
    /**
     * @brief Type used to store jump addresses in the code
     *
     */

    class State;
    class Object;
    using FunctionValue = void (*)(State &);
    class Closure;
    class GobFunction;
    using NilType = std::monostate;
    static const NilType NilValue = NilType();
    using Value = std::variant<NilType, bool, char, NumberType, IntegerType, UIntegerType, Object *, Closure const *, GobFunction const *>;

    namespace ValueOperations
    {
        /**
         * @brief Compare two memory values and validate that both are equal
         *
         * @param a
         * @param b
         * @return true
         * @return false
         */
        bool areEqual(Value const &a, Value const &b);

        /**
         * @brief Create a string representation of a given value
         *
         * @param val Value to convert to string
         * @param pretty Whether to add decorators. Only is relevant for strings during printing
         * @return std::string
         */
        std::string toString(Value const &val);

        /// @brief Increase reference count for value if value if refcounted, otherwise do nothing
        /// @param v Value
        void increaseValueRefCount(Value const &v);

        /// @brief Decrease reference count for value if value if refcounted, otherwise do nothing
        /// @param v Value
        void decreaseValueRefCount(Value const &v);

        bool less(Value const &a, Value const &b);

    }
}