#pragma once
#include <vector>
#include <map>
#include <cstdint>
#include <memory>
#include <algorithm>

namespace GobLang::Codegen
{
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
