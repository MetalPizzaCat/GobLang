#pragma once
#include <map>
#include <vector>
#include <cstdint>
#include <string>
#include <cassert>
#include <exception>
#include <memory>
#include "Type.hpp"
#include "Memory.hpp"
#include "Operations.hpp"
#include "Value.hpp"
#include "Array.hpp"
#include "String.hpp"
#include "Exception.hpp"
#include "Structure.hpp"
#include "../codegen/ByteCode.hpp"
#include "NativeStructure.hpp"

using namespace GobLang::Struct;
namespace GobLang
{
    class Machine
    {
    public:
        explicit Machine() = default;

        explicit Machine(Codegen::ByteCode const &code);

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

        Value *getStackTop();

        Value getStackTopAndPop();

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

        /**
         * @brief Register object to be handled by the garbage collector. This object will be ref counted and deleted once it is no longer in use
         *
         * @param obj Object to register
         */
        void addObject(MemoryNode *obj);

        void popStack();

        void pushToStack(Value const &val);

        void pushIntToStack(int32_t val);
        void pushFloatToStack(float val);
        void pushObjectToStack(MemoryNode *obj);

        template <class T>
        T popFromStack()
        {
            Value top = _getFromTopAndPop();
            if (!std::holds_alternative<T>(top))
            {
                return 0;
            }
            return std::get<T>(top);
        }

        template <class T>
        T *popObjectFromStack()
        {
            return dynamic_cast<T*>(popFromStack<MemoryNode *>());
        }

        Value getVariableValue(std::string const &name) { return m_globals[name]; }

        /**
         * @brief Set local variable value using id. If id is larger than current amount of variables the array will be expanded to match the id
         *
         * @param id id of the variable
         * @param val Value of the variable
         */
        void setLocalVariableValue(size_t id, Value const &val);

        /**
         * @brief Get value of a local variable. Beware that addressing id that is no longer in use by the block that created it is undefined behaviour
         *
         * @param id Id of the local variable
         * @return Value* Value of the local variable or nullptr if no value uses this id
         */
        Value *getLocalVariableValue(size_t id);

        void shrinkLocalVariableStackBy(size_t size);

        void removeFunctionFrame();

        /// @brief Call local function that is stored under id funcId
        /// @param funcId Id of the function
        void callLocalFunction(size_t funcId);

        /**
         * @brief Create a custom variable that will be accessible in code. Useful for binding with c code
         *
         * @param name Name of the variable
         * @param value Value of the variable
         */
        void createVariable(std::string const &name, Value const &value);

        void createType(std::string const &name, FunctionValue const &constructor, std::map<std::string, FunctionValue> const &methods = {});

        NativeStructureInfo const *getNativeStructure(std::string const &name);

        void collectGarbage();

        ~Machine();

    private:
        inline Value _operationTop() { return m_operationStack.back().back(); }

        Value _getFromTopAndPop()
        {
            if (m_operationStack.back().empty())
            {
                throw RuntimeException("Can not pop from stack because stack is empty");
            }
            Value v = _operationTop();
            popStack();
            return v;
        }
        ProgramAddressType _getAddressFromByteCode(size_t start);

        /// @brief Parse next `sizeof(T)` bytes into a T value using bitshifts and reinterpret cast
        /// @tparam T Type of the value to convert into
        /// @param start Where in the byte code to start from
        /// @return Parsed value
        template <typename T>
        T _parseOperationConstant(size_t start)
        {
            uint64_t res = 0;
            for (uint64_t i = 0; i < sizeof(T); i++)
            {
                uint64_t offset = (sizeof(T) - i - 1) * 8;
                res |= (uint64_t)(m_operations[start + i]) << offset;
            }
            T *f = reinterpret_cast<T *>(&res);
            return *f;
        }

        inline void _jump();

        inline void _jumpIfNot();

         inline void _jumpIf();

        inline void _jumpBack();

        inline void _add();

        inline void _sub();

        inline void _mul();

        inline void _div();

        inline void _mod();

        inline void _set();

        inline void _get();

        inline void _bitAnd();

        inline void _bitOr();

        inline void _bitXor();

        inline void _bitNot();

        inline void _shiftLeft();

        inline void _shiftRight();

        inline void _setLocal();

        inline void _getLocal();

        inline void _call();

        inline void _getLocalFunc();

        inline void _return();

        inline void _returnWithValue();

        inline void _pushConstInt();

        inline void _pushConstUnsignedInt();

        inline void _pushConstFloat();

        inline void _pushConstChar();

        inline void _pushConstString();

        inline void _pushConstNull();

        inline void _getArray();

        inline void _setArray();

        inline void _getField();

        inline void _setField();

        inline void _callMethod();

        inline void _eq();

        inline void _neq();

        inline void _and();

        inline void _or();

        inline void _less();

        inline void _more();

        inline void _lessOrEq();

        inline void _moreOrEq();

        inline void _negate();

        inline void _not();

        inline void _shrink();

        inline void _createArray();

        inline void _new();

        bool m_forcedEnd = false;

        MemoryNode m_memoryRoot;
        size_t m_programCounter = 0;
        std::vector<uint8_t> m_operations;
        std::vector<std::vector<Value>> m_operationStack = {{}};
        /**
         * @brief Special dictionary that can be written externally and internally which uses strings to identify variables.
         *
         * Any variable that doesn't have a valid local variable attached will attempt to read a global variable value
         */
        std::map<std::string, Value> m_globals;
        /**
         * @brief Array of currently present local variables.
         *  These variables can only be addressed by their index and will be overriden once the id is used in a different block
         *
         */
        std::vector<std::vector<Value>> m_variables = {{}};
        std::vector<std::string> m_constStrings;
        std::vector<Function> m_functions;
        std::vector<std::unique_ptr<Struct::Structure>> m_structures;

        std::map<std::string, std::unique_ptr<Struct::NativeStructureInfo>> m_nativeStructures;

        /**
         * @brief Return locations for all of the call operations. This points to where the jump happened from
         *
         */
        std::vector<size_t> m_callStack;
    };
}