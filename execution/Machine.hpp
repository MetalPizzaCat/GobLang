#pragma once
#include <map>
#include <vector>
#include <cstdint>
#include <string>
#include <cassert>
#include <exception>

#include "Type.hpp"
#include "Memory.hpp"
#include "Operations.hpp"
#include "Value.hpp"
#include "Array.hpp"
#include "Exception.hpp"
#include "../compiler/ByteCode.hpp"

namespace GobLang
{
    /**
     * @brief Type used to store jump addresses in the code
     *
     */
    using ProgramAddressType = size_t;
    class Machine
    {
    public:
        explicit Machine()
        {
        }

        explicit Machine(Compiler::ByteCode const &code);

        void addOperation(Operation op)
        {
            m_operations.push_back((uint8_t)op);
        }

        void addUInt8(uint8_t val)
        {
            m_operations.push_back(val);
        }

        void addStringConst(std::string const &str)
        {
            m_constStrings.push_back(str);
        }

        void addIntConst(int32_t val)
        {
            m_constInts.push_back(val);
        }

        size_t getProgramCounter() const
        {
            return m_programCounter;
        }

        bool isAtTheEnd() const
        {
            return m_programCounter >= m_operations.size() || m_forcedEnd;
        }
        void addFunction(FunctionValue const &func, std::string const &name);
        void step();

        void printGlobalsInfo();

        void printVariablesInfo();

        void printStack();

        MemoryValue *getStackTop();

        MemoryValue *getStackTopAndPop();

        ArrayNode *createArrayOfSize(int32_t size);

        /**
         * @brief Create a new string object in memory
         *
         * @param str Base string to store in memory
         * @param alwaysNew If true that means that it will skip search and always create new memory object.
         * This is useful to avoid messing variables that were set from constants
         * @return StringNode* Pointer to new string object or other string object that was found in memory
         */
        StringNode *createString(std::string const &str, bool alwaysNew = false);

        void popStack();

        void pushToStack(MemoryValue const &val);

        MemoryValue getVariableValue(std::string const &name) { return m_globals[name]; }

        /**
         * @brief Set local variable value using id. If id is larger than current amount of variables the array will be expanded to match the id
         *
         * @param id id of the variable
         * @param val Value of the variable
         */
        void setLocalVariableValue(size_t id, MemoryValue const &val);

        /**
         * @brief Get value of a local variable. Beware that addressing id that is no longer in use by the block that created it is undefined behaviour
         *
         * @param id Id of the local variable
         * @return MemoryValue* Value of the local variable or nullptr if no value uses this id
         */
        MemoryValue *getLocalVariableValue(size_t id);

        void shrinkLocalVariableStackBy(size_t size);

        void removeFunctionFrame();

        /**
         * @brief Create a custom variable that will be accessible in code. Useful for binding with c code
         *
         * @param name Name of the variable
         * @param value Value of the variable
         */
        void createVariable(std::string const &name, MemoryValue const &value);

        void collectGarbage();

        ~Machine();

    private:
        inline MemoryValue _operationTop() { return m_operationStack.back().back(); }

        inline MemoryValue _getFromTopAndPop()
        {
            MemoryValue v = _operationTop();
            popStack();
            return v;
        }
        ProgramAddressType _getAddressFromByteCode(size_t start);

        void _jump();

        void _jumpIf();

        void _add();

        void _sub();

        void _mul();

        void _div();

        void _set();

        void _get();

        void _setLocal();

        void _getLocal();

        void _call();

        void _callLocal();

        void _return();

        void _returnWithValue();

        void _pushConstInt();

        void _pushConstFloat();

        void _pushConstChar();

        void _pushConstString();

        void _getArray();

        void _setArray();

        void _eq();

        void _neq();

        void _and();

        void _or();

        void _less();

        void _more();

        void _lessOrEq();

        void _moreOrEq();

        void _negate();

        void _not();

        void _shrink();

        bool m_forcedEnd = false;

        MemoryNode *m_memoryRoot = new MemoryNode();
        size_t m_programCounter = 0;
        std::vector<uint8_t> m_operations;
        std::vector<std::vector<MemoryValue>> m_operationStack = {{}};
        /**
         * @brief Special dictionary that can be written externally and internally which uses strings to identify variables.
         *
         * Any variable that doesn't have a valid local variable attached will attempt to read a global variable value
         */
        std::map<std::string, MemoryValue> m_globals;
        /**
         * @brief Array of currently present local variables.
         *  These variables can only be addressed by their index and will be overriden once the id is used in a different block
         *
         */
        std::vector<std::vector<MemoryValue>> m_variables = {{}};
        std::vector<int32_t> m_constInts;
        std::vector<float> m_constFloats;
        std::vector<std::string> m_constStrings;
        std::vector<Function> m_functions;

        /**
         * @brief Return locations for all of the call operations. This points to where the jump happened from
         *
         */
        std::vector<size_t> m_callStack;
    };
}