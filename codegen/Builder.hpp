#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "Lexems.hpp"

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

    class CodeGenValue
    {
    public:
        virtual ~CodeGenValue() = default;

        virtual std::vector<uint8_t> getGetOperationBytes() = 0;

        virtual std::vector<uint8_t> getSetOperationBytes() { return {}; }

    private:
    };

    class GeneratedCodeGenValue : public CodeGenValue
    {
    public:
        explicit GeneratedCodeGenValue(std::vector<uint8_t> val);

        std::vector<uint8_t> getGetOperationBytes() override { return m_bytes; }

    private:
        std::vector<uint8_t> m_bytes;
    };

    class BlockContext
    {
    public:
        void insertVariable(size_t name) {}
        explicit BlockContext() = default;

        size_t getVariableCount() const { return m_variables.size(); }

        size_t getVariableLocalId(size_t nameId);

        bool hasVariableWithNameId(size_t nameId);

        void insert(std::vector<uint8_t> const &bytes);

        std::vector<uint8_t> const &getBytes() { return m_bytes; }

        /// @brief Append instructions for clearing variable stack
        void appendMemoryClear();

    private:
        std::map<size_t, size_t> m_jumps;
        std::vector<size_t> m_variables;
        std::vector<uint8_t> m_bytes;
    };

    class BlockCodeGenValue : public CodeGenValue
    {
    public:
        explicit BlockCodeGenValue(std::unique_ptr<BlockContext> block);

        std::vector<uint8_t> getGetOperationBytes() override { return m_block->getBytes(); }

    private:
        std::unique_ptr<BlockContext> m_block;
    };

    class Builder
    {
    public:
        explicit Builder() = default;

        std::unique_ptr<CodeGenValue> createConstFloat(float val);

        std::unique_ptr<CodeGenValue> createConstInt(int32_t val);

        std::unique_ptr<CodeGenValue> createFloatOperation(std::unique_ptr<CodeGenValue> left, std::unique_ptr<CodeGenValue> right, Operator op);

        std::unique_ptr<CodeGenValue> createVariableInit(size_t id, std::unique_ptr<CodeGenValue> init);

        BlockContext *getCurrentBlock() { return m_blocks.back().get(); }

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
