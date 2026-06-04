#pragma once
#include <vector>
#include <memory>
#include "Memory.hpp"
#include "Closure.hpp"
#include "Error.hpp"
#include "String.hpp"
#include "Array.hpp"
#include "StructureObject.hpp"
namespace GobLang
{
    /**
     * @brief Class that represents active state within language and is capable of altering it by executing code
     *
     */
    class State
    {
    public:
        /**
         * @brief Run code provided in the close by either executing it as byte code or by calling native c++ function depending on stored value
         *
         * @param closure Closure to execute
         */
        void executeClosure(Closure const &closure);

        /**
         * @brief Sequentially execute commands stored within the bytecode of the provided function,
         *  creating new layer of stack and variable blocks
         *
         * @param func Function to execute
         */
        void runBytes(GobFunction const *func);

        /// @brief Get "global" variable by name or throw error if no variable uses that name
        /// @param name Name of the variable
        /// @return Value of the variable
        Value getGlobalVariable(std::string const &name) const;

        /**
         * @brief Load and compile code from the string into the interpreter without running it
         *
         * @param str
         */
        Closure const *loadString(std::string const &str);

        /**
         * @brief Try to get global variable by name or return None if it's not present
         *
         * @tparam T Type to get
         * @param name Name of the variable
         * @return std::optional<T>
         */
        template <class T>
        std::optional<T> getGlobalVariableAsType(std::string const &name) const
        {
            if (m_globals.contains(name))
            {
                if (!std::holds_alternative<T>(m_globals.at(name)))
                {
                    throw Errors::ExecutionError("Incorrect type in stack");
                }
                return std::get<T>(m_globals.at(name));
            }
            return {};
        }

        /// @brief Pop value from current stack frame or throw error if no stack frame exists or stack is empty
        Value popFromStackOrError();

        /// @brief Set "global" variable by name or throw error if no variable uses that name
        /// @param name Name of the variable
        /// @param v Value to set
        void setGlobalVariable(std::string const &name, Value const &v);

        /// @brief Parse next `sizeof(T)` bytes into a T value using bitshifts and reinterpret cast
        /// @tparam T Type of the value to convert into
        /// @param start Where in the byte code to start from
        /// @param end End point of the array
        /// @return Parsed value
        template <typename T>
        T parseOperationConstant(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end)
        {
            uint64_t res = 0;
            for (uint64_t i = 0; i < sizeof(T) && (begin + i) != end; i++)
            {
                uint64_t offset = (sizeof(T) - i - 1) * 8;
                res |= (uint64_t)(*(begin + i)) << offset;
            }
            T *f = reinterpret_cast<T *>(&res);
            return *f;
        }

        /// @brief Attempt to get value from top of the current stack frame as given type. Throws error if no value is present or
        /// @tparam T
        /// @param errorMessage
        /// @return
        template <class T>
        T popFromStackAsType(std::string const &errorMessage)
        {
            if (m_stack.empty() || m_stack.back().empty())
            {
                throw Errors::RuntimeMemoryError("Can not pop from stack because stack is empty");
            }
            if (!std::holds_alternative<T>(m_stack.back().back()))
            {
                throw Errors::RuntimeMemoryError(errorMessage);
            }
            Value v = m_stack.back().back();
            m_stack.back().pop_back();
            return std::get<T>(v);
        }

        /**
         * @brief Parse next `sizeof(T)` bytes into a T value using bitshifts and reinterpret cast. Advance program counter by `sizeof(T)`
         *
         * @tparam T Type of the value to convert into
         * @param begin  Where in the byte code to start from
         * @param end End point of the array
         * @param pc Program counter
         * @return T Parsed value
         */
        template <typename T>
        inline T parseOperationConstantAndAdvance(std::vector<uint8_t>::const_iterator begin,
                                                  std::vector<uint8_t>::const_iterator end,
                                                  ProgramAddressType &pc)
        {
            uint64_t res = 0;
            for (uint64_t i = 0; i < sizeof(T) && (begin + i) != end; i++)
            {
                uint64_t offset = (sizeof(T) - i - 1) * 8;
                res |= (uint64_t)(*(begin + i)) << offset;
            }
            T *f = reinterpret_cast<T *>(&res);
            pc += sizeof(T);
            return *f;
        }

        /// @brief Shrink current local variable frame by given value, decreasing reference counter for all included values.
        /// If frame is smaller than provided value, frame is resized to 0
        /// @param size
        void shrinkVariableFrameBy(size_t size);
        /**
         * @brief Get variable with given id in the current variable block
         *
         * @param id Of the
         * @return std::optional<Value>
         */
        std::optional<Value> getVariableValue(size_t id) const;

        /**
         * @brief Set the value of local variable with given id. Expands the variable block size to fit
         *
         * @param id If of the variable
         * @param val Value to set to
         */
        void setVariableValue(size_t id, Value const &val);

        std::optional<Value> popFromStack();

        /// @brief Create a new string object and store it in the memory list
        /// @param str String to create the object from
        /// @return Pointer to the managed string object
        StringObject *createString(std::string const &str);

        /// @brief Create array of given size
        /// @param size Default size of the array
        /// @return Pointer to the managed array object
        ArrayObject *createArray(size_t size = 0);

        /**
         * @brief Create a closure from function data object and put it into garbage collector
         *
         * @param bytecode Bytecode to execute
         * @param strings Constant strings
         * @param name Name of the function for debugging purposes
         * @return GobFunction const* Pointer to the object
         */
        Closure const *createClosure(std::vector<uint8_t> const &bytecode, std::vector<std::string> strings, std::string const &name = "?");

        /**
         * @brief Create a closure from c++ function and put it into garbage collector
         *
         * @param f Function
         * @return Closure const*
         */
        Closure const *createClosure(FunctionValue const &f);

        /**
         * @brief Create create closure from function object and put it into garbage collector
         *
         * @param f Function
         * @param owner
         * @return Closure const*
         */
        Closure const *createClosure(GobFunction *f, Object *owner);

        /**
         * @brief Create new empty function and store it in memory
         *
         * @return GobFunction* Empty function to be filled through compilation
         */
        GobFunction *createFunction();

        void pushToStack(Value val);
        void collectGarbage();

    private:
        std::unordered_map<std::string, Value> m_globals;
        std::vector<std::vector<Value>> m_variables;
        std::vector<std::vector<Value>> m_stack;
        std::vector<std::unique_ptr<Object>> m_objects;
    };
} // namespace Gobang
