#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "Lexems.hpp"
#include "CodeGenValue.hpp"
#include "../execution/Structure.hpp"
#include "../execution/Function.hpp"

namespace GobLang::Codegen
{
    class Builder
    {
    public:
        explicit Builder() = default;

        std::unique_ptr<CodeGenValue> createConstNull();
        std::unique_ptr<CodeGenValue> createConstFloat(float val);

        std::unique_ptr<CodeGenValue> createConstInt(int32_t val);

        std::unique_ptr<CodeGenValue> createConstUnsignedInt(uint32_t val);

        std::unique_ptr<CodeGenValue> createConstString(size_t strId);

        std::unique_ptr<CodeGenValue> createConstBool(bool val);

        std::unique_ptr<CodeGenValue> createConstChar(char ch);

        Function const *addFunction(size_t nameId, std::vector<size_t> argIds);

        void addType(std::string const &name, std::vector<std::string> const &fieldNames, size_t nameId, std::vector<size_t> fieldIds);

        /// @brief Check if a local function with given name id was already registered
        /// @param nameId
        /// @return True if function exists
        bool hasLocalFunctionWithName(size_t nameId);

        bool hasTypeWithName(size_t nameId);

        std::unique_ptr<CodeGenValue> createOperation(std::unique_ptr<CodeGenValue> left, std::unique_ptr<CodeGenValue> right, Operator op);

        std::unique_ptr<CodeGenValue> createUnaryOperator(std::unique_ptr<CodeGenValue> right, Operator op);

        std::unique_ptr<CodeGenValue> createAssignment(std::unique_ptr<CodeGenValue> left, std::unique_ptr<CodeGenValue> right);

        std::unique_ptr<CodeGenValue> createVariableInit(size_t id, std::unique_ptr<CodeGenValue> init);

        std::unique_ptr<CodeGenValue> createCall(size_t nameId, std::vector<std::unique_ptr<CodeGenValue>> args);

        std::unique_ptr<CodeGenValue> createConstructorCall(size_t typeId, std::vector<std::unique_ptr<CodeGenValue>> args);

        std::unique_ptr<CodeGenValue> createCallFromValue(
            std::unique_ptr<CodeGenValue> value,
            std::vector<std::unique_ptr<CodeGenValue>> args);

        std::unique_ptr<ArrayAccessCodeGenValue> createArrayAccess(
            std::unique_ptr<CodeGenValue> array,
            std::unique_ptr<CodeGenValue> index);

        std::unique_ptr<FieldAccessCodeGenValue> createFieldAccess(
            std::unique_ptr<CodeGenValue> object,
            std::unique_ptr<CodeGenValue> field);

        std::unique_ptr<VariableCodeGenValue> createVariableAccess(size_t nameId);

        std::unique_ptr<CodeGenValue> createLocalFunctionAccess(size_t nameId);

        BlockContext *getCurrentBlock();

        bool isCurrentlyInFunction();

        size_t getLocalVariableId(size_t nameId) const;

        void pushEmptyBlock();

        void pushBlockForFunction(Function const *func);

        std::unique_ptr<BlockContext> popBlock();

        /// @brief Insert variable name into the current block variable name
        /// @param nameId Variable name string id
        /// @return Inserted variable id
        size_t insertVariable(size_t nameId);

        std::vector<Struct::Structure> getTypes() const;

    private:
        std::vector<std::unique_ptr<BlockContext>> m_blocks;

        std::vector<std::unique_ptr<Function>> m_functions;

        std::vector<std::unique_ptr<TypeCodeGenInfo>> m_types;
    };
} // namespace Goblang::Codegen
