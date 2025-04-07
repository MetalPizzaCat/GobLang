#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "Lexems.hpp"
#include "CodeGenValue.hpp"

namespace GobLang::Codegen
{
    template <typename T>
    std::vector<uint8_t> parseToBytes(T val)
    {
        uint32_t *v = reinterpret_cast<uint32_t *>(&val);
        std::vector<uint8_t> res;
        for (int32_t i = sizeof(T) - 1; i >= 0; i--)
        {
            uint64_t offset = (sizeof(uint8_t) * i) * 8;
            const size_t mask = 0xff;
            uint64_t num = (*v) & (mask << offset);
            uint64_t numFixed = num >> offset;
            res.push_back((uint8_t)numFixed);
        }
        return res;
    }


    class Builder
    {
    public:
        explicit Builder() = default;

        std::unique_ptr<CodeGenValue> createConstFloat(float val);

        std::unique_ptr<CodeGenValue> createConstInt(int32_t val);

        std::unique_ptr<CodeGenValue> createConstString(size_t strId);

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
