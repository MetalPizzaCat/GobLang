#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "Lexems.hpp"
#include "CodeGenValue.hpp"

namespace GobLang::Codegen
{
    class Builder
    {
    public:
        explicit Builder() = default;

        std::unique_ptr<CodeGenValue> createConstFloat(float val);

        std::unique_ptr<CodeGenValue> createConstInt(int32_t val);

        std::unique_ptr<CodeGenValue> createConstString(size_t strId);

        std::unique_ptr<CodeGenValue> createConstBool(bool val);

        std::unique_ptr<CodeGenValue> createOperation(std::unique_ptr<CodeGenValue> left, std::unique_ptr<CodeGenValue> right, Operator op);

        std::unique_ptr<CodeGenValue> createAssignment(std::unique_ptr<CodeGenValue> left, std::unique_ptr<CodeGenValue> right, Operator op);

        std::unique_ptr<CodeGenValue> createVariableInit(size_t id, std::unique_ptr<CodeGenValue> init);

        std::unique_ptr<CodeGenValue> createCall(size_t nameId, std::vector<std::unique_ptr<CodeGenValue>> args);

        std::unique_ptr<VariableCodeGenValue> createVariableAccess(size_t nameId);

        BlockContext *getCurrentBlock() { return m_blocks.back().get(); }

        size_t getLocalVariableId(size_t nameId) const;

        std::vector<uint8_t> const &getBytes() { return m_bytes; }

        void pushEmptyBlock();

        std::unique_ptr<BlockContext> popBlock();

        /// @brief Insert variable name into the current block variable name
        /// @param nameId Variable name string id
        /// @return Inserted variable id
        size_t insertVariable(size_t nameId);

    private:
        std::vector<std::unique_ptr<BlockContext>> m_blocks;
        std::vector<uint8_t> m_bytes;
    };
} // namespace Goblang::Codegen
