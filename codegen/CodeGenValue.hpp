#pragma once
#include <vector>
#include <map>
#include <cstdint>
#include <memory>
#include <algorithm>

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

    class Builder;
    class BlockContext
    {
    public:
        explicit BlockContext() = default;

        size_t getVariableCount() const { return m_variables.size(); }

        void insertVariable(size_t name);

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

    class BranchCodeGenValue : public CodeGenValue
    {
    public:
        explicit BranchCodeGenValue(std::vector<uint8_t> cond, std::vector<uint8_t> body);

        /// @brief Add jump at the end of the body block(For use with elif and else)
        /// @param offset
        void addJump(size_t offset);

        std::vector<uint8_t> getGetOperationBytes() override;

        /// @brief Set value for the jump that occurs if condition is evaluated to false
        /// @param offset
        void setConditionJumpOffset(size_t offset);

        /// @brief Get size of the body block in bytes, accounting for the optional end jump
        /// @return
        size_t getBodySize();

        /// @brief Get the full size of the block in bytes
        /// @return
        size_t getFullSize();

        size_t getConditionSize();

        bool hasEndJump() const { return m_jumpAfter != -1; }

    private:
        size_t m_jumpAfter = -1;
        std::vector<uint8_t> m_condBytes;
        std::vector<uint8_t> m_bodyBytes;
    };

    class VariableCodeGenValue : public CodeGenValue
    {
    public:
        explicit VariableCodeGenValue(size_t nameId, bool local);

        std::vector<uint8_t> getGetOperationBytes() override;

        std::vector<uint8_t> getSetOperationBytes() override;

    private:
        size_t m_id;
        bool m_local;
    };

    class BlockCodeGenValue : public CodeGenValue
    {
    public:
        explicit BlockCodeGenValue(std::unique_ptr<BlockContext> block);

        std::vector<uint8_t> getGetOperationBytes() override { return m_block->getBytes(); }

    private:
        std::unique_ptr<BlockContext> m_block;
    };
} // namespace GobLang::Codegen
